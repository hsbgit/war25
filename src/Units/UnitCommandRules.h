#pragma once

namespace UnitCommandRules {

enum class Mode { Idle, Move, AttackMove, FocusAttack, StandGround, Patrol };

inline bool ignoresAttackersWhileMoving(Mode mode, bool isMoving) {
	return mode == Mode::Move && isMoving;
}

inline bool scansForTargetsWhileMoving(Mode mode) {
	return mode == Mode::AttackMove;
}

inline bool shouldInterruptPursuit(bool isMoving, bool targetInRange) {
	return isMoving && targetInRange;
}

} // namespace UnitCommandRules
