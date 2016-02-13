#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <algorithm>
#include "EasyBMP/EasyBMP.h"
#include "EasyBMP/EasyBMP_OpenGL.h"
#include "Vec3.h"
#include "GLProgram.h"

class XToon
{
public:
	enum ShaderState{ NONE, DEPTH, FOCUS, SILHOUETTE, HIGHLIGHT, CPUDEPTH, CPUFOCUS, CPUSILHOUETTE, CPUHIGHLIGHT };

	ShaderState state();

	XToon(const std::string& textureFileName, const Vec3f& lightpos);
	~XToon();

	//choose and set shader with parameters
	//D = 1−log(z/zmin)/log(zmax/zmin)
	void setForDepth(float* zmin, float* zmax, bool enableShader = false);
	//D =1−log(z / z−min) / log(z−max / z−min) if z < zc and log(z / z+max) / log(z+min / z+max) if z > zc
	// z±min = zc ± zmin and z±max = zc ± zmax
	void setForFocus(float* zfocal, float* zmin, float* zmax, bool enableShader = false);
	//D = |n*v|^r
	void setForSilhouette(float* r, bool enableShader = false);
	//D = |r*v|^s
	void setForHighlight(float* s, bool enableShader = false);

	//refresh shader parameters in GPU.
	void refresh();

	//1st dimension value for shader by CPU
	//--  return value between 0..1
	float getLambertian(const Vec3f& p, const Vec3f& n);
	
	//2nd dimension value for shader by CPU
	//--  return value between 0..1, used after proper set and for the second argument of the get function below
	float getForDepth(float z);
	float getForFocus(float z);
	//--  n normal, v normalized view vector
	float getForSilhouette(const Vec3f& n, const Vec3f& v);
	float getForHighlight(const Vec3f& p, const Vec3f& n, const Vec3f& v);
	
	//per-vertex texture rendering by CPU
	//--  dim1,dim2 = 0..1
	Vec3f get(const Vec3f& p, const Vec3f& n, float dim2);
	Vec3f get(float dim1, float dim2);
	Vec3b get(int w, int h);

private:
	ShaderState _state = NONE;
	Program * glprog;
	BMP texture;
	EasyBMP_Texture BMPtexture;
	GLuint texName; // Identifiant opengl de la texture
	float *_zmax, *_zmin , *_zc;
	float zmax, zmin, zc;
	Vec3f light;
	bool initProgram(const std::string& vert, const std::string& frag);
	Vec3f bTof(const Vec3b& in);
	void refreshForDepth();
	void refreshForFocus();
	void refreshForSilhouette();
	void refreshForHighlight();
};

