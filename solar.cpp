/***************************************************************************//**
 * solar.cpp
 *
 * Assignment - PA 3 - solar
 *
 * Authors - Derek Stotz, Charles Parsons
 *
 * Date - November 24, 2014
 *
 * Instructor - Dr. Weiss
 *
 * Course - CSC 433
 *
 * Location - McLaury - Room 313
 *
 *
 * Usage - 
            To Compile: make
            To Run: ./solar

            This application is a simplified model solar system

            Controls:

                - W: Rotate forward
                - S: Rotate backward
                - A: Rotate System Left
                - D: Rotate System Right

                - Left: Pan Left
                - Right: Pan Right
                - Up: Pan Up
                - Down: Pan Down

                - R: Reset
                - Space: Pause / Play
                - P: Pause and move forward while p is pressed

                - +: speed up the simulation by 10%
                - -: slow down the simulation by 10%

            Press Esc to end the program.
                
                        
 * Details -  
            The structure of solar is somewhat modular.  The main file, solar.cpp, holds all
            global variables, callback functions and opengl functionality.  The
            state of the application is determined by a mode enum.  There are four modes:

                - SMOOTH: Draw the planets in the solar system with the smooth shading model.
                - FLAT: Draw the planets in the solar system with the flat shading model.
                - WIREFRAME: Draw the planets in the solar system as wireframes.
                - TEXTURE: Draw the planets in the solar system with mapped textures.
            
            Switch statements in the display and step functions determine which screen is drawn.
            I/O, such as rotating the camera and navigating the environment, is handled in switch
            statements and applied in the update steps.  A camera object stores current camera
            inform.

            All graphical functions, from drawing planets to mapping textures and displaying text, are found in
            solarGraphics.cpp.  It makes use of const colors and object classes found in structs.h.    solar.h links
            together solarGraphics.h and solar.cpp so both files may make use of the structs found in structs.h and their
            functionaliy found in structs.cpp.

            structs.h holds the Camera and Body classes.  The camera class is a singleton, and is mostly there for code
            reuse and cleanliness.  The body class is reused for every body in the solar system, and includes options for
            nested orbitals, lighting properties and rings.
 *
 * Issues and Bugs - 
            No bugs to speak of.
 *
 ******************************************************************************/
#include "solar.h"
#include <iostream>
#include <cmath>

using namespace std;

// I/O
Keyboard controller;
Point mouse_point;
bool mouse_pressed = false;
GLubyte* asteroid_image = NULL;

// screen state
mode current_mode = TEXTURE;
int ScreenWidth = 1280;
int ScreenHeight = 512;
int fps = 60;
double speed = 1.0;
double camera_speed = 10.0;
bool paused = false;

// solar system
Camera camera;
Body bodies[100];
Belt belts[10];
int num_bodies = 0;
int num_belts = 0;

// function prototypes
void initOpenGL();
void step( int value);
void reshape( int w, int h );
void create_camera();
void keyboard_down( unsigned char key, int x, int y);
void keyboard_up( unsigned char key, int x, int y);
void special_up( int key, int x, int y );
void special_down( int key, int x, int y );
void mouse_action(int button, int state, int x, int y);
void right_up(int x, int y);
void left_up(int x, int y);

void add_body(const float color[3], double emissivity, double radius, double orbital_period, 
    double rotation_period, double orbital_radius, const char* texture_file);
void add_belt(int count, const float color[3], double min_orbital_period, double max_orbital_period,
    double min_rotation_period, double max_rotation_period, double min_orbital_radius,
    double max_orbital_radius,double min_radius, double max_radius);
void create_solar_system();

// display functions
void display();
void display_smooth();
void display_flat();
void display_wireframe();
void display_texture();
 
 /***************************************************************************//**
 * main
 * Authors - Derek Stotz, Charles Parsons
 *
 * The entry point of the solar application.  Sets up the environment.
 *
 * Parameters - 
            argc - the number of arguments from the command prompt.
            argv - the command line arguments.
 ******************************************************************************/
int main ( int argc, char *argv[] )
{
    // start displaying
    create_solar_system();
    glutInit(&argc, argv);
    initOpenGL();
    glutMainLoop();    

    return 0;
}


 /***************************************************************************//**
 * initOpenGL
 * Authors - Derek Stotz, Charles Parsons
 *
 * Sets up OpenGL and creates the main window.
 ******************************************************************************/
