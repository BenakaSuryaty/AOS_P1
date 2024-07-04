# Project 1

This project includes directories and files structured as follows:

```
└── Project_1/
    ├── Client_C4/
    │   └── Client.c
    ├── Server_1/
    │   └── Server_p1.c
    ├── Server_2/
    │   └── Server_p2.c
    ├── Server_3/
    │   └── Server_p3.c
    ├── Makefile
    └── file_create.c
```

## Overview

`file_create.c` generates 6 files of 300 bytes each, containing unique data, and distributes them equally across 3 server folders.

## Compilation Instructions

To compile the project:

1. Navigate to the project's root directory:
   ```
   cd Project_1/
   ```
   
2. Run the following command to compile all `.c` files and execute `file_create.c`:
   ```
   make
   ```

This will compile all the `.c` files and execute `file_create.c`, resulting in the creation of the server files as described.
