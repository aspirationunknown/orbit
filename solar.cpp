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

                - W: Pan forward
                - S: Pan backward
                - A: Pan left
                - D: Pan Right

                - Left: Rotate Left
                - Right: Rotate Right
                - Up: Rotate Up
                - Down: Rotate Down

                - R: Reset the camera position and the speed
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

//externs
// global texture used for asteroids
GLubyte* asteroid_image;

// fidelity of wireframes
int fidelity = 20;


// screen state
mode current_mode = FLAT;
int ScreenWidth = 720;
int ScreenHeight = 720;
int fps = 60;
double speed = .1;
double camera_speed = 100.0;
bool paused = true;

// solar system
Camera camera;
Body bodies[100];
Belt belts[10];
int num_bodies = 0;
int num_belts = 0;

// for help
const char* open_readme_command = "xdg-open README";
const char* open_readme_command_win = "notepad.exe README";
const char* open_readme_command_gedit = "gedit README";

// function prototypes
void initOpenGL();
void step( int value);
void reshape( int w, int h );
void create_camera();
void keyboard_down( unsigned char key, int x, int y);
void keyboard_up( unsigned char key, int x, int y);
void special_up( int key, int x, int y );
void special_down( int key, int x, int y );

// menus
void MainMenuHandler(int item);
void CreateMenus();

