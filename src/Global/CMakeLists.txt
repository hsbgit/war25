# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/Action.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Action.h
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalAttributes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GlobalAttributes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Object.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/PseudoRandom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PseudoRandom.h  
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Resources.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductionItem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ProductionItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/TechnologyManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/TechnologyManager.h       
    ${CMAKE_CURRENT_SOURCE_DIR}/UpgradeEffect.cpp     
    ${CMAKE_CURRENT_SOURCE_DIR}/UpgradeEffect.h       
    ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Viewport.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/Upgrade.h  
    PARENT_SCOPE)
