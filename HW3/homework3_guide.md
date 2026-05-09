# Login & Other External Commands (with Database)

## 1. login

### login.c
```c
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void finish(MYSQL *con) {
    printf("%d\n", -1);
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("2\n");
        return 1;
    }

    MYSQL *con = mysql_init(NULL);
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

    if (con == NULL) finish(con);

    if (mysql_real_connect(con, "134.208.6.50", "np2023", "@np2023@", "np2023", 0, NULL, 0) == NULL)
        finish(con);

    char sql[200];
    sprintf(sql, "SELECT * FROM user WHERE name='%s'", argv[1]);

    if (mysql_query(con, sql)) finish(con);

    MYSQL_RES *result = mysql_store_result(con);
    if (result == NULL) finish(con);

    int res;
    int num_rows = mysql_num_rows(result);

    if (num_rows > 0) {
        MYSQL_ROW row = mysql_fetch_row(result);
        if (strcmp(row[2], argv[2]) == 0)
            res = 0;        // success
        else
            res = 1;        // password error
    } else {
        res = 2;            // user not found
    }

    mysql_free_result(result);
    mysql_close(con);

    printf("%d\n", res);
    exit(0);
}
```

### login.py
```python
import sys
import pymysql

if len(sys.argv) != 3:
    print(2)
    sys.exit(1)

user = sys.argv[1]
passwd = sys.argv[2]

try:
    dbconn = pymysql.connect(
        host='134.208.6.50',
        user='np2023',
        password='@np2023@',
        database='np2023'
    )
    cur = dbconn.cursor()
    sql = "SELECT * FROM user WHERE name=%s"
    cur.execute(sql, (user,))
    res = cur.fetchone()

    if res:
        if res[2] == passwd:
            print(0)
        else:
            print(1)
    else:
        print(2)

    cur.close()
    dbconn.close()
except:
    print(2)
```

### login.php
```php
<?php
$host = "134.208.6.50";
$user = "np2023";
$password = "@np2023@";
$db_name = "np2023";

$conn = mysqli_connect($host, $user, $password, $db_name);
if (!$conn) {
    echo 2;
    exit;
}
mysqli_query($conn, "SET NAMES UTF8");

$user = $argv[1] ?? '';
$passwd = $argv[2] ?? '';

$sql = sprintf("SELECT * FROM user WHERE name='%s'", mysqli_real_escape_string($conn, $user));
$res = mysqli_query($conn, $sql);

if (mysqli_num_rows($res) > 0) {
    $row = mysqli_fetch_assoc($res);
    if ($row['password'] == $passwd) {
        echo 0;
    } else {
        echo 1;
    }
} else {
    echo 2;
}

mysqli_close($conn);
?>
```

---

**使用範例：**
```bash
$ ./login 大雄 zaq1xsw2
$ python3 login.py 大雄 zaq1xsw2
$ php login.php 大雄 zaq1xsw2
```
