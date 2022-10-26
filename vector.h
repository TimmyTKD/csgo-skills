#pragma once
#include <numbers>
#include <cmath>

struct Vector3
{
    constexpr Vector3(
        const float x = 0.f,
        const float y = 0.f,
        const float z = 0.f) noexcept :
        x(x), y(y), z(z) { }

    constexpr const Vector3& operator-(const Vector3& other) const noexcept
    {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    constexpr const Vector3& operator+(const Vector3& other) const noexcept
    {
        return Vector3{ x + other.x, y + other.y, z + other.z };
    }

    constexpr const Vector3& operator/(const Vector3& other) const noexcept
    {
        return Vector3{ x / other.x, y / other.y, z / other.z };
    }

    constexpr const Vector3& operator*(const Vector3& other) const noexcept
    {
        return Vector3{ x * other.x, y * other.y, z * other.z };
    }

    float x, y, z;
};