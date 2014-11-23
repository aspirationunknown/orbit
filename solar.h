#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include "structs.h"
#include <GL/freeglut.h>

//keys
const int EscapeKey = 27;

//screens
enum mode {SMOOTH, FLAT, WIREFRAME, TEXTURE};

//function prototypes
drawOrbit(Body body);
drawSmooth(Body body);
drawTextured(Body body);
drawWired(Body body);
