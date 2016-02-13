#include "XToon.h"
#include <cmath>

using namespace std;

XToon::ShaderState XToon::state(){
	return _state;
}

XToon::XToon(const std::string& textureFileName, const Vec3f& lightpos, Camera* c){
	SetEasyBMPwarningsOff();
	texture.ReadFromFile(textureFileName.c_str());
	if (texture.TellHeight() != 256 || texture.TellWidth() != 256){
		cout << texture.TellWidth() << " " << texture.TellHeight() << endl;
		texture.SetSize(256, 256);
	}
	this->light = lightpos;
	this->camera = c;
}

Vec3f XToon::lightPos(){
	return camera->getV(light);
}

//initialize program with vertex and fragment shader and load texture
bool XToon::initProgram(const string& vert, const string& frag){
	try {
		BMPtexture.ImportBMP(texture);
		glGenTextures(1, &texName); // Génération d’une texture OpenGL
		glBindTexture(GL_TEXTURE_2D, texName); // Activation de la texture comme texture courante
		// les 4 lignes suivantes paramètre le filtrage de texture ainsi que sa répétition au-delà du carré unitaire
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// La commande suivante remplit la texture (sur GPU) avec les données de l’image
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BMPtexture.TellWidth(), BMPtexture.TellHeight(),0, GL_RGB, GL_UNSIGNED_BYTE, BMPtexture.Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texName);
		glprog = Program::genVFProgram("Simple GL Program", vert, frag);
		glprog->setUniform1i("texsample", 0);
		return true;
	}
	catch (Exception & e) {
		ofstream myfile;
		myfile.open("XToon_shader_error_log.txt");
		cerr << e.msg() << endl;
		myfile << e.msg() << endl;
		myfile.close();
		return false;
	}
}

XToon::~XToon(){}

//D = 1−log(z/zmin)/log(zmax/zmin)
void XToon::setForDepth(float* zmin, float* zmax, bool enableShader){
	this->_zmax = zmax;
	this->_zmin = zmin;
	this->zmin = *_zmin;
	this->zmax = *_zmax;
	if (enableShader){
		initProgram("shader.vert","XToon_depth.frag");
		glprog->setUniform1f("zmin", this->zmin);
		glprog->setUniform1f("zmax", this->zmax);
		glprog->setUniform3f("light", light[0], light[1], light[2]);
		glprog->use(); // Activate the shader program
		_state = DEPTH;
	}
	else
		_state = CPUDEPTH;
}
//D =1−log(z / z−min) / log(z−max / z−min) if z < zc and log(z / z+max) / log(z+min / z+max) if z > zc
// z±min = zc ± zmin and z±max = zc ± r*zmin
void XToon::setForFocus(float* zfocal, float* zmin, float* zmax, bool enableShader){
	this->_zmax = zmax;
	this->_zmin = zmin;
	this->_zc = zfocal;
	this->zmin = *_zmin;
	this->zmax = *_zmax;
	this->zc = *_zc;
	if (enableShader){
		initProgram("shader.vert", "XToon_focus.frag");
		glprog->setUniform1f("zmin", this->zmin);
		glprog->setUniform1f("zmax", this->zmax);
		glprog->setUniform1f("zfoc", this->zc);
		glprog->setUniform3f("light", light[0], light[1], light[2]);
		glprog->use(); // Activate the shader program
		_state = FOCUS;
	}
	else
		_state = CPUFOCUS;
}
//D = |n*v|^r
void XToon::setForSilhouette(float* r, bool enableShader){
	this->_zc = r;
	this->zc = *_zc;
	if (enableShader){
		initProgram("shader.vert", "XToon_silhouette.frag");
		glprog->setUniform1f("r", this->zc);
		glprog->setUniform3f("light", light[0], light[1], light[2]);
		glprog->use(); // Activate the shader program
		_state = SILHOUETTE;
	}
	else
		_state = CPUSILHOUETTE;
}
//D = |r*v|^s
void XToon::setForHighlight(float* s, bool enableShader){
	this->_zc = s;
	this->zc = *_zc;
	if (enableShader){
		initProgram("shader.vert", "XToon_highlight.frag");
		glprog->setUniform1f("s", this->zc);
		glprog->setUniform3f("light", light[0], light[1], light[2]);
		glprog->use(); // Activate the shader program
		_state = HIGHLIGHT;
	}
	else
		_state = CPUHIGHLIGHT;
}

