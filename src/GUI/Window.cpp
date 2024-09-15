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
#include "Window.h"


namespace gui { 
    void ImGuiWindow::update() {
        const int rightButton = 1;
        if(ImGui::IsMouseReleased(rightButton)) {
            m_visible = false; // todo richtig machen
        }


        if(m_setPos) {
            ImGui::SetNextWindowPos(ImVec2((float)m_newPos.x, (float)m_newPos.y));
            m_setPos = false;
        }

        ImGui::SetNextWindowSize(ImVec2(0.0,0.0));
    }
}




