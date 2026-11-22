#include <gtest/gtest.h>

#include "Magic/IMagical.h"
#include "Magic/SpellCosts.h"
#include "Map/MovementRules.h"
#include "Global/SimulationSettings.h"
#include "Resources/Season.h"
#include "Units/CombatRules.h"

int g_tick_ms = 25;

int calculateRequiredTicksForGameAction(int requiredTime_ms) {
	return std::max(1, (requiredTime_ms + g_tick_ms - 1) / g_tick_ms);
}

namespace {
struct TestPoint { int x; int y; };
}

TEST(MovementRulesTest, TransportCanDockButOtherUnitsCannotUseCoast) {
	EXPECT_TRUE(MovementRules::terrainAllows(false, true, true,
		false, false, true, false, false, false));
	EXPECT_FALSE(MovementRules::terrainAllows(false, true, false,
		false, false, true, false, false, false));
	EXPECT_FALSE(MovementRules::terrainAllows(true, false, false,
		false, false, true, false, false, false));
}

TEST(MovementRulesTest, OpenWaterAndClearLandRemainTraversable) {
	EXPECT_TRUE(MovementRules::terrainAllows(false, true, false,
		false, true, false, false, false, false));
	EXPECT_TRUE(MovementRules::terrainAllows(true, false, false,
		true, false, false, false, false, false));
	EXPECT_FALSE(MovementRules::terrainAllows(true, false, false,
		true, false, false, false, true, false));
}

TEST(MovementRulesTest, OnlyAirMovementKeepsSweptFootprintClearance) {
	EXPECT_FALSE(MovementRules::requiresOrthogonalDiagonalClearance(false));
	EXPECT_TRUE(MovementRules::requiresOrthogonalDiagonalClearance(true));
}

TEST(MovementRulesTest, PixelStepStopsWhenItReachesOrCrossesZero) {
	EXPECT_FALSE(MovementRules::reachedTilePixelOffset(-2, 6));
	EXPECT_TRUE(MovementRules::reachedTilePixelOffset(4, 6));
	EXPECT_FALSE(MovementRules::reachedTilePixelOffset(2, -6));
	EXPECT_TRUE(MovementRules::reachedTilePixelOffset(-4, -6));
	EXPECT_TRUE(MovementRules::reachedTilePixelOffset(0, 0));
}

TEST(CombatRulesTest, ShipsChooseTheNearestBroadsideFacing) {
	EXPECT_EQ(CombatRules::broadsideFacing(0, 2), 2);
	EXPECT_EQ(CombatRules::broadsideFacing(2, 0), 0);
	EXPECT_EQ(CombatRules::broadsideFacing(5, 7), 7);
}

TEST(CombatRulesTest, ProjectileLaunchSidePointsTowardTarget) {
	const auto east = CombatRules::directionVector(2);
	const auto northWest = CombatRules::directionVector(7);
	EXPECT_EQ(east.x, 1);
	EXPECT_EQ(east.y, 0);
	EXPECT_EQ(northWest.x, -1);
	EXPECT_EQ(northWest.y, -1);
}

TEST(MovementRulesTest, PermanentOccupantsAreNeverTraversable) {
	EXPECT_FALSE(MovementRules::occupancyTraversalCost(true, false, false).has_value());
}

TEST(MovementRulesTest, UnitsAreExpensiveButTemporaryObstacles) {
	EXPECT_EQ(MovementRules::occupancyTraversalCost(true, false, true), 200);
	EXPECT_FALSE(MovementRules::occupancyTraversalCost(
		true, false, true, false).has_value());
	EXPECT_EQ(MovementRules::occupancyTraversalCost(true, true, true), 0);
	EXPECT_EQ(MovementRules::occupancyTraversalCost(false, false, false), 0);
}

TEST(CombatRulesTest, SiegeWeaponsUseOriginalEightTileRange) {
	EXPECT_EQ(CombatRules::SiegeAttackRangeTiles, 8);
}

TEST(CombatRulesTest, RectangleDistanceMeasuresFootprintEdges) {
	EXPECT_EQ(CombatRules::rectangleDistanceSquared(0, 0, 0, 0, 4, 0, 4, 0), 16);
	EXPECT_EQ(CombatRules::rectangleDistanceSquared(0, 0, 1, 1, 2, 1, 3, 2), 1);
	EXPECT_EQ(CombatRules::rectangleDistanceSquared(0, 0, 1, 1, 1, 1, 2, 2), 0);
}

TEST(MagicTest, FireballConsumesItsConfiguredMana) {
	IMagical mana(125);
	const int cost = SpellCosts::manaCost(static_cast<int>(MageActions::Fireball));
	ASSERT_EQ(cost, 100);
	EXPECT_TRUE(mana.reduceMana(cost));
	EXPECT_EQ(mana.getMana(), 25);
}

TEST(MagicTest, FailedCastAndInvalidCostDoNotChangeMana) {
	IMagical mana(85);
	EXPECT_FALSE(mana.reduceMana(SpellCosts::manaCost(static_cast<int>(MageActions::Fireball))));
	EXPECT_FALSE(mana.reduceMana(-1));
	EXPECT_EQ(mana.getMana(), 85);
}

TEST(MagicTest, ManaRegeneratesAfterExactlyOneInterval) {
	IMagical mana(100);
	for (int tick = 0; tick < 19; ++tick) mana.process();
	EXPECT_EQ(mana.getMana(), 100);
	mana.process();
	EXPECT_EQ(mana.getMana(), 101);
}

TEST(MagicTest, ExorcismUsesFourManaPerActualHitPoint) {
	const auto lethal = SpellCosts::exorcism(255, 60);
	EXPECT_EQ(lethal.damage, 60);
	EXPECT_EQ(lethal.mana, 240);

	const auto limited = SpellCosts::exorcism(100, 60);
	EXPECT_EQ(limited.damage, 25);
	EXPECT_EQ(limited.mana, 100);
}

TEST(MagicTest, ExorcismDoesNotSpendRemainderWithoutEnoughManaForDamage) {
	const auto result = SpellCosts::exorcism(3, 60);
	EXPECT_EQ(result.damage, 0);
	EXPECT_EQ(result.mana, 0);
}

TEST(SimulationSettingsTest, SpeedSupportsSixteenTimesAndClampsInvalidValues) {
	EXPECT_FLOAT_EQ(SimulationSettings::clampSpeed(16.0f), 16.0f);
	EXPECT_FLOAT_EQ(SimulationSettings::clampSpeed(20.0f), 16.0f);
	EXPECT_FLOAT_EQ(SimulationSettings::clampSpeed(0.0f), 0.25f);
}

TEST(SimulationSettingsTest, SixteenTimesAcceleratesEverySimulationTickEqually) {
	EXPECT_DOUBLE_EQ(SimulationSettings::tickDurationMilliseconds(25, 1.0f), 25.0);
	EXPECT_DOUBLE_EQ(SimulationSettings::tickDurationMilliseconds(25, 16.0f), 1.5625);
}

TEST(SeasonHotkeysTest, RuntimeWasdMappingIsPreserved) {
	EXPECT_EQ(seasonForHotkey('w'), Season::Winter);
	EXPECT_EQ(seasonForHotkey('s'), Season::Summer);
	EXPECT_EQ(seasonForHotkey('a'), Season::Wasteland);
	EXPECT_EQ(seasonForHotkey('d'), Season::Swamp);
	EXPECT_FALSE(seasonForHotkey('x').has_value());
}
