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

#include "EasyBMP_OpenGL.h"

double EasyBMP_FPS;
int EasyBMP_FrameNumber;
clock_t EasyBMP_NextFrameClockTime;
clock_t EasyBMP_ClocksPerFrame;
bool EasyBMP_FilmingStarted = false;

int EasyBMP_Round( double input )
{
 double excess = input - floor( input );
 if( excess >= 0.5 )
 { return (int) ( floor(input) + 1.0 ); }
 return (int) floor( input );
}

int CalculateOpenGLlength( int InputLength )
{
 // find a power of 2 that meets or exceed InputLength

 int NewLength = 1;
 while( NewLength < InputLength )
 { NewLength *= 2; }
 
 return NewLength;
}

void CopyBMP( BMP& Input , BMP& Output )
{
 Output.SetSize( Input.TellWidth() , Input.TellHeight() );
 
 for( int j=0 ; j < Input.TellHeight() ; j++ )
 {
  for( int i=0 ; i < Input.TellWidth() ; i++ )
  {
   *Output(i,j) = *Input(i,j);
  }
 }
 return;
}

void PadBMP( BMP& Input , int NewWidth , int NewHeight )
{
 // if the NewWidth and NewHight are unchanged, exit
 
 if( NewWidth  == Input.TellWidth() && 
     NewHeight == Input.TellHeight() )
 { return; }
 
 // find max range for the copy, so that cropping occurs 
 // if necessary 
 
 int MaxWidth = Input.TellWidth();
 if( MaxWidth > NewWidth )
 { MaxWidth = NewWidth; }

 int MaxHeight = Input.TellHeight();
 if( MaxHeight > NewHeight )
 { MaxHeight = NewHeight; }
 
 // create a temporary image to hold the original pixels
 
 BMP Temp;
 Temp.SetSize(NewWidth,NewHeight);
 Temp.SetBitDepth( Input.TellBitDepth() );
 int i,j;
 int Difference = Temp.TellHeight() - MaxHeight;
 for( i=0 ; i < MaxWidth ; i++ )
 {
  for( j=0 ; j < MaxHeight ; j++ )
  {
   *Temp(i,j+Difference) = *Input(i,j);  
  }
 }
 
 // resize the original image, and recopy the pixels
 
 Input.SetSize(NewWidth,NewHeight);
 for( i=0 ; i < Input.TellWidth() ; i++ )
 {
  for( j=0 ; j < Input.TellHeight() ; j++ )
  {
   *Input(i,j) = *Temp(i,j);  
  }
 }

 Temp.SetSize(1,1);
 Temp.SetBitDepth(24);
 
 return;
}

void OpenGLpadBMP( BMP& Input )
{
 int NewWidth  = CalculateOpenGLlength( Input.TellWidth() );
 int NewHeight  = CalculateOpenGLlength( Input.TellHeight() );
 PadBMP( Input , NewWidth, NewHeight );
}

int EasyBMP_Texture::TellWidth( void )
{ return Width; }

int EasyBMP_Texture::TellHeight( void )
{ return Height; }

EasyBMP_Texture::EasyBMP_Texture()
{
 Width=1;
 Height=1;
 MaxX = 1.0;
 MaxY = 1.0;
 Texture = new GLubyte [4*Width*Height];
 return;
}

EasyBMP_Texture::~EasyBMP_Texture()
{
 delete [] Texture;
}

void EasyBMP_Texture::SetSize( int NewWidth , int NewHeight )
{
 Width=NewWidth;
 Height=NewHeight;
 delete [] Texture;
 Texture = new GLubyte [4*Width*Height];
 return;
}

GLubyte* BMPtoTexture( BMP& Input ) 
{
 OpenGLpadBMP( Input );

 GLubyte* Output;
 Output = new GLubyte [Input.TellWidth() * Input.TellHeight() * 3];

 int i,j,k;
 k=0;
 for( j=Input.TellHeight()-1 ; j >= 0 ; j-- )
 {
  for( i=0 ; i < Input.TellWidth() ; i++ )
  {
   Output[k] = (GLubyte) Input(i,j)->Red;k++;
   Output[k] = (GLubyte) Input(i,j)->Green;k++;
   Output[k] = (GLubyte) Input(i,j)->Blue;k++;
  }
 }

 return Output;
}

void EasyBMP_Texture::ImportBMP( BMP& InputImage )
{
 OriginalWidth = InputImage.TellWidth();
 OriginalHeight = InputImage.TellHeight();

 MaxX = (GLfloat) InputImage.TellWidth();
 MaxY = (GLfloat) InputImage.TellHeight();
 
 OpenGLpadBMP( InputImage );
 
 MaxX /= (GLfloat) InputImage.TellWidth(); 
 MaxY /= (GLfloat) InputImage.TellHeight(); 
 
 SetSize( InputImage.TellWidth() , InputImage.TellHeight() );
 
 if( Texture != NULL )
 { delete [] Texture; }

 Texture = BMPtoTexture( InputImage );
 return;
}

RGBApixel EasyBMP_Texture::GetPixel( int i, int j)
{
 RGBApixel Output;

 int I = i;
 int J = OriginalHeight-1-j;

 int k = (I + J*Width)*3;
 if( k > (Height*Width-1)*3 )
 { return Output; }

 Output.Red = (ebmpBYTE) Texture[k];
 k++;
 Output.Green = (ebmpBYTE) Texture[k];
 k++;
 Output.Blue = (ebmpBYTE) Texture[k];
 Output.Alpha = 255;
 return Output;
}

