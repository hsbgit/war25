#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockFramework.h"

#ifdef INTEGRATION_TEST_MODE

// Mock combat unit for integration testing
class IntegrationTestCombatUnit {
public:
    enum class UnitType { Peasant, Footman, Archer, Knight, Mage };

    IntegrationTestCombatUnit(UnitType type, int health, int armor, int basicDamage, int piercingDamage, int range = 1)
        : m_type(type), m_maxHealth(health), m_currentHealth(health), m_armor(armor),
          m_basicDamage(basicDamage), m_piercingDamage(piercingDamage), m_range(range), m_isDead(false) {}

    // Combat properties
    int getHealth() const { return m_currentHealth; }
    int getMaxHealth() const { return m_maxHealth; }
    int getArmor() const { return m_armor; }
    int getBasicDamage() const { return m_basicDamage; }
    int getPiercingDamage() const { return m_piercingDamage; }
    int getRange() const { return m_range; }
    bool isDead() const { return m_isDead; }
    UnitType getType() const { return m_type; }

    float getHealthPercentage() const {
        return static_cast<float>(m_currentHealth) / static_cast<float>(m_maxHealth);
    }

    // Combat actions
    void takeDamage(int basicDamage, int piercingDamage) {
        int totalDamage = calculateDamage(basicDamage, piercingDamage);
        m_currentHealth = std::max(0, m_currentHealth - totalDamage);
        if (m_currentHealth <= 0) {
            m_isDead = true;
        }
    }

    void heal(int amount) {
        if (!m_isDead) {
            m_currentHealth = std::min(m_maxHealth, m_currentHealth + amount);
        }
    }

    void attack(IntegrationTestCombatUnit* target) {
        if (target && !target->isDead() && !m_isDead) {
            target->takeDamage(m_basicDamage, m_piercingDamage);
        }
    }

    // Upgrade system
    void upgradeWeapons(int basicDamageBonus, int piercingDamageBonus) {
        m_basicDamage += basicDamageBonus;
        m_piercingDamage += piercingDamageBonus;
    }

    void upgradeArmor(int armorBonus) {
        m_armor += armorBonus;
    }

    // Special abilities
    bool canCast() const {
        return m_type == UnitType::Mage;
    }

    void castFireball(IntegrationTestCombatUnit* target) {
        if (canCast() && target && !target->isDead()) {
            target->takeDamage(0, 50); // Pure piercing damage
        }
    }

    void castHeal(IntegrationTestCombatUnit* target) {
        if (canCast() && target && !target->isDead()) {
            target->heal(25);
        }
    }

private:
    int calculateDamage(int basicDamage, int piercingDamage) {
        // Basic damage is reduced by armor, piercing damage ignores armor
        int reducedBasicDamage = std::max(1, basicDamage - m_armor);
        return reducedBasicDamage + piercingDamage;
    }

    UnitType m_type;
    int m_maxHealth;
    int m_currentHealth;
    int m_armor;
    int m_basicDamage;
    int m_piercingDamage;
    int m_range;
    bool m_isDead;
};

// Combat scenario manager
class CombatScenario {
public:
    CombatScenario() {}

    void addUnit(std::unique_ptr<IntegrationTestCombatUnit> unit) {
        m_units.push_back(std::move(unit));
    }

    IntegrationTestCombatUnit* getUnit(size_t index) {
        if (index < m_units.size()) {
            return m_units[index].get();
        }
        return nullptr;
    }

    size_t getUnitCount() const {
        return m_units.size();
    }

    size_t getLivingUnitCount() const {
        size_t count = 0;
        for (const auto& unit : m_units) {
            if (!unit->isDead()) {
                count++;
            }
        }
        return count;
    }

    // Simulate combat between two groups
    void simulateCombat(const std::vector<size_t>& group1Indices,
                       const std::vector<size_t>& group2Indices,
                       int maxRounds = 50) {
        for (int round = 0; round < maxRounds; ++round) {
            bool anyAttacks = false;

            // Group 1 attacks Group 2
            for (size_t attackerIndex : group1Indices) {
                auto attacker = getUnit(attackerIndex);
                if (attacker && !attacker->isDead()) {
                    for (size_t targetIndex : group2Indices) {
                        auto target = getUnit(targetIndex);
                        if (target && !target->isDead()) {
                            attacker->attack(target);
                            anyAttacks = true;
                            break; // Attack first available target
                        }
                    }
                }
            }

            // Group 2 attacks Group 1
            for (size_t attackerIndex : group2Indices) {
                auto attacker = getUnit(attackerIndex);
                if (attacker && !attacker->isDead()) {
                    for (size_t targetIndex : group1Indices) {
                        auto target = getUnit(targetIndex);
                        if (target && !target->isDead()) {
                            attacker->attack(target);
                            anyAttacks = true;
                            break; // Attack first available target
                        }
                    }
                }
            }

            if (!anyAttacks) break; // Combat ended
        }
    }

private:
    std::vector<std::unique_ptr<IntegrationTestCombatUnit>> m_units;
};

#endif // INTEGRATION_TEST_MODE

