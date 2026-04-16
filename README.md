# Minishell

> Un shell minimal inspiré de Bash, développé dans le cadre du cursus 42.

## À propos du projet

**Minishell** est une réimplémentation simplifiée d’un interpréteur de commandes Unix.  
Le but du projet est de comprendre concrètement comment fonctionne un shell :
- lecture et parsing de l’entrée utilisateur,
- gestion de l’environnement,
- exécution de commandes,
- redirections et pipes,
- builtins,
- signaux,
- gestion fine de la mémoire et des file descriptors.

---

## Fonctionnalités

### Partie obligatoire

- Affichage d’un prompt interactif
- Historique avec `readline`
- Exécution de commandes à partir du `PATH`
- Exécution avec chemin relatif ou absolu
- Gestion des variables d’environnement
- Builtins :
  - `echo`
  - `cd`
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`
- Redirections :
  - `<`
  - `>`
  - `>>`
  - `<<`
- Pipes `|`
- Gestion des quotes simples et doubles
- Expansion des variables (`$VAR`, `$?`)
- Gestion des signaux principaux (`Ctrl-C`, `Ctrl-D`, `Ctrl-\`)

### Bonus

- Opérateurs logiques `&&` et `||`
- Parenthèses `()`
- Wildcards `*`

---

## Exemple d’utilisation

```bash
$ ./minishell
minishell > echo Hello world
Hello world
minishell > export A=B
minishell > echo $A
B
minishell > echo a | grep a
a
```

---

## Compilation

```bash
make
```

### Règles disponibles

```bash
make
make clean
make fclean
make re
```

---

## Lancement

```bash
./minishell
```

---

## Comportement attendu

Le shell doit se rapprocher du comportement de **Bash** sur les points demandés par le sujet.

### Gestion des signaux

- `Ctrl-C` : interrompt la commande en cours et redonne la main proprement
- `Ctrl-D` : quitte le shell sur prompt vide
- `Ctrl-\` : ignoré au prompt, géré correctement sur un processus enfant si nécessaire

### Exit codes

- `0` si la commande s’est bien passée
- `127` si la commande est introuvable
- `130` après interruption par `Ctrl-C`
- `131` après interruption par `Ctrl-\`

---

## Architecture du projet

Une organisation classique du projet peut ressembler à ceci :

```text
minishell/
├── include/
│   └── minishell.h
├── src/
│   ├── main/
│   ├── lexer/
│   ├── parser/
│   ├── expansion/
│   ├── execution/
│   ├── builtins/
│   ├── signals/
│   ├── redirections/
│   └── utils/
├── libft/
├── Makefile
└── README.md
```

### Étapes principales

#### 1. Lecture de l’entrée
Le shell lit une ligne avec `readline`, affiche un prompt, puis conserve l’historique si la ligne n’est pas vide.

#### 2. Tokenisation
La ligne est découpée en tokens en tenant compte :
- des mots,
- des pipes,
- des redirections,
- des quotes,
- des variables.

#### 3. Parsing
Les tokens sont transformés en structure exploitable pour l’exécution.
Selon l’implémentation, cela peut être :
- une liste chaînée de commandes,
- ou un AST.

#### 4. Expansion
Le shell remplace les variables d’environnement et gère les cas spéciaux comme `$?`.

#### 5. Exécution
Le shell :
- prépare les redirections,
- crée les pipes,
- exécute les builtins dans le bon contexte,
- lance les commandes externes avec `fork` + `execve`.

#### 6. Nettoyage
À chaque cycle, le shell doit libérer proprement la mémoire et fermer les file descriptors inutiles.

---

## Difficultés principales du projet

Ce projet paraît simple de loin, mais il est en réalité exigeant. Les points les plus délicats sont souvent :

- gérer correctement les quotes et les expansions,
- différencier parsing et exécution,
- exécuter les builtins dans le bon process,
- éviter les doubles fermetures ou les fuites de fd,
- gérer proprement les here-documents,
- reproduire un comportement cohérent sur les signaux,
- garder un shell stable même après des erreurs de syntaxe ou des interruptions.

---

### Tests simples

```bash
echo hello
pwd
env
export TEST=42
echo $TEST
unset TEST
echo $TEST
```

### Pipes et redirections

```bash
ls | wc -l
echo bonjour > out.txt
cat < out.txt
cat << LIMITER
hello
LIMITER
```

### Gestion des erreurs

```bash
idontexist
./not_executable
cat < missing_file
```

### Signaux

```bash
cat
# Ctrl-C

sleep 10
# Ctrl-C

cat << LIMITER
hello
# Ctrl-C
```

---

## Gestion mémoire et file descriptors

Un minishell correct doit :

- éviter les **memory leaks**,
- fermer tous les **file descriptors inutiles**,
- rester stable même après plusieurs commandes, erreurs ou interruptions.

Pour cette raison, il est fortement utile de tester le projet avec :

- **Valgrind**
- un tester maison
- des cas limites sur les redirections, pipes, heredocs et signaux

Exemple :

```bash
valgrind --leak-check=full --track-fds=yes ./minishell
```

---

## Ce que ce projet nous a appris

- mieux comprendre le fonctionnement réel d’un shell Unix,
- travailler avec les processus et les signaux,
- manipuler proprement les redirections et les pipes,
- structurer un projet C plus complexe,
- débugger des comportements subtils liés au parsing et à l’exécution,
- écrire du code plus robuste face aux erreurs système.

---

## Ressources utilisées

- man pages : `readline`, `fork`, `execve`, `waitpid`, `dup2`, `pipe`, `signal`
- https://www.gnu.org/software/bash/manual/bash.html
- https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
- https://catonmat.net/bash-one-liners-explained-part-three
- https://brennan.io/2015/01/16/write-a-shell-in-c/
- https://github.com/torvalds/linux/blob/master/fs/dlm/ast.c
- https://apoorvasn.medium.com/minishell-building-a-simple-shell-in-c-55a64a401a4f
- https://www.codequoi.com/envoyer-et-intercepter-un-signal-en-c/
- https://www.baeldung.com/linux/sigint-and-other-termination-signals
- https://www.geeksforgeeks.org/c/signals-c-language/
- https://medium.com/@razika28/signals-ad83f38f80b6
- https://www.youtube.com/watch?v=SwIPOf2YAgI
- https://stackoverflow.com/questions/2485028/signal-handling-in-c
- https://www.geeksforgeeks.org/c/wait-system-call-c/
- https://medium.com/@santiagobedoa/coding-a-shell-using-c-1ea939f10e7e
- https://medium.com/@zouhairlrs/minishell-building-a-mini-bash-a-42-project-5dc20d671fbb
- https://tldp.org/LDP/abs/html/subshells.html
- https://notes.kodekloud.com/docs/Advanced-Bash-Scripting/Expansions-Part-Two/Subshells/page
- https://stratadoc.stratus.com/vos/16.1.0/r537m-00/wwhelp/wwhimpl/common/html/r537m-00.pdf

- L'IA nous a permis dans la comprehension plus profonde de l'AST, dans la structure d'un testeur.

---

## Auteurs

dcresce
alubrano

---

## Remarque

Ce projet ne cherche pas à reproduire Bash à 100 %.  
L’objectif est de recréer un shell cohérent, robuste et fidèle au sujet, en respectant les contraintes du projet.

