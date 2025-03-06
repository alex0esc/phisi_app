#include "vector.hpp"
#include <cmath>

namespace phisi {

  float Vector2D::length() {
    return std::sqrt(std::powf(m_x, 2) + std::powf(m_y, 2));
  }

  Vector2D Vector2D::operator+(const Vector2D& other) {
    return {m_x + other.m_x, m_y + other.m_y};
  }

  Vector2D Vector2D::operator-(const Vector2D& other) {
    return {m_x - other.m_x, m_y - other.m_y};
  }
  
  Vector2D Vector2D::operator*(const float other) {
    return {m_x * other, m_y * other};
  }
  
  Vector2D Vector2D::operator/(const float other) {
    return {m_x / other, m_y / other};
  }
}