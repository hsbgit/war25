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
#include "ProductionItem.h"

int factor_s = 10; // May be changed for debug purposes, should be '1000'


std::unordered_map<ProductionItem::ItemType, ProductionItem> gMapProductionItems = {
	// Units
	// Barracks
	{ProductionItem::ItemType::Footman,  ProductionItem(ProductionItem::ItemType::Footman, Resources(600), 60 * factor_s, UpgradeEffect(), PUD_ICON_FOOTMAN, PUD_ICON_GRUNT)},
	{ProductionItem::ItemType::Archer,  ProductionItem(ProductionItem::ItemType::Archer, Resources(500,50), 70 * factor_s, UpgradeEffect(), PUD_ICON_ARCHER, PUD_ICON_AXETHROWER)},
	{ProductionItem::ItemType::Knight,  ProductionItem(ProductionItem::ItemType::Knight, Resources(800,100), 90 * factor_s, UpgradeEffect(), PUD_ICON_KNIGHT, PUD_ICON_OGRE)},
	{ProductionItem::ItemType::Ballista,  ProductionItem(ProductionItem::ItemType::Ballista, Resources(900,300), 250 * factor_s, UpgradeEffect(), PUD_ICON_BALLISTA, PUD_ICON_CATAPULT)},

	// Misc
	{ProductionItem::ItemType::Peasant,  ProductionItem(ProductionItem::ItemType::Peasant, Resources(400), 50 * factor_s, UpgradeEffect(), PUD_ICON_PEASANT, PUD_ICON_PEON)},
	{ProductionItem::ItemType::Mage,  ProductionItem(ProductionItem::ItemType::Mage, Resources(1200), 120 * factor_s, UpgradeEffect(), PUD_ICON_MAGE, PUD_ICON_DEATH_KNIGHT)},
	{ProductionItem::ItemType::Dwarfes,  ProductionItem(ProductionItem::ItemType::Dwarfes, Resources(750,250), 200 * factor_s, UpgradeEffect(), PUD_ICON_DWARVES, PUD_ICON_GOBLIN_SAPPER)},
	{ProductionItem::ItemType::GnomishFlyingMaschine,  ProductionItem(ProductionItem::ItemType::GnomishFlyingMaschine, Resources(500,100), 65 * factor_s, UpgradeEffect(), PUD_ICON_GNOMISH_FLYING_MACHINE, PUD_ICON_GOBLIN_ZEPPLIN)},
	{ProductionItem::ItemType::GryphonRider,  ProductionItem(ProductionItem::ItemType::GryphonRider, Resources(2500), 250 * factor_s, UpgradeEffect(), PUD_ICON_GRYPHON_RIDER, PUD_ICON_DRAGON)},

	// Harbour
	{ProductionItem::ItemType::Tanker,  ProductionItem(ProductionItem::ItemType::Tanker, Resources(400,250), 50 * factor_s, UpgradeEffect(), PUD_ICON_HUMAN_TANKER, PUD_ICON_ORC_TANKER)},
	{ProductionItem::ItemType::Transport,  ProductionItem(ProductionItem::ItemType::Transport, Resources(600,200,500), 7 * factor_s, UpgradeEffect(), PUD_ICON_HUMAN_TRANSPORT, PUD_ICON_ORC_TRANSPORT)},
	{ProductionItem::ItemType::Destroyer,  ProductionItem(ProductionItem::ItemType::Destroyer, Resources(700,350,700), 90 * factor_s, UpgradeEffect(), PUD_ICON_ELVEN_DESTROYER, PUD_ICON_TROLL_DESTROYER)},
	{ProductionItem::ItemType::Battleship,  ProductionItem(ProductionItem::ItemType::Battleship, Resources(1000,500,1000), 140 * factor_s, UpgradeEffect(), PUD_ICON_BATTLESHIP, PUD_ICON_JUGGERNAUGHT)},
	{ProductionItem::ItemType::GnomishSubmarine,  ProductionItem(ProductionItem::ItemType::GnomishSubmarine, Resources(800,150,800), 100 * factor_s, UpgradeEffect(), PUD_ICON_GNOMISH_SUBMARINE, PUD_ICON_GIANT_TURTLE)},


	// Spells
	{ProductionItem::ItemType::Mage_FlameShield,  ProductionItem(ProductionItem::ItemType::Mage_FlameShield, Resources(1000), 100 * factor_s, UpgradeEffect(), PUD_ICON_FLAME_SHIELD)},
	{ProductionItem::ItemType::Mage_Slow,         ProductionItem(ProductionItem::ItemType::Mage_Slow, Resources(500), 100 * factor_s, UpgradeEffect(), PUD_ICON_SLOW)},
	{ProductionItem::ItemType::Mage_Invisibility, ProductionItem(ProductionItem::ItemType::Mage_Invisibility, Resources(2500), 200 * factor_s, UpgradeEffect(), PUD_ICON_INVISIBILITY)},
	{ProductionItem::ItemType::Mage_Polymorph,    ProductionItem(ProductionItem::ItemType::Mage_Polymorph, Resources(2000), 200 * factor_s, UpgradeEffect(), PUD_ICON_POLYMORPH)},
	{ProductionItem::ItemType::Mage_Blizzard,     ProductionItem(ProductionItem::ItemType::Mage_Blizzard, Resources(2000), 200 * factor_s, UpgradeEffect(), PUD_ICON_BLIZZARD)},

	{ProductionItem::ItemType::DeathKnight_Haste,         ProductionItem(ProductionItem::ItemType::DeathKnight_Haste, Resources(500), 100 * factor_s, UpgradeEffect(), PUD_ICON_HASTE)},
	{ProductionItem::ItemType::DeathKnight_RaiseTheDead,  ProductionItem(ProductionItem::ItemType::DeathKnight_RaiseTheDead, Resources(1500), 100 * factor_s, UpgradeEffect(), PUD_ICON_SKELETON)},
	{ProductionItem::ItemType::DeathKnight_Whirlwind,     ProductionItem(ProductionItem::ItemType::DeathKnight_Whirlwind, Resources(1500), 150 * factor_s, UpgradeEffect(), PUD_ICON_WHIRLWIND)},
	{ProductionItem::ItemType::DeathKnight_UnholyArmor,   ProductionItem(ProductionItem::ItemType::DeathKnight_UnholyArmor, Resources(2500), 200 * factor_s, UpgradeEffect(), PUD_ICON_UNHOLY_ARMOR)},
	{ProductionItem::ItemType::DeathKnight_DeathAndDecay, ProductionItem(ProductionItem::ItemType::DeathKnight_DeathAndDecay, Resources(2000), 200 * factor_s, UpgradeEffect(), PUD_ICON_DEATH_COIL)},




	// Church
	{ProductionItem::ItemType::Church_Paladin,  ProductionItem(ProductionItem::ItemType::Church_Paladin, Resources(1000), 250 * factor_s, UpgradeEffect(), PUD_ICON_PALADIN)},
	{ProductionItem::ItemType::Church_Healing,  ProductionItem(ProductionItem::ItemType::Church_Healing, Resources(1000), 200 * factor_s, UpgradeEffect(), PUD_ICON_HEALING)},
	{ProductionItem::ItemType::Church_Exorcism, ProductionItem(ProductionItem::ItemType::Church_Exorcism, Resources(2000), 200 * factor_s, UpgradeEffect(), PUD_ICON_EXORCISM)},
	// Altar of Storms (Church)
	{ProductionItem::ItemType::AltarOfStorms_OgreMage,  ProductionItem(ProductionItem::ItemType::AltarOfStorms_OgreMage, Resources(1000), 250 * factor_s, UpgradeEffect(), PUD_ICON_OGRE_MAGE)},
	{ProductionItem::ItemType::AltarOfStorms_Bloodlust, ProductionItem(ProductionItem::ItemType::AltarOfStorms_Bloodlust, Resources(1000), 200 * factor_s, UpgradeEffect(), PUD_ICON_BLOODLUST)},
	{ProductionItem::ItemType::AltarOfStorms_Runes,     ProductionItem(ProductionItem::ItemType::AltarOfStorms_Runes, Resources(1000), 150 * factor_s, UpgradeEffect(), PUD_ICON_RUNES)},


	//ProductionItem(ItemType t = ItemType::Invalid, const Resources& costs = Resources(), int researchTime_ms = 0, const Foo& f = Foo(), Pud_Icon iconID_human = PUD_ICON_BURN, Pud_Icon iconID_orc = PUD_ICON_BURN)

   // Blacksmith upgrades
   // In scenarios where you control both Alliance and Horde units, researching an upgrade here will also grant you the equivalent Horde upgrade, except the weapons upgrade as they differ in price
   {ProductionItem::ItemType::Blacksmith_Weapons_1_Human,  ProductionItem(ProductionItem::ItemType::Blacksmith_Weapons_1_Human, Resources(800), 200 * factor_s, UpgradeEffect(2), PUD_ICON_SWORD_1)},
   {ProductionItem::ItemType::Blacksmith_Weapons_2_Human,  ProductionItem(ProductionItem::ItemType::Blacksmith_Weapons_2_Human, Resources(2400), 250 * factor_s, UpgradeEffect(2), PUD_ICON_SWORD_2)},
   // Yes, human and orc weapon upgrades are differing in prices
   {ProductionItem::ItemType::Blacksmith_Weapons_1_Orc,  ProductionItem(ProductionItem::ItemType::Blacksmith_Weapons_1_Orc, Resources(500,100), 200 * factor_s, UpgradeEffect(2), PUD_ICON_AXE_1)},
   {ProductionItem::ItemType::Blacksmith_Weapons_2_Orc,  ProductionItem(ProductionItem::ItemType::Blacksmith_Weapons_2_Orc, Resources(1500,300), 250 * factor_s, UpgradeEffect(2), PUD_ICON_AXE_2)},

   {ProductionItem::ItemType::Blacksmith_Armor_1,  ProductionItem(ProductionItem::ItemType::Blacksmith_Armor_1, Resources(300,300), 200 * factor_s, UpgradeEffect(0, 2), PUD_ICON_HUMAN_SHIELD_1, PUD_ICON_ORC_SHIELD_1)},
   {ProductionItem::ItemType::Blacksmith_Armor_2,  ProductionItem(ProductionItem::ItemType::Blacksmith_Armor_2, Resources(900,500), 250 * factor_s, UpgradeEffect(0, 2), PUD_ICON_HUMAN_SHIELD_2, PUD_ICON_ORC_SHIELD_2)},
   {ProductionItem::ItemType::Blacksmith_Ballista_1,  ProductionItem(ProductionItem::ItemType::Blacksmith_Ballista_1, Resources(1500), 250 * factor_s, UpgradeEffect(15), PUD_ICON_BALLISTA_1, PUD_ICON_CATAPULT_1)},
   {ProductionItem::ItemType::Blacksmith_Ballista_2,  ProductionItem(ProductionItem::ItemType::Blacksmith_Ballista_2, Resources(4000), 250 * factor_s, UpgradeEffect(15), PUD_ICON_BALLISTA_2, PUD_ICON_CATAPULT_2)},


   // LumberMill upgrades
   // In scenarios where you control both Hordeand Alliance units, researching an upgrade here will also grant you the equivalent Alliance upgrade
   {ProductionItem::ItemType::LumberMill_Arrows_1,  ProductionItem(ProductionItem::ItemType::LumberMill_Arrows_1, Resources(300,300), 200 * factor_s, UpgradeEffect(1), PUD_ICON_ARROW_1, PUD_ICON_SPEAR_1)},
   {ProductionItem::ItemType::LumberMill_Arrows_2,  ProductionItem(ProductionItem::ItemType::LumberMill_Arrows_2, Resources(900,500), 250 * factor_s, UpgradeEffect(1), PUD_ICON_ARROW_2, PUD_ICON_SPEAR_2)},
   {ProductionItem::ItemType::LumberMill_Ranger,  ProductionItem(ProductionItem::ItemType::LumberMill_Ranger, Resources(1500), 250 * factor_s, UpgradeEffect(0,0,0,1), PUD_ICON_RANGER, PUD_ICON_BERSERKER)},
   {ProductionItem::ItemType::LumberMill_RangerScouting,  ProductionItem(ProductionItem::ItemType::LumberMill_RangerScouting, Resources(1500), 250 * factor_s, UpgradeEffect(0,0,0,3), PUD_ICON_RANGER_SCOUTING, PUD_ICON_BERSERKER_SCOUTING)},
   {ProductionItem::ItemType::LumberMill_Longbow,  ProductionItem(ProductionItem::ItemType::LumberMill_Longbow, Resources(2000), 250 * factor_s, UpgradeEffect(0,0,0,0,1), PUD_ICON_LONGBOW, PUD_ICON_LIGHTER_AXES)},
   // Human specific
   {ProductionItem::ItemType::LumberMill_Marksmanship,  ProductionItem(ProductionItem::ItemType::LumberMill_Marksmanship, Resources(1500), 250 * factor_s, UpgradeEffect(3), PUD_ICON_RANGER_MARKSMANSHIP)},
   // Troll specific
   {ProductionItem::ItemType::LumberMill_TrollRegeneration,  ProductionItem(ProductionItem::ItemType::LumberMill_TrollRegeneration, Resources(2500), 250 * factor_s, UpgradeEffect(0,0,0,0,0,true), PUD_ICON_BERSERKER_REGENERATION)},


   // Foundry upgrades
   {ProductionItem::ItemType::Foundry_Ship_Cannon_1,  ProductionItem(ProductionItem::ItemType::Foundry_Ship_Cannon_1, Resources(700,100,1000), 200 * factor_s, UpgradeEffect(5), PUD_ICON_HUMAN_SHIP_CANNON_1, PUD_ICON_ORC_SHIP_CANNON_1)},
   {ProductionItem::ItemType::Foundry_Ship_Cannon_2,  ProductionItem(ProductionItem::ItemType::Foundry_Ship_Cannon_2, Resources(2000,350,3000), 250 * factor_s, UpgradeEffect(5), PUD_ICON_HUMAN_SHIP_CANNON_2, PUD_ICON_ORC_SHIP_CANNON_2)},
   {ProductionItem::ItemType::Foundry_Ship_Armor_1,  ProductionItem(ProductionItem::ItemType::Foundry_Ship_Armor_1, Resources(500,500), 200 * factor_s, UpgradeEffect(0,5), PUD_ICON_HUMAN_SHIP_ARMOR_1, PUD_ICON_ORC_SHIP_ARMOR_1)},
   {ProductionItem::ItemType::Foundry_Ship_Armor_2,  ProductionItem(ProductionItem::ItemType::Foundry_Ship_Armor_2, Resources(1500,900), 250 * factor_s, UpgradeEffect(0,5), PUD_ICON_HUMAN_SHIP_ARMOR_2, PUD_ICON_ORC_SHIP_ARMOR_2)},



   // Building Upgrades
   // Townhall upgrades
   {ProductionItem::ItemType::Keep,   ProductionItem(ProductionItem::ItemType::Keep  , Resources(2000, 1000, 200), 200 * factor_s, UpgradeEffect(), PUD_ICON_KEEP, PUD_ICON_STRONGHOLD)},
   {ProductionItem::ItemType::Castle, ProductionItem(ProductionItem::ItemType::Castle, Resources(2500, 1200, 500), 200 * factor_s, UpgradeEffect(), PUD_ICON_CASTLE, PUD_ICON_FORTRESS)},

   // Scout Tower upgrades
   {ProductionItem::ItemType::GuardTower,  ProductionItem(ProductionItem::ItemType::GuardTower,  Resources(550, 150), 140 * factor_s, UpgradeEffect(), PUD_ICON_HUMAN_GUARD_TOWER, PUD_ICON_ORC_GUARD_TOWER)},
   {ProductionItem::ItemType::CannonTower, ProductionItem(ProductionItem::ItemType::CannonTower, Resources(1000, 300), 190 * factor_s, UpgradeEffect(), PUD_ICON_HUMAN_CANNON_TOWER, PUD_ICON_ORC_CANNON_TOWER)}
};



