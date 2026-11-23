#include <gtest/gtest.h>

#include "Magic/IMagical.h"
#include "Magic/BuffRules.h"
#include "Magic/SpellCosts.h"
#include "Magic/SpellRules.h"
#include "Map/MovementRules.h"
#include "Global/SimulationSettings.h"
#include "Global/ProductionRules.h"
#include "Resources/Season.h"
#include "Resources/AnimationRules.h"
#include "Units/CombatRules.h"
#include "Units/ProjectileRules.h"
#include "Units/UnitPresentationRules.h"
#include "GUI/DebugSpellRules.h"

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

TEST(CombatRulesTest, AttackRangeUsesStratagusFlooredMapDistance) {
	EXPECT_TRUE(CombatRules::rectanglesWithinMapRange(
		0, 0, 0, 0, 3, 3, 3, 3, 4));
	EXPECT_FALSE(CombatRules::rectanglesWithinMapRange(
		0, 0, 0, 0, 4, 3, 4, 3, 4));
	EXPECT_TRUE(CombatRules::rectanglesWithinMapRange(
		0, 0, 1, 1, 4, 4, 5, 5, 4));
}

TEST(CombatRulesTest, AutoTargetingPrefersThreatsOverCloserBuildings) {
	const CombatRules::AutoTargetPriority building{
		false, false, false, true, 1, 10};
	const CombatRules::AutoTargetPriority armedUnit{
		false, true, true, false, 16, 11};
	EXPECT_TRUE(CombatRules::isBetterAutoTarget(armedUnit, building));
}

TEST(CombatRulesTest, AutoTargetingFirstAnswersTheUnitAttackingUs) {
	const CombatRules::AutoTargetPriority nearbyThreat{
		false, true, true, true, 1, 10};
	const CombatRules::AutoTargetPriority attacker{
		true, true, true, false, 25, 11};
	EXPECT_TRUE(CombatRules::isBetterAutoTarget(attacker, nearbyThreat));
}

TEST(ProjectileRulesTest, MultiFrameMissilesAnimateDuringFlight) {
	EXPECT_EQ(ProjectileRules::animationStage(1, 20, 6, false), 0);
	EXPECT_EQ(ProjectileRules::animationStage(6, 20, 6, false), 5);
	EXPECT_EQ(ProjectileRules::animationStage(7, 20, 6, false), 0);
}

TEST(ProjectileRulesTest, GrowingCannonAnimationStopsOnItsLastFrame) {
	EXPECT_EQ(ProjectileRules::animationStage(1, 8, 4, true), 0);
	EXPECT_EQ(ProjectileRules::animationStage(8, 8, 4, true), 3);
}

TEST(ProjectileRulesTest, SiegeAndHeavyCannonSplashMatchWargus) {
	EXPECT_EQ(ProjectileRules::SiegeSplash.radiusTiles, 2);
	EXPECT_EQ(ProjectileRules::SiegeSplash.damageDivisor, 4);
	EXPECT_EQ(ProjectileRules::SmallCannonSplash.radiusTiles, 2);
	EXPECT_EQ(ProjectileRules::SmallCannonSplash.damageDivisor, 3);
}

TEST(AnimationRulesTest, WarcraftSpellSheetsUseTheirRealFrameGeometry) {
	EXPECT_EQ(AnimationRules::gridFrameCount(32, 128, 32, 32, 4), 4);
	EXPECT_EQ(AnimationRules::gridFrameCount(160, 64, 32, 32, 8), 8);
	EXPECT_EQ(AnimationRules::gridFrameCount(160, 96, 32, 48, 6), 6);
	EXPECT_EQ(AnimationRules::gridFrameCount(240, 96, 48, 48, 10), 10);
}

