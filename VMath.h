#pragma once

//Include our math headers
#include <d3dx9.h>
#include <cmath>

typedef D3DXVECTOR2 Vec2;
typedef D3DXVECTOR3 Vec3;
typedef D3DXVECTOR4 Vec4;
typedef D3DXMATRIXA16 Mat4;   // NOTE: D3DXMATRIX is being used by Graphics
typedef D3DXQUATERNION Quat;



namespace OBALFramework
{
  
  

  /*
  void Mat4Concatenate(Mat4 * pResult, Mat4 * pMat0, Mat4 * pMat1)
  {
    Mat4 temp;

    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    int m = 4;
    int n = 4;

    int s = 4;
    int t = 4;

    FLOAT result = 0.0f;

    for (i = 0; i < m; ++i)
    {
      for (j = 0; j < t; ++j)
      {
        for (k = 0; k < s; ++k)
        {
          result += pMat0->m[i][k] * pMat1->m[k][j];
        }

        temp.m[i][j] = result;
        result = 0;
      }
    }

    for (i = 0; i < m; ++i)
    {
      for (j = 0; j < n; ++j)
      {
        pResult->m[i][j] = temp.m[i][j];
      }
    }
  }
  */
}