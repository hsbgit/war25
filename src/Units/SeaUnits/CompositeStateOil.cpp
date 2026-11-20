#include "CompositeStateOil.h"

#include "Buildings/OilRig.h"
#include "EventHandling/StateMoving.h"
#include "Map/Map.h"
#include "Player.h"
#include "Units/SeaUnits/Tanker.h"
#include "Units/LandUnits/Peasant/CompositeStateMining/StateEnterBuildingForDelivery.h"
#include "Units/LandUnits/Peasant/CompositeStateMining/StateReturningResource.h"
#include "Units/Resource/IResourceCollector.h"
#include <cmath>

namespace {

class StateMoveToOilRig final : public StateMoving {
public:
	StateMoveToOilRig(Tanker* tanker, State* idle) : StateMoving(tanker), m_idle(idle) {}

	void onEnter(const Event* event) override {
		if (event) {
			Tile* target = m_pOwner->getMap()->getTile({event->tileX(), event->tileY()});
			if (target && dynamic_cast<OilRig*>(target->getSeaOrLandUnit()))
				m_rigTile = target;
		}
		auto* rig = m_rigTile ? dynamic_cast<OilRig*>(m_rigTile->getSeaOrLandUnit()) : nullptr;
		if (!rig) {
			m_approach = nullptr;
			return;
		}
		m_approach = m_pOwner->getMap()->getFreeTileAroundBuilding(rig, m_pOwner);
		if (m_approach)
			setTargetTile(m_approach->getPos());
	}

	State* process() override {
		auto* rig = m_rigTile ? dynamic_cast<OilRig*>(m_rigTile->getSeaOrLandUnit()) : nullptr;
		if (!rig || rig->availableResources().oil() <= 0)
			return m_idle;
		if (!m_approach)
			onEnter(nullptr);
		if (!m_approach)
			return this;
		return StateMoving::process();
	}

	OilRig* selectedRig() const {
		return m_rigTile ? dynamic_cast<OilRig*>(m_rigTile->getSeaOrLandUnit()) : nullptr;
	}

private:
	State* m_idle;
	Tile* m_rigTile = nullptr;
	Tile* m_approach = nullptr;
};

class StateCollectOil final : public State {
public:
	StateCollectOil(Tanker* tanker, State* idle, StateMoveToOilRig* moveToRig)
		: State(tanker), m_idle(idle), m_moveToRig(moveToRig) {}

	void onEnter(const Event*) override {
		m_rigTile = nullptr;
		auto* rig = m_moveToRig->selectedRig();
		if (!rig || rig->isDead() || rig->getOwner() != m_pOwner->getOwner())
			return;

		// Tankers occupy 2x2 tiles. Comparing only their top-left tile rejected
		// valid approaches on the north and west side and sent them oscillating
		// around the platform. Compare both complete footprints instead.
		bool adjacent = false;
		for (Tile* tankerTile : m_pOwner->getOccupiedTiles()) {
			for (Tile* rigTile : rig->getOccupiedTiles()) {
				if (std::abs(tankerTile->getX() - rigTile->getX()) <= 1 &&
					std::abs(tankerTile->getY() - rigTile->getY()) <= 1) {
					adjacent = true;
					break;
				}
			}
			if (adjacent)
				break;
		}
		if (adjacent && rig->enter(dynamic_cast<IResourceCollector*>(m_pOwner)))
			m_rigTile = rig->getTile();
	}

	State* process() override {
		auto* collector = dynamic_cast<IResourceCollector*>(m_pOwner);
		if (collector->currentlyCarrying() == ResourceType::Oil)
			return transitionToNextState();
		auto* rig = m_rigTile ? dynamic_cast<OilRig*>(m_rigTile->getSeaOrLandUnit()) : nullptr;
		if (!rig || rig->isDead())
			return m_idle;
		return this;
	}

private:
	State* m_idle;
	StateMoveToOilRig* m_moveToRig;
	Tile* m_rigTile = nullptr;
};

} // namespace

CompositeStateOil::CompositeStateOil(Tanker* tanker) : State(tanker) {
	m_moveToRig = new StateMoveToOilRig(tanker, tanker->m_pIdle);
	m_collectOil = new StateCollectOil(tanker, tanker->m_pIdle,
		static_cast<StateMoveToOilRig*>(m_moveToRig));
	m_returnOil = new StateReturningResource(tanker, tanker->m_pIdle);
	m_deliverOil = new StateEnterBuildingForDelivery(tanker);
	static_cast<StateReturningResource*>(m_returnOil)->setDeliveryState(
		static_cast<StateEnterBuildingForDelivery*>(m_deliverOil));

	m_moveToRig->setNextState(m_collectOil);
	m_collectOil->setNextState(m_returnOil);
	m_returnOil->setNextState(m_deliverOil);
	m_deliverOil->setNextState(m_moveToRig);
	m_current = m_moveToRig;
}

CompositeStateOil::~CompositeStateOil() {
	delete m_deliverOil;
	delete m_returnOil;
	delete m_collectOil;
	delete m_moveToRig;
}

void CompositeStateOil::onEnter(const Event* event) {
	if (dynamic_cast<IResourceCollector*>(m_pOwner)->currentlyCarrying() == ResourceType::Oil) {
		m_current = m_returnOil;
		m_current->onEnter();
	} else {
		m_current = m_moveToRig;
		m_current->onEnter(event);
	}
}

void CompositeStateOil::onExit() {
	if (m_current)
		m_current->onExit();
	State::onExit();
}

State* CompositeStateOil::process() {
	m_current = m_current->checkForTransition();
	m_current = m_current->process();
	if (m_current == m_moveToRig || m_current == m_collectOil ||
		m_current == m_returnOil || m_current == m_deliverOil)
		return this;
	return m_current;
}