class CombatIntegrationTest : public War25IntegrationTest {
protected:
    void SetUp() override {
        War25IntegrationTest::SetUp();
        scenario = std::make_unique<CombatScenario>();
    }

    std::unique_ptr<IntegrationTestCombatUnit> createPeasant() {
        return std::make_unique<IntegrationTestCombatUnit>(
            IntegrationTestCombatUnit::UnitType::Peasant, 30, 0, 3, 2, 1);
    }

    std::unique_ptr<IntegrationTestCombatUnit> createFootman() {
        return std::make_unique<IntegrationTestCombatUnit>(
            IntegrationTestCombatUnit::UnitType::Footman, 60, 2, 8, 3, 1);
    }

    std::unique_ptr<IntegrationTestCombatUnit> createArcher() {
        return std::make_unique<IntegrationTestCombatUnit>(
            IntegrationTestCombatUnit::UnitType::Archer, 40, 0, 3, 6, 4);
    }

    std::unique_ptr<IntegrationTestCombatUnit> createKnight() {
        return std::make_unique<IntegrationTestCombatUnit>(
            IntegrationTestCombatUnit::UnitType::Knight, 90, 4, 12, 8, 1);
    }

    std::unique_ptr<IntegrationTestCombatUnit> createMage() {
        return std::make_unique<IntegrationTestCombatUnit>(
            IntegrationTestCombatUnit::UnitType::Mage, 25, 0, 0, 2, 3);
    }

    std::unique_ptr<CombatScenario> scenario;
};

TEST_F(CombatIntegrationTest, BasicCombatMechanics) {
    auto footman = createFootman();
    auto archer = createArcher();

    int initialFootmanHealth = footman->getHealth();
    int initialArcherHealth = archer->getHealth();

    // Footman attacks archer
    footman->attack(archer.get());

    EXPECT_LT(archer->getHealth(), initialArcherHealth);
    EXPECT_FALSE(archer->isDead());

    // Archer counter-attacks
    archer->attack(footman.get());

    EXPECT_LT(footman->getHealth(), initialFootmanHealth);
    EXPECT_FALSE(footman->isDead());
}

TEST_F(CombatIntegrationTest, ArmorReduction) {
    auto footman = createFootman(); // 2 armor
    auto peasant = createPeasant();  // 0 armor

    int footmanInitialHealth = footman->getHealth();
    int peasantInitialHealth = peasant->getHealth();

    // Same attack against armored and unarmored targets
    footman->takeDamage(10, 0); // 10 basic damage
    peasant->takeDamage(10, 0);  // 10 basic damage

    // Footman should take less damage due to armor
    int footmanDamage = footmanInitialHealth - footman->getHealth();
    int peasantDamage = peasantInitialHealth - peasant->getHealth();

    EXPECT_LT(footmanDamage, peasantDamage);
    EXPECT_EQ(footmanDamage, 8); // 10 - 2 armor = 8
    EXPECT_EQ(peasantDamage, 10); // 10 - 0 armor = 10
}

TEST_F(CombatIntegrationTest, PiercingDamageIgnoresArmor) {
    auto knight = createKnight(); // High armor (4)

    int initialHealth = knight->getHealth();

    // Pure piercing damage should ignore armor
    knight->takeDamage(0, 20);

    EXPECT_EQ(knight->getHealth(), initialHealth - 20);
}

TEST_F(CombatIntegrationTest, MinimumDamage) {
    auto knight = createKnight(); // High armor (4)

    int initialHealth = knight->getHealth();

    // Very low basic damage should still deal at least 1 damage
    knight->takeDamage(1, 0);

    EXPECT_EQ(knight->getHealth(), initialHealth - 1);
}

TEST_F(CombatIntegrationTest, UnitDeath) {
    auto peasant = createPeasant();

    EXPECT_FALSE(peasant->isDead());
    EXPECT_GT(peasant->getHealth(), 0);

    // Deal massive damage
    peasant->takeDamage(100, 0);

    EXPECT_TRUE(peasant->isDead());
    EXPECT_EQ(peasant->getHealth(), 0);
}

TEST_F(CombatIntegrationTest, HealingMechanics) {
    auto footman = createFootman();

    // Damage the unit
    footman->takeDamage(30, 0);
    int damagedHealth = footman->getHealth();

    // Heal the unit
    footman->heal(15);

    EXPECT_GT(footman->getHealth(), damagedHealth);
    EXPECT_LE(footman->getHealth(), footman->getMaxHealth());
}

TEST_F(CombatIntegrationTest, HealingCannotExceedMaxHealth) {
    auto footman = createFootman();

    int maxHealth = footman->getMaxHealth();

    // Heal beyond max health
    footman->heal(1000);

    EXPECT_EQ(footman->getHealth(), maxHealth);
}

TEST_F(CombatIntegrationTest, CannotHealDeadUnits) {
    auto peasant = createPeasant();

    // Kill the unit
    peasant->takeDamage(100, 0);
    EXPECT_TRUE(peasant->isDead());

    // Try to heal
    peasant->heal(50);

    EXPECT_TRUE(peasant->isDead());
    EXPECT_EQ(peasant->getHealth(), 0);
}

