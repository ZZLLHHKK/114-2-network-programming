---
title: HomeWork2
tags: [2023 network programming]

---

# HomeWork2 shell of hw1 with Chat Room function

## Introduction

Base on shell program of HW1, this project is to  
1. design a **Concurrent connection-oriented** server allowing client to connect, and
2. design a server of the **chat-like systems** that users can communicate with other users.

:::warning
**:bangbang: Your server must support all functions in project 1.**
:::


## Scenario One

**You can use ```telnet``` with dedicated port to connect your shell server (final program of this homework).**

Example:
![](https://i.imgur.com/aDxnD95.png)


## Scenario Two

You are asked to design the following features in your server.
1. New built-in commands:

>* **who**: show information of all users.
>* **tell**: send a message to another user.
>* **yell**: send a message to all users.
>* **name**: change your name.

2. All commands in project 1.

**More details will defined in Spec chapter.**

## Spec


### 1. Working Directory

```c=
working_dir
    |
    |-- bin/         // put command
    |-- include/     // put your .h file
    |-- object/      // put your object file
    |-- src/         // put your source code
    |-- Makefile     // script for compile
    |-- `your service` // this is executable 
```

:::success
All new commands **MUST BE** implemented with external execution files stored in your `bin` directory.

For example:
`who` command is implemented as a file stored in your `bin` directory.
:::

### 2. Format of the New Commands

#### `who`

**Show information of all users.**

Example:
![](https://i.imgur.com/1krBUC7.png)

:::warning
**:bangbang: Your server should always assign the smallest unused id to a new user.**
:::

#### `tell`

**Send the message to the specific user.**

:::warning
**Format:** `tell` `user_id` `message`
:::   

The user will get the message with following format:

:::warning
<user(`id`) told you>: `message`
:::

After send message, if success please send back accept message.

:::warning
send accept!
:::

Example:
![](https://i.imgur.com/kdkFzbK.png)

![](https://i.imgur.com/aaL2GQk.png)


#### `yell`

**Broadcast the message.**

All the users(including yourself) will get the message with the following format:

:::warning
<user(`id`) yelled>: `message`
:::

Example:

user0
![](https://i.imgur.com/SRJUfdo.png)

user1
![](https://i.imgur.com/TRFhrZJ.png)


#### `name`

**Change your name by this command.**

:::warning
**Format :** name 'new name'
:::

After change user name, if success please send back accept message.

:::warning
name change accept!
:::

Example:
![](https://i.imgur.com/gyIwfvy.png)

![](https://i.imgur.com/XxY6ogh.png)


Notice that the name CAN NOT be the same as other users’ name, or you will get the following message:

:::warning
User `new name` already exists !
:::

Example:

![](https://i.imgur.com/g0vYNcp.png)

![](https://i.imgur.com/sFKpaec.png)

## Submission

**Same as the work1.**
