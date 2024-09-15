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
#include <cassert>
#include <sstream>


namespace gui {
	void ObjectInfoWindow::onLeftClicked(const Point& tile_world) {
		std::vector<Action> vecActions = m_pCurrSelectedObject->getActions();
		assert(vecActions.size()); // There must be a selected object and a corresponding action

		Action& action = vecActions[m_selectedAction];

		assert(action.getActionType() == Action::ActionType::WithWorldTile);
		action.trigger(tile_world);

		m_selectedAction = -1;
	}


	void ObjectInfoWindow::onRightClicked(const Point& tile_world) {
		m_selectedAction = -1;

		for (Object* o : m_selectedObjects) {
			Unit* pUnit = dynamic_cast<Unit*>(o);
			if (pUnit) {
				std::vector<Action> actions = pUnit->getActions();

				auto it = std::find_if(actions.begin(), actions.end(),
					[](const Action& a) { return a.getEventID() == (EventID)GeneralActions::Move; });

				if (it != actions.end()) {
					g_pEventBroker.lockEventProcessor((*it).getEventProcessorID());
					(*it).trigger(tile_world);
				}
			}
		}

		// Just play the "Of course", "Yes my lord" etc. sounds for the main selected unit
		// Otherwise, e.g., 20 units are selected, we would get a loud scratchy acknowledge as 20 sound files would be played simultanously
		if (m_pCurrSelectedObject)
			m_pCurrSelectedObject->playSoundAcknowledge();
	}


	void ObjectInfoWindow::draw() {
		if (!m_visible)
			return;

		if (ImGui::IsKeyDown(ImGuiKey_Escape)) {
			m_selectedAction = -1;
		}

		if (!m_selectedObjects.size())
			return;

		if (!m_selectedObjects.contains(m_pCurrSelectedObject)) // The selected unit just died or is initially a nullptr
			m_pCurrSelectedObject = *m_selectedObjects.begin(); // Simply take the first unit of the remaining ones as the new selected unit


	  //  ImGui::SetNextWindowPos(ImGui::GetIO().MousePos, ImGuiCond_Always);
		ImGui::Begin("Unit Selection Window");


		/*
		const int rightButton = 1;
		if(ImGui::IsMouseReleased(rightButton)) {
			m_visible = false; // todo richtig machen
		}
		*/




		// heal als zaubeer: https://wowpedia.fandom.com/wiki/Paladin_(Warcraft_II)

		// Render icons for all selected units

		ImVec2 button_sz(40, 40);
		ImGuiStyle& style = ImGui::GetStyle();
		float window_visible_x2 = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;

		int pushID = 0;
		for (Object* pObj : m_selectedObjects) {
			ImGui::PushID(pushID);

			ImGui::BeginGroup(); // To group icon width progressbar

			ImColor c = ImColor(66, 150, 250, 102);
			if (pObj == m_pCurrSelectedObject) {
				c = ImColor(45, 150, 255, 255); // Highlight currently selected unit from list individually
			}

			if (renderImageButton(pObj->icon(), c)) {
				m_pCurrSelectedObject = pObj;
				m_pCurrSelectedObject->onClicked();
			}
			renderProgressBar(pObj->getRelativeHealth());
			ImGui::EndGroup();

			//ImGui::Button("Box", button_sz);
			float last_button_x2 = ImGui::GetItemRectMax().x;
			float next_button_x2 = last_button_x2 + style.ItemSpacing.x + button_sz.x; // Expected position if next button was on same line
			if (pushID + 1 < m_selectedObjects.size() && next_button_x2 < window_visible_x2)
				ImGui::SameLine();

			++pushID;
			ImGui::PopID();
		}

		// Render general information valid for units and buildings
		ImGui::Separator();
		assert(m_pCurrSelectedObject);

		ImGui::TextUnformatted((m_pCurrSelectedObject->getName()).c_str());
		ImGui::SameLine();
		if (m_pCurrSelectedObject->getFraction() == Fraction::Alliance)
			ImGui::Text("(Alliance)");
		else if (m_pCurrSelectedObject->getFraction() == Fraction::Orc)
			ImGui::Text("(Orc)");
		else
			ImGui::Text("(Neutral)");


		ImGui::SameLine();
		ImGui::TextUnformatted(("(" + m_pCurrSelectedObject->getOwner()->getName() + ")").c_str());
		ImGui::Separator();


		std::string strHealth = "Health: " + std::to_string(m_pCurrSelectedObject->getHealthPoints()) + '/' + std::to_string(m_pCurrSelectedObject->getMaxHealthPoints());
		ImGui::Text("%s", strHealth.c_str());


		if (dynamic_cast<Unit*>(m_pCurrSelectedObject)) {
			renderUnitInfo();
		}
		else if (dynamic_cast<Building*>(m_pCurrSelectedObject)) {
			renderBuildingInfo();
		}
		else {
			throw std::logic_error("Unit is neither a unit nor a building!");
		}

		renderActions();
		renderBuildingProgressIndicatorAndEvents();


		ImGui::NewLine();
		ImGui::Button("Close");
		ImGui::End();
	}