void initOpenGL( void )
{
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );     // 32-bit graphics and double buffering

    glutInitWindowSize( ScreenWidth, ScreenHeight );    // initial window size
    glutInitWindowPosition( 100, 50 );                  // initial window position
    glutCreateWindow( "Solar System" );                  // window title

    glutIgnoreKeyRepeat(1); // ignore repeated key presses
    glutKeyboardFunc( keyboard_down );
    glutKeyboardUpFunc( keyboard_up );
    glutSpecialUpFunc( special_up );
    glutSpecialFunc( special_down );

    glutMouseFunc(mouse_action);

    glClearColor( 0.0, 0.0, 0.0, 1.0 );                 // use black for glClear command
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutTimerFunc( 1000/fps, step, 0 );
}


 /***************************************************************************//**
 * step
 * Authors - Derek Stotz, Charles Parsons
 *
 * Does a step in the application, calling the step functions of each celestial body
 * And applying any camera movements.
 * 
 * Parameters - 
    value - an unused integer passed by glutTimerFunc
 ******************************************************************************/
void step ( int value )
{
    // make speed changes
    if(controller.plus)
    {
        controller.plus = false;
        speed += .1;
    }
    else if (controller.minus)
    {
        controller.minus = false;
        speed -= .1;
    }
    // apply camera changes  based on what keys are pressed
    if(controller.w)    camera.pan_forward(camera_speed);
    if(controller.a)    camera.pan_left(camera_speed);
    if(controller.s)    camera.pan_backward(camera_speed);
    if(controller.d)    camera.pan_right(camera_speed);
    
    if(controller.r)
    {
        create_camera();
        controller.r = false;
    }
    
    if(controller.up)   camera.rotate_up(camera_speed / 10);
    if(controller.down) camera.rotate_down(camera_speed / 10);
    if(controller.left) camera.rotate_left(camera_speed / 10);
    if(controller.right)camera.rotate_right(camera_speed / 10);

	// loop through every body, calling that body's step function
    if (!paused)
    {
        for(int i = 0; i < num_bodies; i++)
            bodies[i].step(speed);
	    for(int i = 0; i < num_belts; i++)
            belts[i].step(speed);
    }

    glutPostRedisplay();
    glutTimerFunc( 1000/fps, step, 0 );
}

 /***************************************************************************//**
 * display
 * Authors - Derek Stotz, Charles Parsons
 *
 * The display callback, taking into account the current mode
 ******************************************************************************/
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    switch( current_mode )
    {
        case SMOOTH:
            display_smooth();
            break;
        case FLAT:
            display_flat();
            break;
        case WIREFRAME:
            display_wireframe();
            break;
        default:
			display_texture();
            break;
    }

    glutSwapBuffers();
    glFlush();

}

 /***************************************************************************//**
 * reshape
 * Authors - Dr. John Weiss
 *
 * The callback function which reshapes the window
 *
 * Parameters -
    w - the width of the screen
    h - the height of the screen
 ******************************************************************************/
