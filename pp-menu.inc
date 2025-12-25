#if defined _INC_PP_MENU
    #endinput
#endif
#define _INC_PP_MENU

#include <PawnPlus>

#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_va>
#include <YSI_Coding\y_inline>

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

#define MENU_CB_NONE (Func:0<ii>)

const static
    MAX_MENU_PAGE_ROWS = 8
;

enum {
    MENU_RESPONSE_UP,
    MENU_RESPONSE_DOWN,
    MENU_RESPONSE_SELECT,
    MENU_RESPONSE_CLOSE
};

static enum _:E_MENU_DATA {
    E_MENU_L_COLUMN[MAX_MENU_ITEM_LENGTH char],
    E_MENU_R_COLUMN[MAX_MENU_ITEM_LENGTH char]
};

static
    List:gMenuItem[MAX_PLAYERS],
    Func:gMenuCallback[MAX_PLAYERS]<ii>,
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

forward bool:AddItemToMenu(playerid, column, const format[], OPEN_MP_TAGS:...);
forward bool:ShowMenuCallback(playerid, Func:cb<ii>, const format[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false, OPEN_MP_TAGS:...);
forward bool:HideCurrentMenu(playerid);

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
        Float:H = clamp(gMenuItemSize[playerid] + 1, 0, MAX_MENU_PAGE_ROWS) * LETTER_SPACING_Y + BORDER_SPACING_Y * 2.0
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

    // `1.800` -> LetterSize
    y += BORDER_SPACING_Y - 1.800 / 2.0;

    #define LETTER_COLOUR(%0) \
        ((%0) ? 0x555E70FF : 0xA3C2EBFF)

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        gMenuItemColumn1TextID[playerid][i] = CreatePlayerTextDraw(playerid, x, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, gMenuItemColumn1TextID[playerid][i], 0.460, 1.800);
        PlayerTextDrawColour(playerid, gMenuItemColumn1TextID[playerid][i], LETTER_COLOUR(i));
        PlayerTextDrawSetShadow(playerid, gMenuItemColumn1TextID[playerid][i], 2);
        PlayerTextDrawSetProportional(playerid, gMenuItemColumn1TextID[playerid][i], true);

        gMenuItemColumn2TextID[playerid][i] = CreatePlayerTextDraw(playerid, X, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, gMenuItemColumn2TextID[playerid][i], 0.460, 1.800);
        PlayerTextDrawAlignment(playerid, gMenuItemColumn2TextID[playerid][i], TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawColour(playerid, gMenuItemColumn2TextID[playerid][i], LETTER_COLOUR(i));
        PlayerTextDrawSetShadow(playerid, gMenuItemColumn2TextID[playerid][i], 2);
        PlayerTextDrawSetProportional(playerid, gMenuItemColumn2TextID[playerid][i], true);
    }

    #undef LETTER_COLOUR
}

static void:ResetMenuDataInternal(playerid) {
    if (list_valid(gMenuItem[playerid])) {
        list_delete(gMenuItem[playerid]);
    }

    if (gMenuCallback[playerid] != MENU_CB_NONE) {
        Indirect_Release(gMenuCallback[playerid]);
    }

    gMenuItem[playerid] = INVALID_LIST;
    gMenuCallback[playerid] = MENU_CB_NONE;
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
    new
        Iter:it = list_iter(gMenuItem[playerid], start),
        row,
        data[E_MENU_DATA],
        buffer[MAX_MENU_ITEM_LENGTH]
    ;

    for (; iter_inside(it) && row != MAX_MENU_PAGE_ROWS; iter_move_next(it), ++row) {
        iter_get_arr(it, data);

        strunpack(buffer, data[E_MENU_L_COLUMN]);
        PlayerTextDrawSetString(playerid, gMenuItemColumn1TextID[playerid][row], buffer);
        PlayerTextDrawShow(playerid, gMenuItemColumn1TextID[playerid][row]);

        strunpack(buffer, data[E_MENU_R_COLUMN]);
        PlayerTextDrawSetString(playerid, gMenuItemColumn2TextID[playerid][row], buffer);
        PlayerTextDrawShow(playerid, gMenuItemColumn2TextID[playerid][row]);
    }
}

/**
 * # External
 */

