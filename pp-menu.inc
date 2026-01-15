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

#if !defined MENU_COOLDOWN_MILISECONDS
    #define MENU_COOLDOWN_MILISECONDS \
        (200)
#endif

enum _:E_ASYNC_MENU_DATA {
    E_ASYNC_MENU_RESPONSE,
    E_ASYNC_MENU_LISTITEM
};

const static
    MAX_MENU_PAGE_ROWS = 8
;

const static
    ITEM_NORMAL_COLOR = 0x555E70FF,
    ITEM_ACTIVE_COLOR = 0xA3C2EBFF
;

static enum E_MENU_RESPONSE_UPDOWN (+= 1) {
    MENU_RESPONSE_UP,
    MENU_RESPONSE_DOWN
};

static enum _:E_MENU_DATA {
    E_MENU_L_COLUMN[MAX_MENU_ITEM_LENGTH],
    E_MENU_R_COLUMN[MAX_MENU_ITEM_LENGTH]
};

static enum E_PLAYER_MENU_DATA {
    List:E_PLAYER_MENU_LIST,
    Task:E_PLAYER_MENU_TASK,
    bool:E_PLAYER_MENU_OPENED,
    E_PLAYER_MENU_SIZE,
    E_PLAYER_MENU_CURRENT_ROW,
    E_PLAYER_MENU_CURRENT_ITEM,
    E_PLAYER_MENU_COOLDOWN_TIME,
    bool:E_PLAYER_MENU_SELECTION_SOUND,
    bool:E_PLAYER_MENU_FREEZE
};

static enum E_PLAYER_TEXT_DATA {
    PlayerText:E_PLAYER_TEXT_TITLE,
    PlayerText:E_PLAYER_TEXT_BACKGROUND,
    PlayerText:E_PLAYER_TEXT_L_COLUMN[MAX_MENU_PAGE_ROWS],
    PlayerText:E_PLAYER_TEXT_R_COLUMN[MAX_MENU_PAGE_ROWS]
};

static
    gPlayerMenuData[MAX_PLAYERS][E_PLAYER_MENU_DATA],
    gPlayerTextData[MAX_PLAYERS][E_PLAYER_TEXT_DATA]
;

/**
 * # Functions
 */

forward bool:AddListMenuItem(playerid, column, const item[]);
forward Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true);
forward bool:DestroyListMenu(playerid);
forward bool:IsListMenuVisible(playerid);

forward bool:AddListMenuItemStr(playerid, column, ConstString:item);
forward Task:ShowAsyncListMenuStr(playerid, ConstString:title, Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true);

/**
 * # Internal
 */

static void:CreateMenuTextDrawInternal(playerid, const title[], Float:x, Float:y, Float:width) {
    new const
        Float:BORDER_SPACING_X = 10.0,
        Float:BORDER_SPACING_Y = 20.0,
        Float:LETTER_SPACING_Y = 16.0,

        rows = min(
            (gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE] + 1),
            MAX_MENU_PAGE_ROWS
        ),

        Float:MENU_RIGHT_X = BORDER_SPACING_X + width,
        Float:MENU_TOTAL_HEIGHT = rows * LETTER_SPACING_Y + BORDER_SPACING_Y * 2.0
    ;

    new
        PlayerText:pt
    ;

    pt = gPlayerTextData[playerid][E_PLAYER_TEXT_TITLE] = CreatePlayerTextDraw(playerid, x, y, "LD_DUAL:BLACK");
    PlayerTextDrawTextSize(playerid, pt, width, MENU_TOTAL_HEIGHT);
    PlayerTextDrawColour(playerid, pt, 220);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_SPRITE);
    PlayerTextDrawShow(playerid, pt);

    x += BORDER_SPACING_X;

    pt = gPlayerTextData[playerid][E_PLAYER_TEXT_BACKGROUND] = CreatePlayerTextDraw(playerid, x, y - BORDER_SPACING_Y, title);
    PlayerTextDrawLetterSize(playerid, pt, 1.0, 3.0);
    PlayerTextDrawSetOutline(playerid, pt, 2);
    PlayerTextDrawFont(playerid, pt, TEXT_DRAW_FONT_BECKETT_REGULAR);
    PlayerTextDrawSetProportional(playerid, pt, true);
    PlayerTextDrawShow(playerid, pt);

    // 1.8 = TextDraw letter height (Y size)
    // 2.0 = Half-height factor, used to center the top-anchored text
    // Offsets the position to visually center the text within the row spacing.
    y += BORDER_SPACING_Y - 1.8 / 2.0;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        pt = gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][i] = CreatePlayerTextDraw(playerid, x, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, 0.460, 1.800);
        PlayerTextDrawColour(playerid, pt, (i) ? ITEM_NORMAL_COLOR : ITEM_ACTIVE_COLOR);
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);

        pt = gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][i] = CreatePlayerTextDraw(playerid, MENU_RIGHT_X, y + LETTER_SPACING_Y * i, "");
        PlayerTextDrawLetterSize(playerid, pt, 0.460, 1.800);
        PlayerTextDrawAlignment(playerid, pt, TEXT_DRAW_ALIGN_RIGHT);
        PlayerTextDrawColour(playerid, pt, (i) ? ITEM_NORMAL_COLOR : ITEM_ACTIVE_COLOR);
        PlayerTextDrawSetShadow(playerid, pt, 2);
        PlayerTextDrawSetProportional(playerid, pt, true);
    }
}

