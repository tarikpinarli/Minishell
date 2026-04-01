# 🐚 Minishell

A simplified Unix shell written in C as part of the [42/Hive](https://www.hive.fi/) curriculum. Minishell replicates core functionality of **bash**, including command parsing, pipes, redirections, heredocs, environment variable expansion, and signal handling.

---

## ✨ Features

| Feature | Description |
|---|---|
| **Interactive Prompt** | Colored prompt powered by GNU Readline with history support |
| **Builtins** | `echo`, `cd`, `pwd`, `export`, `unset`, `env`, `exit` |
| **Pipes** | Chain commands with `\|` (e.g. `ls -la \| grep .c \| wc -l`) |
| **Redirections** | Input `<`, output `>`, append `>>`, heredoc `<<` |
| **Heredoc** | Multi-line input with delimiter, with optional variable expansion |
| **Variable Expansion** | `$VAR`, `$?` (last exit status), inside double quotes |
| **Quoting** | Single quotes (literal), double quotes (expand variables) |
| **Signals** | `Ctrl+C` (new prompt), `Ctrl+D` (exit), `Ctrl+\` (ignored in interactive mode) |
| **PATH Resolution** | Searches `$PATH` for executables, supports absolute/relative paths |

---

## 📁 Project Structure

```
Minishell/
├── main.c                  # Entry point — REPL loop
├── minishell.h             # Header with all types and prototypes
├── Makefile                # Build system (native + Docker targets)
├── Dockerfile              # Container build definition
├── docker-compose.yml      # Docker Compose service config
├── libft/                  # Custom C standard library (42 libft)
├── parser/                 # Tokenizer, parser, variable expansion, token merging
├── executor/               # Command execution, pipes, redirections, PATH lookup
├── builtins/               # Builtin command implementations
├── heredoc/                # Heredoc processing and expansion
└── utils/                  # Signal handling, memory management, env utilities
```

---

## 🛠️ Build & Run

### Prerequisites (Native)

- **C compiler** (gcc or clang)
- **GNU Make**
- **libreadline-dev** and **libncurses-dev**

#### Install dependencies

**Debian/Ubuntu:**
```bash
sudo apt-get install build-essential libreadline-dev libncurses-dev
```

**macOS (Homebrew):**
```bash
brew install readline
```

**macOS (MacPorts):**
```bash
sudo port install readline
```

### Native Build

```bash
# Standard build (Linux)
make

# macOS build (uses Homebrew/MacPorts paths)
make mac

# Run
./bin/minishell
```

### 🐳 Docker Build

No local dependencies needed — everything is containerized.

```bash
# Build the Docker image
make docker-build

# Run minishell interactively in a container
make docker-run

# Clean up (remove container + image)
make docker-down
```

Or using Docker Compose directly:

```bash
docker compose build
docker compose run --rm minishell
```

---

## 🚀 Usage

```
minishell$ echo "Hello, World!"
Hello, World!

minishell$ ls -la | grep .c | wc -l
1

minishell$ export NAME=Minishell
minishell$ echo "I am $NAME"
I am Minishell

minishell$ cat << EOF
> line one
> line two
> EOF
line one
line two

minishell$ echo $?
0

minishell$ exit
```

---

## 🧹 Clean Up

```bash
# Remove object files
make clean

# Remove object files + binary
make fclean

# Full rebuild
make re
```

---

## 🔍 Debug Builds

```bash
# Build with -g flag (for valgrind/lldb)
make valgrind

# Build with address/leak sanitizer
make fsanitize
```

---

## 📝 Authors

- **tpinarli** — [tpinarli@student.hive.fi](mailto:tpinarli@student.hive.fi)

---

## 📄 License

This project was developed as part of the 42/Hive curriculum.
