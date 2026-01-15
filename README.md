## 📜 Description

This menu does not use a traditional page system.

Instead, it behaves similarly to GTA V menus:

> * **The menu displays a fixed number of visible rows (for example, 8 items at a time).**
> * **If more than 8 items are added, the menu automatically scrolls vertically as the selection moves.**
> * **Items beyond the visible limit are positioned “below” the current view and smoothly come into view when navigating up or down.**
> * **The selection cursor remains within the visible area while the list content shifts internally.**

## 📦 Installation

Simply download the include file and place it in your `qawno/include` folder.

```pawn
#include <pp-menu>
```

## 🛠️ Functions

**AddListMenuItem**`(playerid, column, const item[])`
> - Adds an item to the specified menu column.
> - Items are queued internally until the menu is shown.

**ShowAsyncListMenu**`(playerid, const title[], Float:x, Float:y, Float:width, bool:selectionSound = true)`
> - Displays the menu asynchronously and returns a Task.
> - The task resolves when the menu is closed, either by selection or cancel.

**DestroyListMenu**`(playerid)`
> - Immediately destroys the current menu and clears its internal state.

**IsListMenuVisible**`(playerid)`
> - Returns whether the menu is currently visible for the player.

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

## 📸 In-Game Look
<img width="1919" height="1079" alt="image" src="https://github.com/user-attachments/assets/a08a01b4-896c-4be6-8346-8b6e3cc2e613" />
