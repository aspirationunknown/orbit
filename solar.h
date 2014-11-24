#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "structs.h"
#include <GL/freeglut.h>

// global texture used for asteroids
extern GLubyte* asteroid_image;

// keyboard state

struct Keyboard
{
	bool w;
	bool s;
	bool a;
	bool d;
    bool r;
    bool p;
    bool space;
    bool plus;
    bool minus;
    bool left;
    bool right;
    bool up;
    bool down;
};

//keys
const int EscapeKey = 27;

//globals
GLUquadricObj* sphere;
GLUquadricObj* ring;

//screens
enum mode {SMOOTH, FLAT, WIREFRAME, TEXTURE};

//function prototypes
void drawFlat(Body body);
void drawOrbit();
void drawSmooth(Body body);
void drawTextured(Body body);
void drawWired(Body body);
