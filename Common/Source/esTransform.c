///
//  Includes
//
#include "esUtil.h"
#include <math.h>
#include <string.h>

#define PI 3.1415926535897932384626433832795f

void ESUTIL_API
esScale ( ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz )
{
   result->m[0][0] *= sx;
   result->m[0][1] *= sx;
   result->m[0][2] *= sx;
   result->m[0][3] *= sx;

   result->m[1][0] *= sy;
   result->m[1][1] *= sy;
   result->m[1][2] *= sy;
   result->m[1][3] *= sy;

   result->m[2][0] *= sz;
   result->m[2][1] *= sz;
   result->m[2][2] *= sz;
   result->m[2][3] *= sz;
}

void ESUTIL_API
esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz )
{
   result->m[3][0] += ( result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz );
   result->m[3][1] += ( result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz );
   result->m[3][2] += ( result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz );
   result->m[3][3] += ( result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz );
}

void ESUTIL_API
esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf ( x * x + y * y + z * z );

   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );

   if ( mag > 0.0f )
   {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      ESMatrix rotMat;

      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = ( oneMinusCos * xx ) + cosAngle;
      rotMat.m[0][1] = ( oneMinusCos * xy ) - zs;
      rotMat.m[0][2] = ( oneMinusCos * zx ) + ys;
      rotMat.m[0][3] = 0.0F;

      rotMat.m[1][0] = ( oneMinusCos * xy ) + zs;
      rotMat.m[1][1] = ( oneMinusCos * yy ) + cosAngle;
      rotMat.m[1][2] = ( oneMinusCos * yz ) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = ( oneMinusCos * zx ) - ys;
      rotMat.m[2][1] = ( oneMinusCos * yz ) + xs;
      rotMat.m[2][2] = ( oneMinusCos * zz ) + cosAngle;
      rotMat.m[2][3] = 0.0F;

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      esMatrixMultiply ( result, &rotMat, result );
   }
}

void ESUTIL_API
esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ )
{
   float       deltaX = right - left;
   float       deltaY = top - bottom;
   float       deltaZ = farZ - nearZ;
   ESMatrix    frust;

   if ( ( nearZ <= 0.0f ) || ( farZ <= 0.0f ) ||
         ( deltaX <= 0.0f ) || ( deltaY <= 0.0f ) || ( deltaZ <= 0.0f ) )
   {
      return;
   }

   frust.m[0][0] = 2.0f * nearZ / deltaX;
   frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

   frust.m[1][1] = 2.0f * nearZ / deltaY;
   frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

   frust.m[2][0] = ( right + left ) / deltaX;
   frust.m[2][1] = ( top + bottom ) / deltaY;
   frust.m[2][2] = - ( nearZ + farZ ) / deltaZ;
   frust.m[2][3] = -1.0f;

   frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
   frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

   esMatrixMultiply ( result, &frust, result );
}


void ESUTIL_API
esPerspective ( ESMatrix *result, float fovy, float aspect, float nearZ, float farZ )
{
   GLfloat frustumW, frustumH;

   frustumH = tanf ( fovy / 360.0f * PI ) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum ( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void ESUTIL_API
esOrtho ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ )
{
   float       deltaX = right - left;
   float       deltaY = top - bottom;
   float       deltaZ = farZ - nearZ;
   ESMatrix    ortho;

   if ( ( deltaX == 0.0f ) || ( deltaY == 0.0f ) || ( deltaZ == 0.0f ) )
   {
      return;
   }

   esMatrixLoadIdentity ( &ortho );
   ortho.m[0][0] = 2.0f / deltaX;
   ortho.m[3][0] = - ( right + left ) / deltaX;
   ortho.m[1][1] = 2.0f / deltaY;
   ortho.m[3][1] = - ( top + bottom ) / deltaY;
   ortho.m[2][2] = -2.0f / deltaZ;
   ortho.m[3][2] = - ( nearZ + farZ ) / deltaZ;

   esMatrixMultiply ( result, &ortho, result );
}


void ESUTIL_API
esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB )
{
   ESMatrix    tmp;
   int         i;

   for ( i = 0; i < 4; i++ )
   {
      tmp.m[i][0] =  ( srcA->m[i][0] * srcB->m[0][0] ) +
                     ( srcA->m[i][1] * srcB->m[1][0] ) +
                     ( srcA->m[i][2] * srcB->m[2][0] ) +
                     ( srcA->m[i][3] * srcB->m[3][0] ) ;

      tmp.m[i][1] =  ( srcA->m[i][0] * srcB->m[0][1] ) +
                     ( srcA->m[i][1] * srcB->m[1][1] ) +
                     ( srcA->m[i][2] * srcB->m[2][1] ) +
                     ( srcA->m[i][3] * srcB->m[3][1] ) ;

      tmp.m[i][2] =  ( srcA->m[i][0] * srcB->m[0][2] ) +
                     ( srcA->m[i][1] * srcB->m[1][2] ) +
                     ( srcA->m[i][2] * srcB->m[2][2] ) +
                     ( srcA->m[i][3] * srcB->m[3][2] ) ;

      tmp.m[i][3] =  ( srcA->m[i][0] * srcB->m[0][3] ) +
                     ( srcA->m[i][1] * srcB->m[1][3] ) +
                     ( srcA->m[i][2] * srcB->m[2][3] ) +
                     ( srcA->m[i][3] * srcB->m[3][3] ) ;
   }

   memcpy ( result, &tmp, sizeof ( ESMatrix ) );
}

