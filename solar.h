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
    bool z = false;
    bool x = false;
	bool w = false;
	bool s = false;
	bool a = false;
	bool d = false;
    bool r = false;
    bool p = false;
    bool space = false;
    bool plus = false;
    bool minus = false;
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
};

//keys
const int EscapeKey = 27;


//screens
enum mode {SMOOTH, FLAT, WIREFRAME, TEXTURE};

//function prototypes
void cameraFunction(Camera camera);
void displayLabel(Body body);
void drawBitmapString( const char *string, float x, float y, float z, const float color[] );
void drawFlat(Body body);
void drawFlatRing(Body body);
void drawOrbit(Body body);
void drawSmooth(Body body);
void drawSmoothRing(Body body);
void drawTextured(Body body);
void drawTexturedRing(Body body);
void drawWired(Body body);
void drawWiredRing(Body body);