TEST(MagicTest, BombardmentCoversFiveByFiveWithElevenFallingShards) {
	EXPECT_EQ(SpellRules::BombardmentFields, 5);
	EXPECT_EQ(SpellRules::BombardmentShards, 11);
	EXPECT_EQ(SpellRules::BlizzardFallHeightTiles, 4);
	int minX = 10;
	int maxX = 10;
	int minY = 10;
	int maxY = 10;
	for (int shard = 0; shard < SpellRules::BombardmentShards; ++shard) {
		const auto tile = SpellRules::bombardmentTile(TestPoint{10, 10}, shard, 32);
		minX = std::min(minX, tile.x);
		maxX = std::max(maxX, tile.x);
		minY = std::min(minY, tile.y);
		maxY = std::max(maxY, tile.y);
		EXPECT_GE(tile.x, 8);
		EXPECT_LE(tile.x, 12);
		EXPECT_GE(tile.y, 8);
		EXPECT_LE(tile.y, 12);
		EXPECT_GE(SpellRules::bombardmentDamage(shard, TestPoint{10, 10}), 0);
		EXPECT_LE(SpellRules::bombardmentDamage(shard, TestPoint{10, 10}), 9);
	}
	EXPECT_EQ(maxX - minX + 1, SpellRules::BombardmentFields);
	EXPECT_EQ(maxY - minY + 1, SpellRules::BombardmentFields);
}

TEST(MagicTest, BombardmentClampsAtMapEdges) {
	for (int shard = 0; shard < SpellRules::BombardmentShards; ++shard) {
		const auto tile = SpellRules::bombardmentTile(TestPoint{0, 0}, shard, 8);
		EXPECT_GE(tile.x, 0);
		EXPECT_GE(tile.y, 0);
		EXPECT_LT(tile.x, 8);
		EXPECT_LT(tile.y, 8);
	}
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

TEST(MagicTest, HealingSpendsSixManaPerActualHitPoint) {
	const auto partial = SpellCosts::healing(25, 20);
	EXPECT_EQ(partial.health, 4);
	EXPECT_EQ(partial.mana, 24);

	const auto capped = SpellCosts::healing(255, 10);
	EXPECT_EQ(capped.health, 10);
	EXPECT_EQ(capped.mana, 60);
}

TEST(MagicTest, CanonicalPaladinAndOgreSpellCostsAndRanges) {
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(PaladinActions::HolyVision)), 70);
	EXPECT_EQ(SpellCosts::range(static_cast<int>(PaladinActions::Heal)), 6);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(OgreActions::EyeOfKillrogg)), 70);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(BuffType::Bloodlust)), 50);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(OgreActions::Runes)), 200);
	EXPECT_EQ(SpellCosts::range(static_cast<int>(OgreActions::Runes)), 10);
}

TEST(MagicTest, CanonicalMageAndDeathKnightSpellCostsAndRanges) {
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(MageActions::FlameShield)), 80);
	EXPECT_EQ(SpellCosts::range(static_cast<int>(MageActions::Fireball)), 10);
	EXPECT_EQ(SpellRules::FireballDamage, 34);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(MageActions::Blizzard)), 25);
	EXPECT_EQ(SpellCosts::range(static_cast<int>(MageActions::Blizzard)), 12);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(BuffType::UnholyArmor)), 200);
	EXPECT_EQ(SpellCosts::manaCost(static_cast<int>(DeathKnightActions::DeathAndDecay)), 25);
}

TEST(MagicTest, BlizzardDebugToolCreatesFiveCompleteCycles) {
	EXPECT_EQ(SpellRules::blizzardCycles(false), 1);
	EXPECT_EQ(SpellRules::blizzardCycles(true), 5);
	EXPECT_EQ(SpellRules::blizzardCycles(true) * SpellRules::BombardmentShards, 55);
}

TEST(MagicTest, BlizzardFallsDiagonallyDownAndRightAtFortyFiveDegrees) {
	const int offset = SpellRules::fallingOffsetPixels(4, 32, 0, 20);
	EXPECT_EQ(offset, 128);
	// BombardmentEffect subtracts this same offset from x and y, so its
	// trajectory has equal horizontal and vertical travel.
	EXPECT_EQ(SpellRules::fallingOffsetPixels(4, 32, 10, 20), 64);
	EXPECT_EQ(SpellRules::fallingOffsetPixels(4, 32, 20, 20), 0);
}