static void:ResetMenuDataInternal(playerid) {
    if (list_valid(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST])) {
        list_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]);
    }

    gPlayerMenuData[playerid][E_PLAYER_MENU_LIST] = INVALID_LIST;
    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
    gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED] = false;
    gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE] = false;
}

static void:DestroyMenuTextDrawInternal(playerid) {
    PlayerTextDrawDestroy(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_BACKGROUND]);
    PlayerTextDrawDestroy(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_TITLE]);

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        PlayerTextDrawDestroy(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][i]);
        PlayerTextDrawDestroy(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][i]);

        gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE]) {
        TogglePlayerControllable(playerid, true);
    }

    ResetMenuDataInternal(playerid);
}

static void:UpdateMenuListRowColourInternal(playerid, E_MENU_RESPONSE_UPDOWN:updown, row) {
    new const
        lastRow = (updown == MENU_RESPONSE_UP) ? (row + 1) : (row - 1)
    ;

    PlayerTextDrawColour(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][lastRow], ITEM_NORMAL_COLOR);
    PlayerTextDrawColour(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][lastRow], ITEM_NORMAL_COLOR);
    PlayerTextDrawColour(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][row], ITEM_ACTIVE_COLOR);
    PlayerTextDrawColour(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][row], ITEM_ACTIVE_COLOR);
    
    PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][lastRow]);
    PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][lastRow]);
    PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][row]);
    PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][row]);
}

static void:UpdateMenuListRowsInternal(playerid, start = 0) {
    for (new Iter:iter = list_iter(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], start), data[E_MENU_DATA], row; iter_inside(iter) && row != MAX_MENU_PAGE_ROWS; iter_move_next(iter), ++row) {
        iter_get_arr(iter, data);

        PlayerTextDrawSetString(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][row], data[E_MENU_L_COLUMN]);
        PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][row]);

        PlayerTextDrawSetString(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][row], data[E_MENU_R_COLUMN]);
        PlayerTextDrawShow(playerid, gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][row]);
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

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE] >= MAX_MENU_ITEMS) {
        printf("(AddItemToMenu) -> Cannot add more items: menu has reached the maximum number of items.");

        return false;
    }

    if (!list_valid(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST])) {
        gPlayerMenuData[playerid][E_PLAYER_MENU_LIST] = list_new();
    }

    new
        data[E_MENU_DATA]
    ;

    if (column == 1) {
        new const
            last = (list_size(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) - 1)
        ;

        if (last == -1) {
            printf("(AddItemToMenu) -> Cannot add information to column `1` without an existing item in column `0`.");

            return false;
        }

        list_get_arr(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], last, data);
        strcat(data[E_MENU_R_COLUMN], item);
        list_set_arr(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], last, data);

        return true;
    }

    strcat(data[E_MENU_L_COLUMN], item);
    list_add_arr(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST], data);

    return true;
}

stock Task:ShowAsyncListMenu(playerid, const title[], Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true) {
    if (!IsPlayerConnected(playerid)) {
        return INVALID_TASK;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        printf("(ShowAsyncListMenu) -> Menu is already being shown.");

        return INVALID_TASK;
    }

    if (task_valid(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK])) {
        task_delete(gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]);
    }

    if (freeze) {
        TogglePlayerControllable(playerid, false);
    }

    new const
        Task:t = task_new()
    ;

    gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]            = t;
    gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]          = true;
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ROW]     = 0;
    gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]    = 0;
    gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE]            = (list_size(gPlayerMenuData[playerid][E_PLAYER_MENU_LIST]) - 1);
    gPlayerMenuData[playerid][E_PLAYER_MENU_SELECTION_SOUND] = selectionSound;
    gPlayerMenuData[playerid][E_PLAYER_MENU_FREEZE]          = freeze;

    CreateMenuTextDrawInternal(playerid, title, x, y, width);
    UpdateMenuListRowsInternal(playerid);

    return t;
}

stock bool:DestroyListMenu(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        return false;
    }

    DestroyMenuTextDrawInternal(playerid);

    return true;
}

stock bool:IsListMenuVisible(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    return gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED];
}

