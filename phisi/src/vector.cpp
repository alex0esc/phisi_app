#include "vector.hpp"
#include <cmath>

namespace phisi {

  float Vector2D::length() {
    return std::sqrt(std::pow(m_x, 2) + std::pow(m_y, 2));
  }

  Vector2D Vector2D::operator+(const Vector2D& other) {
    return {m_x + other.m_x, m_y + other.m_y};
  }

  Vector2D Vector2D::operator+=(const Vector2D& other) {
    this->m_x += other.m_x;
    this->m_y += other.m_y;
    return *this;
  }
  
  
  Vector2D Vector2D::operator-(const Vector2D& other) {
    return {m_x - other.m_x, m_y - other.m_y};
  }

  Vector2D Vector2D::operator-=(const Vector2D& other) {
    this->m_x -= other.m_x;
    this->m_y -= other.m_y;
    return *this;
  }  

    
  Vector2D Vector2D::operator*(const float other) {
    return {m_x * other, m_y * other};
  }
  
  Vector2D Vector2D::operator*=(const float other) {
    this->m_x *= other;
    this->m_y *= other;
    return *this;
  }  
  
  Vector2D operator*(const float other, const Vector2D& vec) {
    return {vec.m_x * other, vec.m_y * other};
  }
  
  
  Vector2D Vector2D::operator/(const float other) {
    return {m_x / other, m_y / other};
  }
  
  Vector2D Vector2D::operator/=(const float other) {
    this->m_x /= other;
    this->m_y /= other;
    return *this;
  }
  
  
  float Vector2D::operator*(const Vector2D& other) {
    return m_x * other.m_x + m_y * other.m_y;
  }


  ///for k = 0 parallel or equal
  ///calculates the factor k of g : x = a + k * b wich gives the point of intersection with h : x = c + u * d 
  float intersection(Vector2D a, Vector2D b, Vector2D c, Vector2D d) {
    float div = d.m_x * b.m_y - d.m_y * b.m_x; 
    if (div == 0.0f)
      return 0.0f;
    return ((c.m_y - a.m_y) * d.m_x - (c.m_x - a.m_x) * d.m_y) / div;
  }

  ///a is the vector that is mirrowed
  Vector2D reflect(Vector2D a, Vector2D b) {
    return 2 * ((a * b) / (b * b)) * b - a;  
  }
}
