#if defined _INC_PP_MENU
    #endinput
#endif
#define _INC_PP_MENU

#include <pp-hooks>

/**
 * # Header
 */

#if !defined MAX_MENU_TITLE_LENGTH
    #define MAX_MENU_TITLE_LENGTH \
        (32)
#endif

#if !defined MAX_MENU_ITEM_LENGTH
    #define MAX_MENU_ITEM_LENGTH \
        (32)
#endif

#if !defined MAX_MENU_ITEMS
    #define MAX_MENU_ITEMS \
        (64)
#endif

#if !defined MENU_SOUND_UPDOWN
    #define MENU_SOUND_UPDOWN \
        (1054)
#endif

#if !defined MENU_SOUND_SELECT
    #define MENU_SOUND_SELECT \
        (1083)
#endif

#if !defined MENU_SOUND_CLOSE
    #define MENU_SOUND_CLOSE \
        (1084)
#endif

#if !defined MENU_SOUND_ERROR
    #define MENU_SOUND_ERROR \
        (1085)
#endif

const static
    MAX_MENU_PAGE_ROWS = 8
;

enum _:E_ASYNC_MENU_DATA {
    E_ASYNC_MENU_RESPONSE,
    E_ASYNC_MENU_LISTITEM
};

static enum {
    MENU_RESPONSE_UP,
    MENU_RESPONSE_DOWN
};

static enum _:E_MENU_DATA {
    E_MENU_L_COLUMN[MAX_MENU_ITEM_LENGTH],
    E_MENU_R_COLUMN[MAX_MENU_ITEM_LENGTH]
};

static
    List:gMenuList[MAX_PLAYERS],
    Task:gMenuTask[MAX_PLAYERS],
    gMenuItemSize[MAX_PLAYERS],
    gMenuRowCount[MAX_PLAYERS],
    gMenuListItem[MAX_PLAYERS],
    gMenuCooldown[MAX_PLAYERS],
    bool:gMenuCancelSelectSound[MAX_PLAYERS]
;

static
    PlayerText:gMenuTextID[MAX_PLAYERS][2] = { { INVALID_PLAYER_TEXT_DRAW, ... }, ... },
    PlayerText:gMenuItemColumn1TextID[MAX_PLAYERS][MAX_MENU_PAGE_ROWS] = { { INVALID_PLAYER_TEXT_DRAW, ... }, ... },
    PlayerText:gMenuItemColumn2TextID[MAX_PLAYERS][MAX_MENU_PAGE_ROWS] = { { INVALID_PLAYER_TEXT_DRAW, ... }, ... }
;

/**
 * # Functions
 */

forward bool:AddListMenuItem(playerid, column, const item[]);
forward Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false);
forward bool:HideListMenu(playerid);

/**
 * # Internal
 */

static void:CreateMenuTextDrawInternal(playerid, const title[], Float:x, Float:y, Float:width) {
    new const
        Float:BORDER_SPACING_X = 10.0,
        Float:BORDER_SPACING_Y = 20.0,
        Float:LETTER_SPACING_Y = 16.0
    ;

    new const
        Float:X = BORDER_SPACING_X + width,
        Float:H = clamp((gMenuItemSize[playerid] + 1), 0, MAX_MENU_PAGE_ROWS) * LETTER_SPACING_Y + BORDER_SPACING_Y * 2.0
    ;

    gMenuTextID[playerid][0] = CreatePlayerTextDraw(playerid, x, y, "LD_DUAL:BLACK");
    PlayerTextDrawTextSize(playerid, gMenuTextID[playerid][0], width, H);
    PlayerTextDrawColour(playerid, gMenuTextID[playerid][0], 220);
    PlayerTextDrawFont(playerid, gMenuTextID[playerid][0], TEXT_DRAW_FONT_SPRITE_DRAW);
    PlayerTextDrawShow(playerid, gMenuTextID[playerid][0]);

    x += BORDER_SPACING_X;

    gMenuTextID[playerid][1] = CreatePlayerTextDraw(playerid, x, y - BORDER_SPACING_Y, title);
    PlayerTextDrawLetterSize(playerid, gMenuTextID[playerid][1], 1.0, 3.0);
    PlayerTextDrawSetOutline(playerid, gMenuTextID[playerid][1], 2);
    PlayerTextDrawFont(playerid, gMenuTextID[playerid][1], TEXT_DRAW_FONT_0);
    PlayerTextDrawSetProportional(playerid, gMenuTextID[playerid][1], true);
    PlayerTextDrawShow(playerid, gMenuTextID[playerid][1]);

    // `1.8` -> LetterSize
    y += BORDER_SPACING_Y - 1.8 / 2.0;

    #define LETTER_COLOUR(%0) \
        ((%0) ? 0x555E70FF : 0xA3C2EBFF)

    new
        PlayerText:pt
    ;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        pt = gMenuItemColumn1TextID[playerid][i] = CreatePlayerTextDraw(playerid, x, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, 0.460, 1.800);
        PlayerTextDrawColour(playerid, pt, LETTER_COLOUR(i));
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);

        pt = gMenuItemColumn2TextID[playerid][i] = CreatePlayerTextDraw(playerid, X, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, 0.460, 1.800);
        PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawColour(playerid, pt, LETTER_COLOUR(i));
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);
    }

    #undef LETTER_COLOUR
}