void reshape( int w, int h )
{
    // orthographic projection of 3-D scene onto 2-D, maintaining aspect ratio
    glMatrixMode( GL_PROJECTION );      // use an orthographic projection
    glLoadIdentity();                   // initialize transformation matrix
    if ( w > h )                        // use width:height aspect ratio to specify view extents
        gluOrtho2D( - ScreenWidth, ScreenWidth, - ScreenHeight, ScreenHeight );
    else
        gluOrtho2D( - ScreenWidth, ScreenWidth, -ScreenHeight, ScreenHeight );
    glViewport( 0, 0, w, h );           // adjust viewport to new window

    // switch back to (default) model view mode, for transformations
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

 /***************************************************************************//**
 * keyboard_down
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles key press events
 ******************************************************************************/
void keyboard_down( unsigned char key, int x, int y )
{
    std::cout << endl << key << " pressed" << std::endl;
    switch( key )
    {
        case 27:
            exit ( 0 );
		case 'W':
		case 'w':
            controller.w = true;
			break;
		case 'S':
		case 's':
            controller.s = true;
			break;
		case 'A':
		case 'a':
            controller.a = true;
			break;
		case 'D':
		case 'd':
            controller.d = true;
			break;
        case 'R':
        case 'r':
            controller.r = true;
            break;
        case 'p':
        case 'P':
            controller.p = true;
            paused = false;
            break;
        case ' ':
            controller.space = true;
            paused = true;
            break;
        case '=':
        case '+':
            controller.plus = true;
            break;
        case '-':
        case '_':
            controller.minus = true;
            break;
        default:
            break;
    }
}

 /***************************************************************************//**
 * keyboard_up
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles key release events
 ******************************************************************************/
void keyboard_up( unsigned char key, int x, int y )
{

    std::cout << endl << key << " released" << std::endl;
    switch( key )
    {
		case 'W':
		case 'w':
            controller.w = false;
			break;
		case 'S':
		case 's':
            controller.s = false;
			break;
		case 'A':
		case 'a':
            controller.a = false;
			break;
		case 'D':
		case 'd':
            controller.d = false;
			break;
        case 'R':
        case 'r':
            controller.r = false;
            break;
        case 'p':
        case 'P':
            controller.p = false;
            paused = true;
            break;
        case ' ':
            controller.space = false;
            paused = false;
            break;
        case '=':
        case '+':
            controller.plus = false;
            break;
        case '-':
        case '_':
            controller.minus = false;
            break;
        default:
            break;
    }
}

 /***************************************************************************//**
 * special_down
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles key press events for arrow keys
 ******************************************************************************/
void special_down( int key, int x, int y )
{
    switch( key )
    {
        case GLUT_KEY_UP:
            // pan up
            break;
        case GLUT_KEY_DOWN:
            // pan down
            break;
        case GLUT_KEY_LEFT:
            // pan left
            break;
        case GLUT_KEY_RIGHT:
            // pan right
        default:
            break;
    }
}

 /***************************************************************************//**
 * special_up
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles key release events for special keys
 ******************************************************************************/
void special_up( int key, int x, int y )
{
    switch( key )
    {
        case GLUT_KEY_UP:
            // stop panning up
            break;
        case GLUT_KEY_DOWN:
            // stop panning down
            break;
        case GLUT_KEY_LEFT:
            // stop panning left
            break;
        case GLUT_KEY_RIGHT:
            // stop panning right
        default:
            break;
    }
}



 /***************************************************************************//**
 * mouse_action
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles mouse press and release events
 ******************************************************************************/
void mouse_action(int button, int state, int x, int y)
{
    mouse_point.x = x * 2 - ScreenWidth;
    mouse_point.y = - y * 2 + ScreenHeight;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_UP)
            left_up(x, y);
        else
            mouse_pressed = true;
    }
    else
    {
        if(state == GLUT_UP)
            right_up(x, y);
    }
}

 /***************************************************************************//**
 * left_up
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles left mouse button click actions
 ******************************************************************************/
void left_up(int x, int y)
{
    mouse_pressed = false;
}


 /***************************************************************************//**
 * right_up
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles right mouse button click actions
 ******************************************************************************/
void right_up(int x, int y)
{
    switch( current_mode )
    {
        default:
            break;
    }
}

 /***************************************************************************//**
 * Display Flat
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system with flat shading
 ******************************************************************************/
void display_flat()
{
    for(int i = 0; i < num_bodies; i++)
    {   drawOrbit(bodies[i]);
        drawFlat(bodies[i]);
    }
    for(int i = 0; i < num_belts; i++)
    {
        for(int j = 0; j < belts[i].count; j++)
        {
            drawFlat(belts[i].asteroids[j]);
        }
    }
}

 /***************************************************************************//**
 * Display Smooth
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system with smooth shading
 ******************************************************************************/
void display_smooth()
{
    
}

 /***************************************************************************//**
 * Display Wireframe
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system in wireframes
 ******************************************************************************/
void display_wireframe()
{
    
}

  /***************************************************************************//**
 * Display Texture
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system with mapped textures
 ******************************************************************************/
void display_texture()
{
    
}

  /***************************************************************************//**
 * Add Body
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates and adds a single body to the body list.  Scaling constants are found here.
 ******************************************************************************/
void add_body(
    const float color[3],
    double emissivity,      // 0 - 255
    double radius,          // in km
    double orbital_radius,  // in millions of km
    double orbital_period,  // in days
    double rotation_period,  // in hours
    const char* texture_file)
{
    Body newBody;

    newBody.color[0] = color[0];
    newBody.color[1] = color[1];
    newBody.color[2] = color[2];

    newBody.emissivity = emissivity;
    newBody.orbital_period = orbital_period * orbital_period_scale;
    newBody.rotation_period = rotation_period * rotation_period_scale;
    newBody.orbital_radius = orbital_radius * orbital_radius_scale + orbital_radius_offset;
    newBody.radius = radius * radius_scale;

    getTexture(newBody.image, texture_file);

    bodies[num_bodies] = newBody;
    num_bodies++;
    std::cout << "\ncreated body with texture " << texture_file << "\n";
}


  /***************************************************************************//**
 * Add Belt
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates and adds a single belt to the belt list
 ******************************************************************************/
