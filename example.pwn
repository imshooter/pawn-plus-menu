#include <open.mp>
#include <humanize>

#define PP_SYNTAX_AWAIT
#define PP_SYNTAX_YIELD

#include <pp-menu>

#define MAX_AMMU_CLASSES (8)
#define MAX_AMMU_WEAPONS_PER_CLASS (4)

static const gAmmunationClassNames[MAX_AMMU_CLASSES][MAX_MENU_ITEM_LENGTH] = {
    "Pistols",      // 0
    "Micro SMGs",   // 1
    "Shotguns",     // 2
    "Thrown",       // 3
    "SMG",          // 4
    "Rifles",       // 5
    "Assault",      // 6
    "Melee"         // 7
};

static enum E_AMMU_WEAPON_DATA {
    WEAPON:E_AMMU_WEAPON_ID,
    E_AMMU_WEAPON_COST,
    E_AMMU_WEAPON_AMMO
};

static const gAmmunationData[MAX_AMMU_CLASSES][MAX_AMMU_WEAPONS_PER_CLASS][E_AMMU_WEAPON_DATA] = {
    // Class 0: Pistols
    {
        {WEAPON_COLT45,    200,  30},   // Pistol (9mm) - $200, 30 rounds
        {WEAPON_SILENCED,  600,  30},   // Silenced pistol - $600, 30 rounds
        {WEAPON_DEAGLE,   1200,  15},   // Desert Eagle - $1,200, 15 rounds
        {WEAPON_FIST,        0,   0}
    },
    // Class 1: Micro SMGs
    {
        {WEAPON_TEC9,      300,  60},   // Tec-9 - $300, 60 rounds
        {WEAPON_UZI,       500,  60},   // Uzi / Micro SMG - $500, 60 rounds
        {WEAPON_FIST,        0,   0},
        {WEAPON_FIST,        0,   0}
    },
    // Class 2: Shotguns
    {
        {WEAPON_SHOTGUN,   600,  15},   // Pump Shotgun - $600, 15 shells
        {WEAPON_SAWEDOFF,  800,  12},   // Sawn-off - $800, 12 shells
        {WEAPON_SHOTGSPA, 1000,  10},   // SPAS / Combat Shotgun - $1,000, 10 shells
        {WEAPON_FIST,        0,   0}
    },
    // Class 3: Thrown
    {
        {WEAPON_GRENADE,   300,   1},   // Grenade - $300, 1 unit
        {WEAPON_SATCHEL,  2000,   1},   // Satchel charge - $2,000, 1 unit
        {WEAPON_MOLOTOV,   500,   1},   // Molotov - $500, 1 unit
        {WEAPON_FIST,        0,   0}
    },
    // Class 4: SMG
    {
        {WEAPON_MP5,      2000,  30},   // MP5/SMG - $2,000, 30 rounds
        {WEAPON_FIST,        0,   0},
        {WEAPON_FIST,        0,   0},
        {WEAPON_FIST,        0,   0}
    },
    // Class 5: Rifles
    {
        {WEAPON_RIFLE,    1000,   5},   // Country Rifle - $1,000, 5 rounds
        {WEAPON_SNIPER,   5000,   5},   // Sniper Rifle - $5,000, 5 rounds
        {WEAPON_FIST,        0,   0},
        {WEAPON_FIST,        0,   0}
    },
    // Class 6: Assault
    {
        {WEAPON_AK47,     3500,  30},   // AK-47 - $3,500, 30 rounds
        {WEAPON_M4,       4500, 150},   // M4 - $4,500, 150 rounds
        {WEAPON_FIST,        0,   0},
        {WEAPON_FIST,        0,   0}
    },
    // Class 7: Melee
    {
        {WEAPON_KNIFE,      10,   1},   // Knife - $10, 1
        {WEAPON_BAT,        10,   1},   // Baseball bat - $10, 1
        {WEAPON_KATANA,     50,   1},   // Katana - $50, 1
        {WEAPON_CHAINSAW,  300,   1}    // Chainsaw - $300, 1
    }
};

