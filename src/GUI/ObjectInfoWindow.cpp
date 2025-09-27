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
#include "ObjectInfoWindow.h"

#include "HelperFunctions.h"
#include "EventHandling/EventBroker.h"
#include "Resources/SoundManager.h"
#include "Units/Unit.h"
#include "Units/Critter.h"
#include "Buildings/Goldmine.h"
#include "Buildings/Farm.h"
#include "Buildings/Townhall.h"
#include "Player.h"
#include "Units/LandUnits/Peasant/Peasant.h"
#include "Magic/IMagical.h"

// jetzt wo wir die Typen kennen:
#include "Global/Action.h"        // Action + Action::ActionType
#include "Global/Fractions.h"      // enum class Fraction

#include <algorithm>
#include <string>
#include <vector>

#include "Global/Constants.h"

namespace {
constexpr ImU32  kIconBorderSelected   = War25::Constants::ICON_BORDER_SELECTED;
constexpr ImU32  kIconBorderUnselected = War25::Constants::ICON_BORDER_UNSELECTED;
constexpr ImVec4 kBuffColor            = ImVec4(0.176f, 0.588f, 1.000f, 1.0f);
constexpr int    kActionsPerRow        = War25::Constants::ACTIONS_PER_ROW;
} // namespace

namespace gui {

// --------- Auswahl vor/zurück (per Tab/Shift+Tab) ----------
void ObjectInfoWindow::cycleSelection(int delta) {
    if (m_selectedObjects.empty()) return;
    ensureValidCurrentSelection();
    if (!m_pCurrSelectedObject) return;

    std::vector<Object*> order(m_selectedObjects.begin(), m_selectedObjects.end());
    auto it  = std::find(order.begin(), order.end(), m_pCurrSelectedObject);
    int idx  = (it == order.end()) ? 0 : static_cast<int>(std::distance(order.begin(), it));
    int size = static_cast<int>(order.size());
    idx = (idx + delta) % size;
    if (idx < 0) idx += size;
    m_pCurrSelectedObject = order[idx];

    if (m_pCurrSelectedObject) {
        m_pCurrSelectedObject->playSoundSelected();
    }
}

void ObjectInfoWindow::onLeftClicked(const Point& tile_world) {
    if (!m_pCurrSelectedObject) return;
    auto actions = m_pCurrSelectedObject->getActions();
    if (!isValidActionIndex(m_selectedAction, actions)) return;

    auto& action = actions[m_selectedAction];
    if (action.getActionType() != Action::ActionType::WithWorldTile) {
        m_selectedAction = kNoAction;
        return;
    }
    action.trigger(tile_world);
    m_selectedAction = kNoAction;
}

void ObjectInfoWindow::playAcknowledgeDebounced(std::chrono::milliseconds minInterval) {
    const auto now = std::chrono::steady_clock::now();
    if (now - m_lastAcknowledgeTs >= minInterval) {
        if (m_pCurrSelectedObject) m_pCurrSelectedObject->playSoundSelected();
        m_lastAcknowledgeTs = now;
    }
}

void ObjectInfoWindow::onRightClicked(const Point& tile_world) {
    m_selectedAction = kNoAction;
    if (m_selectedObjects.empty()) return;

    for (Object* o : m_selectedObjects) {
        if (auto* pUnit = dynamic_cast<Unit*>(o)) {
            auto actions = pUnit->getActions();
            const auto it = std::find_if(actions.begin(), actions.end(),
                [](const Action& a){ return a.getEventID() == static_cast<EventID>(GeneralActions::Move); });
            if (it != actions.end()) {
                g_pEventBroker.lockEventProcessor(it->getEventProcessorID());
                it->trigger(tile_world);
            }
        }
    }
    // kein Krach bei Multi-Select
    playAcknowledgeDebounced(War25::Constants::SOUND_DEBOUNCE_INTERVAL);
}

void ObjectInfoWindow::draw() {
    if (!m_visible) return;

    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) m_selectedAction = kNoAction;
    if (ImGui::IsKeyPressed(ImGuiKey_Tab)) {
        if (ImGui::GetIO().KeyShift) cycleSelection(-1);
        else                         cycleSelection(+1);
    }

    if (m_selectedObjects.empty()) return;
    ensureValidCurrentSelection();
    if (!m_pCurrSelectedObject) return;

    if (!ImGui::Begin("Unit Selection Window")) {
        ImGui::End();
        return;
    }

    // --- Icon-Leiste ---
    const ImVec2 buttonSz(40, 40);
    const ImGuiStyle& style = ImGui::GetStyle();
    const float windowRight = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

    int id = 0;
    for (Object* pObj : m_selectedObjects) {
        ImGui::PushID(id);
        ImGui::BeginGroup();

        const ImU32 border = (pObj == m_pCurrSelectedObject) ? kIconBorderSelected : kIconBorderUnselected;
        if (renderImageButton(pObj->icon(), ImColor(border))) {
            m_pCurrSelectedObject = pObj;
            m_pCurrSelectedObject->onClicked();
        }
        renderProgressBar(pObj->getRelativeHealth());
        ImGui::EndGroup();

        const float lastX2 = ImGui::GetItemRectMax().x;
        const float nextX2 = lastX2 + style.ItemSpacing.x + buttonSz.x;
        if (id + 1 < static_cast<int>(m_selectedObjects.size()) && nextX2 < windowRight) ImGui::SameLine();

        ++id;
        ImGui::PopID();
    }