static void:ResetMenuDataInternal(playerid) {
    if (list_valid(gMenuList[playerid])) {
        list_delete(gMenuList[playerid]);
    }

    gMenuTask[playerid] = INVALID_TASK;
    gMenuList[playerid] = INVALID_LIST;
}

static void:DestroyMenuTextDrawInternal(playerid) {
    for (new i = sizeof (gMenuTextID[]); --i >= 0;) {
        PlayerTextDrawDestroy(playerid, gMenuTextID[playerid][i]);
        gMenuTextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        PlayerTextDrawDestroy(playerid, gMenuItemColumn1TextID[playerid][i]);
        gMenuItemColumn1TextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;

        PlayerTextDrawDestroy(playerid, gMenuItemColumn2TextID[playerid][i]);
        gMenuItemColumn2TextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    ResetMenuDataInternal(playerid);
}

static void:UpdateMenuListRowColourInternal(playerid, response, row) {
    new const
        lastRow = (response == MENU_RESPONSE_UP) ? (row + 1) : (row - 1)
    ;

    #define CUR_LETTER_COLOUR (0xA3C2EBFF)
    #define REM_LETTER_COLOUR (0x555E70FF)

    // Column 1

    PlayerTextDrawColour(playerid, gMenuItemColumn1TextID[playerid][lastRow], REM_LETTER_COLOUR);
    PlayerTextDrawShow(playerid, gMenuItemColumn1TextID[playerid][lastRow]);

    PlayerTextDrawColour(playerid, gMenuItemColumn1TextID[playerid][row], CUR_LETTER_COLOUR);
    PlayerTextDrawShow(playerid, gMenuItemColumn1TextID[playerid][row]);

    // Column 2

    PlayerTextDrawColour(playerid, gMenuItemColumn2TextID[playerid][lastRow], REM_LETTER_COLOUR);
    PlayerTextDrawShow(playerid, gMenuItemColumn2TextID[playerid][lastRow]);

    PlayerTextDrawColour(playerid, gMenuItemColumn2TextID[playerid][row], CUR_LETTER_COLOUR);
    PlayerTextDrawShow(playerid, gMenuItemColumn2TextID[playerid][row]);

    #undef CUR_LETTER_COLOUR
    #undef REM_LETTER_COLOUR
}

static void:UpdateMenuListRowsInternal(playerid, start = 0) {
    for (new Iter:iter = list_iter(gMenuList[playerid], start), data[E_MENU_DATA], row; iter_inside(iter) && row != MAX_MENU_PAGE_ROWS; iter_move_next(iter), ++row) {
        iter_get_arr(iter, data);

        PlayerTextDrawSetString(playerid, gMenuItemColumn1TextID[playerid][row], data[E_MENU_L_COLUMN]);
        PlayerTextDrawShow(playerid, gMenuItemColumn1TextID[playerid][row]);

        PlayerTextDrawSetString(playerid, gMenuItemColumn2TextID[playerid][row], data[E_MENU_R_COLUMN]);
        PlayerTextDrawShow(playerid, gMenuItemColumn2TextID[playerid][row]);
    }
}

/**
 * # External
 */

stock bool:AddListMenuItem(playerid, column, const item[]) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!(0 <= column <= 1)) {
        printf("(AddItemToMenu) -> The `column` parameter is not within the range of `0` to `1`.");

        return false;
    }

    if (gMenuItemSize[playerid] >= MAX_MENU_ITEMS) {
        printf("(AddItemToMenu) -> Cannot add more items: menu has reached the maximum number of items.");

        return false;
    }

    if (!list_valid(gMenuList[playerid])) {
        gMenuList[playerid] = list_new();
    }

    new
        data[E_MENU_DATA]
    ;

    if (column == 1) {
        new const
            last = (list_size(gMenuList[playerid]) - 1)
        ;

        if (last == -1) {
            printf("(AddItemToMenu) -> Cannot add information to column `1` without an existing item in column `0`.");

            return false;
        }

        list_get_arr(gMenuList[playerid], last, data);
        strcat(data[E_MENU_R_COLUMN], item);
        list_set_arr(gMenuList[playerid], last, data);

        return true;
    }

    strcat(data[E_MENU_L_COLUMN], item);
    list_add_arr(gMenuList[playerid], data);

    return true;
}