void add_body(const float color[3], double emissivity, double radius, double orbital_period, 
    double rotation_period, double orbital_radius, const char* texture_file, const char* name);
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
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );     // 32-bit graphics and double buffering
    glEnable(GL_LIGHTING);

    glutInitWindowSize( ScreenWidth, ScreenHeight );    // initial window size
    glutInitWindowPosition( 100, 50 );                  // initial window position
    glutCreateWindow( "Solar System" );                  // window title

    glutIgnoreKeyRepeat(1); // ignore repeated key presses
    glutKeyboardFunc( keyboard_down );
    glutKeyboardUpFunc( keyboard_up );
    glutSpecialUpFunc( special_up );
    glutSpecialFunc( special_down );

    CreateMenus();

    glClearColor( 0.0, 0.0, 0.0, 1.0 );                 // use black for glClear command
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    glutTimerFunc( 1000/fps, step, 0 );
    
    glMatrixMode( GL_PROJECTION );      // use an orthographic projection
    glLoadIdentity();

    //glClearColor(1.0, 1.0, 1.0, 1.0);
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
        speed += .01;
    }
    else if (controller.minus)
    {
        controller.minus = false;
        speed -= .01;
    }

    // change fidelity
    if(controller.greaterthan && fidelity < 50)
    {
        controller.greaterthan = false;
        fidelity += 4;
    }
    if(controller.lessthan && fidelity > 2)
    {
        controller.lessthan = false;
        fidelity -= 4;
    }

    // apply camera changes  based on what keys are pressed
    if(controller.w)    camera.pan_up(camera_speed);
    if(controller.a)    camera.pan_left(camera_speed);
    if(controller.s)    camera.pan_down(camera_speed);
    if(controller.d)    camera.pan_right(camera_speed);
    if(controller.x)    camera.zoom_out(camera_speed);
    if(controller.z)    camera.zoom_in(camera_speed);
    
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
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    cameraFunction(camera);
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

    glFrontFace( GL_CW );
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
    glViewport( 0, 0, w, h );           // adjust viewport to new window
    glMatrixMode( GL_PROJECTION );      // use an orthographic projection
    glLoadIdentity();                   // initialize transformation matrix
    gluPerspective(30.0, ScreenWidth/ScreenHeight, 1.0, 100000.0);

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

    // for precision movement
    if (glutGetModifiers() == GLUT_ACTIVE_SHIFT)
    {
        camera_speed = 3;
    }
    else
    {
        camera_speed = 50;
    }

    switch( key )
    {
        case 27:
            exit ( 0 );
		case 'Z':
		case 'z':
            controller.z = true;
			break;
		case 'x':
		case 'X':
            controller.x = true;
			break;
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
            speed = .005;
            controller.p = true;
            paused = false;
            break;
        case ' ':
            controller.space = true;
            paused = !paused;
            break;
        case '=':
        case '+':
            controller.plus = true;
            break;
        case '-':
        case '_':
            controller.minus = true;
            break;
        case '.':
        case '>':
            controller.greaterthan = true;
            break;
        case ',':
        case '<':
            controller.lessthan = true;
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
    switch( key )
    {
        case 'Z':
		case 'z':
            controller.z = false;
			break;
		case 'x':
		case 'X':
            controller.x = false;
			break;
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
        displayLabel(bodies[i]);
        for(int j = 0; j < bodies[i].num_moons; j++)
        {        
            drawOrbit(bodies[i].moons[j]);
            drawFlat(bodies[i].moons[j]);
            displayLabel(bodies[i].moons[j]);
        }
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
    for(int i = 0; i < num_bodies; i++)
    {   drawOrbit(bodies[i]);
        drawSmooth(bodies[i]);
        displayLabel(bodies[i]);
        for(int j = 0; j < bodies[i].num_moons; j++)
        {        
            drawOrbit(bodies[i].moons[j]);
            drawSmooth(bodies[i].moons[j]);
            displayLabel(bodies[i].moons[j]);
        }
    }
    for(int i = 0; i < num_belts; i++)
    {
        for(int j = 0; j < belts[i].count; j++)
        {
            drawSmooth(belts[i].asteroids[j]);
        }
    }   
}

 /***************************************************************************//**
 * Display Wireframe
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system in wireframes
 ******************************************************************************/
void display_wireframe()
{
    for(int i = 0; i < num_bodies; i++)
    {   drawOrbit(bodies[i]);
        drawWired(bodies[i]);
        displayLabel(bodies[i]);
        for(int j = 0; j < bodies[i].num_moons; j++)
        {        
            drawOrbit(bodies[i].moons[j]);
            drawWired(bodies[i].moons[j]);
            displayLabel(bodies[i].moons[j]);
        }
    }
    for(int i = 0; i < num_belts; i++)
    {
        for(int j = 0; j < belts[i].count; j++)
        {
            drawWired(belts[i].asteroids[j]);
        }
    }
}

  /***************************************************************************//**
 * Display Texture
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system with mapped textures
 ******************************************************************************/
void display_texture()
{
    for(int i = 0; i < num_bodies; i++)
    {   drawOrbit(bodies[i]);
        drawTextured(bodies[i]);
        displayLabel(bodies[i]);
        for(int j = 0; j < bodies[i].num_moons; j++)
        {        
            drawOrbit(bodies[i].moons[j]);
            drawTextured(bodies[i].moons[j]);
            displayLabel(bodies[i].moons[j]);
        }
    }
    for(int i = 0; i < num_belts; i++)
    {
        for(int j = 0; j < belts[i].count; j++)
        {
            drawTextured(belts[i].asteroids[j]);
        }
    }
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
    const char* texture_file,
    const char* name)
{
    Body newBody;
    newBody.name = std::string(name);

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

    newBelt.color[0] = color[0];
    newBelt.color[1] = color[1];
    newBelt.color[2] = color[2];

    newBelt.min_orbital_period = min_orbital_period;
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
 * Creates the initial camera and speed states
 ******************************************************************************/
void create_camera()
{
    speed = .05;
    camera.position.x = 0;
    camera.position.y = 0;
    camera.position.z = 1000;
    camera.look_at.x = 0;
    camera.look_at.y = 0;
    camera.look_at.z = 0;

    camera.left.dx = 1;
    camera.left.dy = 0;
    camera.left.dz = 0;

    camera.up.dx = 0;
    camera.up.dy = -1;
    camera.up.dz = 0;
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

    add_body(Yellow, 255, 69600, 0, 1, 25, "sun.bmp", "Sol" );
    add_body(White, 0, 2439, 108, 88, 1416, "mercury.bmp", "Mercury" );
    add_body(Green, 0, 6052, 158, 225, 5832, "venus.bmp", "Venus" );
    
    add_body(Blue, 0, 6378, 200, 365, 24, "earth.bmp", "Terra");
    bodies[num_bodies-1].moons = new Body[1];    
    bodies[num_bodies-1].add_moon(White, 0, 1738, 1.384, 27.3, 27.3 * 24, "moon.bmp", "Luna");
    
    add_body(Red, 0, 3394, 278, 687, 24.6, "mars.bmp", "Mars");
    
    add_belt(100, White, 600, 4000, 10, 10000, 350, 600, 500, 1500);  // asteroid belt

    add_body(Red, 0, 35398, 829, 4332, 9.8, "jupiter.bmp", "Jupiter");
    bodies[num_bodies-1].moons = new Body[4];
    bodies[num_bodies-1].add_moon(Red, 0 , 1821, 5.21, 1.7, 1.7 * 24, "io.bmp", "Io"); // Io
    bodies[num_bodies-1].add_moon(White, 0, 2410, 12.79, 16.6, 16.6 * 24, "callisto.bmp", "Callisto"); // Callisto
    bodies[num_bodies-1].add_moon(Blue, 0, 1560, 6.709, 3.55, 3.55 * 24, "europa.bmp", "Europa"); // Europa
    bodies[num_bodies-1].add_moon(Green, 0, 2634, 8.7, 7.154, 7.154 * 24, "ganymede.bmp", "Ganymede"); //Ganymede
    
    add_body(Yellow, 0, 30270, 1174, 10761, 10.2, "saturn.bmp", "Saturn");
    bodies[num_bodies-1].add_rings(White, 67270, 140270, "saturnrings.bmp");

    add_body(Cyan, 0, 12550, 1717, 30682, 15.5, "uranus.bmp", "Uranus");
    add_body(Blue, 0, 11750, 2642, 60195, 15.8, "neptune.bmp", "Neptune");
    
    add_belt(300, White, 6000, 10000, 10, 10000, 2735, 4079, 500, 3000); // kuiper belt

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

/*********************************************************
|               Menu Code                                |
*********************************************************/


  /***************************************************************************//**
 * Create Solar System
 * Authors - Derek Stotz, Charles Parsons
 *
 * Adds items to the right-click dropdown menu
 ******************************************************************************/
void CreateMenus()
{
    // create main menu
    int value = 1;
    int mainmenu = glutCreateMenu( MainMenuHandler );

    glutAddMenuEntry( "Flat Shading", value++ );
    glutAddMenuEntry( "Smooth Shading", value++ );
    glutAddMenuEntry( "Wireframe", value++ );
    glutAddMenuEntry( "Textured", value++ );
    glutAddMenuEntry( "Help", value++ );
    glutAddMenuEntry( "Exit", value++ );

    // right button click activates menu
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

  /***************************************************************************//**
 * Create Solar System
 * Authors - Derek Stotz, Charles Parsons
 *
 * Handles menu selection
 ******************************************************************************/
void MainMenuHandler( int item )
{
    switch ( item )
    {
        case 1:
            current_mode = FLAT;
            break;
        case 2:
            current_mode = SMOOTH;
            break;
        case 3:
            current_mode = WIREFRAME;
            break;
        case 4:
            current_mode = TEXTURE;
            break;
        case 5:
            if(system(open_readme_command_gedit) != 0)
                if( system(open_readme_command) != 0)
                    system(open_readme_command_win );  // well, if the default linux editor fails to launch, we're probably in windows.
            break;
        case 6:
            exit( 0 );
            break;
        default:    // should not occur
            cout << "invalid main menu item " << item << endl;
            break;
    }
}
