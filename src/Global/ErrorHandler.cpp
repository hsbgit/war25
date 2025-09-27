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
#include "ErrorHandler.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>

namespace War25::Error {
    namespace {
        void logToFile(const std::string& level, const std::string& message,
                      const std::source_location& location) {
            static std::ofstream logFile("war25_errors.log", std::ios::app);

            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);

            logFile << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                   << " [" << level << "] "
                   << location.file_name() << ":" << location.line()
                   << " in " << location.function_name()
                   << " - " << message << std::endl;
        }
    }

    void ErrorHandler::logWarning(const std::string& message,
                                 const std::source_location& location) {
        std::cerr << "[WARNING] " << message << " ("
                 << location.file_name() << ":" << location.line() << ")" << std::endl;
        logToFile("WARNING", message, location);
    }

    void ErrorHandler::logError(const std::string& message,
                               const std::source_location& location) {
        std::cerr << "[ERROR] " << message << " ("
                 << location.file_name() << ":" << location.line() << ")" << std::endl;
        logToFile("ERROR", message, location);
    }

    void ErrorHandler::logCritical(const std::string& message,
                                  const std::source_location& location) {
        std::cerr << "[CRITICAL] " << message << " ("
                 << location.file_name() << ":" << location.line() << ")" << std::endl;
        logToFile("CRITICAL", message, location);

        // Could throw exception or terminate based on configuration
        throw GameException(message, ErrorLevel::Critical, location);
    }

    void ErrorHandler::assertCondition(bool condition, const std::string& message,
                                      const std::source_location& location) {
        if (!condition) {
            logCritical("Assertion failed: " + message, location);
        }
    }
}