TEST_F(CombatIntegrationTest, MagicAbilities) {
    auto mage = createMage();
    auto target = createFootman();

    EXPECT_TRUE(mage->canCast());

    int targetInitialHealth = target->getHealth();

    // Cast fireball (damage spell)
    mage->castFireball(target.get());

    EXPECT_LT(target->getHealth(), targetInitialHealth);

    // Damage the target more
    target->takeDamage(20, 0);
    int damagedHealth = target->getHealth();

    // Cast heal
    mage->castHeal(target.get());

    EXPECT_GT(target->getHealth(), damagedHealth);
}

TEST_F(CombatIntegrationTest, NonMagesCannotCast) {
    auto footman = createFootman();

    EXPECT_FALSE(footman->canCast());
}

TEST_F(CombatIntegrationTest, UpgradeSystem) {
    auto footman = createFootman();

    int initialBasicDamage = footman->getBasicDamage();
    int initialPiercingDamage = footman->getPiercingDamage();
    int initialArmor = footman->getArmor();

    // Upgrade weapons
    footman->upgradeWeapons(2, 1);

    EXPECT_EQ(footman->getBasicDamage(), initialBasicDamage + 2);
    EXPECT_EQ(footman->getPiercingDamage(), initialPiercingDamage + 1);

    // Upgrade armor
    footman->upgradeArmor(1);

    EXPECT_EQ(footman->getArmor(), initialArmor + 1);
}

TEST_F(CombatIntegrationTest, GroupCombatScenario) {
    // Create two groups of units
    scenario->addUnit(createFootman()); // Index 0
    scenario->addUnit(createArcher());  // Index 1
    scenario->addUnit(createFootman()); // Index 2
    scenario->addUnit(createArcher());  // Index 3

    std::vector<size_t> group1 = {0, 1}; // First footman and archer
    std::vector<size_t> group2 = {2, 3}; // Second footman and archer

    EXPECT_EQ(scenario->getLivingUnitCount(), 4);

    // Simulate combat
    scenario->simulateCombat(group1, group2);

    // Some units should be dead or damaged
    EXPECT_LT(scenario->getLivingUnitCount(), 4);

    // Verify no unit has negative health
    for (size_t i = 0; i < scenario->getUnitCount(); ++i) {
        auto unit = scenario->getUnit(i);
        EXPECT_GE(unit->getHealth(), 0);
    }
}

TEST_F(CombatIntegrationTest, RangedVsMeleeAdvantage) {
    auto archer = createArcher();  // Range 4
    auto footman = createFootman(); // Range 1

    // In a real scenario, archer would have range advantage
    // For this test, we simulate the archer getting free shots

    // Archer attacks first (range advantage)
    archer->attack(footman.get());
    int footmanHealthAfterFirstShot = footman->getHealth();

    // Archer attacks again before footman can close distance
    archer->attack(footman.get());
    int footmanHealthAfterSecondShot = footman->getHealth();

    // Now footman closes distance and attacks
    footman->attack(archer.get());

    EXPECT_LT(footmanHealthAfterSecondShot, footmanHealthAfterFirstShot);
    EXPECT_LT(archer->getHealth(), archer->getMaxHealth());

    // Archer should have done significant damage before taking any
    EXPECT_LT(footman->getHealthPercentage(), archer->getHealthPercentage());
}

TEST_F(CombatIntegrationTest, EliteUnitVsBasicUnits) {
    auto knight = createKnight();  // Elite unit

    scenario->addUnit(std::move(knight));
    scenario->addUnit(createPeasant()); // Index 1
    scenario->addUnit(createPeasant()); // Index 2
    scenario->addUnit(createPeasant()); // Index 3

    std::vector<size_t> eliteGroup = {0};      // Knight alone
    std::vector<size_t> peasantGroup = {1, 2, 3}; // Three peasants

    scenario->simulateCombat(eliteGroup, peasantGroup);

    // Knight should survive against multiple peasants
    auto knightPtr = scenario->getUnit(0);
    EXPECT_FALSE(knightPtr->isDead());

    // Some or all peasants should be dead
    int deadPeasants = 0;
    for (size_t i = 1; i <= 3; ++i) {
        if (scenario->getUnit(i)->isDead()) {
            deadPeasants++;
        }
    }
    EXPECT_GT(deadPeasants, 0);
}

TEST_F(CombatIntegrationTest, MageSupportRole) {
    auto mage = createMage();
    auto footman = createFootman();
    auto enemy = createKnight();

    // Damage the footman
    footman->takeDamage(30, 0);
    int footmanDamagedHealth = footman->getHealth();

    // Mage heals ally
    mage->castHeal(footman.get());
    EXPECT_GT(footman->getHealth(), footmanDamagedHealth);

    // Mage attacks enemy with fireball
    int enemyInitialHealth = enemy->getHealth();
    mage->castFireball(enemy.get());
    EXPECT_LT(enemy->getHealth(), enemyInitialHealth);

    // Demonstrate mage's support role effectiveness
    EXPECT_GT(footman->getHealthPercentage(), 0.8f); // Well healed
    EXPECT_LT(enemy->getHealthPercentage(), 0.9f);   // Damaged by fireball
}