TEST(MagicTest, CorpseCanOnlyBeRaisedDuringExposedSkeletonPhase) {
	EXPECT_FALSE(SpellRules::corpseIsRaiseable(0));
	EXPECT_FALSE(SpellRules::corpseIsRaiseable(SpellRules::CorpseFreshDurationMs - 1));
	EXPECT_TRUE(SpellRules::corpseIsRaiseable(SpellRules::CorpseFreshDurationMs));
	EXPECT_TRUE(SpellRules::corpseIsRaiseable(
		SpellRules::CorpseFreshDurationMs + SpellRules::CorpseRaiseableDurationMs - 1));
	EXPECT_FALSE(SpellRules::corpseIsRaiseable(
		SpellRules::CorpseFreshDurationMs + SpellRules::CorpseRaiseableDurationMs));
}

TEST(MagicTest, RaiseDeadCreatesTwoTemporaryControllableUnits) {
	EXPECT_EQ(SpellRules::RaisedSkeletonCount, 2);
	EXPECT_GT(SpellRules::RaisedSkeletonLifetimeMs, 0);
	EXPECT_LT(SpellRules::RaisedSkeletonLifetimeMs, 2 * 60 * 1000);
}

TEST(MagicTest, WhirlwindInterpolatesSmoothlyBetweenTiles) {
	const SpellRules::TilePoint origin{10, 10};
	const auto start = SpellRules::whirlwindGroundPixel(origin, 0, 32);
	const auto quarter = SpellRules::whirlwindGroundPixel(origin, 250, 32);
	const auto half = SpellRules::whirlwindGroundPixel(origin, 500, 32);
	EXPECT_EQ(start.x, 10 * 32 + 16);
	EXPECT_EQ(quarter.x - start.x, 8);
	EXPECT_EQ(half.x - start.x, 16);
	EXPECT_EQ(start.y, quarter.y);
	EXPECT_EQ(SpellRules::WhirlwindSoundIntervalMs, 20000);
	EXPECT_LT(SpellRules::WhirlwindDamageIntervalMs, 1000);
}

TEST(MagicTest, DeathAndDecayAppearsInNonRowOrderedRectanglePositions) {
	const TestPoint center{10, 10};
	const auto first = SpellRules::decayTile(center, 0, 64);
	const auto second = SpellRules::decayTile(center, 1, 64);
	const auto third = SpellRules::decayTile(center, 2, 64);
	EXPECT_FALSE(first.y == second.y && second.y == third.y);
	EXPECT_NE(SpellRules::decayDelayMs(1) - SpellRules::decayDelayMs(0),
		SpellRules::decayDelayMs(2) - SpellRules::decayDelayMs(1));
}

TEST(MagicTest, FlameShieldUsesSixOrbitingFlamesAndSpellDetonationHasNoSplash) {
	EXPECT_EQ(SpellRules::FlameShieldFlames, 6);
	EXPECT_EQ(SpellRules::SpellTriggeredDemolitionSplashDamage, 0);
}

TEST(MagicTest, HasteAndSlowCancelEachOther) {
	EXPECT_EQ(BuffRules::cancelledBy(BuffType::Haste), BuffType::Slow);
	EXPECT_EQ(BuffRules::cancelledBy(BuffType::Slow), BuffType::Haste);
	EXPECT_FALSE(BuffRules::cancelledBy(BuffType::Bloodlust).has_value());
}

TEST(ProductionRulesTest, OgreMageUsesItsOwnUpgradeAndExistingReadySound) {
	EXPECT_TRUE(ProductionRules::cavalryUpgradeResearched(Fraction::Orc, false, true));
	EXPECT_FALSE(ProductionRules::cavalryUpgradeResearched(Fraction::Orc, true, false));
	EXPECT_EQ(ProductionRules::cavalryReadySound(Fraction::Orc, true),
		"orc/units/ogre-mage/ready.wav");
}