stock bool:AddItemToMenu(playerid, column, const format[], OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (!(0 <= column <= 1)) {
        Debug_Fatal("(AddItemToMenu) -> The `column` parameter is not within the range of `0` to `1`.");

        return false;
    }

    if (gMenuItemSize[playerid] >= MAX_MENU_ITEMS) {
        Debug_Warning("(AddItemToMenu) -> Cannot add more items: menu has reached the maximum number of items.");

        return false;
    }

    if (!list_valid(gMenuItem[playerid])) {
        gMenuItem[playerid] = list_new();
    }

    new
        data[E_MENU_DATA],
        buffer[MAX_MENU_ITEM_LENGTH]
    ;

    Format(
        buffer,
        _,
        format,
        ___(3)
    );

    if (column == 1) {
        new const
            last = (list_size(gMenuItem[playerid]) - 1)
        ;

        if (last == -1) {
            Debug_Fatal("(AddItemToMenu) -> Cannot add information to column `1` without an existing item in column `0`.");

            return false;
        }

        strpack(data[E_MENU_R_COLUMN], buffer);
        list_set_cell(gMenuItem[playerid], last, E_MENU_R_COLUMN, buffer);
    } else {
        strpack(data[E_MENU_L_COLUMN], buffer);
        list_add_arr(gMenuItem[playerid], data);
    }

    return true;
}

stock bool:ShowMenuCallback(playerid, Func:cb<ii>, const format[], Float:x, Float:y, Float:width, bool:cancelSelectSound = false, OPEN_MP_TAGS:...) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gMenuCallback[playerid] != MENU_CB_NONE) {
        return false;
    }

    Indirect_Claim(cb);
    
    gMenuCallback[playerid] = cb;
    gMenuRowCount[playerid] = 0;
    gMenuListItem[playerid] = 0;
    gMenuItemSize[playerid] = (list_size(gMenuItem[playerid]) - 1);
    gMenuCancelSelectSound[playerid] = cancelSelectSound;

    new
        buffer[MAX_MENU_TITLE_LENGTH]
    ;

    Format(
        buffer,
        _,
        format,
        ___(7)
    );

    CreateMenuTextDrawInternal(playerid, buffer, x, y, width);
    UpdateMenuListRowsInternal(playerid);

    return true;
}

stock bool:HideCurrentMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gMenuCallback[playerid] == MENU_CB_NONE) {
        return false;
    }

    DestroyMenuTextDrawInternal(playerid);

    return true;
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (gMenuCallback[playerid] != MENU_CB_NONE) {
        new const
            Func:cb<ii> = gMenuCallback[playerid]
        ;

        if ((newkeys & KEY_SECONDARY_ATTACK) && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);

            PlayerPlaySound(playerid, MENU_SOUND_CLOSE, 0.0, 0.0, 0.0);
            @.cb(MENU_RESPONSE_CLOSE, gMenuListItem[playerid]);
        }
        else if ((newkeys & KEY_SPRINT) && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);
            
            if (!gMenuCancelSelectSound[playerid]) {
                PlayerPlaySound(playerid, MENU_SOUND_SELECT, 0.0, 0.0, 0.0);
            }

            @.cb(MENU_RESPONSE_SELECT, gMenuListItem[playerid]);
        }

        Indirect_Release(cb);
    }

    return 1;
}

hook OnPlayerUpdate(playerid) {
    if (gMenuCallback[playerid] != MENU_CB_NONE) {
        static
            updown, unused
        ;

        new const
            Func:cb<ii> = gMenuCallback[playerid]
        ;

        GetPlayerKeys(playerid, KEY:unused, updown, unused);

        if (updown == KEY_UP && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            if (gMenuListItem[playerid] == 0) {
                return PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);
            }

            gMenuListItem[playerid]--;

            if (gMenuRowCount[playerid] != 0) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_UP, --gMenuRowCount[playerid]);
            } else {
                UpdateMenuListRowsInternal(playerid, gMenuListItem[playerid]);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);
            @.cb(MENU_RESPONSE_UP, gMenuListItem[playerid]);
        }
        else if (updown == KEY_DOWN && (GetTickCount() - gMenuCooldown[playerid]) >= 200) {
            gMenuCooldown[playerid] = GetTickCount();

            if (gMenuListItem[playerid] == gMenuItemSize[playerid]) {
                return PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);
            }

            gMenuListItem[playerid]++;

            if ((gMenuRowCount[playerid] + 1) < MAX_MENU_PAGE_ROWS) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_DOWN, ++gMenuRowCount[playerid]);
            } else {
                UpdateMenuListRowsInternal(playerid, gMenuListItem[playerid] - MAX_MENU_PAGE_ROWS + 1);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);
            @.cb(MENU_RESPONSE_DOWN, gMenuListItem[playerid]);
        }
    }

    return 1;
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

    return 1;
}
