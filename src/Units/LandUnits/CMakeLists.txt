# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
add_subdirectory(Dwarfes)
add_subdirectory(Peasant)

SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/Archer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Archer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Ballista.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Ballista.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Footman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Footman.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Knight.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Knight.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Mage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Mage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Skeleton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Skeleton.h
    PARENT_SCOPE)