stock Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_TASK;
    }

    if (task_valid(gMenuTask[playerid])) {
        task_delete(gMenuTask[playerid]);
    }

    new const
        Task:t = task_new()
    ;
    
    gMenuTask[playerid] = t;
    gMenuRowCount[playerid] = 0;
    gMenuListItem[playerid] = 0;
    gMenuItemSize[playerid] = (list_size(gMenuList[playerid]) - 1);
    gMenuCancelSelectSound[playerid] = cancelSelectSound;

    CreateMenuTextDrawInternal(playerid, title, x, y, width);
    UpdateMenuListRowsInternal(playerid);

    return t;
}

stock bool:HideListMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gMenuList[playerid] == INVALID_LIST) {
        return false;
    }

    DestroyMenuTextDrawInternal(playerid);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (gMenuList[playerid] != INVALID_LIST) {
        if ((newkeys & KEY_SPRINT) && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            new const
                Task:t = gMenuTask[playerid]
            ;

            gMenuTask[playerid] = INVALID_TASK;
            gMenuCooldown[playerid] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);

            if (!gMenuCancelSelectSound[playerid]) {
                PlayerPlaySound(playerid, MENU_SOUND_SELECT, 0.0, 0.0, 0.0);
            }
    
            new
                responses[E_ASYNC_MENU_DATA]
            ;

            responses[E_ASYNC_MENU_RESPONSE] = true;
            responses[E_ASYNC_MENU_LISTITEM] = gMenuListItem[playerid];
            task_set_result_arr(t, responses);

            return 0;
        }

        if ((newkeys & KEY_SECONDARY_ATTACK) && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            new const
                Task:t = gMenuTask[playerid]
            ;

            gMenuTask[playerid] = INVALID_TASK;
            gMenuCooldown[playerid] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);
            PlayerPlaySound(playerid, MENU_SOUND_CLOSE, 0.0, 0.0, 0.0);

            new
                responses[E_ASYNC_MENU_DATA]
            ;

            responses[E_ASYNC_MENU_RESPONSE] = false;
            responses[E_ASYNC_MENU_LISTITEM] = gMenuListItem[playerid];
            task_set_result_arr(t, responses);
        }
    }

    return 0;
}

hook OnPlayerUpdate(playerid) {
    if (gMenuList[playerid] != INVALID_LIST) {
        static
            updown, unused
        ;

        GetPlayerKeys(playerid, KEY:unused, updown, unused);

        if (updown == KEY_UP && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            if (gMenuListItem[playerid] == 0) {
                PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

                return 0;
            }

            gMenuListItem[playerid]--;

            if (gMenuRowCount[playerid] != 0) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_UP, --gMenuRowCount[playerid]);
            } else {
                UpdateMenuListRowsInternal(playerid, gMenuListItem[playerid]);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);

            return 0;
        }

        if (updown == KEY_DOWN && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            if (gMenuListItem[playerid] == gMenuItemSize[playerid]) {
                PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

                return 0;
            }

            gMenuListItem[playerid]++;

            if ((gMenuRowCount[playerid] + 1) < MAX_MENU_PAGE_ROWS) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_DOWN, ++gMenuRowCount[playerid]);
            } else {
                UpdateMenuListRowsInternal(playerid, gMenuListItem[playerid] - MAX_MENU_PAGE_ROWS + 1);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);
        }
    }

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    for (new i = sizeof (gMenuTextID[]); --i >= 0;) {
        gMenuTextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        gMenuItemColumn1TextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
        gMenuItemColumn2TextID[playerid][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    ResetMenuDataInternal(playerid);

    return 0;
}
