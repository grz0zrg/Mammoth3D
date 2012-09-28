#ifndef VECTOR2_HPP
#define VECTOR2_HPP

	namespace core
	{
		class Vector2 {
			public:
				inline Vector2() { };
				inline Vector2(const float x, const float y = 0.0f) {
					this->x = x;
					this->y = y;
				};
				
				inline Vector2 operator=(const Vector2& A ) {
					this->x = A.x;
					this->y = A.y;
					
					return *this;
				}
				
				inline Vector2 operator=(const float s) {
					this->x = s;
					this->y = s;
					
					return *this;
				}
				
				inline Vector2 operator + ( const Vector2& A ) const {
					return Vector2( this->x + A.x, this->y + A.y ); 
				}

				inline Vector2 operator + ( const float A ) const { 
					return Vector2( this->x + A, this->y + A ); 
				}
				
				inline Vector2 operator - ( const Vector2& A ) const {
					return Vector2( this->x - A.x, this->y - A.y ); 
				}

				inline Vector2 operator - ( const float A ) const { 
					return Vector2( this->x - A, this->y - A ); 
				}
				
				inline float length( const Vector2& A ) const {
					return sqrt(A.x * A.x + A.y * A.y);
				}
				
				inline float distance(const Vector2 &A, const Vector2 &B) {
					return length(A - B);
				}
				
				inline void normalize() {
					float norm = sqrt(this->x * this->x + this->y * this->y);
					this->x /= norm;
					this->y /= norm;
				}
				
				inline float norm() {
					return sqrt(this->x * this->x + this->y * this->y);
				}
				
				float x, y;
		};
	}

#endif