/*
 PUD_ICON_ARROW_1 : PUD_ICON_AXE_1,
PUD_ICON_ARROW_2 : PUD_ICON_AXE_2));

PUD_ICON_RANGER : PUD_ICON_BERSERKER));
 PUD_ICON_RANGER_SCOUTING : PUD_ICON_BERSERKER_SCOUTING));

PUD_ICON_LONGBOW : PUD_ICON_LIGHTER_AXES));

PUD_ICON_RANGER_MARKSMANSHIP : PUD_ICON_BERSERKER_REGENERATION));
*/




/*
	// Blacksmith upgrades (Costs are different, extend enum or not?)
	{ProductionItem::ItemType::Blacksmith_Weapons_1_Human, Upgrade(ProductionItem::ItemType::Blacksmith_Weapons_1_Human, PUD_ICON_SWORD_1, Resources(800), 4000, 2)},   // +2 dmg
	{ProductionItem::ItemType::Blacksmith_Weapons_2_Human, Upgrade(ProductionItem::ItemType::Blacksmith_Weapons_2_Human, PUD_ICON_SWORD_2, Resources(2400), 4000, 2)},  // +2 dmg
	{ProductionItem::ItemType::Blacksmith_Weapons_1_Orc,   Upgrade(ProductionItem::ItemType::Blacksmith_Weapons_1_Orc, Resources(500, 100), 4000, 2)},  // +2 dmg
	{ProductionItem::ItemType::Blacksmith_Weapons_2_Orc,   Upgrade(ProductionItem::ItemType::Blacksmith_Weapons_2_Orc, Resources(1500, 300), 4000, 2)}, // +2 dmg
	{ProductionItem::ItemType::Blacksmith_Armor_1,         Upgrade(ProductionItem::ItemType::Blacksmith_Armor_1, Resources(300, 300), 4000, 0, 2)}, // +2 armor
	{ProductionItem::ItemType::Blacksmith_Armor_2,         Upgrade(ProductionItem::ItemType::Blacksmith_Armor_2, Resources(900, 500), 4000, 0, 2)}, // +2 armor
	{ProductionItem::ItemType::Blacksmith_Ballista_1,      Upgrade(ProductionItem::ItemType::Blacksmith_Ballista_1, Resources(1500), 4000, 15)}, // +15 dmg
	{ProductionItem::ItemType::Blacksmith_Ballista_2,      Upgrade(ProductionItem::ItemType::Blacksmith_Ballista_2, Resources(4000), 4000, 15)}, // +15 dmg




			PUD_ICON_HUMAN_SHIELD_2
			PUD_ICON_BALLISTA_1
			PUD_ICON_BALLISTA_2
	*/
	/*

*/
/*
	// Mage
	{ProductionItem::ItemType::Mage_FlameShield,   ProductionItem(ProductionItem::ItemType::Mage_FlameShield, PUD_ICON_FLAME_SHIELD, Resources(1000), 100*1000)}, // Debug times: /10
	{ProductionItem::ItemType::Mage_Slow,          ProductionItem(ProductionItem::ItemType::Mage_Slow, PUD_ICON_SLOW, Resources(500), 100*1000)},
	{ProductionItem::ItemType::Mage_Invisibility,  ProductionItem(ProductionItem::ItemType::Mage_Invisibility, PUD_ICON_INVISIBILITY, Resources(2500), 200*1000)},
	{ProductionItem::ItemType::Mage_Polymorph,     ProductionItem(ProductionItem::ItemType::Mage_Polymorph, PUD_ICON_POLYMORPH, Resources(2000), 200*1000)},
	{ProductionItem::ItemType::Mage_Blizzard,      ProductionItem(ProductionItem::ItemType::Mage_Blizzard, PUD_ICON_BLIZZARD, Resources(2000), 200*1000)}*/

	/*


		// Townhall upgrades
		{ProductionItem::ItemType::Townhall_Keep, Upgrade(ProductionItem::ItemType::Townhall_Keep, Resources(), 4000)},
		{ProductionItem::ItemType::Townhall_Castle, Upgrade(ProductionItem::ItemType::Townhall_Castle, Resources(1000), 4000)},


			*/


