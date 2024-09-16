# -----------------------------------------------------------------------------
# This project is licensed under the GNU General Public License v3.0.
# See the LICENSE file in the root directory or <https://www.gnu.org/licenses/>.
# -----------------------------------------------------------------------------
SET(SRC_LIST
    ${SRC_LIST}
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBroker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventBroker.h    
    ${CMAKE_CURRENT_SOURCE_DIR}/EventProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EventProcessor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildingEventProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BuildingEventProcessor.h          
    ${CMAKE_CURRENT_SOURCE_DIR}/Event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Event.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EventHandlerID.h
    ${CMAKE_CURRENT_SOURCE_DIR}/State.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/State.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StateCondition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StateDead.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StateDead.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StateIdle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StateIdle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StateMachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StateMachine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StateMoving.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StateMoving.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StatePatrol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StatePatrol.h
    PARENT_SCOPE)