bool EasyBMP_Texture::SetPixel( int i, int j, RGBApixel Input )
{
 int I = i;
 int J = OriginalHeight-1-j;

 int k = (I + J*Width)*3;
 if( k > (Height*Width-1)*3 )
 { return false; }

 Texture[k] = (GLubyte) Input.Red;
 k++;
 Texture[k] = (GLubyte) Input.Green;
 k++;
 Texture[k] = (GLubyte) Input.Blue;

 return true;
}

bool EasyBMP_Screenshot( const char* FileName )
{
 BMP Output;

 GLsizei viewport[4];
 glGetIntegerv( GL_VIEWPORT , viewport );

 int width = viewport[2] - viewport[0];
 int height = viewport[3] - viewport[1];
 
 Output.SetSize(width,height);

 GLint swapbytes, lsbfirst, rowlength, skiprows, skippixels, alignment;

 /* Save current pixel store state. */
 glGetIntegerv(GL_UNPACK_SWAP_BYTES, &swapbytes);
 glGetIntegerv(GL_UNPACK_LSB_FIRST, &lsbfirst);
 glGetIntegerv(GL_UNPACK_ROW_LENGTH, &rowlength);
 glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skiprows);
 glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skippixels);
 glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);

 /* Set desired pixel store state. */

 glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE);
 glPixelStorei(GL_UNPACK_LSB_FIRST, GL_FALSE);
 glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
 glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
 glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
 glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

 RGBApixel* pixels;
 pixels = new RGBApixel [ Output.TellWidth() * Output.TellHeight() ];
 glReadPixels( viewport[0],viewport[1], width,height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
 
 int n=0;
 for( int j=Output.TellHeight()-1 ; j >= 0 ; j-- )
 {
  for( int i=0; i < Output.TellWidth() ; i++ )
  {
   Output(i,j)->Red = (pixels[n]).Blue;
   Output(i,j)->Green = (pixels[n]).Green;
   Output(i,j)->Blue = (pixels[n]).Red;
   n++;
  }
 }

 /* Restore current pixel store state. */
 glPixelStorei(GL_UNPACK_SWAP_BYTES, swapbytes);
 glPixelStorei(GL_UNPACK_LSB_FIRST, lsbfirst);
 glPixelStorei(GL_UNPACK_ROW_LENGTH, rowlength);
 glPixelStorei(GL_UNPACK_SKIP_ROWS, skiprows);
 glPixelStorei(GL_UNPACK_SKIP_PIXELS, skippixels);
 glPixelStorei(GL_UNPACK_ALIGNMENT, alignment); 
 
 
 Output.WriteToFile( FileName );

 return true;
}

bool EasyBMP_StartFilming( double fps )
{
 EasyBMP_FilmingStarted = true;

 EasyBMP_FrameNumber = 0;

 char FileName [1024];
 sprintf( FileName , "frame%06u.bmp" , EasyBMP_FrameNumber );
 EasyBMP_Screenshot( FileName );
 EasyBMP_FrameNumber++;

 EasyBMP_FPS = fps;

 EasyBMP_ClocksPerFrame = (clock_t) EasyBMP_Round(  (double) CLK_TCK  / (double) EasyBMP_FPS); 
 EasyBMP_NextFrameClockTime = clock() + EasyBMP_ClocksPerFrame; 
 return true;
}

bool EasyBMP_StartFilming( void )
{
 EasyBMP_FilmingStarted = true;

 EasyBMP_FrameNumber = 0;
 
 char FileName [1024];
 sprintf( FileName , "frame%06u.bmp" , EasyBMP_FrameNumber );
 EasyBMP_Screenshot( FileName );
 EasyBMP_FrameNumber++;
 
 EasyBMP_FPS = 15.0;

 EasyBMP_ClocksPerFrame = (clock_t) EasyBMP_Round(  (double) CLK_TCK  / (double) EasyBMP_FPS); 
 EasyBMP_NextFrameClockTime = clock() + EasyBMP_ClocksPerFrame; 
 return true;
}

bool EasyBMP_ContinueFilming( void )
{
 if( !EasyBMP_FilmingStarted )
 { return false; }

 if( clock() >= EasyBMP_NextFrameClockTime )
 {
  char FileName [1024];
  sprintf( FileName , "frame%06u.bmp" , EasyBMP_FrameNumber );
  EasyBMP_Screenshot( FileName );
  EasyBMP_FrameNumber++;
 
  EasyBMP_NextFrameClockTime = clock() + EasyBMP_ClocksPerFrame; 
  return true;
 }
 return false;
}

bool EasyBMP_EndFilming( void )
{
 EasyBMP_FilmingStarted = false;
 return true;
}

bool EasyBMP_ToggleFilming( void )
{
 if( EasyBMP_FilmingStarted )
 { EasyBMP_EndFilming(); }
 else
 { EasyBMP_StartFilming(); }
 return true;
}

bool EasyBMP_ToggleFilming( double fps )
{
 if( EasyBMP_FilmingStarted )
 { EasyBMP_EndFilming(); }
 else
 { EasyBMP_StartFilming( fps ); }
 return true;
}

