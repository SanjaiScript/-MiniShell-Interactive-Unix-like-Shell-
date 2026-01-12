# MiniShell – Interactive Unix-like Shell

---

## Description
MiniShell (msh) is a minimalistic Unix-like shell implemented in C as part of the Linux Internal module.  
It allows execution of built-in commands, external programs, background jobs, and supports signals, pipes, and job control.  

The project helps in understanding system calls, process management, signal handling, and command parsing.

---

## Features
- Customizable prompt (`PS1`)  
- Built-in commands: `cd`, `pwd`, `exit`  
- Special variables:  
  - `$$` – PID of msh  
  - `$?` – Exit status of last command  
  - `$SHELL` – Shell executable path  
- Background process support (`&`)  
- Job control commands: `jobs`, `fg`, `bg`  
- Signal handling: Ctrl+C (SIGINT), Ctrl+Z (SIGTSTP)  
- Pipe functionality: Multiple processes communication (`ls | wc`, `ls -l /dev | grep tty | wc -l`)  

---

## Installation
1. Clone the repository:

bash
git clone git@github.com:SanjaiScript/-MiniShell-Interactive-Unix-like-Shell-.git

---

## Navigate to the project folder:

cd MiniShell-Interactive-Unix-like-Shell

---

## Compile the C files:

gcc *.c -o msh

---

## Usage
Run the shell:
./msh

---

## Sample commands inside MiniShell:

msh> pwd
msh> echo Hello World!
msh> ls -l | grep ".c"
msh> sleep 10 &
msh> jobs
msh> fg
msh> exit

---

## Change prompt:

PS1=NewPrompt>

## Debug / Sample Output

---

MiniShell> pwd
/home/linux_engineer

MiniShell> ls -l
-rw-r--r-- 1 user user 1024 Jan 12 12:00 main.c

MiniShell> sleep 10 &
DEBUG: Created background process with PID 12345

MiniShell> jobs
[1]+ 12345 Running    sleep 10 &

MiniShell> fg
Bringing PID 12345 to foreground

MiniShell> exit
INFO: Exiting MiniShell. Goodbye!


---

## Author

Sanjai Kumar M
@SanjaiScript