    // --- Basis-Infos ---
    ImGui::Separator();
    ImGui::TextUnformatted(m_pCurrSelectedObject->getName().c_str());
    ImGui::SameLine(); ImGui::Text("(%s)", factionLabel(m_pCurrSelectedObject->getFraction()));
    if (auto* owner = m_pCurrSelectedObject->getOwner()) {
        ImGui::SameLine(); ImGui::Text("(%s)", owner->getName().c_str());
    }
    ImGui::Separator();
    ImGui::Text("Health: %d/%d",
        m_pCurrSelectedObject->getHealthPoints(),
        m_pCurrSelectedObject->getMaxHealthPoints());

    // Typ-spezifisch
    if (dynamic_cast<Unit*>(m_pCurrSelectedObject)) {
        renderUnitInfo();
    } else if (dynamic_cast<Building*>(m_pCurrSelectedObject)) {
        renderBuildingInfo();
    } else {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1,0.6f,0.2f,1), "Unknown object type");
    }

    renderActions();
    renderBuildingProgressIndicatorAndEvents();

    ImGui::NewLine();
    if (ImGui::Button("Close")) {
        m_visible = false;
        m_selectedAction = kNoAction;
    }
    ImGui::End();
}

void ObjectInfoWindow::renderActions() {
    if (!m_pCurrSelectedObject) return;
    auto actions = m_pCurrSelectedObject->getActions();
    if (actions.empty()) return;

    ImGui::Separator();
    ImGui::TextUnformatted("Actions:");

    for (int i = 0; i < static_cast<int>(actions.size()); ++i) {
        if (i % kActionsPerRow != 0) ImGui::SameLine();

        auto& action = actions[i];
        if (action.getActionType() == Action::ActionType::EmptyAction) {
            ImGui::Dummy(ImVec2(1.0f, 1.0f));
            continue;
        }

        ImGui::PushID(i + 7653);
        const ImVec4 color = (i == m_selectedAction) ? ImVec4(1, 0, 0, 0.4f) : DefaultButtonColor;

        if (renderImageButton(action.icon(), color)) {
            g_pEventBroker.lockEventProcessor(action.getEventProcessorID());
            g_pSoundManager->playWAV("ui/click.wav");

            const auto* pBuilding = dynamic_cast<const Building*>(m_pCurrSelectedObject);
            if (pBuilding) {
                // Gebäudeaktionen: sofort
                if (action.getActionType() == Action::ActionType::TriggerOnly) {
                    action.trigger();
                } else {
                    // falls ein Gebäude doch ein Tile bräuchte (sollte nicht vorkommen):
                    m_selectedAction = i;
                }
            } else {
                if (action.getActionType() == Action::ActionType::TriggerOnly) {
                    action.trigger();
                } else { // WithWorldTile/SubMenus
                    m_selectedAction = i; // wartet auf Linksklick
                }
            }
        }

        // einfacher Tooltip (ausbaubar, wenn Action name/description hat)
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
            ImGui::BeginTooltip();
            ImGui::Text("Type: %d", static_cast<int>(action.getActionType()));
            ImGui::Text("EventID: %d", static_cast<int>(action.getEventID()));
            ImGui::EndTooltip();
        }

        ImGui::PopID();
    }
}

void ObjectInfoWindow::renderArmedInfo() {
    ImGui::Separator();
    ImGui::Text("Armor : %d", m_pCurrSelectedObject->getArmor());
    if (m_pCurrSelectedObject->m_upgradeStats.additionalArmor()) {
        ImGui::SameLine();
        ImGui::TextColored(kBuffColor, "(+%d)", m_pCurrSelectedObject->m_upgradeStats.additionalArmor());
    }

    if (const auto* pArmed = dynamic_cast<const IArmed*>(m_pCurrSelectedObject)) {
        if (pArmed->isArmed()) {
            const int baseDmg      = pArmed->getBasicDamage();
            const int basePiercing = pArmed->getBasicPiercingDamage();
            ImGui::Text("Damage: %d-%d", basePiercing, baseDmg + basePiercing);
            if (m_pCurrSelectedObject->m_upgradeStats.additionalPiercingDamage()) {
                ImGui::SameLine();
                ImGui::TextColored(kBuffColor, "(+%d)", m_pCurrSelectedObject->m_upgradeStats.additionalPiercingDamage());
            }

            ImGui::Text("Range : %d", pArmed->attackRange());
            if (m_pCurrSelectedObject->m_upgradeStats.additionalRange()) {
                ImGui::SameLine();
                ImGui::TextColored(kBuffColor, "(+%d)", m_pCurrSelectedObject->m_upgradeStats.additionalRange());
            }

            if (pArmed->canAttackAirUnits()) {
                ImGui::SameLine();
                ImGui::TextColored(kBuffColor, "Can attack air units");
            }
        }
    }
}

