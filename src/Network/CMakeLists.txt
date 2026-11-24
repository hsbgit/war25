set(NETWORK_SOURCES
    CommandTransport.h
    LocalNetworkSession.cpp
    LocalNetworkSession.h
    NetworkProtocol.h
)

add_library(war25_network OBJECT ${NETWORK_SOURCES})
target_link_libraries(war25_network PUBLIC war25_common)
