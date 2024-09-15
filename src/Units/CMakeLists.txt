# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
add_subdirectory(AirUnits)
add_subdirectory(LandUnits)
add_subdirectory(SeaUnits)
add_subdirectory(Resource)

SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/AttackerStats.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Critter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Critter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IArmed.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IArmed.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Projectile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Projectile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Unit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Unit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/UnitActions.h
    PARENT_SCOPE)

