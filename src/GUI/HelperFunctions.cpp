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

#include "HelperFunctions.h"


namespace gui {
	class SpriteRenderInfo {
	public:
		// Texture is always rendered completely. 
		SpriteRenderInfo(const Texture* t) {
			// The actual size we want to render, directly passed to ImGui::Image
			m_icon.x = (float)t->getWidth();
			m_icon.y = (float)t->getHeight();

			my_tex_id = (void*)t->getSDLTexture(); // Our SDL_Texture holding the data


			/* ImVec2(0,0) and ImVec2(1,1) would render the complete texture.
			 *  However, our sprite likely represents just a specific part inside the texture.
			 *  Hence, we have to calculate the correct start and end vector
			 */
			float my_tex_w = (float)t->getWidth();
			float my_tex_h = (float)t->getHeight();

			m_start.x = 0.0f;
			m_start.y = 0.0f;

			m_end.x = 1.0f;
			m_end.y = 1.0f;
		}

		// Sprites may be a subpart of an texture
		SpriteRenderInfo(const Sprite& sprite) {
			const Texture* t = sprite.getTexture();

			// The actual size we want to render, directly passed to ImGui::Image
			m_icon.x = (float)sprite.getWidth();
			m_icon.y = (float)sprite.getHeight();

			my_tex_id = (void*)t->getSDLTexture(); // Our SDL_Texture holding the data


			/* ImVec2(0,0) and ImVec2(1,1) would render the complete texture.
			 *  However, our sprite likely represents just a specific part inside the texture.
			 *  Hence, we have to calculate the correct start and end vector
			 */
			float my_tex_w = (float)t->getWidth();
			float my_tex_h = (float)t->getHeight();

			m_start.x = sprite.getOffsetTextureX() / my_tex_w;
			m_start.y = sprite.getOffsetTextureY() / my_tex_h;

			m_end.x = (sprite.getOffsetTextureX() + sprite.getWidth()) / my_tex_w;
			m_end.y = (sprite.getOffsetTextureY() + sprite.getHeight()) / my_tex_h;
		}

		ImTextureID my_tex_id;
		ImVec2 m_icon, m_start, m_end;
	};

	void renderImage(const Texture* pTexture) {
		SpriteRenderInfo r(pTexture);
		ImGui::Image(r.my_tex_id, r.m_icon, r.m_start, r.m_end, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	}

	// renderImage and renderImageButton could be optimized by somehow saving that a specific sprite was already rendered and re-use this information
	// However, using the Texture* as a key does not work as most sprites are based on the same texture...
	void renderImage(const Sprite& sprite) {
		SpriteRenderInfo r(sprite);
		ImGui::Image(r.my_tex_id, r.m_icon, r.m_start, r.m_end, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
	}


	bool renderImageButton(const Sprite& sprite, const ImVec4& c) {
		SpriteRenderInfo r(sprite);

		ImGui::PushStyleColor(ImGuiCol_Button, c);
		bool rVal = ImGui::ImageButton(r.my_tex_id, r.m_icon, r.m_start, r.m_end);
		ImGui::PopStyleColor();
		return rVal;
}


	void renderProgressBar(float progress) {
		float progress_saturated = progress;
		char buf[32];
#ifdef _WIN32         
		sprintf_s(buf, "%d/%d", (int)(progress_saturated * 1753), 1753); // Todo: hitpoints?
#else
		snprintf(buf, sizeof(buf), "%d/%d", (int)(progress_saturated * 1753), 1753);
#endif

		if (progress > 0.7) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));    // Green
		}
		else if (progress > 0.3) {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.647f, 0.0f, 1.0f));  // Orange
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));  // Red
		}

		const float iconWidth_px = 54.0f;
		ImGui::ProgressBar(progress, ImVec2(iconWidth_px, 2.f), buf);
		ImGui::PopStyleColor();
	}
}
