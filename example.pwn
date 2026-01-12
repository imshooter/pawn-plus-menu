#include <open.mp>

#define PP_SYNTAX_AWAIT
#define PP_SYNTAX_YIELD

#include <pp-menu>

main(){}

ShowAsyncMenu(playerid) {
    yield 1;

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
        AddListMenuItem(playerid, 0, ITEM_DATA[i][0]);
        AddListMenuItem(playerid, 1, ITEM_DATA[i][1]);
    }

    new responses[E_ASYNC_MENU_DATA];

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
