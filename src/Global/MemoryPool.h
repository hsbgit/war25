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

#include <vector>
#include <memory>
#include <queue>

namespace War25::Memory {
    template<typename T>
    class ObjectPool {
    public:
        explicit ObjectPool(size_t initial_size = 100) {
            m_pool.reserve(initial_size);
            for (size_t i = 0; i < initial_size; ++i) {
                m_available.push(std::make_unique<T>());
            }
        }

        template<typename... Args>
        std::unique_ptr<T> acquire(Args&&... args) {
            std::unique_ptr<T> obj;

            if (!m_available.empty()) {
                obj = std::move(m_available.front());
                m_available.pop();
            } else {
                obj = std::make_unique<T>();
            }

            // Reset object state if needed
            if constexpr (requires { obj->reset(std::forward<Args>(args)...); }) {
                obj->reset(std::forward<Args>(args)...);
            }

            return obj;
        }

        void release(std::unique_ptr<T> obj) {
            if (obj && m_available.size() < m_max_pool_size) {
                m_available.push(std::move(obj));
            }
        }

        size_t available_count() const { return m_available.size(); }
        void set_max_pool_size(size_t size) { m_max_pool_size = size; }

    private:
        std::queue<std::unique_ptr<T>> m_available;
        std::vector<std::unique_ptr<T>> m_pool;
        size_t m_max_pool_size = 1000;
    };

    // Specialized pools for common game objects
    extern ObjectPool<class Projectile> projectilePool;
    extern ObjectPool<class Animation> animationPool;
}