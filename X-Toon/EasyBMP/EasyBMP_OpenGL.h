/*************************************************
*                                                *
*  EasyBMP Cross-Platform Windows Bitmap Library * 
*                                                *
*  Author: Paul Macklin                          *
*                                                *
*   email: pmacklin@math.uci.edu                 *
*                                                *
*    file: EasyBMP_OpenGL.h                      *
*    date: 8-27-2005                             *
* version: 1.05.00                               *
*                                                *
*   License: BSD (revised)                       *
* Copyright: 2005-2006 by the EasyBMP Project    * 
*                                                *
* description: EasyBMP add-on to interface with  *
*              OpenGL                            *
*                                                *
*************************************************/

#include "EasyBMP.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

using namespace std;

#ifndef _EasyBMP_OpenGL_h_
#define _EasyBMP_OpenGL_h_

int CalculateOpenGLlength( int InputLength );
void PadBMP( BMP& Input , int NewWidth , int NewHeight );
void OpenGLpadBMP( BMP& Input );
GLubyte* BMPtoTexture( BMP& Input );

class EasyBMP_Texture{
public:
 GLubyte* Texture;
 
 int Width;
 int Height; 
 
 int OriginalWidth;
 int OriginalHeight;
 
 GLfloat MaxX;
 GLfloat MaxY;
 
 EasyBMP_Texture();
 ~EasyBMP_Texture();
 
 int TellWidth( void );
 int TellHeight( void );

 RGBApixel GetPixel( int i, int j);
 bool SetPixel( int i, int j, RGBApixel Input );
 
 void SetSize( int NewWidth, int NewHeight );
 void ImportBMP( BMP& InputImage );
};

bool EasyBMP_Screenshot( const char* FileName );

bool EasyBMP_StartFilming( void );
bool EasyBMP_StartFilming( double fps );
bool EasyBMP_ContinueFilming( void );
bool EasyBMP_EndFilming( void );
bool EasyBMP_ToggleFilming( void );
bool EasyBMP_ToggleFilming( double fps );

#endif