	void ObjectInfoWindow::renderActions() {
		const int maxActionsPerRow = 3;
		std::vector<Action> vecActions = m_pCurrSelectedObject->getActions();

		if (!vecActions.size())
			return;

		ImGui::Separator();
		ImGui::Text("Actions:"); // Problem: Empty actions!

		for (int i = 0; i < vecActions.size(); ++i) {
			if (i % 3 != 0)
				ImGui::SameLine();

			Action& action = vecActions[i];

			if (action.getActionType() == Action::ActionType::EmptyAction) {
				ImGui::Dummy(ImVec2(1.0, 1.0));
				continue;
			}

			/*
			 * 04.12.2021: So far there is a Bug within ImageButton: It uses the textureID
			 * as internal ID what is okay if there is just one button. However, all our button icons
			 * are located inside the same texture -> only the first one is clickable.
			 *
			 * Workaround: PushID, see https://github.com/ocornut/imgui/issues/2464
			 */
			ImGui::PushID((int)i + 7653);

			ImVec4 c = (i == m_selectedAction) ? ImVec4(255, 0, 0, 102) : DefaultButtonColor;

			if (renderImageButton(action.icon(), c)) {
				g_pEventBroker.lockEventProcessor(action.getEventProcessorID());
				g_pSoundManager->playWAV("ui/click.wav");

				Building* pBuilding = dynamic_cast<Building*>(m_pCurrSelectedObject);
				Peasant* pPeasant = dynamic_cast<Peasant*>(m_pCurrSelectedObject);

				// Building actions can be executed immediately as they to not have a target tile
				if (pBuilding != nullptr) {
					assert(action.getActionType() == Action::ActionType::TriggerOnly);
					action.trigger();
				}
				/**  else if (pPeasant != nullptr) {
					   // sub menus??
				   }*/
				else {
					if (action.getActionType() == Action::ActionType::TriggerOnly)
						action.trigger();
					else {
						// The selected object is a land or air unit. The action must be connected to a tile the user left clicks on
						m_selectedAction = i;
					}
				}
			}

			ImGui::PopID();
		}
	}


	void ObjectInfoWindow::renderArmedInfo() {
		// Building: Armor, damage, range sight

		ImGui::Separator();
		std::stringstream ssArmor;
		ssArmor << "Armor : " << m_pCurrSelectedObject->getArmor();
		ImGui::Text("%s", ssArmor.str().c_str());

		if (m_pCurrSelectedObject->m_upgradeStats.additionalArmor()) {
			//if (pArmed->additionalArmor()) {
			ImGui::SameLine();
			ImGui::TextColored(ImColor(45, 150, 255, 255), "%s", ("(+" + std::to_string(m_pCurrSelectedObject->m_upgradeStats.additionalArmor()) + ")").c_str());
		}

		const IArmed* pArmed = dynamic_cast<const IArmed*>(m_pCurrSelectedObject);
		if (pArmed && pArmed->isArmed()) { // Actually only the Scout Tower implements IArmed and is unarmed without upgrade...
			int basicDamage = pArmed->getBasicDamage();
			int basicPiercing = pArmed->getBasicPiercingDamage();

			ImGui::Text("%s", ("Damage: " + std::to_string(basicPiercing) + "-" + std::to_string(basicDamage + basicPiercing)).c_str());

			if (m_pCurrSelectedObject->m_upgradeStats.additionalPiercingDamage()) {
				ImGui::SameLine();
				ImGui::TextColored(ImColor(45, 150, 255, 255), "%s", ("(+" + std::to_string(m_pCurrSelectedObject->m_upgradeStats.additionalPiercingDamage()) + ")").c_str());
			}

			ImGui::Text("%s", ("Range : " + std::to_string(pArmed->attackRange())).c_str());
			if (m_pCurrSelectedObject->m_upgradeStats.additionalRange()) {
				ImGui::SameLine();
				ImGui::TextColored(ImColor(45, 150, 255, 255), "%s", ("(+" + std::to_string(m_pCurrSelectedObject->m_upgradeStats.additionalRange()) + ")").c_str());
			}

			if (pArmed->canAttackAirUnits()) {
				ImGui::SameLine();
				ImGui::TextColored(ImColor(45, 150, 255, 255), "Can attack air units");
			}
		}
	}