void ObjectInfoWindow::renderUnitInfo() {
    const auto* pCritter = dynamic_cast<const Critter*>(m_pCurrSelectedObject);
    if (!pCritter) renderArmedInfo();

    ImGui::Text("Sight : %d", m_pCurrSelectedObject->getSight());
    if (m_pCurrSelectedObject->m_upgradeStats.additionalSight()) {
        ImGui::SameLine();
        ImGui::TextColored(kBuffColor, "(+%d)", m_pCurrSelectedObject->m_upgradeStats.additionalSight());
    }

    if (const auto* pUnit = dynamic_cast<const Unit*>(m_pCurrSelectedObject)) {
        ImGui::Text("Speed : %d", pUnit->getSpeed());
    }

    if (const auto* pMagical = dynamic_cast<const IMagical*>(m_pCurrSelectedObject)) {
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.204f, 0.443f, 0.651f, 1.0f));
        char buf[32];
    #ifdef _WIN32
        sprintf_s(buf, sizeof(buf), "%d", pMagical->getMana());
    #else
        snprintf(buf, sizeof(buf), "%d", pMagical->getMana());
    #endif
        ImGui::ProgressBar(
            static_cast<float>(pMagical->getMana()) / static_cast<float>(pMagical->getMaxMana()),
            ImVec2(0.0f, 0.0f), buf);
        ImGui::PopStyleColor();
    }
}

void ObjectInfoWindow::renderBuildingInfo() {
    if (const auto* pGoldMine = dynamic_cast<const GoldMine*>(m_pCurrSelectedObject)) {
        const auto gold = static_cast<const IResourceMine*>(pGoldMine)->availableResources().gold();
        static const ImVec4 colorGold(1.0f, 0.84f, 0.0f, 1.0f);
        ImGui::TextColored(colorGold, "Gold left: %d", gold);
    }

    if (const auto* pFarm = dynamic_cast<const Farm*>(m_pCurrSelectedObject)) {
        ImGui::TextUnformatted("Food Usage:");
        ImGui::Text("Grown: %d", pFarm->getOwner()->getAvailableFood());
        ImGui::Text("Used : %d", pFarm->getOwner()->getUsedFood());
    }

    if (const auto* pTownhall = dynamic_cast<const Townhall*>(m_pCurrSelectedObject)) {
        const Resources& base = pTownhall->getOwner()->getBasicResourceProduction();
        const Resources& add  = pTownhall->getOwner()->getAdditionResourceProduction();

        ImGui::TextUnformatted("Production:");
        ImGui::Text("Gold  : %d +%d",   base.gold(),   add.gold());
        ImGui::Text("Lumber: %d +%d",   base.lumber(), add.lumber());
        ImGui::Text("Oil   : %d +%d",   base.oil(),    add.oil());
    }
}

void ObjectInfoWindow::renderBuildingProgressIndicatorAndEvents() {
    const auto* pEH = dynamic_cast<const BuildingEventProcessor*>(m_pCurrSelectedObject);
    if (!pEH) return;

    if (pEH->isEventInProgress()) {
        ImGui::Separator();
        ImGui::TextUnformatted("Current Event Progress:");
        const Sprite sprite = pEH->getCurrentEventIcon();
        renderImage(sprite);
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
        ImGui::ProgressBar(pEH->getRelativeProgressOfCurrentEvent(), ImVec2(0.0f, 0.0f));
        ImGui::PopStyleColor();
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    }

    if (!pEH->isEventInProgress()) return;
    if (g_pEventBroker.isEventProcessorLocked(pEH->getID())) return;

    const auto queued = pEH->getQueuedEventIcons();
    if (!queued.empty()) {
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::TextUnformatted("Queued Events");
        for (const auto& sprite : queued) {
            renderImage(sprite);
            ImGui::SameLine();
        }
    }
}

void ObjectInfoWindow::ensureValidCurrentSelection() {
    if (!m_pCurrSelectedObject) {
        if (!m_selectedObjects.empty())
            m_pCurrSelectedObject = *m_selectedObjects.begin();
        return;
    }
#if __cpp_lib_erase_if || __cplusplus >= 202002L
    if (!m_selectedObjects.contains(m_pCurrSelectedObject)) {
        m_pCurrSelectedObject = m_selectedObjects.empty() ? nullptr : *m_selectedObjects.begin();
    }
#else
    if (m_selectedObjects.find(m_pCurrSelectedObject) == m_selectedObjects.end()) {
        m_pCurrSelectedObject = m_selectedObjects.empty() ? nullptr : *m_selectedObjects.begin();
    }
#endif
}

const char* ObjectInfoWindow::factionLabel(Fraction f) {
    switch (f) {
        case Fraction::Alliance: return "Alliance";
        case Fraction::Orc:      return "Orc";
        default:                 return "Neutral";
    }
}

} // namespace gui
