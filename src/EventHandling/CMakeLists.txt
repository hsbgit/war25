# war25 EventHandling component library
set(EVENTHANDLING_SOURCES
    EventBroker.cpp
    EventBroker.h
    EventProcessor.cpp
    EventProcessor.h
    BuildingEventProcessor.cpp
    BuildingEventProcessor.h
    Event.cpp
    Event.h
    EventHandlerID.h
    State.cpp
    State.h
    StateCondition.h
    StateDead.cpp
    StateDead.h
    StateIdle.cpp
    StateIdle.h
    StateMachine.cpp
    StateMachine.h
    StateMoving.cpp
    StateMoving.h
    StatePatrol.cpp
    StatePatrol.h
)

add_library(war25_eventhandling OBJECT ${EVENTHANDLING_SOURCES})
target_link_libraries(war25_eventhandling PUBLIC war25_common)
