// ----------------------------------------------
// PROJET INF584
// X-TOON NPR
// By Yuesong SHEN
// Implementation based on "X-Toon: An Extended Toon Shader"
// http://www.cs.ucr.edu/~vbz/cs230papers/x-toon.pdf
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>

#include "Vec3.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"
#include "XToon.h"
#include "EasyBMP/EasyBMP.h"

#define M_PI 3.14159265358979323846
#define DEG2RAD (M_PI/180)
using namespace std;

static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
//static const string DEFAULT_MESH_FILE ("models/mount.off");	//AP
static const string DEFAULT_MESH_FILE("models/bunny.off");	//normal

static string appTitle ("X-Toon NPR shading");
static GLint window;
static unsigned int FPS = 0;
static bool fullScreen = false;
static float nearplane = 1, farplane = 100, 
	zmind = 1, zmaxd = 100,
	zmin = 0.f,zmax = 2.5f,zfoc = 7,
	r = 2, s = 1;

static Camera camera(nearplane, farplane);
static Mesh mesh;

const int LIGHTSIZE = 2;
Light light0(Vec3f(1.,.9,.8), Vec3f(10.,10.,10.)), light1(Vec3f(0.,1.,.3),Vec3f(-10.,0.,-1.));
Light lights[LIGHTSIZE] = {light0,light1};

//XToon xtoon("texture2D/db2.bmp", light0.position);	//DEPTH
//XToon xtoon("texture2D/ap3.bmp", light0.position);	//DEPTH(AP)
//XToon xtoon("texture2D/fb2.bmp", light0.position);	//FOCUS
//XToon xtoon("texture2D/ns3.bmp", light0.position);	//SILHOUETTE
XToon xtoon("texture2D/hl1.bmp", light0.position);	//HIGHLIGHT

void printUsage () {
	std::cerr << std::endl 
		 << appTitle << std::endl
         << "By: Yuesong Shen" << std::endl << std::endl
		 << "Based on code provided by professor Tamy Boubekeur" << std::endl << std::endl
         << "Usage: ./main [<file.off>]" << std::endl
         << "Commands:" << std::endl 
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
		 << " w: Toggle wireframe mode" << std::endl
		 << " r: refocus (for depth/focus shader)"
         << " <left button drag>: rotate model" << std::endl 
         << " <right button drag>: move model" << std::endl
         << " <middle button drag>: zoom" << std::endl
		 << " <left button drag>+<right button click>: zoom" << std::endl
         << " q, <esc>: Quit" << std::endl << std::endl; 
}

void init(const char * modelFilename) {
	glewInit();
	glCullFace(GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
    glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
	glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
	// Background color
	//glClearColor (0.5f, 0.5f, 0.5f, 1.0f); 
    glClearColor (.8f, .8f, .8f, 1.0f);
	//glClearColor (.0f, .0f, .0f, 1.0f);
	
	//set xtoon

	//xtoon.setForDepth(&zmind, &zmaxd, true);		//DEPTH + shader implementation
	//xtoon.setForFocus(&zfoc, &zmin, &zmax, true);	//FOCUS + shader implementation
	//xtoon.setForSilhouette(&r, true);	//SILHOUETTE + shader implementation
	xtoon.setForHighlight(&s,true);	//HIGHLIGHT + shader implementation
	
	//xtoon.setForDepth(&zmind, &zmaxd);	//DEPTH no shader implementation
	//xtoon.setForFocus(&zfoc, &zmin, &zmax);	//FOCUS no shader implementation
	//xtoon.setForSilhouette(&r);	//SILHOUETTE no shader implementation
	//xtoon.setForHighlight(&s);	//HIGHLIGHT no shader implementation
	
	mesh.loadOFF(modelFilename);
    camera.resize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
}

void drawScene(){
	Vec3f clr, campos;
	camera.getPos(campos);
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < mesh.T.size(); i++) 
        for (unsigned int j = 0; j < 3; j++) {
            const Vertex & v = mesh.V[mesh.T[i].v[j]];

			//clr = xtoon.get(v.p, v.n, xtoon.getForDepth(camera.getZ(v.p)));	//DEPTH no shader implementation
			//clr = xtoon.get(v.p, v.n, xtoon.getForFocus((v.p-campos).length()));	//FOCUS no shader implementation
			//clr = xtoon.get(v.p, v.n, xtoon.getForSilhouette(v.n, normalize(campos - v.p)));	//SILHOUETTE no shader implementation
			//clr = xtoon.get(v.p, v.n, xtoon.getForHighlight(v.p,v.n, normalize(campos - v.p)));	//HIGHLIGHT no shader implementation

			//glColor3f (clr[0], clr[1], clr[2]);	//CPU rendering
            glNormal3f (v.n[0], v.n[1], v.n[2]); // Specifies current normal vertex   
            glVertex3f (v.p[0], v.p[1], v.p[2]); // Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
        }
    glEnd ();
}

void reshape(int w, int h) {
    camera.resize (w, h);
}

void display () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply (); 
    drawScene ();
    glFlush ();
    glutSwapBuffers (); 
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen) {
            glutReshapeWindow (camera.getScreenWidth (), camera.getScreenHeight ());
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }      
        break;
    case 'q':
    case 27:
        exit (0);
        break;
    case 'w':
        GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
        break;
	case 'r':
		if (xtoon.state() == XToon::DEPTH || xtoon.state() == XToon::CPUDEPTH){
			float minz = farplane, maxz = nearplane;
			for (unsigned int i = 0; i < mesh.T.size(); i++)
				for (unsigned int j = 0; j < 3; j++) {
					const Vertex & v = mesh.V[mesh.T[i].v[j]];
					float z = camera.getZ(v.p);
					if (z < minz)
						minz = z;
					if (z > maxz)
						maxz = z;
				}
			zmaxd = maxz;
			zmind = minz;
		}
		else if (xtoon.state() == XToon::FOCUS || xtoon.state() == XToon::CPUFOCUS){
			float minz = farplane, maxz = nearplane;
			for (unsigned int i = 0; i < mesh.T.size(); i++)
				for (unsigned int j = 0; j < 3; j++) {
					const Vertex & v = mesh.V[mesh.T[i].v[j]];
					float z = camera.getZ(v.p);
					if (z < minz)
						minz = z;
					if (z > maxz)
						maxz = z;
				}
			zmax = (maxz - minz) / 4;
			zmin = 0.f;
			zfoc = (minz + maxz) / 2;
		}
		xtoon.refresh();
        break;
	case 's':
		EasyBMP_Screenshot("screenshot.bmp");
		break;
    default:
        printUsage ();
        break;
    }
}

void mouse (int button, int state, int x, int y) {
    camera.handleMouseClickEvent (button, state, x, y);
}

void motion (int x, int y) {
    camera.handleMouseMoveEvent (x, y);
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [128];
        unsigned int numOfTriangles = mesh.T.size ();
        sprintf_s (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay (); 
}

int main (int argc, char ** argv) {
	char c;
	if (argc > 2) {
        printUsage ();
        exit (1);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    window = glutCreateWindow (appTitle.c_str ());
    init (argc == 2 ? argv[1] : DEFAULT_MESH_FILE.c_str ());
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    printUsage ();  
    glutMainLoop ();
	cin >> c;
    return 0;
}

