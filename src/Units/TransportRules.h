#pragma once

namespace TransportRules {

inline bool shouldCancelPendingBoarding(bool commandIsInternalBoardingMove) {
	return !commandIsInternalBoardingMove;
}

} // namespace TransportRules