stock bool:AddListMenuItemStr(playerid, column, ConstString:item) {
    new buffer[MAX_MENU_ITEM_LENGTH];
    str_get(item, buffer);
    return AddListMenuItem(playerid, column, buffer);
}

stock Task:ShowAsyncListMenuStr(playerid, ConstString:title, Float:x, Float:y, Float:width, bool:freeze = true, bool:selectionSound = true) {
    new buffer[MAX_MENU_TITLE_LENGTH];
    str_get(title, buffer);
    return ShowAsyncListMenu(playerid, buffer, x, y, width, selectionSound, freeze);
}

/**
 * # Calls
 */

hook OnPlayerKeyStateChange(playerid, KEY:newkeys, KEY:oldkeys) {
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        if ((newkeys & KEY_SPRINT) && (GetTickCount() - gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME]) >= MENU_COOLDOWN_MILISECONDS) {
            new const
                Task:t = gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]
            ;

            gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
            gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);

            if (gPlayerMenuData[playerid][E_PLAYER_MENU_SELECTION_SOUND]) {
                PlayerPlaySound(playerid, MENU_SOUND_SELECT, 0.0, 0.0, 0.0);
            }
    
            new
                responses[E_ASYNC_MENU_DATA]
            ;

            responses[E_ASYNC_MENU_RESPONSE] = true;
            responses[E_ASYNC_MENU_LISTITEM] = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM];
            task_set_result_arr(t, responses);

            return 0;
        }

        if ((newkeys & KEY_SECONDARY_ATTACK) && (GetTickCount() - gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME]) >= MENU_COOLDOWN_MILISECONDS) {
            new const
                Task:t = gPlayerMenuData[playerid][E_PLAYER_MENU_TASK]
            ;

            gPlayerMenuData[playerid][E_PLAYER_MENU_TASK] = INVALID_TASK;
            gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

            DestroyMenuTextDrawInternal(playerid);
            PlayerPlaySound(playerid, MENU_SOUND_CLOSE, 0.0, 0.0, 0.0);

            new
                responses[E_ASYNC_MENU_DATA]
            ;

            responses[E_ASYNC_MENU_RESPONSE] = false;
            responses[E_ASYNC_MENU_LISTITEM] = gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM];
            task_set_result_arr(t, responses);
        }
    }

    return 0;
}

hook OnPlayerUpdate(playerid) {
    if (gPlayerMenuData[playerid][E_PLAYER_MENU_OPENED]) {
        new
            updown, unused
        ;

        GetPlayerKeys(playerid, KEY:unused, updown, unused);

        if (updown == KEY_UP && (GetTickCount() - gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME]) >= MENU_COOLDOWN_MILISECONDS) {
            gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

            if (gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] == 0) {
                PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

                return 0;
            }

            gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]--;

            if (gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ROW] != 0) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_UP, --gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ROW]);
            } else {
                UpdateMenuListRowsInternal(playerid, gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);

            return 0;
        }

        if (updown == KEY_DOWN && (GetTickCount() - gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME]) >= MENU_COOLDOWN_MILISECONDS) {
            gPlayerMenuData[playerid][E_PLAYER_MENU_COOLDOWN_TIME] = GetTickCount();

            if (gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] == gPlayerMenuData[playerid][E_PLAYER_MENU_SIZE]) {
                PlayerPlaySound(playerid, MENU_SOUND_ERROR, 0.0, 0.0, 0.0);

                return 0;
            }

            gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM]++;

            if ((gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ROW] + 1) < MAX_MENU_PAGE_ROWS) {
                UpdateMenuListRowColourInternal(playerid, MENU_RESPONSE_DOWN, ++gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ROW]);
            } else {
                UpdateMenuListRowsInternal(playerid, gPlayerMenuData[playerid][E_PLAYER_MENU_CURRENT_ITEM] - MAX_MENU_PAGE_ROWS + 1);
            }

            PlayerPlaySound(playerid, MENU_SOUND_UPDOWN, 0.0, 0.0, 0.0);
        }
    }

    return 0;
}

hook OnPlayerDisconnect(playerid, reason) {
    gPlayerTextData[playerid][E_PLAYER_TEXT_TITLE] = INVALID_PLAYER_TEXT_DRAW;
    gPlayerTextData[playerid][E_PLAYER_TEXT_BACKGROUND] = INVALID_PLAYER_TEXT_DRAW;

    for (new i; i != MAX_MENU_PAGE_ROWS; ++i) {
        gPlayerTextData[playerid][E_PLAYER_TEXT_L_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
        gPlayerTextData[playerid][E_PLAYER_TEXT_R_COLUMN][i] = INVALID_PLAYER_TEXT_DRAW;
    }

    ResetMenuDataInternal(playerid);

    return 0;
}
