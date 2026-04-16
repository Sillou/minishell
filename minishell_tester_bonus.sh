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

MINISHELL="$(cd "$(dirname "$MINISHELL_INPUT")" && pwd -P)/$(basename "$MINISHELL_INPUT")"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
REPORT_FILE="${REPORT_FILE:-$SCRIPT_DIR/minishell_report.html}"
TMP_ROOT="$(mktemp -d /tmp/minishell_html_test.XXXXXX)"
WORKDIR="$TMP_ROOT/workdir"
mkdir -p "$WORKDIR"

MAIN_TOTAL=0
MAIN_PASS=0
MAIN_FAIL=0
BONUS_TOTAL=0
BONUS_PASS=0
BONUS_FAIL=0
MANUAL=0
TEST_INDEX=0
CURRENT_SECTION="main"
VG_MEM_OK=0
VG_MEM_KO=0
VG_FD_OK=0
VG_FD_KO=0
VG_ALL_OK=0

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

parse_valgrind_log() {
    python3 - "$1" "$2" <<'PY'
from pathlib import Path
import re, sys
log_path = Path(sys.argv[1])
out_path = Path(sys.argv[2])
text = log_path.read_text(errors='ignore') if log_path.exists() else ''
mem_ok = True
fd_ok  = True
for label in ['definitely lost', 'indirectly lost', 'possibly lost']:
    m = re.search(rf'{re.escape(label)}:\s*([0-9,]+) bytes', text)
    if m and int(m.group(1).replace(',','')) != 0:
        mem_ok = False
fd_lines = []
for line in text.splitlines():
    if 'Open file descriptor ' not in line:
        continue
    m = re.search(r'Open file descriptor\s+(\d+)', line)
    if not m:
        continue
    fd = int(m.group(1))
    if fd <= 2:
        continue
    if 'valgrind.log' in line:
        continue
    fd_ok = False
    fd_lines.append(line.strip())
summary = []
for key in ['definitely lost', 'indirectly lost', 'possibly lost', 'still reachable']:
    m = re.search(rf'{re.escape(key)}:\s*[^\n]+', text)
    if m:
        summary.append(m.group(0).strip())
if not summary:
    summary.append('Aucun résumé mémoire valgrind.')
if fd_lines:
    summary.append('FD supplémentaires ouverts (>2) :')
    summary.extend(fd_lines[:10])
out_path.write_text(
    f"MEM_OK={'1' if mem_ok else '0'}\n"
    f"FD_OK={'1' if fd_ok else '0'}\n"
    + '\n'.join(summary) + '\n'
)
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

cat > "$REPORT_FILE" <<EOF2
<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Rapport Minishell</title>
<style>
body { font-family: Arial, sans-serif; background:#f6f7fb; color:#222; margin:0; padding:24px; }
.container { max-width:1100px; margin:0 auto; }
.card { background:#fff; border-radius:14px; padding:18px 20px; margin-bottom:16px; box-shadow:0 2px 12px rgba(0,0,0,0.08); }
.status { display:inline-block; padding:6px 10px; border-radius:999px; font-weight:700; color:#fff; font-size:14px; }
.ok { background:#1f9d55; }
.ko { background:#d64545; }
.manual { background:#2563eb; }
.bonus { background:#7c3aed; }
.summary { display:grid; grid-template-columns: repeat(auto-fill, minmax(110px, 1fr)); gap:10px; margin-top:12px; }
.box { padding:12px; border-radius:12px; color:#fff; font-weight:700; text-align:center; font-size:13px; }
.box.main { background:#4a5568; }
.box.ok { background:#1f9d55; }
.box.ko { background:#d64545; }
.box.manual { background:#2563eb; }
.box.bonus { background:#7c3aed; }
.box.bonusok { background:#6d28d9; }
.box.bonusko { background:#9333ea; }
.box.memok { background:#15803d; }
.box.memko { background:#b91c1c; }
.box.fdok { background:#0f766e; }
.box.fdko { background:#9f1239; }
.box.vgall { background:#4338ca; }
.pill { display:inline-block; padding:4px 10px; border-radius:999px; color:#fff; font-weight:700; font-size:12px; margin:2px; }
.substatus { display:flex; flex-wrap:wrap; gap:6px; margin:8px 0 12px; }
pre { background:#111827; color:#e5e7eb; padding:12px; border-radius:10px; overflow:auto; white-space:pre-wrap; word-wrap:break-word; }
.small { color:#555; font-size:14px; }
.grid { display:grid; grid-template-columns:1fr 1fr; gap:12px; }
.note { background:#eef6ff; border-left:4px solid #3b82f6; padding:12px; border-radius:10px; color:#234; }
.note-purple { background:#f3e8ff; border-left:4px solid #7c3aed; padding:12px; border-radius:10px; color:#3b0764; }
@media (max-width: 900px) { .grid { grid-template-columns:1fr; } .summary { grid-template-columns:1fr 1fr; } }
h1,h2,h3 { margin-top:0; }
.code-title { font-size:13px; color:#666; margin-bottom:6px; font-weight:700; }
.manual-list pre { margin-bottom:10px; }
</style>
</head>
<body>
<div class="container">
<div class="card">
<h1>Rapport de test Minishell</h1>
<p class="small"><strong>Binaire :</strong> $(printf '%s' "$MINISHELL" | html_escape)</p>
<p class="small"><strong>Workdir :</strong> $(printf '%s' "$WORKDIR" | html_escape)</p>
<p class="small"><strong>Date :</strong> $(date '+%Y-%m-%d %H:%M:%S')</p>
<p class="note"><strong>Version bonus auto v2 :</strong> plus de tests obligatoires automatiques, y compris des cas un peu plus complexes, tout en gardant de côté les cas interactifs qui donnent trop de faux KO avec <code>readline</code>.</p>
<div class="summary">
<div class="box main">Obligatoires<br>__MAIN_TOTAL__</div>
<div class="box ok">OK obligatoires<br>__MAIN_PASS__</div>
<div class="box ko">KO obligatoires<br>__MAIN_FAIL__</div>
<div class="box bonus">Bonus auto<br>__BONUS_TOTAL__</div>
<div class="box bonusok">OK bonus<br>__BONUS_PASS__</div>
<div class="box bonusko">KO bonus<br>__BONUS_FAIL__</div>
<div class="box manual">Manuel<br>__MANUAL__</div>
<div class="box memok">Mémoire OK<br>__VG_MEM_OK__</div>
<div class="box memko">Mémoire KO<br>__VG_MEM_KO__</div>
<div class="box fdok">FD OK<br>__VG_FD_OK__</div>
<div class="box fdko">FD KO<br>__VG_FD_KO__</div>
<div class="box vgall">Tout OK<br>__VG_ALL_OK__</div>
</div>
</div>

<div class="card">
<h2>Tests obligatoires automatiques</h2>
<p class="small">Ce bloc compte l'obligatoire stable, avec plus de cas de variables, quotes, pipes, redirections et chemins relatifs.</p>
</div>
EOF2

write_test_card() {
    local status="$1"
    local name="$2"
    local input_file="$3"
    local ms_out="$4"
    local sh_out="$5"
    local ms_err="$6"
    local sh_err="$7"
    local ms_code="$8"
    local sh_code="$9"
    local mem_status="${10:-?}"
    local fd_status="${11:-?}"
    local vg_summary="${12:-}"
    local badge_class="ok"
    local title_prefix=""

    if [[ "$status" == "KO" || "$mem_status" == "KO" || "$fd_status" == "KO" ]]; then
        badge_class="ko"
    fi
    if [[ "$CURRENT_SECTION" == "bonus" ]]; then
        title_prefix="[BONUS] "
    fi

    {
        echo '<div class="card">'
        printf '<div class="status %s">%s</div>\n' "$badge_class" "$status"
        printf '<h2>%s%s</h2>\n' "$title_prefix" "$(printf '%s' "$name" | html_escape)"
        echo '<div class="substatus">'
        printf '<span class="pill %s">Fonctionnel %s</span>\n' "$([[ "$status" == "OK" ]] && echo ok || echo ko)" "$status"
        printf '<span class="pill %s">Mémoire %s</span>\n'     "$([[ "$mem_status" == "OK" ]] && echo ok || echo ko)" "$mem_status"
        printf '<span class="pill %s">FD %s</span>\n'          "$([[ "$fd_status" == "OK" ]] && echo ok || echo ko)" "$fd_status"
        echo '</div>'
        echo '<div class="code-title">Commande envoyée</div>'
        echo '<pre>'
        html_escape < "$input_file"
        echo '</pre>'
        printf '<p class="small"><strong>Exit minishell :</strong> %s &nbsp; | &nbsp; <strong>Exit bash :</strong> %s</p>\n' "$(cat "$ms_code")" "$(cat "$sh_code")"
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
        if [[ -n "$vg_summary" && -f "$vg_summary" ]]; then
            echo '<div class="code-title">Résumé Valgrind</div><pre>'
            html_escape < "$vg_summary"
            echo '</pre>'
        fi
        echo '</div>'
    } >> "$REPORT_FILE"
}

run_test() {
    local name="$1"
    shift

    if [[ "$CURRENT_SECTION" == "bonus" ]]; then
        BONUS_TOTAL=$((BONUS_TOTAL + 1))
    else
        MAIN_TOTAL=$((MAIN_TOTAL + 1))
    fi
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

    # Valgrind — memory leaks + FD leaks
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
            --log-file="$tdir/valgrind.log" \
            "$MINISHELL" < "$input_file" > /dev/null 2>&1
    )
    parse_valgrind_log "$tdir/valgrind.log" "$tdir/vg_meta"
    local mem_ok_val fd_ok_val
    mem_ok_val="$(awk -F= '/^MEM_OK/{print $2;exit}' "$tdir/vg_meta")"
    fd_ok_val="$(awk  -F= '/^FD_OK/{print $2;exit}'  "$tdir/vg_meta")"
    grep -vE '^(MEM_OK|FD_OK)=' "$tdir/vg_meta" > "$tdir/vg_summary" || true
    local mem_status="KO" fd_status="KO"
    [[ "$mem_ok_val" == "1" ]] && { mem_status="OK"; VG_MEM_OK=$((VG_MEM_OK+1)); } || VG_MEM_KO=$((VG_MEM_KO+1))
    [[ "$fd_ok_val"  == "1" ]] && { fd_status="OK";  VG_FD_OK=$((VG_FD_OK+1));   } || VG_FD_KO=$((VG_FD_KO+1))

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
    if cmp -s "$tdir/ms_out" "$tdir/sh_out" && cmp -s "$tdir/ms_err" "$tdir/sh_err" && cmp -s "$tdir/ms_code" "$tdir/sh_code"; then
        func_status="OK"
        if [[ "$CURRENT_SECTION" == "bonus" ]]; then
            BONUS_PASS=$((BONUS_PASS + 1))
        else
            MAIN_PASS=$((MAIN_PASS + 1))
        fi
    else
        if [[ "$CURRENT_SECTION" == "bonus" ]]; then
            BONUS_FAIL=$((BONUS_FAIL + 1))
        else
            MAIN_FAIL=$((MAIN_FAIL + 1))
        fi
    fi
    [[ "$func_status" == "OK" && "$mem_status" == "OK" && "$fd_status" == "OK" ]] && VG_ALL_OK=$((VG_ALL_OK+1))
    write_test_card "$func_status" "$name" "$input_file" \
        "$tdir/ms_out" "$tdir/sh_out" "$tdir/ms_err" "$tdir/sh_err" \
        "$tdir/ms_code" "$tdir/sh_code" \
        "$mem_status" "$fd_status" "$tdir/vg_summary"
}

add_manual_tests_section() {
    MANUAL=10
    cat >> "$REPORT_FILE" <<'EOF2'
<div class="card manual-list">
<div class="status manual">Manuel</div>
<h2>Tu peux aussi tester directement dans le shell</h2>
<p class="small">Ces commandes restent pratiques à vérifier à la main, surtout pour le comportement interactif réel.</p>
<pre>echo -n hello</pre>
<pre>./not_here
echo $?</pre>
<pre>cat &lt;&lt; EOF
bonjour
EOF</pre>
<pre>cat &lt;&lt; EOF | wc -l
a
b
EOF</pre>
<pre>|</pre>
<pre>echo a &gt;</pre>
<pre>&gt; onlyfile
ls onlyfile</pre>
<pre>&gt;&gt; appendfile
ls appendfile</pre>
<pre>&lt; infile cat</pre>
<pre>exit 42</pre>
<pre>exit 1 2
echo after
echo $?</pre>
</div>
EOF2
cat >> "$REPORT_FILE" <<'EOF'
<pre>Tu peux aussi t'amuser avec les fd!!
TERMINAL 1
mkfifo fd
cat fd

TERMINAL 2
echo MESSAGE QUE TU VEUX > fd</pre>
EOF
}

printf 'alpha\n' > "$WORKDIR/infile"
printf 'beta\ngamma\n' > "$WORKDIR/infile2"
printf 'one\ntwo\nthree\n' > "$WORKDIR/infile3"
printf '#!/usr/bin/env bash\necho script-ok\n' > "$WORKDIR/hello.sh"
chmod +x "$WORKDIR/hello.sh"
mkdir -p "$WORKDIR/dir1/dir2" "$WORKDIR/subdir"
printf 'inside-subdir\n' > "$WORKDIR/subdir/file.txt"
printf 'space file content\n' > "$WORKDIR/file with space.txt"
printf 'qqq\n' > "$WORKDIR/q.txt"
printf 'zzz\n' > "$WORKDIR/z.txt"

printf 'A\n' > "$WORKDIR/bonus_a.txt"
printf 'B\n' > "$WORKDIR/bonus_b.txt"
printf 'LOG\n' > "$WORKDIR/bonus.log"
printf 'int main(void){return 0;}\n' > "$WORKDIR/star1.c"
printf 'int x;\n' > "$WORKDIR/star2.c"

CURRENT_SECTION="main"
run_test "pwd" "pwd"
run_test "echo simple" "echo hello"
run_test "echo avec espaces multiples" "echo        salut"
run_test "echo quotes simples" "echo 'abc def'"
run_test "echo quotes doubles" "echo \"abc def\""
run_test "echo concat simple" "echo bon\"jour\""
run_test "echo variable simple" "export TESTVAR=abc" "echo \$TESTVAR"
run_test "echo variable collée" "export TESTVAR=abc" "echo x\$TESTVAR"
run_test "echo variable dans doubles quotes" "export TESTVAR=abc def" "echo \"\$TESTVAR\""
run_test "echo variable vide après unset" "export TESTVAR=abc" "unset TESTVAR" "echo x\$TESTVAR y"
run_test "cd valide puis pwd" "cd ." "pwd"
run_test "cd dans un sous-dossier puis retour parent" "cd dir1/dir2" "cd .." "pwd"
run_test "cd absolu puis pwd" "cd $WORKDIR/subdir" "pwd"
run_test "cd relatif puis fichier" "cd subdir" "cat file.txt"
run_test "script exécutable relatif" "./hello.sh"
run_test "commande absolue /bin/echo" "/bin/echo absolute"
run_test "status après true" "true" "echo \$?"
run_test "status après false" "false" "echo \$?"
run_test "status pipeline dernière commande false" "echo ok | false" "echo \$?"
run_test "status pipeline dernière commande true" "false | true" "echo \$?"
run_test "export puis env externe" "export TESTVAR=abc" "/usr/bin/env | grep '^TESTVAR=abc$'"
run_test "unset puis variable absente" "export TESTVAR=abc" "unset TESTVAR" "/usr/bin/env | grep '^TESTVAR='" "echo \$?"
run_test "redir output puis lecture" "echo hello > out" "cat out"
run_test "append output puis lecture" "echo a > out" "echo b >> out" "cat out"
run_test "overwrite output" "echo first > out" "echo second > out" "cat out"
run_test "redir input simple" "cat < infile"
run_test "redir input puis output" "cat < infile > out" "cat out"
run_test "commande avec input et output" "grep beta < infile2 > out" "cat out"
run_test "redir output fichier avec espace" "echo wow > \"file with space.txt\"" "cat \"file with space.txt\""
run_test "pipe simple" "echo hello | cat"
run_test "pipe vers wc" "echo hello | wc -c"
run_test "double pipe" "cat infile2 | grep beta | wc -l"
run_test "triple pipe" "cat infile3 | grep -v two | wc -l"
run_test "pipe puis redirection" "cat infile2 | grep gamma > out" "cat out"
run_test "builtin dans pipeline" "echo bonjour | cat | wc -c"
run_test "grep dans pipeline avec status" "cat infile2 | grep gamma" "echo \$?"
run_test "fichier avec espace en argument" "cat \"file with space.txt\""
run_test "quotes vides" "echo \"\" \"\" a"
run_test "commande inexistante puis status" "command_that_does_not_exist" "echo \$?"

cat >> "$REPORT_FILE" <<'EOF2'
<div class="card">
<h2>Tests bonus automatiques</h2>
<p class="note-purple"><strong>Important :</strong> ici les bonus sont exécutés automatiquement et comptés séparément.</p>
</div>
EOF2

CURRENT_SECTION="bonus"
run_test "bonus && simple" "echo ok && echo suite"
run_test "bonus && avec échec" "false && echo no" "echo \$?"
run_test "bonus || simple" "false || echo fallback"
run_test "bonus || après succès" "true || echo no" "echo \$?"
run_test "bonus parenthèses simple" "(echo inside)"
run_test "bonus parenthèses avec pipe" "(echo A && echo B) | cat"
run_test "bonus parenthèses cd isolé" "pwd" "(cd subdir && pwd)" "pwd"
run_test "bonus wildcard txt" "echo *.txt"
run_test "bonus wildcard prefix txt" "echo bonus_*.txt"
run_test "bonus wildcard c" "echo *.c"
run_test "bonus export VAR and print" "export A=b && echo \$A"
run_test "bonus wildcard sans match" "echo *.xyz"
run_test "bonus combo" "(false || echo recovered) && echo done"
run_test "bonus multiple combo" "(ls && echo a) && (echo b || echo c) && pwd"

add_manual_tests_section

sed -i \
    -e "s/__MAIN_TOTAL__/$MAIN_TOTAL/g" \
    -e "s/__MAIN_PASS__/$MAIN_PASS/g" \
    -e "s/__MAIN_FAIL__/$MAIN_FAIL/g" \
    -e "s/__BONUS_TOTAL__/$BONUS_TOTAL/g" \
    -e "s/__BONUS_PASS__/$BONUS_PASS/g" \
    -e "s/__BONUS_FAIL__/$BONUS_FAIL/g" \
    -e "s/__MANUAL__/$MANUAL/g" \
    -e "s/__VG_MEM_OK__/$VG_MEM_OK/g" \
    -e "s/__VG_MEM_KO__/$VG_MEM_KO/g" \
    -e "s/__VG_FD_OK__/$VG_FD_OK/g" \
    -e "s/__VG_FD_KO__/$VG_FD_KO/g" \
    -e "s/__VG_ALL_OK__/$VG_ALL_OK/g" \
    "$REPORT_FILE"

cat >> "$REPORT_FILE" <<EOF2
</div>
</body>
</html>
EOF2

echo "Rapport créé : $REPORT_FILE"
echo "Obligatoires : OK=$MAIN_PASS | KO=$MAIN_FAIL | TOTAL=$MAIN_TOTAL"
echo "Bonus auto  : OK=$BONUS_PASS | KO=$BONUS_FAIL | TOTAL=$BONUS_TOTAL"
echo "Manuel      : $MANUAL"
echo "Valgrind    : MEM OK=$VG_MEM_OK KO=$VG_MEM_KO | FD OK=$VG_FD_OK KO=$VG_FD_KO | Tout OK=$VG_ALL_OK"

if [[ "$MAIN_FAIL" -ne 0 || "$BONUS_FAIL" -ne 0 || "$VG_MEM_KO" -ne 0 || "$VG_FD_KO" -ne 0 ]]; then
    exit 1
fi
