# Using

Include in your code and begin using the library:
```pwn
#include <pp-menu>
```

### Responses

* `MENU_RESPONSE_UP`
* `MENU_RESPONSE_DOWN`
* `MENU_RESPONSE_SELECT`
* `MENU_RESPONSE_CLOSE`

### Functions

* **`bool:`AddItemToMenu**`(playerid, column, const format[], OPEN_MP_TAGS:...)`
* **`bool:`ShowMenuCallback**`(playerid, Func:cb<ii>, const format[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false, OPEN_MP_TAGS:...)`
* **`bool:`HideCurrentMenu**`(playerid)`

# Example

```pwn
public OnPlayerCommandText(playerid, cmdtext[]) {
    if (strequal(cmdtext, "/menu")) {
        new const
            ITEM_DATA[][][] =
        {
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
            AddItemToMenu(playerid, 0, ITEM_DATA[i][0]);
            AddItemToMenu(playerid, 1, ITEM_DATA[i][1]);
        }

        inline const OnResponse(response, listitem) {
            SendClientMessage(playerid, -1, "Response: %i, Listitem: %i", response, listitem);
        }

        ShowMenuCallback(playerid, using inline OnResponse, "Menu", 20.0, 120.0, 200.0);

        return 1;
    }

    return 0;
}
```
<img width="1919" height="1071" alt="image" src="https://github.com/user-attachments/assets/9cc892c9-1b97-4429-8ae6-fc206bb7f8e7" />

