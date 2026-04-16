#!/usr/bin/env bash
set -u

if [[ $# -lt 1 ]]; then
    echo "Usage: $0 /chemin/vers/minishell"
    exit 1
fi

MINISHELL_INPUT="$1"
if [[ ! -e "$MINISHELL_INPUT" ]]; then
    echo "Erreur: '$MINISHELL_INPUT' n'existe pas."
    exit 1
fi
if [[ ! -f "$MINISHELL_INPUT" ]]; then
    echo "Erreur: '$MINISHELL_INPUT' n'est pas un fichier exécutable."
    exit 1
fi
if [[ ! -x "$MINISHELL_INPUT" ]]; then
    echo "Erreur: '$MINISHELL_INPUT' n'est pas exécutable. Fais: chmod +x '$MINISHELL_INPUT'"
    exit 1
fi
if ! command -v valgrind >/dev/null 2>&1; then
    echo "Erreur: valgrind n'est pas installé sur cette machine."
    exit 1
fi
if ! command -v python3 >/dev/null 2>&1; then
    echo "Erreur: python3 est requis."
    exit 1
fi

MINISHELL="$(cd "$(dirname "$MINISHELL_INPUT")" && pwd -P)/$(basename "$MINISHELL_INPUT")"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPORT_FILE="${REPORT_FILE:-$SCRIPT_DIR/minishell_valgrind_report.html}"
TMP_ROOT="$(mktemp -d /tmp/minishell_vg_test.XXXXXX)"
WORKDIR="$TMP_ROOT/workdir"
mkdir -p "$WORKDIR"

TOTAL=0
FUNC_OK=0
FUNC_KO=0
MEM_OK=0
MEM_KO=0
FD_OK=0
FD_KO=0
ALL_OK=0
ALL_KO=0
TEST_INDEX=0
CURRENT_SECTION="failure"

html_escape() {
    sed -e 's/&/\&amp;/g' -e 's/</\&lt;/g' -e 's/>/\&gt;/g'
}

trim_trailing_newlines_file() {
    python3 - "$1" <<'PY'
from pathlib import Path
import sys
p = Path(sys.argv[1])
data = p.read_bytes()
while data.endswith(b"\n"):
    data = data[:-1]
p.write_bytes(data)
PY
}

normalize_for_compare() {
    python3 - "$1" "$2" "$3" "$4" <<'PY'
from pathlib import Path
import re
import sys

out_path = Path(sys.argv[1])
input_path = Path(sys.argv[2])
kind = sys.argv[3]
stream = sys.argv[4]

text = out_path.read_text(errors='ignore')
input_lines = input_path.read_text(errors='ignore').splitlines()

text = re.sub(r'\x1b\[[0-9;?]*[ -/]*[@-~]', '', text)
text = text.replace('\r', '')
text = text.replace('\x00', '')

if stream == 'stderr':
    text = re.sub(r'(?m)^bash: line \d+:\s*', '', text)
    text = re.sub(r'(?m)^bash:\s*', '', text)
    text = re.sub(r'(?m)^minishell:\s*', '', text)

text = re.sub(r'(?m)^(?:minishell >\s*|>\s*)+', '', text)
text = re.sub(r'(?:minishell >\s*|>\s*)+$', '', text)
text = text.replace('minishell > ', '')

if kind == 'minishell':
    for raw in input_lines:
        line = raw.rstrip('\n')
        if not line:
            continue
        pattern = re.compile(r'(?m)^(?:minishell >\s*|>\s*)?' + re.escape(line) + r'\s*$\n?')
        text, _ = pattern.subn('', text, count=1)
        if text.startswith(line + '\n'):
            text = text[len(line)+1:]
        elif text == line:
            text = ''

text = re.sub(r'\n{3,}', '\n\n', text)
text = text.strip('\n')
out_path.write_text(text)
PY
}

run_valgrind_for_test() {
    local input_file="$1"
    local log_file="$2"
    local rc_file="$3"

    (
        cd "$WORKDIR" || exit 1
        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --errors-for-leak-kinds=definite,indirect,possible \
            --track-fds=yes \
            --trace-children=no \
            --child-silent-after-fork=yes \
            --error-exitcode=91 \
            --log-file="$log_file" \
            "$MINISHELL" < "$input_file" > /dev/null 2>&1
        printf '%s' "$?" > "$rc_file"
    )
}

parse_valgrind_log() {
    python3 - "$1" "$2" <<'PY'
from pathlib import Path
import re
import sys

log_path = Path(sys.argv[1])
out_path = Path(sys.argv[2])
text = log_path.read_text(errors='ignore') if log_path.exists() else ''

mem_ok = True
fd_ok = True

def parse_bytes(kind: str):
    m = re.search(rf'{re.escape(kind)}:\s*([0-9,]+) bytes', text)
    if not m:
        return None
    return int(m.group(1).replace(',', ''))

for label in ['definitely lost', 'indirectly lost', 'possibly lost']:
    value = parse_bytes(label)
    if value is not None and value != 0:
        mem_ok = False

fd_lines = []
ignored_fd_lines = []
for line in text.splitlines():
    if 'Open file descriptor ' not in line:
        continue
    m = re.search(r'Open file descriptor\s+(\d+)', line)
    if not m:
        continue
    fd = int(m.group(1))
    cleaned = line.strip()
    if fd <= 2:
        continue
    if 'valgrind.log' in cleaned:
        ignored_fd_lines.append(cleaned)
        continue
    fd_ok = False
    fd_lines.append(cleaned)

summary = []
for key in ['definitely lost', 'indirectly lost', 'possibly lost', 'still reachable']:
    m = re.search(rf'{re.escape(key)}:\s*[^\n]+', text)
    if m:
        summary.append(m.group(0).strip())
if not summary:
    summary.append('Aucun résumé mémoire trouvé dans le log valgrind.')

if fd_lines:
    summary.append('FD supplémentaires encore ouverts (>2, hors valgrind.log) :')
    summary.extend(fd_lines[:10])
elif ignored_fd_lines:
    summary.append('Aucun FD supplémentaire encore ouvert détecté. Le FD de valgrind.log est ignoré volontairement.')
else:
    m = re.search(r'FILE DESCRIPTORS:\s*([^\n]+)', text)
    if m:
        summary.append(m.group(0).strip())

out_path.write_text(
    f"MEM_OK={'1' if mem_ok else '0'}\n"
    f"FD_OK={'1' if fd_ok else '0'}\n"
    + '\n'.join(summary) + '\n'
)
PY
}

get_meta_value() {
    local file="$1"
    local key="$2"
    awk -F= -v k="$key" '$1==k {print $2; exit}' "$file"
}

strip_meta_lines() {
    local src="$1"
    local dst="$2"
    grep -vE '^(MEM_OK|FD_OK)=' "$src" > "$dst" || true
}

write_report_header() {
cat > "$REPORT_FILE" <<EOF2
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Rapport Minishell Valgrind</title>
<style>
body { font-family: Arial, sans-serif; background:#f6f7fb; color:#222; margin:0; padding:24px; }
.container { max-width:1200px; margin:0 auto; }
.card { background:#fff; border-radius:14px; padding:18px 20px; margin-bottom:16px; box-shadow:0 2px 12px rgba(0,0,0,0.08); }
.status { display:inline-block; padding:6px 10px; border-radius:999px; font-weight:700; color:#fff; font-size:14px; }
.ok { background:#1f9d55; }
.ko { background:#d64545; }
.failcase { background:#b45309; }
.resource { background:#0f766e; }
.summary { display:grid; grid-template-columns: repeat(8, minmax(120px, 1fr)); gap:12px; margin-top:12px; }
.box { padding:14px; border-radius:12px; color:#fff; font-weight:700; text-align:center; }
.box.total { background:#334155; }
.box.funcok { background:#1f9d55; }
.box.funcko { background:#d64545; }
.box.memok { background:#15803d; }
.box.memko { background:#b91c1c; }
.box.fdok { background:#0f766e; }
.box.fdko { background:#9f1239; }
.box.global { background:#4338ca; }
pre { background:#111827; color:#e5e7eb; padding:12px; border-radius:10px; overflow:auto; white-space:pre-wrap; word-wrap:break-word; }
.small { color:#555; font-size:14px; }
.grid { display:grid; grid-template-columns:1fr 1fr; gap:12px; }
.grid-3 { display:grid; grid-template-columns:1fr 1fr 1fr; gap:12px; }
.note { background:#eef6ff; border-left:4px solid #3b82f6; padding:12px; border-radius:10px; color:#234; }
.substatus { display:flex; flex-wrap:wrap; gap:8px; margin:10px 0 14px; }
.pill { display:inline-block; padding:5px 10px; border-radius:999px; color:#fff; font-weight:700; font-size:13px; }
@media (max-width: 900px) { .grid { grid-template-columns:1fr; } .grid-3 { grid-template-columns:1fr; } .summary { grid-template-columns:1fr 1fr; } }
h1,h2,h3 { margin-top:0; }
.code-title { font-size:13px; color:#666; margin-bottom:6px; font-weight:700; }
</style>
</head>
<body>
<div class="container">
<div class="card">
<h1>Rapport Minishell — erreurs + Valgrind</h1>
<p class="small"><strong>Binaire :</strong> $(printf '%s' "$MINISHELL" | html_escape)</p>
<p class="small"><strong>Workdir :</strong> $(printf '%s' "$WORKDIR" | html_escape)</p>
<p class="small"><strong>Date :</strong> $(date '+%Y-%m-%d %H:%M:%S')</p>
<p class="note"><strong>But du script :</strong> vérifier des cas d'échec utiles pour l'évaluation, puis lancer aussi Valgrind pour repérer les fuites mémoire et les descripteurs de fichier qui restent ouverts. Le rapport reste volontairement tolérant sur le texte exact de stderr, pour éviter les faux KO inutiles.</p>
<div class="summary">
<div class="box total">Tests<br>__TOTAL__</div>
<div class="box funcok">Fonctionnel OK<br>__FUNC_OK__</div>
<div class="box funcko">Fonctionnel KO<br>__FUNC_KO__</div>
<div class="box memok">Valgrind mémoire OK<br>__MEM_OK__</div>
<div class="box memko">Valgrind mémoire KO<br>__MEM_KO__</div>
<div class="box fdok">FD OK<br>__FD_OK__</div>
<div class="box fdko">FD KO<br>__FD_KO__</div>
<div class="box global">Tout OK<br>__ALL_OK__/__TOTAL__</div>
</div>
</div>

<div class="card">
<div class="status failcase">Cas d'échec</div>
<h2>Tests automatiques — cas d'échec</h2>
<p class="small">Ici on force des erreurs classiques : commandes introuvables, syntaxe invalide, redirections qui échouent, droits insuffisants, etc.</p>
</div>
EOF2
}

write_section_resources() {
cat >> "$REPORT_FILE" <<'EOF2'
<div class="card">
<div class="status resource">Ressources</div>
<h2>Tests automatiques — fds / redirections / pipes / heredoc</h2>
<p class="small">Ces tests servent surtout à faire sortir les leaks et les fd mal fermés avec Valgrind.</p>
</div>
EOF2
}

write_test_card() {
    local name="$1"
    local label="$2"
    local input_file="$3"
    local ms_out="$4"
    local sh_out="$5"
    local ms_err="$6"
    local sh_err="$7"
    local ms_code="$8"
    local sh_code="$9"
    local func_status="${10}"
    local mem_status="${11}"
    local fd_status="${12}"
    local vg_summary_file="${13}"
    local vg_rc_file="${14}"

    local badge_class="ok"
    if [[ "$func_status" == "KO" || "$mem_status" == "KO" || "$fd_status" == "KO" ]]; then
        badge_class="ko"
    fi

    {
        echo '<div class="card">'
        printf '<div class="status %s">%s</div>\n' "$badge_class" "$label"
        printf '<h2>%s</h2>\n' "$(printf '%s' "$name" | html_escape)"
        echo '<div class="substatus">'
        printf '<span class="pill %s">Fonctionnel %s</span>\n' "$( [[ "$func_status" == "OK" ]] && echo ok || echo ko )" "$func_status"
        printf '<span class="pill %s">Mémoire %s</span>\n' "$( [[ "$mem_status" == "OK" ]] && echo ok || echo ko )" "$mem_status"
        printf '<span class="pill %s">FD %s</span>\n' "$( [[ "$fd_status" == "OK" ]] && echo ok || echo ko )" "$fd_status"
        echo '</div>'
        echo '<div class="code-title">Commande envoyée</div>'
        echo '<pre>'
        html_escape < "$input_file"
        echo '</pre>'
        printf '<p class="small"><strong>Exit minishell :</strong> %s &nbsp; | &nbsp; <strong>Exit bash :</strong> %s &nbsp; | &nbsp; <strong>Exit valgrind :</strong> %s</p>\n' "$(cat "$ms_code")" "$(cat "$sh_code")" "$(cat "$vg_rc_file")"
        echo '<div class="grid">'
        echo '<div>'
        echo '<div class="code-title">STDOUT minishell</div>'
        echo '<pre>'
        html_escape < "$ms_out"
        echo '</pre>'
        echo '</div>'
        echo '<div>'
        echo '<div class="code-title">STDOUT bash</div>'
        echo '<pre>'
        html_escape < "$sh_out"
        echo '</pre>'
        echo '</div>'
        echo '<div>'
        echo '<div class="code-title">STDERR minishell</div>'
        echo '<pre>'
        html_escape < "$ms_err"
        echo '</pre>'
        echo '</div>'
        echo '<div>'
        echo '<div class="code-title">STDERR bash</div>'
        echo '<pre>'
        html_escape < "$sh_err"
        echo '</pre>'
        echo '</div>'
        echo '</div>'
        echo '<div class="code-title">Résumé Valgrind</div>'
        echo '<pre>'
        html_escape < "$vg_summary_file"
        echo '</pre>'
        echo '</div>'
    } >> "$REPORT_FILE"
}

run_test() {
    local name="$1"
    shift

    TOTAL=$((TOTAL + 1))
    TEST_INDEX=$((TEST_INDEX + 1))

    local tdir="$TMP_ROOT/test_$TEST_INDEX"
    mkdir -p "$tdir"
    local input_file="$tdir/input.sh"
    printf '%s\n' "$@" > "$input_file"

    (
        cd "$WORKDIR" || exit 1
        bash --posix < "$input_file" > "$tdir/sh_out_raw" 2> "$tdir/sh_err_raw"
        printf '%s' "$?" > "$tdir/sh_code"
    )

    (
        cd "$WORKDIR" || exit 1
        "$MINISHELL" < "$input_file" > "$tdir/ms_out_raw" 2> "$tdir/ms_err_raw"
        printf '%s' "$?" > "$tdir/ms_code"
    )

    cp "$tdir/sh_out_raw" "$tdir/sh_out"
    cp "$tdir/sh_err_raw" "$tdir/sh_err"
    cp "$tdir/ms_out_raw" "$tdir/ms_out"
    cp "$tdir/ms_err_raw" "$tdir/ms_err"

    normalize_for_compare "$tdir/sh_out" "$input_file" "bash" "stdout"
    normalize_for_compare "$tdir/sh_err" "$input_file" "bash" "stderr"
    normalize_for_compare "$tdir/ms_out" "$input_file" "minishell" "stdout"
    normalize_for_compare "$tdir/ms_err" "$input_file" "minishell" "stderr"

    trim_trailing_newlines_file "$tdir/sh_out"
    trim_trailing_newlines_file "$tdir/sh_err"
    trim_trailing_newlines_file "$tdir/ms_out"
    trim_trailing_newlines_file "$tdir/ms_err"

    local func_status="KO"
    local sh_err_has="0"
    local ms_err_has="0"
    [[ -s "$tdir/sh_err" ]] && sh_err_has="1"
    [[ -s "$tdir/ms_err" ]] && ms_err_has="1"
    if cmp -s "$tdir/ms_out" "$tdir/sh_out" && cmp -s "$tdir/ms_code" "$tdir/sh_code" && [[ "$sh_err_has" == "$ms_err_has" ]]; then
        func_status="OK"
        FUNC_OK=$((FUNC_OK + 1))
    else
        FUNC_KO=$((FUNC_KO + 1))
    fi

    run_valgrind_for_test "$input_file" "$tdir/valgrind.log" "$tdir/vg_rc"
    parse_valgrind_log "$tdir/valgrind.log" "$tdir/vg_meta"
    local mem_ok_val
    local fd_ok_val
    mem_ok_val="$(get_meta_value "$tdir/vg_meta" MEM_OK)"
    fd_ok_val="$(get_meta_value "$tdir/vg_meta" FD_OK)"
    strip_meta_lines "$tdir/vg_meta" "$tdir/vg_summary"

    local mem_status="KO"
    local fd_status="KO"
    if [[ "$mem_ok_val" == "1" ]]; then
        mem_status="OK"
        MEM_OK=$((MEM_OK + 1))
    else
        MEM_KO=$((MEM_KO + 1))
    fi
    if [[ "$fd_ok_val" == "1" ]]; then
        fd_status="OK"
        FD_OK=$((FD_OK + 1))
    else
        FD_KO=$((FD_KO + 1))
    fi

    if [[ "$func_status" == "OK" && "$mem_status" == "OK" && "$fd_status" == "OK" ]]; then
        ALL_OK=$((ALL_OK + 1))
    else
        ALL_KO=$((ALL_KO + 1))
    fi

    local label="Cas d'échec"
    [[ "$CURRENT_SECTION" == "resource" ]] && label="Ressources"

    write_test_card "$name" "$label" "$input_file" "$tdir/ms_out" "$tdir/sh_out" "$tdir/ms_err" "$tdir/sh_err" "$tdir/ms_code" "$tdir/sh_code" "$func_status" "$mem_status" "$fd_status" "$tdir/vg_summary" "$tdir/vg_rc"
}

finalize_report() {
    sed -i \
        -e "s/__TOTAL__/$TOTAL/g" \
        -e "s/__FUNC_OK__/$FUNC_OK/g" \
        -e "s/__FUNC_KO__/$FUNC_KO/g" \
        -e "s/__MEM_OK__/$MEM_OK/g" \
        -e "s/__MEM_KO__/$MEM_KO/g" \
        -e "s/__FD_OK__/$FD_OK/g" \
        -e "s/__FD_KO__/$FD_KO/g" \
        -e "s#__ALL_OK__#$ALL_OK#g" \
        "$REPORT_FILE"

cat >> "$REPORT_FILE" <<EOF2
<div class="card">
<h2>À savoir</h2>
<ul>
<li>Le test fonctionnel compare surtout <strong>le code de retour</strong>, <strong>stdout</strong> et la présence d'un message d'erreur, sans exiger le texte exact de bash.</li>
<li>Le test Valgrind ignore les <strong>still reachable</strong> et considère KO seulement les vraies pertes mémoire (<code>definitely/indirectly/possibly lost</code>).</li>
<li>Pour les fd, le rapport considère KO seulement si Valgrind signale des descripteurs supplémentaires encore ouverts <strong>au-dessus de 2</strong>, en ignorant volontairement le fichier <code>valgrind.log</code>.</li>
<li>Les cas <code>Ctrl-C</code>, <code>Ctrl-\</code> et certains comportements purement interactifs restent mieux testés à la main.</li>
</ul>
</div>
</div>
</body>
</html>
EOF2
}

# Fichiers de travail
printf 'alpha\n' > "$WORKDIR/infile"
printf 'beta\ngamma\n' > "$WORKDIR/infile2"
printf 'line1\nline2\nline3\n' > "$WORKDIR/infile3"
printf '#!/usr/bin/env bash\necho noexec\n' > "$WORKDIR/noexec.sh"
chmod -x "$WORKDIR/noexec.sh"
mkdir -p "$WORKDIR/execdir" "$WORKDIR/subdir"
printf 'inside-subdir\n' > "$WORKDIR/subdir/file.txt"

write_report_header

CURRENT_SECTION="failure"
run_test "commande introuvable" "command_that_does_not_exist" "echo \$?"
run_test "chemin relatif absent" "./not_here" "echo \$?"
run_test "cd dossier absent" "cd does_not_exist" "echo \$?"
run_test "export identifiant invalide" "export 1BAD=abc" "echo \$?"
run_test "unset identifiant invalide" "unset 1BAD" "echo \$?"
run_test "commande absente avec redirection" "idontexist > out" "echo \$?" "ls out" "echo \$?"
run_test "grep sans match" "grep zzz infile2" "echo \$?"

write_section_resources
CURRENT_SECTION="resource"
run_test "leading redirection avant commande" "< infile cat"
run_test "pipe simple" "echo hello | cat"
run_test "double pipe" "cat infile2 | grep beta | wc -l"
run_test "triple pipe" "cat infile3 | grep -v line2 | wc -l"
run_test "input puis output" "cat < infile > out" "cat out"
run_test "builtin dans pipeline" "echo bonjour | cat | wc -c"
run_test "heredoc simple" "cat << EOF" "bonjour" "EOF"
run_test "heredoc puis fichier" "cat << EOF > hdout" "line1" "line2" "EOF" "cat hdout"
run_test "plusieurs commandes dans une session" "echo one" "echo two > out" "cat out" "command_that_does_not_exist" "echo \$?"

finalize_report

echo "Rapport créé : $REPORT_FILE"
echo "Tests : $TOTAL"
echo "Fonctionnel : OK=$FUNC_OK | KO=$FUNC_KO"
echo "Valgrind mémoire : OK=$MEM_OK | KO=$MEM_KO"
echo "Valgrind fd : OK=$FD_OK | KO=$FD_KO"
echo "Tout OK : $ALL_OK / $TOTAL"

if [[ "$FUNC_KO" -ne 0 || "$MEM_KO" -ne 0 || "$FD_KO" -ne 0 ]]; then
    exit 1
fi