TEST(ProductionRulesTest, HumanCavalrySoundSelectionRemainsCanonical) {
	EXPECT_EQ(ProductionRules::cavalryReadySound(Fraction::Alliance, false),
		"human/units/knight/ready.wav");
	EXPECT_EQ(ProductionRules::cavalryReadySound(Fraction::Alliance, true),
		"human/units/paladin/ready.wav");
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

TEST(SeasonAssetsTest, MissingRequestedSeasonUsesDeterministicOriginalFallback) {
	const auto onlySwamp = [](Season season) { return season == Season::Swamp; };
	EXPECT_EQ(resolveAvailableSeason(Season::Summer, onlySwamp), Season::Swamp);
	EXPECT_EQ(resolveAvailableSeason(Season::Swamp, onlySwamp), Season::Swamp);
}

TEST(SeasonAssetsTest, MissingAllSeasonAssetsIsReported) {
	EXPECT_FALSE(resolveAvailableSeason(Season::Winter, [](Season) { return false; }).has_value());
}

TEST(MovementRulesTest, WalkableGroundOverlayDoesNotBlockUnits) {
	EXPECT_FALSE(MovementRules::occupantBlocksMovement(true, false, true));
	EXPECT_TRUE(MovementRules::occupantBlocksMovement(true, false, false));
}

TEST(AnimationRulesTest, WarcraftBluePaletteCyclesThroughFivePhases) {
	EXPECT_EQ(AnimationRules::PaletteCycleFrames, 5);
	EXPECT_EQ(AnimationRules::paletteCycleSourceIndex(240, 0), 240);
	EXPECT_EQ(AnimationRules::paletteCycleSourceIndex(240, 1), 241);
	EXPECT_EQ(AnimationRules::paletteCycleSourceIndex(244, 1), 240);
	EXPECT_EQ(AnimationRules::paletteCycleSourceIndex(100, 3), 100);
}

TEST(UnitPresentationRulesTest, AttackAnimationReturnsToIdleWhenFramesFinish) {
	EXPECT_FALSE(UnitPresentationRules::shouldReturnToIdle(false, true));
	EXPECT_FALSE(UnitPresentationRules::shouldReturnToIdle(true, false));
	EXPECT_TRUE(UnitPresentationRules::shouldReturnToIdle(true, true));
}

TEST(UnitPresentationRulesTest, CanonicalAttackSoundsAreMapped) {
	using Sound = UnitPresentationRules::AttackSound;
	EXPECT_EQ(UnitPresentationRules::swordAttackSound(2),
		"missiles/sword_attack/2.wav");
	EXPECT_EQ(UnitPresentationRules::fixedAttackSound(Sound::Punch),
		"missiles/punch.wav");
	EXPECT_EQ(UnitPresentationRules::fixedAttackSound(Sound::Bow),
		"missiles/bow_throw.wav");
	EXPECT_EQ(UnitPresentationRules::fixedAttackSound(Sound::Axe),
		"missiles/axe_throw.wav");
	EXPECT_EQ(UnitPresentationRules::fixedAttackSound(Sound::MageFireball),
		"missiles/fireball_hit.wav");
}

TEST(UnitPresentationRulesTest, RepeatedClicksSwitchToAnnoyedWithoutExplosion) {
	using Selection = UnitPresentationRules::SelectionSound;
	EXPECT_EQ(UnitPresentationRules::selectionSoundForClick(4, true), Selection::Selected);
	EXPECT_EQ(UnitPresentationRules::selectionSoundForClick(5, true), Selection::Annoyed);
	EXPECT_EQ(UnitPresentationRules::selectionSoundForClick(50, true), Selection::Annoyed);
	EXPECT_EQ(UnitPresentationRules::selectionSoundForClick(50, false), Selection::Selected);
}

TEST(UnitPresentationRulesTest, DragonsAndGryphonsExposeAttackGround) {
	EXPECT_TRUE(UnitPresentationRules::supportsAttackGround(true));
	EXPECT_FALSE(UnitPresentationRules::supportsAttackGround(false));
}

TEST(DebugSpellRulesTest, EveryVisibleSpellButtonHasATargetedSpell) {
	EXPECT_EQ(DebugSpellRules::Spells.size(), 18u);
	for (int button = 0; button < 18; ++button)
		EXPECT_TRUE(DebugSpellRules::spellForButton(button).has_value());
	EXPECT_EQ(DebugSpellRules::spellForButton(0),
		static_cast<EventID>(MageActions::Fireball));
	EXPECT_EQ(DebugSpellRules::spellForButton(5),
		static_cast<EventID>(MageActions::Blizzard));
	EXPECT_EQ(DebugSpellRules::spellForButton(13),
		static_cast<EventID>(PaladinActions::Heal));
	EXPECT_FALSE(DebugSpellRules::spellForButton(18).has_value());
}
