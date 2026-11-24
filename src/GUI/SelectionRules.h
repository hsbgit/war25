#pragma once

namespace gui::SelectionRules {

enum class DragObjectKind { OwnedUnit, Critter, Other };

inline bool keepInDragSelection(DragObjectKind kind, bool containsOwnedUnit) {
	return !containsOwnedUnit || kind == DragObjectKind::OwnedUnit;
}

} // namespace gui::SelectionRules
