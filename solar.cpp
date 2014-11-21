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

                - 
                - 

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
#include <cmath>

using namespace std;

// screen state
mode current_mode = TEXTURE;
int ScreenWidth = 1280;
int ScreenHeight = 512;
int fps = 60;

// function prototypes
void initOpenGL();
void screenSetup();
void step( int value);
void reshape( int w, int h );
void keyboard_down( unsigned char key, int x, int y);
void mouse_action(int button, int state, int x, int y);
void right_up(int x, int y);
void left_up(int x, int y);

// display functions
void display();
void display_smooth();
void display_flat();
void display_wireframe();
void display_textured();
 
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
    screenSetup();
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
    glutMouseFunc(mouse_action);
    glutMotionFunc(mouse_movement);

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
    // apply camera changes  based on what keys are pressed
	
	// loop through every body, calling that body's step function
	
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
			display_textured();
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
 * screenSetup
 * Authors - Derek Stotz, Charles Parsons
 *
 * Sets up the objects to be used in the screen modes
 ******************************************************************************/
void screenSetup()
{

}

 /***************************************************************************//**
 * keyboard_down
 * Authors - Derek Stotz, Charles Parsons
 *
 * handles key press events
 ******************************************************************************/
void keyboard_down( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 27:
            exit ( 0 );
		case 'W':
		case 'w':
			// Rotate forward
			break;
		case 'A':
		case 'a':
			// Rotate System Left
			break;
		case 'S':
		case 's':
			// Rotate backward
			break;
		case 'D':
		case 'd':
			// Rotate System Right
			break;
		case 'Q':
		case 'q':
			// Strafe Left
			break;
		case 'E':
		case 'e':
			// Strafe Right
			break;
		case 'z':
		case 'Z':
			// Zoom In
		case 'x':
		case 'X':
			// Zoom Out
		
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
        case INITIATOR_SHAPE:
            if( initiator.length > 2 )
            {
                initiator.close();
                current_mode = GENERATOR_PATTERN;
            }
            break;
        case GENERATOR_PATTERN:
            if( generator.length > 1 )
            {
                generator.normalize();
                current_mode = FRACTAL;
            }
            break;
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
 * Display Wireframe
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays planetary system with mapped textures
 ******************************************************************************/
void display_texture()
{
    
}