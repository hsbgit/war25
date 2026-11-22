#pragma once

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace PathSearch {

template<typename Node>
struct Result {
	std::vector<Node*> path;
	std::unordered_set<Node*> open;
	std::unordered_set<Node*> closed;
	Node* endpoint = nullptr;
	bool reachedGoal = false;
};

template<typename Node>
struct QueueEntry {
	int f;
	int h;
	Node* node;
};

template<typename Node>
struct LowestScoreFirst {
	bool operator()(const QueueEntry<Node>& lhs, const QueueEntry<Node>& rhs) const {
		if (lhs.f != rhs.f) return lhs.f > rhs.f;
		if (lhs.h != rhs.h) return lhs.h > rhs.h;
		if (lhs.node->getY() != rhs.node->getY())
			return lhs.node->getY() > rhs.node->getY();
		return lhs.node->getX() > rhs.node->getX();
	}
};

// Generic A* core. Keeping map-specific passability outside makes the actual
// routing algorithm deterministic and directly testable without constructing a game Map.
template<typename Node, typename Neighbours, typename CanVisit, typename StepCost, typename Heuristic>
Result<Node> find(Node* start, Node* end, Neighbours neighboursFor,
	CanVisit canVisit, StepCost stepCost, Heuristic heuristic,
	bool returnClosestReachable = false) {
	Result<Node> result;
	if (!start || !end || start == end)
		return result;

	std::priority_queue<QueueEntry<Node>, std::vector<QueueEntry<Node>>, LowestScoreFirst<Node>> openQueue;
	std::unordered_map<Node*, int> scores;
	std::unordered_map<Node*, Node*> parents;
	scores[start] = 0;
	openQueue.push({heuristic(start), heuristic(start), start});
	result.open.insert(start);
	Node* closest = start;
	int closestHeuristic = heuristic(start);
	int closestScore = 0;

	auto buildPath = [&parents, start](Node* endpoint) {
		std::vector<Node*> path;
		for (Node* node = endpoint; node != start; node = parents.at(node))
			path.push_back(node);
		std::reverse(path.begin(), path.end());
		return path;
	};

	while (!openQueue.empty()) {
		const QueueEntry<Node> entry = openQueue.top();
		openQueue.pop();
		Node* current = entry.node;
		if (result.closed.contains(current))
			continue;
		result.open.erase(current);
		result.closed.insert(current);
		const int currentHeuristic = heuristic(current);
		if (currentHeuristic < closestHeuristic ||
			(currentHeuristic == closestHeuristic && scores[current] < closestScore)) {
			closest = current;
			closestHeuristic = currentHeuristic;
			closestScore = scores[current];
		}

		if (current == end) {
			result.path = buildPath(current);
			result.endpoint = current;
			result.reachedGoal = true;
			break;
		}

		for (const auto& [direction, neighbour] : neighboursFor(current)) {
			if (!neighbour || result.closed.contains(neighbour) ||
				!canVisit(current, neighbour))
				continue;
			const int tentative = scores[current] + stepCost(direction, neighbour);
			auto old = scores.find(neighbour);
			if (old != scores.end() && tentative >= old->second)
				continue;
			scores[neighbour] = tentative;
			parents[neighbour] = current;
			const int h = heuristic(neighbour);
			openQueue.push({tentative + h, h, neighbour});
			result.open.insert(neighbour);
		}
	}
	if (!result.reachedGoal && returnClosestReachable && closest != start) {
		result.path = buildPath(closest);
		result.endpoint = closest;
	}
	return result;
}

} // namespace PathSearch
