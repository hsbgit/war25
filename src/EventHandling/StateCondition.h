/*
	Copyright (C) 2024 P. Last

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once


class StateCondition {
public:
	StateCondition() { }
	virtual ~StateCondition() { }
	virtual bool isTrue() const = 0;
};



// The way to go: https://stackoverflow.com/questions/4530588/passing-operator-as-a-parameter
template<class T, class U>
class ConditionThreshold : public StateCondition {
public:
	const T& m_Val;
	T m_Soll;

	const U& m_lambda;

	ConditionThreshold(T& val, const T& soll, const U& lambda) : StateCondition(), m_Val(val), m_Soll(soll), m_lambda(lambda) {
		//std::logical_or
		// https://stackoverflow.com/questions/4530588/passing-operator-as-a-parameter
	}

	bool isTrue() const override {
		return m_lambda(m_Val, m_Soll);
	}
};



// Returns true if the called 'm_lambda' returns true
template<class U>
class ConditionBoolean : public StateCondition {
public:
	const U& m_lambda;

	ConditionBoolean(const U& lambda) : StateCondition(), m_lambda(lambda) {

	}

	bool isTrue() const override {
		return m_lambda();
	}
};

