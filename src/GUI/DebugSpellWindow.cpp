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
#include "DebugSpellWindow.h"
#include "HelperFunctions.h"
#include "Map/Map.h"
#include "Magic/SpellExecutor.h"
#include "EventHandling/EventBroker.h"
#include "Resources/SoundManager.h"


namespace gui {
    void DebugSpellWindow::onLeftClicked(const Point& tile_world) {
        assert(m_pMap);
        // mana -= 200; 

        // A bit hacked but I think it's okay for a debug spell window...
        // Just do not change the button ordering ;-)
        switch (m_selectedSpell) {
        case 1:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), BuffType::Slow, tile_world.x, tile_world.y));
            break;

        case 3:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), BuffType::Invisibility, tile_world.x, tile_world.y));
            break;

        case 4:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), MageActions::Polymorph, tile_world.x, tile_world.y));
            break;

        case 7:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), BuffType::Haste, tile_world.x, tile_world.y));
            break;

        case 10:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), BuffType::UnholyArmor, tile_world.x, tile_world.y));
            break;

        case 15:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), PaladinActions::Heal, tile_world.x, tile_world.y));
            break;

        case 16:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), BuffType::Bloodlust, tile_world.x, tile_world.y));
            break;

        case 17:
            g_pEventBroker.addEvent(Event(gSpellExecutor.getID(), OgreActions::Runes, tile_world.x, tile_world.y));
            break;

        default:
            break;
        }

        m_selectedSpell = -1;
    }


    void DebugSpellWindow::draw() {
        if (!m_visible)
            return;

        if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
            m_selectedSpell = -1;
        }

        ImGui::Begin("Spells");
        PlayerColor c = PlayerColor::Neutral;


        std::vector<Sprite> vecSpellIcons;

        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_FIREBALL, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_SLOW, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_FLAME_SHIELD, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_INVISIBILITY, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_POLYMORPH, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_BLIZZARD, c));



        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_DEATH_AND_DECAY, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_HASTE, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_SKELETON, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_WHIRLWIND, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_UNHOLY_ARMOR, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_DEATH_COIL, c)); // Bug in pud.h -> PUD_ICON_DEATH_AND_DECAY and PUD_ICON_DEATH_COIL are mixed up (fixed in new version)


        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_HOLY_VISION, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_HEALING, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_EXORCISM, c));

        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_EYE_OF_KILROGG, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_BLOODLUST, c));
        vecSpellIcons.push_back(g_pRessourceManager->getIcon(PUD_ICON_RUNES, c));



        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.204f, 0.443f, 0.651f, 1.0f));
        char buf[32];
#ifdef _WIN32        
        sprintf_s(buf, "%i", mana);
#else
        snprintf(buf, sizeof(buf), "%i", mana);
#endif
        ImGui::ProgressBar((float)mana / (float)maxMana, ImVec2(0.0f, 0.0f), buf);
        ImGui::PopStyleColor();


        for (int i = 0; i < vecSpellIcons.size(); ++i) {
            if (i % 3 != 0)
                ImGui::SameLine();
            else
                ImGui::Separator();

            /*
             * 04.12.2021: So far there is a Bug within ImageButton: It uses the textureID
             * as internal ID what is okay if there is just one button. However, all our button icons
             * are located inside the same texture -> only the first one is clickable.
             *
             * Workaround: PushID, see https://github.com/ocornut/imgui/issues/2464
             */
            ImGui::PushID((int)i + 7653);

            ImVec4 c = (i == m_selectedSpell) ? ImVec4(255, 0, 0, 102) : DefaultButtonColor;

            if (renderImageButton(vecSpellIcons[i], c)) {
                //  g_pEventBroker->lockEventProcessor(action.getEventProcessorID());
                g_pSoundManager->playWAV("ui/click.wav");
                m_selectedSpell = i;
            }

            ImGui::PopID();
        }


        ImGui::NewLine();
        if (ImGui::Button("Reset Mana")) {
            mana = maxMana;
        }
        ImGui::SameLine();
        ImGui::Button("Close");
        ImGui::End();
    }
}

