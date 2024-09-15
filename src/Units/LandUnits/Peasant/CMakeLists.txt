# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
add_subdirectory(CompositeStateChopWood)
add_subdirectory(CompositeStateConstructBuilding)
add_subdirectory(CompositeStateMining)

SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/Peasant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Peasant.h
    PARENT_SCOPE)