native bool:SendClientMessageStr(playerid, colour, ConstAmxString:str) = SendClientMessage;

main(){}

ShowAmmunationClassesMenu(playerid) {
    yield 1;

    for (new i, size = sizeof (gAmmunationClassNames); i != size; ++i) {
        AddListMenuItem(playerid, 0, gAmmunationClassNames[i]);
    }

    new const
        Task:t = ShowAsyncListMenu(playerid, "Ammunation", 20.0, 120.0, 250.0)
    ;

    if (t) {
        new
            responses[E_ASYNC_MENU_DATA]
        ;

        await_arr(responses) t;

        if (responses[E_ASYNC_MENU_RESPONSE]) {
            ShowAmmunationWeaponsMenu(playerid, responses[E_ASYNC_MENU_LISTITEM]);
        }
    }
}

ShowAmmunationWeaponsMenu(playerid, classid) {
    yield 1;

    new
        name[MAX_MENU_ITEM_LENGTH],
        cost[MAX_MENU_ITEM_LENGTH]
    ;

    for (new i, size = sizeof (gAmmunationData[]); i != size && gAmmunationData[classid][i][E_AMMU_WEAPON_ID] != WEAPON_FIST; ++i) {
        GetWeaponName(gAmmunationData[classid][i][E_AMMU_WEAPON_ID], name);
        HumanizeThousand(gAmmunationData[classid][i][E_AMMU_WEAPON_COST], cost, .delimiter = ".");

        strins(cost, "$", 0);

        AddListMenuItem(playerid, 0, name);
        AddListMenuItem(playerid, 1, cost);
    }

    new const
        Task:t = ShowAsyncListMenu(playerid, "Ammunation", 20.0, 120.0, 250.0)
    ;

    if (t) {
        new
            responses[E_ASYNC_MENU_DATA]
        ;

        await_arr(responses) t;

        if (responses[E_ASYNC_MENU_RESPONSE]) {
            ShowAmmunationFinalizePurchaseMenu(playerid, classid, responses[E_ASYNC_MENU_LISTITEM]);
        } else {
            ShowAmmunationClassesMenu(playerid);
        }
    }
}

ShowAmmunationFinalizePurchaseMenu(playerid, classid, listItem) {
    yield 1;

    new const
        WEAPON:weaponid = gAmmunationData[classid][listItem][E_AMMU_WEAPON_ID],
        weaponCost = gAmmunationData[classid][listItem][E_AMMU_WEAPON_COST],
        weaponAmmo = gAmmunationData[classid][listItem][E_AMMU_WEAPON_AMMO]
    ;

    new
        name[MAX_MENU_ITEM_LENGTH],
        cost[MAX_MENU_ITEM_LENGTH]
    ;

    GetWeaponName(weaponid, name);
    HumanizeThousand(weaponCost, cost, .delimiter = ".");

    strins(cost, "$", 0);

    AddListMenuItem(playerid, 0, name);
    AddListMenuItem(playerid, 1, cost);

    new const
        Task:t = ShowAsyncListMenu(playerid, "Ammunation", 20.0, 120.0, 250.0)
    ;

    if (t) {
        new
            responses[E_ASYNC_MENU_DATA]
        ;

        await_arr(responses) t;

        if (responses[E_ASYNC_MENU_RESPONSE]) {
            if (GetPlayerMoney(playerid) >= weaponCost) {
                GivePlayerWeapon(playerid, weaponid, weaponAmmo);
                GivePlayerMoney(playerid, 0 - weaponCost);
                SendClientMessage(playerid, -1, "You have purchased the weapon '%s'.", name);
            } else {
                SendClientMessage(playerid, -1, "You don't have enough money.");
            }
        } else {
            ShowAmmunationWeaponsMenu(playerid, classid);
        }
    }
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if (strequal(cmdtext, "/money")) {
        GivePlayerMoney(playerid, 100000);

        return 1;
    }

    if (strequal(cmdtext, "/ammu")) {
        ShowAmmunationClassesMenu(playerid);

        return 1;
    }

    return 0;
}
