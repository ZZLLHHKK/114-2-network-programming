# 114-2 network programming 網路程式設計

課程作業練習，使用 C 語言實作，逐步從單機 shell 擴充為具備聊天室與資料庫功能的 client-server 系統。

## HW1 - Simple Shell

在 Linux 上以 C 實作一個簡易 shell，支援：

- 一般指令執行（透過 `PATH` 尋找 `bin/` 與 `./` 下的執行檔）
- 一般管線 `|` 與 numbered pipe `|N`（將輸出接到下 N 行指令）
- 內建指令 `setenv` / `printenv`（環境變數操作）與 `quit`
- 未知指令錯誤訊息 `Unknown command: [cmd].`

原始碼在 [HW1/src](HW1/src)，`make` 後產生可執行檔 `shell`。

## HW2 - Shell + Chat Room（Concurrent Server）

延伸 HW1 的 shell，加上多人連線的 concurrent connection-oriented server，可用 `telnet` 連線並互動聊天。

新增內建指令（皆以外部執行檔實作於 `bin/`）：

- `who`：列出所有在線使用者
- `tell`：傳送私訊給指定使用者
- `yell`：廣播訊息給所有使用者
- `name`：修改自己的暱稱

原始碼在 [HW2/src](HW2/src) 與 [HW2/bin](HW2/bin)，同樣以 `make` 建置。

## HW3 - Shell + Chat Room + Database（帳號/信箱/群組）

在 HW2 基礎上加入資料庫（MySQL），實作使用者系統：

- 登入 / 註冊 / 登出機制（`login`）
- 離線訊息信箱（`listMail`、`mailto`、`delMail`）
- 群組聊天（`createGroup`、`delGroup`、`addTo`、`leaveGroup`、`listGroup`、`remove`、`gyell`）

原始碼在 [HW3/src](HW3/src) 與 [HW3/bin](HW3/bin)。使用前需將 [HW3/include/db_config.h.example](HW3/include/db_config.h.example) 複製為 `db_config.h` 並填入資料庫連線資訊。

## 建置方式

各 HW 目錄皆附獨立 `Makefile`，於該目錄下執行：

```bash
make
```

即可編譯出對應的可執行檔。
