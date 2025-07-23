# Ext2 Filesystem Explorer

This is a command-line tool written in C to explore a disk image formatted with the **Ext2 (Second Extended Filesystem)**. It allows users to **navigate**, **list**, and **view** files inside the image without mounting it on an operating system.

---

## Features

- Read and parse an Ext2 filesystem image
- Navigate into directories using `cd`
- List directory contents with `ls`
- View file contents with `cat`
- Built-in `help`, `clear`, and `exit` commands

---

## Available Commands

| Command                   | Description                                                   |
|---------------------------|---------------------------------------------------------------|
| `help`                    | Show help menu with available commands                        |
| `exit`                    | Exit the explorer                                             |
| `ls`                      | List the contents of the current directory                   |
| `cls` or `clear`          | Clear the terminal screen                                     |
| `cd <directory name>`     | Change to the specified subdirectory                          |
| `cat <file name>`         | Display the contents of the specified file (read-only view)  |

---

## Build

Make sure you have `gcc` installed.

```bash
gcc main.c common.c ls.c cd.c cat.c -o ext2explorer
```

## RUN

```bash
./ext2explorer <filesystem_image.img>
```