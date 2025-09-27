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

#include <stdexcept>
#include <string>
#include <source_location>

namespace War25::Error {
    enum class ErrorLevel {
        Warning,
        Error,
        Critical
    };

    class GameException : public std::runtime_error {
    public:
        GameException(const std::string& message,
                     ErrorLevel level = ErrorLevel::Error,
                     const std::source_location& location = std::source_location::current())
            : std::runtime_error(message), m_level(level), m_location(location) {}

        ErrorLevel getLevel() const { return m_level; }
        const std::source_location& getLocation() const { return m_location; }

    private:
        ErrorLevel m_level;
        std::source_location m_location;
    };

    class ErrorHandler {
    public:
        static void logWarning(const std::string& message,
                              const std::source_location& location = std::source_location::current());
        static void logError(const std::string& message,
                            const std::source_location& location = std::source_location::current());
        static void logCritical(const std::string& message,
                               const std::source_location& location = std::source_location::current());

        static void assertCondition(bool condition, const std::string& message,
                                   const std::source_location& location = std::source_location::current());

        // Safe pointer checks
        template<typename T>
        static T* checkNotNull(T* ptr, const std::string& name = "pointer",
                              const std::source_location& location = std::source_location::current()) {
            if (!ptr) {
                throw GameException("Null pointer: " + name, ErrorLevel::Critical, location);
            }
            return ptr;
        }

        // Safe array bounds checks
        template<typename Container>
        static void checkBounds(const Container& container, size_t index,
                               const std::source_location& location = std::source_location::current()) {
            if (index >= container.size()) {
                throw GameException("Index " + std::to_string(index) + " out of bounds (size: " +
                                  std::to_string(container.size()) + ")", ErrorLevel::Error, location);
            }
        }
    };
}