void add_belt(
    int count,
    const float color[3],
    double min_orbital_period,
    double max_orbital_period,
    double min_rotation_period,
    double max_rotation_period,
    double min_orbital_radius,
    double max_orbital_radius,
    double min_radius,
    double max_radius)
{
    Belt newBelt;
    newBelt.count = count;

    newBelt.max_orbital_period = max_orbital_period;
    newBelt.min_rotation_period = min_rotation_period;
    newBelt.max_rotation_period = max_rotation_period;
    newBelt.min_orbital_radius = min_orbital_radius;
    newBelt.max_orbital_radius = max_orbital_radius;
    newBelt.min_radius = min_radius;
    newBelt.max_radius = max_radius;

    newBelt.create_asteroids();
    belts[num_belts] = newBelt;
    num_belts++;
}

  /***************************************************************************//**
 * Create Solar System
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates the initial camera state
 ******************************************************************************/
void create_camera()
{
    camera.position.x = 100;
    camera.position.y = 0;
    camera.position.z = 50;
    camera.look_at.x = 0;
    camera.look_at.y = 0;
    camera.look_at.z = 0;

    camera.left.dx = 0;
    camera.left.dy = -1;
    camera.left.dz = 0;

    camera.up.dx = 0;
    camera.up.dy = 0;
    camera.up.dz = -1;
}

  /***************************************************************************//**
 * Create Solar System
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates all bodies in the solar system
 ******************************************************************************/
void create_solar_system()
{
    // add the camera
    create_camera();

    // hardcoded system can have up to 10 asteroid belts and up to 100 bodies outside of the asteroid belts

    // load the static texture
    getTexture(asteroid_image, "ceres.bmp");

    add_body(Yellow, 255, 696000/10, 0, 0, 25, "sun.bmp" );
    add_body(White, 0, 2439, 58, 88, 1416, "mercury.bmp" );
    add_body(Green, 0, 6052, 108, 225, 5832, "venus.bmp" );
    
    add_body(Blue, 0, 6378, 150, 365, 24, "earth.bmp");
    bodies[num_bodies-1].moons = new Body[1];    
    bodies[num_bodies-1].add_moon(White, 0, 1738, .384, 27.3, 27.3 * 24, "moon.bmp");
    
    add_body(Red, 0, 3394, 228, 687, 24.6, "mars.bmp");
    
    add_belt(100, White, 300, 700, 10, 10000, 600, 5000, 20, 500);  // asteroid belt

    add_body(Red, 0, 71398, 779, 4332, 9.8, "jupiter.bmp");
    bodies[num_bodies-1].moons = new Body[4];
    bodies[num_bodies-1].add_moon(Red, 0 , 1821, .421, 1.7, 1.7 * 24, "io.bmp"); // Io
    bodies[num_bodies-1].add_moon(White, 0, 2410, 1.879, 16.6, 16.6 * 24, "callisto.bmp"); // Callisto
    bodies[num_bodies-1].add_moon(Blue, 0, 1560, .6709, 3.55, 3.55 * 24, "europa.bmp"); // Europa
    bodies[num_bodies-1].add_moon(Green, 0, 2634, 1.07, 7.154, 7.154 * 24, "ganymede.bmp"); //Ganymede
    
    add_body(Yellow, 0, 60270, 1424, 10761, 10.2, "saturn.bmp");
    bodies[num_bodies-1].add_rings(White, 67270, 140270, "saturnrings.bmp");

    add_body(Cyan, 0, 25550, 2867, 30682, 15.5, "uranus.bmp");
    add_body(Blue, 0, 24750, 4492, 60195, 15.8, "neptune.bmp");
    
    add_belt(500, White, 6000, 10000, 10, 10000, 5235, 7479, 20, 1000); // kuiper belt

/*
    radius distance year day
Sun 696000 0 0 25
Mercury 2439 58 88 1416
Venus 6052 108 225 5832
Earth 6378 150 365 24
Mars 3394 228 687 24.6
Jupiter 71398 779 4332 9.8
Saturn 60270 1424 10761 10.2
Uranus 25550 2867 30682 15.5
Neptune 24750 4492 60195 15.8
Moon 1738 0.384 27.3 27.3
*/

}
