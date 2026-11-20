#pragma once

#include "EventHandling/State.h"

class Tanker;

class CompositeStateOil : public State {
public:
	explicit CompositeStateOil(Tanker* tanker);
	~CompositeStateOil() override;

	void onEnter(const Event* event = nullptr) override;
	void onExit() override;
	State* process() override;

private:
	State* m_current = nullptr;
	State* m_moveToRig = nullptr;
	State* m_collectOil = nullptr;
	State* m_returnOil = nullptr;
	State* m_deliverOil = nullptr;
};