void ESUTIL_API
esMatrixVectorMultiply(ESVector *result, ESMatrix *matrix, ESVector *vec)
{
	ESVector tmp;
	int i;
	for (i = 0; i < 4; i++)
	{
		tmp.v[i] = matrix->m[i][0] * vec->v[0] + matrix->m[i][1] * vec->v[1] + matrix->m[i][2] * vec->v[2] + matrix->m[i][3] * vec->v[3];
	}
	memcpy(result, &tmp, sizeof(ESVector));
}

GLboolean ESUTIL_API
esMatrixInvert(ESMatrix *src, ESMatrix *inverse)
{
	double t;
	int i, j, k, swap;
	GLfloat tmp[4][4];

	esMatrixLoadIdentity(inverse);

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			tmp[i][j] = src->m[i][j];
		}
	}

	for (i = 0; i < 4; i++) {
		/* look for largest element in column. */
		swap = i;
		for (j = i + 1; j < 4; j++) {
			if (fabs(tmp[j][i]) > fabs(tmp[i][i])) {
				swap = j;
			}
		}

		if (swap != i) {
			/* swap rows. */
			for (k = 0; k < 4; k++) {
				t = tmp[i][k];
				tmp[i][k] = tmp[swap][k];
				tmp[swap][k] = t;

				t = inverse->m[i][k];
				inverse->m[i][k] = inverse->m[swap][k];
				inverse->m[swap][k] = t;
			}
		}

		if (tmp[i][i] == 0) {
			/* no non-zero pivot.  the matrix is singular, which
			shouldn't happen.  This means the user gave us a bad
			matrix. */
			return GL_FALSE;
		}

		t = tmp[i][i];
		for (k = 0; k < 4; k++) {
			tmp[i][k] /= t;
			inverse->m[i][k] /= t;
		}
		for (j = 0; j < 4; j++) {
			if (j != i) {
				t = tmp[j][i];
				for (k = 0; k < 4; k++) {
					tmp[j][k] -= tmp[i][k] * t;
					inverse->m[j][k] -= inverse->m[i][k] * t;
				}
			}
		}
	}
	return GL_TRUE;
}

void ESUTIL_API
esMatrixLoadIdentity ( ESMatrix *result )
{
   memset ( result, 0x0, sizeof ( ESMatrix ) );
   result->m[0][0] = 1.0f;
   result->m[1][1] = 1.0f;
   result->m[2][2] = 1.0f;
   result->m[3][3] = 1.0f;
}

void ESUTIL_API
esMatrixLookAt ( ESMatrix *result,
                 float posX,    float posY,    float posZ,
                 float lookAtX, float lookAtY, float lookAtZ,
                 float upX,     float upY,     float upZ )
{
   float axisX[3], axisY[3], axisZ[3];
   float length;

   // axisZ = lookAt - pos
   axisZ[0] = lookAtX - posX;
   axisZ[1] = lookAtY - posY;
   axisZ[2] = lookAtZ - posZ;

   // normalize axisZ
   length = sqrtf ( axisZ[0] * axisZ[0] + axisZ[1] * axisZ[1] + axisZ[2] * axisZ[2] );

   if ( length != 0.0f )
   {
      axisZ[0] /= length;
      axisZ[1] /= length;
      axisZ[2] /= length;
   }

   // axisX = up X axisZ
   axisX[0] = upY * axisZ[2] - upZ * axisZ[1];
   axisX[1] = upZ * axisZ[0] - upX * axisZ[2];
   axisX[2] = upX * axisZ[1] - upY * axisZ[0];

   // normalize axisX
   length = sqrtf ( axisX[0] * axisX[0] + axisX[1] * axisX[1] + axisX[2] * axisX[2] );

   if ( length != 0.0f )
   {
      axisX[0] /= length;
      axisX[1] /= length;
      axisX[2] /= length;
   }

   // axisY = axisZ x axisX
   axisY[0] = axisZ[1] * axisX[2] - axisZ[2] * axisX[1];
   axisY[1] = axisZ[2] * axisX[0] - axisZ[0] * axisX[2];
   axisY[2] = axisZ[0] * axisX[1] - axisZ[1] * axisX[0];

   // normalize axisY
   length = sqrtf ( axisY[0] * axisY[0] + axisY[1] * axisY[1] + axisY[2] * axisY[2] );

   if ( length != 0.0f )
   {
      axisY[0] /= length;
      axisY[1] /= length;
      axisY[2] /= length;
   }

   memset ( result, 0x0, sizeof ( ESMatrix ) );

   result->m[0][0] = -axisX[0];
   result->m[0][1] =  axisY[0];
   result->m[0][2] = -axisZ[0];

   result->m[1][0] = -axisX[1];
   result->m[1][1] =  axisY[1];
   result->m[1][2] = -axisZ[1];

   result->m[2][0] = -axisX[2];
   result->m[2][1] =  axisY[2];
   result->m[2][2] = -axisZ[2];

   // translate (-posX, -posY, -posZ)
   result->m[3][0] =  axisX[0] * posX + axisX[1] * posY + axisX[2] * posZ;
   result->m[3][1] = -axisY[0] * posX - axisY[1] * posY - axisY[2] * posZ;
   result->m[3][2] =  axisZ[0] * posX + axisZ[1] * posY + axisZ[2] * posZ;
   result->m[3][3] = 1.0f;
}
