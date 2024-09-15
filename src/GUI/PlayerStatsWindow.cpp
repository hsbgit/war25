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
#include "PlayerStatsWindow.h"
#include "Player.h"
#include <sstream>
#include <iomanip>


namespace gui {
	void PlayerStatsWindow::draw() {
		static bool show_another_window = true;

		if (!show_another_window)
			return;

		ImGui::Begin("Player Statistics", &show_another_window);

		std::stringstream ssInfoMap;
		ssInfoMap << "Explored area in percent: " << std::setprecision(2) << m_pPlayer->getMapInfo()->exploredAreaInPercent();
		ImGui::Text("%s", ssInfoMap.str().c_str());

		if (ImGui::CollapsingHeader("Icon Textures")) {
			/*
				for (std::size_t i = 0; i < m_pResManager->cntIcontextures(); i++)
				{
					try {
						const Texture* t = m_pResManager->getIconTexture(m_pPlayer, i);
						ImTextureID my_tex_id = (void*)t->textureID;
						float my_tex_w = (float)t->width;
						float my_tex_h = (float)t->height;


						ImGui::PushID(i);
						int frame_padding = 2; // default padding
						//ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
						if (ImGui::ImageButton(my_tex_id,  ImVec2(my_tex_w, my_tex_h), ImVec2(0,0), ImVec2(1,1), frame_padding, ImColor(0,0,0,255)))
						{
							std::cout<<"button pressed"<<std::endl;
							//   pressed_count += 1;
						}
						ImGui::PopID();

						ImGui::SameLine();

						std::stringstream ssId;
						ssId<< std::setw(3) <<std::setfill('0')<<i;
						ImGui::Text(ssId.str().c_str());



						if((i== 0) || (i%10 !=0))
							ImGui::SameLine();


					} catch(...) {
						std::cerr<<"Texture id invalid"<<std::endl;
					}
				}
				*/
		}


		//  ImGui::NewLine();

		if (ImGui::Button("Close"))
			show_another_window = false;
		ImGui::End();
	}
}
