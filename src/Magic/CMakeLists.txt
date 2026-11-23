# war25 Magic component library
set(MAGIC_SOURCES
	BombardmentEffect.cpp
	BombardmentEffect.h
    Buff.cpp
    Buff.h
	BuffRules.h
    IMagical.cpp
    IMagical.h
    Runes.cpp
    Runes.h
    SpellExecutor.cpp
    SpellExecutor.h
	SpellCosts.h
	SpellRules.h
	SpellEffect.cpp
	SpellEffect.h
)

add_library(war25_magic OBJECT ${MAGIC_SOURCES})
target_link_libraries(war25_magic PUBLIC war25_common)
