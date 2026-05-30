---
title: HW3導引
tags: [2023 network programming]

---


# login external file commands
## login.c

```clike=
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void finish(MYSQL *con) {
    printf("%d\n",-1);
    mysql_close(con);
    exit(1);
}

int main(int argc, char **argv)
{
  MYSQL *con = mysql_init(NULL);
  mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
  mysql_options(con, MYSQL_INIT_COMMAND, "SET NAMES utf8");

  int res;

  if (con==NULL) finish(con); 
 	
  if (mysql_real_connect(con, "134.208.6.50", "np2023", "@np2023@", "np2023",0,NULL,0)==NULL ) finish(con);

  char sql[100];
  
  sprintf(sql, "SELECT * FROM user WHERE name='%s'", argv[1]);
  if (mysql_query(con, sql)) finish(con);

  MYSQL_RES *result = mysql_store_result(con);

  if (result==NULL) finish(con);

  int num_rows = mysql_num_rows(result);
  MYSQL_ROW row;
  if (num_rows>0) {
    row=mysql_fetch_row(result);
    if (strcmp(row[2], argv[2])==0)
      res = 0;
    else
      res = 1;
  } else
    res = 2;
  mysql_free_result(result);
  printf("%d\n",res);
  mysql_close(con);
  exit(0);
}
```

```
$login 大雄 zaq1xsw2
```



---

## login.py
```python=
import sys
import pymysql

n = len(sys.argv)
user = sys.argv[1]
passwd = sys.argv[2]

dbconn = pymysql.connect(host='134.208.6.50', user='np2023', password='@np2023@', database='np2023')
sql = f"select * from user where name='{user}'"
cur = dbconn.cursor()

try:
    cur.execute(sql)
    res = cur.fetchone()
    if(res[2]==passwd):
        print(0)
    else:
        print(1)
except:
    print(2)
cur.close()
dbconn.close()

```

:::success
```
$python3 login.py 大雄 zaq1xsw2
```
:::
---

## login.php
```php=
<?php
$host = "134.208.6.50";
$user = "np2023";
$password = "@np2023@";
$db_name = "np2023";

$conn = mysqli_connect($host,$user,$password,$db_name);
if ($conn)
	mysqli_query($conn, "SET NAMES UTF8");
$user = $argv[1];
$passwd = $argv[2];

$sql = sprintf("select * from user where name='%s'", $user);

$res = mysqli_query($conn, $sql);

if (mysqli_num_rows($res)>0) {
	$row = mysqli_fetch_array($res, MYSQLI_ASSOC);
	if ($row['password'] == $passwd)
	    echo(0);
	else
		echo(1);
} else 
	echo(2);
//$conn.close();
?>

```



```
$php login.php 大雄 zaq1xsw2
```

