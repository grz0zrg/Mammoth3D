#ifndef VECTOR3_HPP
#define VECTOR3_HPP

	#include <cmath>
	
	namespace core
	{
		class Vector3 {
			public:
				inline Vector3() { };
				inline Vector3(const float x, const float y = 0.0f, 
												const float z = 0.0f) {
					this->x = x;
					this->y = y;
					this->z = z;
				};
				
				inline Vector3 operator=(const Vector3& A ) {
					this->x = A.x;
					this->y = A.y;
					this->z = A.z;
					
					return *this;
				}
				
				inline Vector3 operator=(const float s) {
					this->x = s;
					this->y = s;
					this->z = s;
					
					return *this;
				}
				
				inline Vector3 operator + ( const Vector3& A ) const {
					return Vector3( this->x + A.x, this->y + A.y, this->z + A.z ); 
				}

				inline Vector3 operator + ( const float A ) const { 
					return Vector3( this->x + A, this->y + A, this->z + A ); 
				}
				
				inline Vector3 operator - ( const Vector3& A ) const {
					return Vector3( this->x - A.x, this->y - A.y, this->z - A.z ); 
				}

				inline Vector3 operator - ( const float A ) const { 
					return Vector3( this->x - A, this->y - A, this->z - A ); 
				}
				
				inline Vector3 cross(const Vector3 &A, const Vector3 &B)
				{
					return Vector3(A.y * B.z - A.z*B.y,
								A.z*B.x - A.x*B.z, 
								A.x*B.y - A.y*B.x);
				}
	
				inline float dot( const Vector3& A ) const {
					return A.x * this->x + A.y * this->y + A.z * this->z;
				}
				
				inline float dot( const Vector3& A, const Vector3& B ) const {
					return A.x * B.x + A.y * B.y + A.z * B.z;
				}
				
				inline float length( const Vector3& A ) const {
					return sqrt(A.x * A.x + A.y * A.y + A.z * A.z);
				}
				
				inline float length() {
					return sqrt(x * x + y * y + z * z);
				}
				
				inline float distance(const Vector3 &A, const Vector3 &B) {
					return length(A - B);
				}
				
				inline void normalize() {
					float norm = sqrt(this->x * this->x + this->y * 
										this->y + this->z * this->z);
					if (norm != 0) {
						this->x /= norm;
						this->y /= norm;
						this->z /= norm;
					} else {
						this->x = this->y = this->z = 0.0f;
					}
				}
				
				inline float norm() {
					return sqrt(this->x * this->x + this->y * this->y + 
														this->z * this->z);
				}
				
				float x, y, z;
		};
	}

#endif