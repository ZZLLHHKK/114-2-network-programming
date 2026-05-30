---
title: HomeWork1
tags: [2023 network programming]

---

# HomeWork1 Simple Shell


## Introduction

This project is to implement a shell program in linux with special piping mechanisms.

## Scenario of using shell

### A. Project directory structure

- The structure of the working directory
```c=
working_dir(project dir)
    |
    |-- bin/         // put command
    |-- include/     // put your .h file
    |-- object/      // put your object file
    |-- src/         // put your source code
    |-- Makefile     // script for compile
    |-- `your shell` // this is executable 
```

### B. Scenario


#### 1. Generel command
![](https://i.imgur.com/xh20xsg.png)

#### 2. Command with pipe
![](https://i.imgur.com/1UWJVqA.png)

![](https://i.imgur.com/b1v6wp5.png)

#### 3. Command with some number pipes

![](https://i.imgur.com/XkWEF8S.png)

![](https://i.imgur.com/Xw1gLnL.png)


## Requirements and Hints

### A.

In this project, the commands **number** are supposed by TA. Please download it from here, and put it  into the folder `working_dir`/bin/ .

:::warning
Please don't execute number directly !!
:::

### B.

You can copy some command files from /bin/ directory to your project bin directory, `working_dir`/bin/ if you need them.
> for example:
> **ls**, **cat**, **cp**, etc. 

### C.


When your project demo for TA, he may be copy additional commands to your project bin directory. Your shell program should be able to execute them.


### D.

:::warning
You can only implement the shell with C, other third-party libraries and **system()** command are <font color="#FF0000">**NOT allowed**</font>.
:::

## Specification

### A. Input

1. The length of a single-line input will not exceed 5000 characters.
2. Each command will not exceed 256 characters.
3. There must be one or more spaces between commands and symbols (or arguments), but **no spaces between pipe and numbers.**

:::warning

`cat hello.txt | 4` is bad command because a space after pipe is not allowed.
but `cat hello.txt |4` is correct command

:::

### B. Shell Behavior

1. Use **"%"** as the command line prompt. Notice that there is one space character after it.
2. `quit` is an internal command and then the shell terminates.
 
:::warning
3. Notice that you must handle the forked processes properly, or there might be zombie processes.
:::

### C. setenv and printenv

:::success
`setenv` and `printenv` are two internal commands not files in bin directory.

`PATH` is a environment string variable and initial is set to `bin:.` that mean `bin` directory and `.` directory. 
:::

1. The initial environment variable PATH should be set to **bin/** and **./** by default.
```
% printenv PATH
     bin:.
```
2. setenv usage: `setenv [variable name] [value to assign]`
3. printenv usage: `printenv [variable name]`
```
    % printenv QQ
        # Show nothing if the variable does not exist.
    % printenv LANG
        en_US.UTF-8
```

### D. Numbered-Pipes and Ordinary Pipe

1. **|N** means the STDOUT of the left hand side command should be piped to the first command of the next N-th line, where 1 ≤ N ≤ 128.

:::warning
2. **|N** will only appear at the end of the line.
:::

3. **|** is an ordinary pipe, it means the STDOUT of the left hand side command will be piped to the right hand side command. It will only appear between two commands, not at the beginning or at the end of the line.

4. The command number **:bangbang:<font color="#FF0000">should ignore unknown commands.</font>**
```
    % ls |2
    % ctt
    Unknown command: [ctt].
    % ls
    bin/
    test.html
    % number
        1 bin/
        2 test.html
```

![](https://i.imgur.com/yepXaIA.png)

![](https://i.imgur.com/YpGxNBx.png)


5. setenv and printenv count as one command.
```
    % ls |2
    % printenv PATH
    bin:.
    % cat
        bin
        test.html
```
6. Empty line does not count.
```
    % ls |1
    % #press Enter
    % number
        1 bin/
        2 test.html
```

![](https://i.imgur.com/L0kh0km.png)


### E. Unknown Command

1. If there is an unknown command, print error message as the following format: **Unknown command: [command].**
```
    % ctt
    Unknown command: [ctt].
```
![](https://i.imgur.com/aaevJPh.png)

     
2. You don't have to print out the arguments.
```
    % ctt -n
    Unknown command: [ctt].
```
![](https://i.imgur.com/gq2rVkh.png)

3. The commands after unknown commands will still be executed.
```
     % ctt | ls
     Unknown command: [ctt].
     bin/
     test.html
```
![](https://i.imgur.com/dTBFw6K.png)

4. Messages piped to unknown commands will disappear.
```
     % ls | ctt
     Unknown command: [ctt].
```
![](https://i.imgur.com/4UGsBYZ.png)


### F. Submission

1. Create a directory named as your student ID, put your project zip into the directory.

**Format:** <your student ID>_work1.zip

:::warning
2. You **MUST use GNU Make to build your project** and compile your source code into one executable named shell. The executable and Makefile should be placed at the top layer of the directory. We will use this executable for demo. **You are NOT allowed to demo if we are unable to compile your project with a single make command.**
:::

3. Suggest to upload your source codes into github or gitee.
    
4. zip the directory and upload the .zip file to the E learning platform.
    
## Number source code
    
```cpp=
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

int main(int argc, char* const argv[]) {
  ifstream file;
  if (argc == 2) {
    file.open(argv[1]);
    cin.rdbuf(file.rdbuf());
  } else if (argc > 2) {
    cerr << "usage: " << argv[0] << " [filename]" << endl;
  }

  string line;
  int lineno = 0;
  while (getline(cin, line)) {
    cout << setw(4) << setfill(' ') << ++lineno << ' ' << line << endl;
  }
  return 0;
}
```