/*
    Copyright (C) 2024 P. Last

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include <pud.h>
#include <set>
#include <unordered_map>
#include "Resources.h"
#include "PlayerColor.h"
#include "UpgradeEffect.h"

/*
 * A production item can be produced or researched by a
 * building, e.g., a Footman or the Mage's Blizzard spell
 */
#include "Global/Fractions.h"



class ProductionItem {
public:
    enum class ItemType {
        Invalid,

        // Units
        LANDUNITS_BEGIN,
        Peasant,
        Footman,
        Archer,
        Knight,
        Ballista,
        Mage,
        Dwarfes,
        LANDUNITS_END,

        AIRUNITS_BEGIN,
        GnomishFlyingMaschine,
        GryphonRider,
        AIRUNITS_END,

        SEAUNITS_BEGIN,
        Tanker,
        Transport,
        Destroyer,
        Battleship,
        GnomishSubmarine,
        SEAUNITS_END,

        // Orcs units?

        // ------------------------------------------------------------------------------------------------------------
        // !!! Do not change order, see method 'isResearchItem' or TechnologyCoordinator::researchFinished !!! 
        // These are upgrades for units
        // ------------------------------------------------------------------------------------------------------------
        
        UPGRADES_BEGIN, // See TechnologyManager::researchFinished

        // LumberMill upgrades
        LumberMill_Arrows_1,       // +1 dmg
        LumberMill_Arrows_2,       // +1 dmg
        LumberMill_Ranger,         // +10 health
        LumberMill_RangerScouting, // +4 sight
        LumberMill_Longbow,        // +1 range
        // Human specific
        LumberMill_Marksmanship,   // +3 dmg
        // Troll specific
        LumberMill_TrollRegeneration, // Slowly healing over time

        // Church
        Church_Paladin,
        // Altar of Storms (Church)
        AltarOfStorms_OgreMage,

        // Blacksmith upgrades (https://wowpedia.fandom.com/wiki/Blacksmith_(WC2_Human))
        Blacksmith_Weapons_1_Human,  // Increases the piercing attack damage of Footmen, Knights, Paladins, and Dwarven Demolition Squads by +2                  800 200s  .
        Blacksmith_Weapons_2_Human,  // Further increases the piercing attack damage of Footmen, Knights, Paladins, and Dwarven Demolition Squads by +2    2400  250s
        // Yes, human and orc weapon upgrades are differing in prices
        Blacksmith_Weapons_1_Orc,  // +2 dmg  500,100
        Blacksmith_Weapons_2_Orc,  // +2 dmg   1500,300
        Blacksmith_Armor_1,    // Increases the armor of Footmen, Knights and Paladins by +2    300g, 300w   200s
        Blacksmith_Armor_2,    // Further increases the armor of Footmen, Knights and Paladins by +2  900,500         250s
        Blacksmith_Ballista_1, // Increases the piercing attack damage of Ballistae by +15.   1500      250s
        Blacksmith_Ballista_2, // Further increases the piercing attack damage of Ballistae by +15.   4000      250s

        // Foundry upgrades (completely identical from price to research time to effects)
        Foundry_Ship_Cannon_1, // +5 dmg
        Foundry_Ship_Cannon_2, // +5 dmg
        Foundry_Ship_Armor_1,  // +5 armor
        Foundry_Ship_Armor_2,  // +5 armor


        // Townhall upgrades
        Keep,
        Castle,

        // Scout Tower upgrades
        GuardTower,
        CannonTower,

        UPGRADES_END, // See TechnologyManager::researchFinished


        // ------------------------------------------------------------------------------------------------------------
        // No upgrades, just spells
        // ------------------------------------------------------------------------------------------------------------
        // Mage
        Mage_FlameShield,
        Mage_Slow,
        Mage_Invisibility,
        Mage_Polymorph,
        Mage_Blizzard,
        // Temple of Doom        
        DeathKnight_Haste,
        DeathKnight_RaiseTheDead,
        DeathKnight_Whirlwind,
        DeathKnight_UnholyArmor,
        DeathKnight_DeathAndDecay,


        // Church
        Church_Healing,
        Church_Exorcism,
        // Altar of Storms (Church)
        AltarOfStorms_Bloodlust,
        AltarOfStorms_Runes
    };

 


    // PUD_ICON_BURN is a placeholder icon for a spell that has never been used in the game. Hence, we use it to mark unset icons
    ProductionItem(ItemType t = ItemType::Invalid, const Resources& costs = Resources(), int researchTime_ms = 0, const UpgradeEffect& f = UpgradeEffect(), Pud_Icon iconID_human = PUD_ICON_BURN, Pud_Icon iconID_orc = PUD_ICON_BURN) : m_itemType(t), m_iconID_human(iconID_human), m_iconID_orc(iconID_orc), m_costs(costs), m_researchTime_ms(researchTime_ms), m_f(f) {

    }

    inline bool isResearchItem() const {
        return (m_itemType >= ItemType::LumberMill_Arrows_1);
    }

    inline ItemType getItemType() const {
        return m_itemType;
    }

    bool isAvailable(PlayerColor col) const{
        return (m_setNotAvailableForPlayer.find(col) == m_setNotAvailableForPlayer.end());
    }

    int researchTime() const {
        return m_researchTime_ms;
    }

    // class Producable?
    const Resources& costs() const {
        return m_costs;
    }

    inline const UpgradeEffect& getUpgradeEffect() const {
        return m_f;
    }

    inline Pud_Icon getPUDIconID(Fraction f = Fraction::Alliance) const {
        // If there is just the alliance icon available, return it!
        if (m_iconID_orc == PUD_ICON_BURN)
            return m_iconID_human;

        return f == Fraction::Alliance ? m_iconID_human : m_iconID_orc;
    }

private:
    std::set<PlayerColor> m_setNotAvailableForPlayer;

    ItemType m_itemType;
    Pud_Icon m_iconID_human, m_iconID_orc;
    Resources m_costs;
    int m_researchTime_ms;

    UpgradeEffect m_f;
};


/*
 * Yep, it's basically the same.
 *
 * Anything that can be researched is also an Upgrade which
 * can be installed in units
 */
using Upgrade = ProductionItem;

// EventID: move, stop, ....  ProduceItem
extern std::unordered_map<ProductionItem::ItemType, ProductionItem> gMapProductionItems;
