#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "structs.h"
#include <GL/freeglut.h>
#include <cmath>

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


//screens
enum mode {SMOOTH, FLAT, WIREFRAME, TEXTURE};

//function prototypes
void cameraFunction(Camera camera);
void drawFlat(Body body);
void drawFlatRing(Body body);
void drawOrbit(Body body);
void drawSmooth(Body body);
void drawSmoothRing(Body body);
void drawTextured(Body body);
void drawTexturedRing(Body body);
void drawWired(Body body);
void drawWiredRing(Body body);
