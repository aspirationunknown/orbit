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
#include "fractal.h"
#include <cmath>

using namespace std;

// screen state
screen current_screen = INITIATOR_SHAPE;
int ScreenWidth = 1280;
int ScreenHeight = 512;
int fps = 60;

// fractal drawing stuff
Point mouse_point;
bool mouse_pressed = false;
int iterations = 0;
int max_iterations = 1;
Polygon generator;
Polygon initiator;

// function prototypes
void initOpenGL();
void screenSetup();
void step( int value);
void reshape( int w, int h );
void keyboard_down( unsigned char key, int x, int y);
void mouse_action(int button, int state, int x, int y);
void mouse_movement(int x, int y);
void right_up(int x, int y);
void left_up(int x, int y);

// step functions
void initiator_step();
void generator_step();
void fractal_step();
void shared_step();

// display functions
void display();
void display_initiator();
void display_generator();
void display_fractal();
 
 /***************************************************************************//**
 * main
 * Authors - Derek Stotz, Charles Parsons
 *
 * The entry point of the fractal application.  Sets up the environment.
 *
 * Parameters - 
            argc - the number of arguments from the command prompt.
            argv - the command line arguments.
 ******************************************************************************/
int main ( int argc, char *argv[] )
{  
    // set the max iterations to the command line argument

    if ( argc > 1 )
        iterations = atoi(argv[1]);

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
    glutCreateWindow( "Fractals" );                  // window title

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
 * Does a step in the application, handled differently depending on which screen is active
 * 
 * Parameters - 
    value - an unused integer passed by glutTimerFunc
 ******************************************************************************/
void step ( int value )
{
    switch( current_screen )
    {
        case FRACTAL:
            fractal_step();
            break;
        default:
            break;
    }
    glutPostRedisplay();
    glutTimerFunc( 1000/fps, step, 0 );
}

 /***************************************************************************//**
 * display
 * Authors - Derek Stotz, Charles Parsons
 *
 * The display callback, taking into account the current screen
 ******************************************************************************/
void display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    drawField(ScreenHeight, ScreenWidth);
    switch( current_screen )
    {
        case INITIATOR_SHAPE:
            if(mouse_pressed && initiator.length >= 1)
                drawLine(initiator.points[initiator.length-1], mouse_point, White);
            display_initiator();
            break;
        case GENERATOR_PATTERN:
            if(mouse_pressed && generator.length >= 1)
                drawLine(generator.points[generator.length-1], mouse_point, White);
            display_generator();
            break;
        case FRACTAL:
            display_fractal();
            break;
        default:
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
    initiator.points = new Point[100000];
    initiator.addPoint(-ScreenWidth + 256, 0);
    generator.points = new Point[100];
    generator.addPoint(64, 0);
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
    switch( current_screen )
    {
        case INITIATOR_SHAPE:
            if( x < ScreenWidth/2 )
            {
                initiator.addPoint(x * 2 - ScreenWidth, - y * 2 + ScreenHeight);
            }
            break;
        case GENERATOR_PATTERN:
            if( x > ScreenWidth/2 )
            {
                generator.addPoint(x * 2 - ScreenWidth, - y * 2 + ScreenHeight);
            }
            break;
        default:
            break;
    }
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
    switch( current_screen )
    {
        case INITIATOR_SHAPE:
            if( initiator.length > 2 )
            {
                initiator.close();
                current_screen = GENERATOR_PATTERN;
            }
            break;
        case GENERATOR_PATTERN:
            if( generator.length > 1 )
            {
                generator.normalize();
                current_screen = FRACTAL;
            }
            break;
        default:
            break;
    }
}

 /***************************************************************************//**
 * Display Initiator
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays relevant elements to the initiator drawing screen
 ******************************************************************************/
void display_initiator()
{
    char* text = new(nothrow) char[30];
    strcpy(text, "Right-Click to Close Initiator");
    drawText(text, - ScreenWidth + 32 );
    drawPolygon(initiator, White, true);
    delete text;
}

 /***************************************************************************//**
 * Display Generator
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays relevant elements to the generator drawing screen
 ******************************************************************************/
void display_generator()
{
    char* text = new(nothrow) char[30];
    strcpy(text, "Right-Click to End Generator");
    drawText(text, 32 );
    drawPolygon(initiator, White, false);
    drawPolygon(generator, White, true);
    delete text;
}

 /***************************************************************************//**
 * Display Fractal
 * Authors - Derek Stotz, Charles Parsons
 *
 * Displays relevant elements to the fractal generation screen
 ******************************************************************************/
void display_fractal()
{
    const char* iter_str = itoa(iterations);
    char* text = new char[30];

    strcpy(text, "Fractal iteration: ");
    strcat(text, iter_str);
    drawText(text, - ScreenWidth + 32 );
    drawPolygon(initiator, White, false);
    drawPolygon(generator, White, false);
    delete text;
}

 /***************************************************************************//**
 * Fractal Step
 * Authors - Derek Stotz, Charles Parsons
 *
 * Does a step in the fractal generation screen mode.
 * Creates a new fractal from the old one and the generation pattern.
 ******************************************************************************/
void fractal_step()
{
    if(iterations >= max_iterations)
        return;

    Polygon new_fractal;
    cout << "Fractal Length: " << itoa(initiator.length) << endl;
    new_fractal.points = new Point[1000000];
    for( unsigned long i = 0; i < initiator.length - 1; i++ )
    {
        cout << "starting new edge" << endl;
        Polygon fractal_addition;
        fractal_addition.points = new Point[100000];
	    fitPattern(generator, initiator.points[i], initiator.points[i + 1], fractal_addition);
        for ( unsigned long j = 0; j < fractal_addition.length; j++ )
        {
            cout << "adding fractal piont x = " << new_fractal.points[j].x << " y = " << new_fractal.points[j].y << endl;
            new_fractal.addPoint(fractal_addition.points[j]);
        }
        delete fractal_addition.points;
    }
    iterations++;
    cout << "replacing old fractal points" << endl;
    initiator.length = new_fractal.length;

    for(unsigned long i = 0; i < initiator.length; i++)
    {
        initiator.points[i] = new_fractal.points[i];
    }

    delete new_fractal.points;
}

 /***************************************************************************//**
 * Mouse Movement
 * Authors - Derek Stotz, Charles Parsons
 *
 * Updates the mouse point when the pointer is moved
 ******************************************************************************/
void mouse_movement(int x, int y)
{
    cout << "MOUSE MOVED to x = " << x << endl;
    mouse_point.x = x * 2 - ScreenWidth;
    mouse_point.y = - y * 2 + ScreenHeight;
}
