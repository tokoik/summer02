#if defined(_WIN32)
#  define _USE_MATH_DEFINES
#  define _CRT_SECURE_NO_WARNINGS
#  include <GL/glut.h>
#  if defined(near)
#    undef near
#  endif
#  if defined(far)
#    undef far
#  endif
#elif defined(__APPLE__) || defined(MACOSX)
#  define GL_SILENCE_DEPRECATION
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

/*
** 平行投影変換行列を求める
*/
void orthogonalMatrix(float left, float right,
                      float bottom, float top,
                      float near, float far,
                      GLfloat *matrix)
{
  float dx = right - left;
  float dy = top - bottom;
  float dz = far - near;

  matrix[ 0] =  2.0f / dx;
  matrix[ 5] =  2.0f / dy;
  matrix[10] = -2.0f / dz;
  matrix[12] = -(right + left) / dx;
  matrix[13] = -(top + bottom) / dy;
  matrix[14] = -(far + near) / dz;
  matrix[15] =  1.0f;
  matrix[ 1] = matrix[ 2] = matrix[ 3] = matrix[ 4] =
  matrix[ 6] = matrix[ 7] = matrix[ 8] = matrix[ 9] = matrix[11] = 0.0f;
}

/*
** 透視投影変換行列を求める
*/
void perspectiveMatrix(float left, float right,
                       float bottom, float top,
                       float near, float far,
                       GLfloat *matrix)
{
  float dx = right - left;
  float dy = top - bottom;
  float dz = far - near;

  matrix[ 0] =  2.0f * near / dx;
  matrix[ 5] =  2.0f * near / dy;
  matrix[ 8] =  (right + left) / dx;
  matrix[ 9] =  (top + bottom) / dy;
  matrix[10] = -(far + near) / dz;
  matrix[11] = -1.0f;
  matrix[14] = -2.0f * far * near / dz;
  matrix[ 1] = matrix[ 2] = matrix[ 3] = matrix[ 4] =
  matrix[ 6] = matrix[ 7] = matrix[12] = matrix[13] = matrix[15] = 0.0f;
}
