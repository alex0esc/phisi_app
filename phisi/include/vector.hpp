namespace phisi {
  
  struct Vector2D {
    float m_x;
    float m_y;
    
    float length();
    Vector2D operator+(const Vector2D& other);
    Vector2D operator-(const Vector2D& other);
    Vector2D operator*(const float other);
    Vector2D operator/(const float other);
  };

}