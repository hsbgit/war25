# war25 Magic component library
set(MAGIC_SOURCES
    Buff.cpp
    Buff.h
    IMagical.cpp
    IMagical.h
    Runes.cpp
    Runes.h
    SpellExecutor.cpp
    SpellExecutor.h
	SpellCosts.h
	SpellEffect.cpp
	SpellEffect.h
)

add_library(war25_magic OBJECT ${MAGIC_SOURCES})
target_link_libraries(war25_magic PUBLIC war25_common)