	void ObjectInfoWindow::renderUnitInfo() {
		const Critter* pCritter = dynamic_cast<const Critter*>(m_pCurrSelectedObject);

		// Special case: Critter is the only land unit that is NOT armed
		// Hierarchy is: Object <- LandUnit (Armed) <- Critter
		// We prevent rendering any armed information for critter by using the cast
		if (pCritter == nullptr)
			renderArmedInfo();

		std::string strSight = "Sight : " + std::to_string(m_pCurrSelectedObject->getSight());
		ImGui::Text("%s", strSight.c_str());
		if (m_pCurrSelectedObject->m_upgradeStats.additionalSight()) {
			ImGui::SameLine();
			ImGui::TextColored(ImColor(45, 150, 255, 255), "%s", ("(+" + std::to_string(m_pCurrSelectedObject->m_upgradeStats.additionalSight()) + ")").c_str());
		}

		std::string strSpeed = "Speed : " + std::to_string(dynamic_cast<const Unit*>(m_pCurrSelectedObject)->getSpeed());
		ImGui::Text("%s", strSpeed.c_str());

		// Magical?
		const IMagical* pMagical = dynamic_cast<const IMagical*>(m_pCurrSelectedObject);

		if (pMagical != nullptr) {
			ImGui::Separator();
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.204f, 0.443f, 0.651f, 1.0f));
			char buf[32];
#ifdef _WIN32 
			sprintf_s(buf, sizeof(buf), "%i", pMagical->getMana());
#else 
			snprintf(buf, sizeof(buf), "%i", pMagical->getMana());
#endif
			ImGui::ProgressBar((float)pMagical->getMana() / (float)pMagical->getMaxMana(), ImVec2(0.0f, 0.0f), buf);
			ImGui::PopStyleColor();
		}
		}


	void ObjectInfoWindow::renderBuildingInfo() {
		// Iface quelle
		const GoldMine* pGoldMine = dynamic_cast<const GoldMine*>(m_pCurrSelectedObject);
		if (pGoldMine) {
			std::stringstream ssInfoGold;
			ssInfoGold << "Gold left: " << dynamic_cast<const IResourceMine*>(pGoldMine)->availableResources().gold();
			static ImVec4 colorGold = ImVec4(1.0f, 0.84f, 0.0f, 1.00f);
			ImGui::TextColored(colorGold, "%s", ssInfoGold.str().c_str());
		}


		const Farm* pFarm = dynamic_cast<const Farm*>(m_pCurrSelectedObject);
		if (pFarm) {
			ImGui::Text("Food Usage:");
			ImGui::TextUnformatted(("Grown:  " + std::to_string(pFarm->getOwner()->getAvailableFood())).c_str());
			ImGui::TextUnformatted(("Used :  " + std::to_string(pFarm->getOwner()->getUsedFood())).c_str());
		}


		const Townhall* pTownhall = dynamic_cast<const Townhall*>(m_pCurrSelectedObject);
		if (pTownhall) {
			const Resources& rBasic = pTownhall->getOwner()->getBasicResourceProduction();
			const Resources& rAdditional = pTownhall->getOwner()->getAdditionResourceProduction();

			ImGui::Text("Production:");
			ImGui::TextUnformatted(("Gold  : " + std::to_string(rBasic.gold()) + " +" + std::to_string(rAdditional.gold())).c_str()); // auftrennen und mit color das +?
			ImGui::TextUnformatted(("Lumber: " + std::to_string(rBasic.lumber()) + " +" + std::to_string(rAdditional.lumber())).c_str());
			ImGui::TextUnformatted(("Oil   : " + std::to_string(rBasic.oil()) + " +" + std::to_string(rAdditional.oil())).c_str());
		}

		// arrow und cannon tower -> render armed info  
		//renderArmedInfo
	}


	void ObjectInfoWindow::renderBuildingProgressIndicatorAndEvents() {
		// Progress Indicator
		const BuildingEventProcessor* pEH = dynamic_cast<const BuildingEventProcessor*>(m_pCurrSelectedObject);

		if (pEH && pEH->isEventInProgress()) {
			ImGui::Separator();
			ImGui::Text("Current Event Progress:");

			Sprite sprite = dynamic_cast<const BuildingEventProcessor*>(m_pCurrSelectedObject)->getCurrentEventIcon();
			renderImage(sprite);

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
			ImGui::ProgressBar(pEH->getRelativeProgressOfCurrentEvent(), ImVec2(0.0f, 0.0f));   // % Completed
			ImGui::PopStyleColor();
			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
		}


		// Queued Events
		if (pEH && pEH->isEventInProgress()) {
			if (g_pEventBroker.isEventProcessorLocked(pEH->getID()))
				return;

			std::vector<Sprite> vecIcons = dynamic_cast<const BuildingEventProcessor*>(m_pCurrSelectedObject)->getQueuedEventIcons();

			if (vecIcons.size()) {
				ImGui::NewLine();
				ImGui::Separator();
				ImGui::Text("Queued Events");

				for (const auto& sprite : vecIcons) {
					renderImage(sprite);
					ImGui::SameLine();
				}
			}
		}
	}
	}
