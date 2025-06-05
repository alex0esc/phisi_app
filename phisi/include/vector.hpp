namespace phisi {
  
  struct Vector2D {
    float m_x;
    float m_y;
    
    float length();
    Vector2D operator+(const Vector2D& other);
    Vector2D operator+=(const Vector2D& other);
    
    Vector2D operator-(const Vector2D& other);
    Vector2D operator-=(const Vector2D& other);
    
    Vector2D operator*(const float other);
    Vector2D operator*=(const float other);
    
    Vector2D operator/(const float other);
    Vector2D operator/=(const float other);
    
    float operator*(const Vector2D& other);
  };

  Vector2D operator*(const float other, const Vector2D& vec);
  
  
  float intersection(Vector2D a, Vector2D b, Vector2D c, Vector2D d);
  Vector2D reflect(Vector2D a, Vector2D b);

}