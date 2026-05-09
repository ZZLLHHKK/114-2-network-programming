
# work3 with DB

## Introduction

1. Design a user system with database, detail will show in Spec chapter.
2. Your program must contain at least following features:
   - Including login/logout mechanisms
   - Mail mechanism so that user can receive message offline
   - Group mechanism so that user can chat with members in the same group (like Line group)

## Spec

### 1. Working Directory

```bash
working_dir
    |
    |-- bin/          # put command
    |-- include/      # put your .h file
    |-- object/       # put your object file
    |-- src/          # put your source code
    |-- Makefile      # script for compile
    |-- your_service  # this is executable
```

> **Important**: The following commands **MUST BE** implemented by external files with database (MySQL or Redis).  
> You can use any programming language (C/C++, PHP, Python, etc.) for the commands in the `bin/` folder.  
> Example: `login`, `logout`, `listMail`, `mailto`, etc.

### 2. Database

- **MySQL** or **Redis**
- Redis Client: [**hiredis**](https://github.com/redis/hiredis)
- MySQL Manager: [HeidiSQL](https://www.heidisql.com)

### 3. Login/Logout Mechanisms

**Login Format:**

```
Login: <user name>
Password: <password>
```

#### Accept Condition
After successful login, reload the screen and show the command line prompt:

```
(user_name)% 
```

> Leave **one space** after the `%` sign.

#### Error Condition 1 (Password Error)
If username exists but password is wrong:

```
Password error !
```

Then return to login interface.

#### Error Condition 2 (User Not Found)

```
User not found !
Create account or login again ? <1/2> : 
```

**Option 1**: Add new user account  
**Option 2**: Back to login page

**Register Flow:**

```
your user name: 
your password: 
```

- If username already exists → Show `User name already exist !` and prompt again.
- After successful registration → Show `Create success !`

---

### 4. Mail Box Mechanism

Each user has a mailbox with the same name as their username.

**Required Commands** (in `bin/`):

- `listMail`
- `mailto`
- `delMail`

#### `listMail`
List all mails in your mailbox.

**Example Output:**

(Refer to image)

If no mail: **`empty !`**

#### `mailto`

**Format:**
```bash
mailto <user_name> message
```

Or redirect output:
```bash
mailto <user_name> < ls
```

**Success Message:**
(Refer to image)

If user not found:
```
User not found !
```

#### `delMail`

**Format:**
```bash
delMail <mail_id>
```

**Success:**
(Refer to image)

If mail ID doesn't exist:
```
Mail id unexist !
```

---

### 5. Group Mechanism

- A user can belong to multiple groups.
- Only members in the same group can chat (like Line group chat).
- Users can chat in multiple groups concurrently.

**Required Commands** (in `bin/`):

- `gyell`
- `createGroup`
- `delGroup`
- `addTo`
- `leaveGroup`
- `remove`
- `listGroup`

#### `gyell`
Broadcast message to all members in a specific group.

**Format:**
```bash
gyell <group_name> <message>
```

**Message Format:**
```
<group_name:user_name>: <message>
```

If group doesn't exist:
```
Group not found !
```

#### `createGroup`

**Format:**
```bash
createGroup <group_name>
```

- If group already exists → `Group already exist !`
- Success → `Create group success !` (or similar)

#### `delGroup`

**Format:**
```bash
delGroup <group_name>
```

> **Only the group owner can delete the group.**

Success: `Group delete success !`

If group not found: `Group not found !`

#### `addTo`

**Format:**
```bash
addTo <group_name> <user_name1> <user_name2> ...
```

> **Only the group owner can invite users.**

- Success: `<user_name1> <user_name2> ... add success !`
- User not found: `<user_name1> <user_name2> ... not found !`
- Already in group: `<user_name1> <user_name2> ... already in group !`
- No permission: `You don't have permission !`
- Group not found: `Group not found !`

#### `leaveGroup`

**Format:**
```bash
leaveGroup <group_name>
```

Success: (Refer to image)  
Fail: `Leave fault !`  
Group not found: `Group not found !`

#### `listGroup`

**Format:**
```bash
listGroup
```

Shows all groups the user belongs to.  
If none: `Empty !`

#### `remove`

**Format:**
```bash
remove <group_name> <user_name> <user_name> ...
```

> **Only the group owner can remove users.**

- Success: `<user_name1> <user_name2> ... remove success !`
- Not in group: `<user_name1>, <user_name2>, ... is not in group.`
- User not found: `<user_name1> <user_name2> ... not found !`
- Group not found: `Group not found !`

---

## Submission

Same as previous work.
