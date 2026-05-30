---
title: HomeWork3
tags: [2023 network programming]

---

# work3 with DB

## Introduction

1. Design a user system with database, detail will show in Spec chapter.
2. Your program must contain at least following features.
    - including login/logout mechanisms,
    - mail mechanism so that user receive message offline.
    - group mechanism so that user can chat members in the same group that is like Line group.

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
The following commands **MUST BE** implemented by external files with database (mysql or redis et. al.).

If you implmenet commands with a file, you can use any programming languages, for example, `php` file or `python` or `C/C++`.

For example:
`login`, `logout`, `listMail`, `mailto` ... are commands stored in your `bin`.
:::


### 2. Database

The database we choose **MySQL** or **redis**.

[Link of Redis](https://redis.io)

> **Redis Clients** : we choice [**hiredis**](https://github.com/redis/hiredis)
> **MySQL Manager Program**: we choice [HeidiSQL](https://www.heidisql.com)


### 3. Login/Logout mechanisms

**Login format:**

:::warning
Login: <user name>
Passowrd: <password>
:::
    
![](https://i.imgur.com/O4RDyM8.png)

    
#### 1. To Accept condition
    
Please reload screen and show the command line prompt.
    
:bangbang: leave one space after **"%"** sign.

:::warning
(user_name)% 
:::

![](https://i.imgur.com/hN1iJzh.png)
    
:::success
If user exist & password error, please go to error conditon 1.

If user doesnot exist, go to error condition 2.
:::
   

#### 2. error condition 1

**If user name exist and password error, please show `Password error !` then reload the login interface.**
:::warning
Password error !
:::
    
![](https://i.imgur.com/b7THzCm.png)

![](https://i.imgur.com/Mdmyfu0.png)

   
#### 3. error condition 2
:::warning
User not found !
Create account or login again ? <1/2> : `your input`
:::

1. adding a new user account

![](https://i.imgur.com/bsBH8Ko.png)

2. backing to login page

![](https://i.imgur.com/oqFKkw5.png)

**Register**
    
:::warning
your user name: (input)
your password: (input)
:::

If user name exist, please show a warning message. **User name already exist !**

Then prompt the input message again. **your user name:_** 

![](https://i.imgur.com/KY6QNUC.png)

    
**:bangbang: After register, please show "Create success !".**


    
    
### 4. Mail box mechanism

**A user has a mail account and the mail account is same as user name.** 
**The mail account stores messages from other user sending.**

- The following commands are commands for a mail box that must be implemented and put them into your `bin`.
    
:::success
- listMail
- mailto
- delMail
:::
    
    
#### 1. `listMail`  : List all mails in your mail account.

**Print format: follow below example.**
![](https://i.imgur.com/8Wmotbg.png)

If not mail exist, please show **"empty !"**

![](https://i.imgur.com/SlHqS6H.png)


#### 2. `mailto`

Format:
:::warning
mailto <user_name> message
:::

message is a string.
    
or 

:::warning
mailto <user_name> < ls
:::

`<` this means redirect output from `ls` to <user_name>
    
If success:
![](https://i.imgur.com/yNOIyYT.png)
    
![](https://i.imgur.com/dd5AAkv.png)

If user name not found, please show **"User not found !"**
![](https://i.imgur.com/3RNf6b9.png)

    
#### 3. `delMail`
    
Format:
:::warning
delMail <mail_id>
:::

If success:
![](https://i.imgur.com/YwTHEXU.png)
 
If mail id not found, please show **"Mail id unexist !"**
![](https://i.imgur.com/8aprIHK.png)

### 5. Group mechanism

A user can belong to some groups.
Only members in the same group can chat in the group that is like Line group chat.
    
:::info
- A user can join more than one group.
- A user can chat concurrently in more than one group.
:::

    
- The following commands must be implemented for group mechanism and put them into your 'bin'.

:::success
- gyell
- createGroup
- delGroup
- addTo
- leaveGroup
- remove
:::
    
In this case, you need provide commmand below.
    
#### `gyell`

This command same as `yell`, only different between two command, is `yell` broadcast to every users they are online and `gyell` broadcase to users in the same group.

:::warning
gyell <group_name> <message>
:::

message:
:::success
<group_name:user_name>: <message>
:::

If group unexist, please show **"Group not found !"**
    
**Example:**
    
member in group:
![](https://i.imgur.com/GvOJXfw.png)

message:
![](https://i.imgur.com/0trUD5o.png)
    
![](https://i.imgur.com/9v75ksy.png)
    
![](https://i.imgur.com/QyPZyQH.png)
    
![](https://i.imgur.com/U79AsF8.png)

#### `createGroup`

Create a group.
    
Format:
:::warning
createGroup <group_name>
:::

If group exist. please show **"Group already exist !"**

![](https://i.imgur.com/hXKk5np.png)

If create success.

![](https://i.imgur.com/NocHxbQ.png)

    
#### `delGroup`

Delete a group.

:::danger
**Only the owner can delete group !!**
:::
    
Format:
:::warning
delGroup <group_name>
:::

If success, please show: **"Group delete success !"** 

**Before delete:**
![](https://i.imgur.com/wzrbiNE.png)

![](https://i.imgur.com/bdD0PwW.png)

![](https://i.imgur.com/TI3kNF7.png)

![](https://i.imgur.com/XGEpH2I.png)

![](https://i.imgur.com/XQ9zevV.png)


**After delete:**
![](https://i.imgur.com/tlaxSLe.png)

![](https://i.imgur.com/Vmhaays.png)
    
![](https://i.imgur.com/ZY69xw9.png)
    
![](https://i.imgur.com/brhDbCG.png)
    
If group unexist. please show **"Group not found !"**
 
#### `addTo`

invite user come in group.

:::danger
**Only the owner can invite other user !!**
:::
    
Format:
:::warning
addTo <group_name> <user_name1> <user_name2> ...
:::

If user add success, please show **"<User_name>... add success !"**
:::warning
<user_name1> <user_name2> ... add success !
:::

If user unexist, please show:
:::warning
<user_name1> <user_name2> ... not found !
:::

If user already in group, please show **"<User_name>... already in group !"**

:::warning
<user_name1> <user_name2> ... already in group !
:::

**Example:**
![](https://i.imgur.com/1N2CDDk.png)

![](https://i.imgur.com/8U26jHy.png)

![](https://i.imgur.com/k1TUwBe.png)
    
If other user want access these group they don't have permission, please show **"You don't have permission !"**
    
![](https://i.imgur.com/IiZJHx7.png)

If group unexist, please show **"Group not found !"**
    
#### `leaveGroup`

Leave a group.
    
Format:
:::warning
leaveGroup <group name>
:::

If leave auccess.

![](https://i.imgur.com/pDEiFl1.png)

    
Otherwise. please show **“Leave fault !**

![](https://i.imgur.com/wolgpoL.png)

If group unexist, please show **"Group not found !"**

#### `listGroup`

List groups that you belong to.

Format:
:::warning
listGroup
:::    

![](https://i.imgur.com/yrhuTNX.png)

![](https://i.imgur.com/5HzAUBf.png)

    
If no group you belong to, show **"Empty !"**
    
#### `remove`

Remove user from group.    

:::danger
**Only the group owner can remove a user !!**
:::

Format:
:::warning
remove <group_name> <user_name> <user_name> ...
:::

If user is not in group, please show: **"<User_name>... is not in group !"**
:::warning
<user_name1>, <user_name2>, ... is not in group.
:::

If user remove success, please show **"<User_name>... remove success !"**
:::warning
<user_name1> <user_name2> ... remove success !
:::

If user unexist, please show: **"<User_name>... not found !"**
:::warning
<user_name1> <user_name2> ... not found !
:::

If group unexist, please show **"Group not found !"**

**Example:**

Group member before remove:
![](https://i.imgur.com/2to41zW.png)

After remove:
![](https://i.imgur.com/cWmL2so.png)

![](https://i.imgur.com/9aJmAVV.png)

![](https://i.imgur.com/gh569V4.png)

User is not in group.
![](https://i.imgur.com/nzPluhB.png)

    
## Submission

**Same as work above.**