void XToon::refresh(){
	switch (_state){
	case XToon::DEPTH:
		refreshForDepth();
		break;
	case XToon::FOCUS:
		refreshForFocus();
		break;
	case XToon::SILHOUETTE:
		refreshForSilhouette();
		break;
	case XToon::HIGHLIGHT:
		refreshForHighlight();
		break;
	default:
		zmin = *_zmin;
		zmax = *_zmax;
		zc = *_zc;
	}
}

void XToon::refreshForDepth(){
	zmin = *_zmin;
	zmax = *_zmax;
	glprog->setUniform1f("zmin", zmin);
	glprog->setUniform1f("zmax", zmax);
}
void XToon::refreshForFocus(){
	zmin = *_zmin;
	zmax = *_zmax;
	zc = *_zc;
	glprog->setUniform1f("zmin", zmin);
	glprog->setUniform1f("zmax", zmax);
	glprog->setUniform1f("zfoc", zc);
}
void XToon::refreshForSilhouette(){
	zc = *_zc;
	glprog->setUniform1f("r", this->zc);
}
void XToon::refreshForHighlight(){
	zc = *_zc;
	glprog->setUniform1f("s", this->zc);
}

//return value between 0..1
float XToon::getLambertian(const Vec3f& p, const Vec3f& n) {
	return max(0.f, dot(normalize(lightPos() - p), n));
}

//return value between 0..1, used after proper set and for the get function below 
float XToon::getForDepth(const Vec3f& p){
	float z = camera->getZ(p);
	return 1 - log(z / zmin) / log(zmax / zmin);
}
float XToon::getForFocus(const Vec3f& p){
	Vec3f campos;
	camera->getPos(campos);
	float z = (p - campos).length();
	if (z > zc+zmin){
		return log(z / (zc + zmax)) / log((zc + zmin) / (zc + zmax));
	}
	else if (z < zc-zmin){
		return 1 - log(z / (zc - zmin)) / log((zc - zmax) / (zc - zmin));
	}
	else{
		return 1;
	}
}

// n normal, v normalized view vector
float XToon::getForSilhouette(const Vec3f& p, const Vec3f& n){
	Vec3f campos;
	camera->getPos(campos);
	Vec3f v = normalize(campos - p);
	return pow(abs(dot(n, v)), zc);
}

// n normal, v normalized view vector
float XToon::getForHighlight(const Vec3f& p, const Vec3f& n){
	Vec3f campos;
	camera->getPos(campos);
	Vec3f v = normalize(campos - p);
	Vec3f l = normalize(lightPos() - p);
	Vec3f r = dot(n, l)*n + cross(cross(l, n), n);
	return pow(abs(dot(r, v)), zc);
}

Vec3f XToon::get(const Vec3f& p, const Vec3f& n, float dim2){
	return get(getLambertian(p,n),dim2);
}

Vec3f XToon::get(float dim1, float dim2){
	int i = (int)(min(max(dim2,0.f),1.f) * (256. - 0.000000001)),
		j = (int)(min(max(dim1,0.f),1.f) * (256. - 0.000000001));
	return bTof(get(i,j));
}

Vec3b XToon::get(int w, int h){
	RGBApixel* p = texture(h,w);
	return Vec3b(p->Red, p->Green, p->Blue);
}

Vec3f XToon::bTof(const Vec3b& in){
	return Vec3f(in[0] / 255.f, in[1] / 255.f, in[2] / 255.f);
}
