## 📦 Installation

Simply download the include file and place it in your `qawno/include` folder.

```pawn
#include <pp-menu>
```

## 🛠️ Functions

* **AddListMenuItem**`(playerid, column, const item[])`
* **ShowAsyncListMenu**`(playerid, const title[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false)`
* **HideListMenu**`(playerid)`

## ⚡ Quick Example

```pwn
task_yield(1);

// 1. Add items
AddListMenuItem(playerid, 0, "Item A");
AddListMenuItem(playerid, 1, "Description A");

AddListMenuItem(playerid, 0, "Item B");
AddListMenuItem(playerid, 1, "Description B");

// 2. Show menu and await response
new responses[E_ASYNC_MENU_DATA];
await_arr(responses) ShowAsyncListMenu(playerid, "My Menu", 20.0, 120.0, 200.0);

// 3. Handle result
printf("Response: %d, Item: %d", responses[E_ASYNC_MENU_RESPONSE], responses[E_ASYNC_MENU_LISTITEM]);
```

## 💻 Full Code

```pwn
#include <open.mp>

#define PP_SYNTAX_AWAIT
#define PP_SYNTAX_YIELD

#include <pp-menu>

main(){}

ShowAsyncMenu(playerid) {
    yield 1;

    new const ITEM_DATA[][][] = {
        {"Item: A", "ABCDEFGH"},
        {"Item: AB", "ABCDEFG"},
        {"Item: ABC", "ABCDEF"},
        {"Item: ABCD", "ABCDE"},
        {"Item: ABCDE", "ABCD"},
        {"Item: ABCDEF", "ABC"},
        {"Item: ABCDEFG", "AB"},
        {"Item: ABCDEFGH", "A"},
        {"Item: ABCDEFG", "AB"},
        {"Item: ABCDEF", "ABC"},
        {"Item: ABCDE", "ABCD"},
        {"Item: ABCD", "ABCDE"},
        {"Item: ABC", "ABCDEF"},
        {"Item: AB", "ABCDEFG"},
        {"Item: A", "ABCDEFGH"}
    };

    for (new i, size = sizeof (ITEM_DATA); i != size; ++i) {
        AddListMenuItem(playerid, 0, ITEM_DATA[i][0]);
        AddListMenuItem(playerid, 1, ITEM_DATA[i][1]);
    }

    new
        responses[E_ASYNC_MENU_DATA]
    ;

    await_arr(responses) ShowAsyncListMenu(playerid, "Menu", 20.0, 120.0, 200.0);

    SendClientMessage(playerid, -1, "Response: %i, Listitem: %i", responses[E_ASYNC_MENU_RESPONSE], responses[E_ASYNC_MENU_LISTITEM]);
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if (strequal(cmdtext, "/menu")) {
        ShowAsyncMenu(playerid);

        return 1;
    }

    return 0;
}
```

## 📸 In-Game Look
<img width="1919" height="1071" alt="image" src="https://github.com/user-attachments/assets/9cc892c9-1b97-4429-8ae6-fc206bb7f8e7" />

