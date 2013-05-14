#ifndef MATRIX4_HPP
#define MATRIX4_HPP

	#include <cmath>
	
	namespace core {
		class Matrix4 {
			public:
				inline Matrix4(float n11 = 1.0f, float n12 = 0.0f, float n13 = 0.0f, float n14 = 0.0f, 
						float n21 = 0.0f, float n22 = 1.0f, float n23 = 0.0f, float n24 = 0.0f, 
						float n31 = 0.0f, float n32 = 0.0f, float n33 = 1.0f, float n34 = 0.0f, 
						float n41 = 0.0f, float n42 = 0.0f, float n43 = 0.0f, float n44 = 1.0f) {
					m[0 ] = n11; m[4 ] = n12; m[8 ] = n13; m[12 ] = n14;
					m[1 ] = n21; m[5 ] = n22; m[9 ] = n23; m[13 ] = n24;
					m[2 ] = n31; m[6 ] = n32; m[10] = n33; m[14 ] = n34;
					m[3 ] = n41; m[7 ] = n42; m[11] = n43; m[15 ] = n44;
				}
				
				inline void makeFrustum(float left, float right, 
									float bottom, float top, 
									float near, float far) {
					float x = 2 * near / ( right - left );
					float y = 2 * near / ( top - bottom );

					float a = (right + left) / (right - left);
					float b = (top + bottom) / (top - bottom);
					float c = -(far + near) / (far - near);
					float d = -2 * far * near / (far - near);

					m[0] = x; m[4] = 0; m[8 ] = a;  m[12] = 0;
					m[1] = 0; m[5] = y; m[9 ] = b;  m[13] = 0;
					m[2] = 0; m[6] = 0; m[10] = c;  m[14] = d;
					m[3] = 0; m[7] = 0; m[11] = -1; m[15] = 0;
				}
				
				inline void makeOrthographic(float left, float right, 
									float top, float bottom, 
									float near, float far) {
					float w = right - left;
					float h = top - bottom;
					float p = far - near;

					float x = (right + left) / w;
					float y = (top + bottom) / h;
					float z = (far + near) / p;

					m[0] = 2 / w; m[4] = 0;     m[8 ] = 0;    m[12] = -x;
					m[1] = 0;     m[5] = 2 / h; m[9 ] = 0;    m[13] = -y;
					m[2] = 0;     m[6] = 0;     m[10] = -2/p; m[14] = -z;
					m[3] = 0;     m[7] = 0;     m[11] = 0;    m[15] = 1;
				}
				
				inline Matrix4 operator * (const Matrix4& mb) const {
					core::Matrix4 matrix;
					
					float a11 = m[0], a12 = m[4], a13 = m[8 ], a14 = m[12];
					float a21 = m[1], a22 = m[5], a23 = m[9 ], a24 = m[13];
					float a31 = m[2], a32 = m[6], a33 = m[10], a34 = m[14];
					float a41 = m[3], a42 = m[7], a43 = m[11], a44 = m[15];

					float b11 = mb.m[0], b12 = mb.m[4], b13 = mb.m[8 ], b14 = mb.m[12];
					float b21 = mb.m[1], b22 = mb.m[5], b23 = mb.m[9 ], b24 = mb.m[13];
					float b31 = mb.m[2], b32 = mb.m[6], b33 = mb.m[10], b34 = mb.m[14];
					float b41 = mb.m[3], b42 = mb.m[7], b43 = mb.m[11], b44 = mb.m[15];

					matrix.m[0 ] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
					matrix.m[4 ] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
					matrix.m[8 ] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
					matrix.m[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

					matrix.m[1 ] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
					matrix.m[5 ] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
					matrix.m[9 ] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
					matrix.m[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

					matrix.m[2 ] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
					matrix.m[6 ] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
					matrix.m[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
					matrix.m[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

					matrix.m[3 ] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
					matrix.m[7 ] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
					matrix.m[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
					matrix.m[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;
					
					return matrix; 
				}
				
				inline void setPosition(float x, float y, float z) {
					m[12] = x;
					m[13] = y;
					m[14] = z;
				}
				
				inline void scale(float x, float y, float z) {
					m[0] *= x; m[4] *= y; m[8 ] *= z;
					m[1] *= x; m[5] *= y; m[9 ] *= z;
					m[2] *= x; m[6] *= y; m[10] *= z;
					m[3] *= x; m[7] *= y; m[11] *= z;
				}
				
				inline void makeRotationFromEuler(float x, float y, float z) {
					float a = cosf(x), b = sinf(x);
					float c = cosf(y), d = sinf(y);
					float e = cosf(z), f = sinf(z);
					
					float ac = a * c, ad = a * d, bc = b * c, bd = b * d;

					m[0] = c * e;
					m[4] = bd - ac * f;
					m[8] = bc * f + ad;

					m[1] = f;
					m[5] = a * e;
					m[9] = - b * e;

					m[2] = - d * e;
					m[6] = ad * f + bc;
					m[10] = ac - bd * f;

					m[3] = 0;
					m[7] = 0;
					m[11] = 0;

					m[12] = 0;
					m[13] = 0;
					m[14] = 0;
					m[15] = 1;
				}
				
			float m[16];
		};
	}

#endif