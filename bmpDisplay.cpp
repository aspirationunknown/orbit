/*
	***** bmpDisplay.cc *****

Read in and display a BMP image file.

This program illustrates how to load an image from a BMP file and display it in OpenGL.

CSC433/533 Computer Graphics - Fall 2014

Author: John M. Weiss, Ph.D.
Written March 23, 2006.

Modifications:
*/

// include files
#include <iostream>
using namespace std;

// the GLUT header automatically includes gl.h and glu.h
#include <GL/freeglut.h>

// symbolic constants
const int EscapeKey = 27;

// typedefs
typedef unsigned char byte;

// global vars
int ScreenWidth  = 1024;
int ScreenHeight =  768;

char *FileName;
byte* ImagePtr;		// array of RGB pixel values (range 0 to 255)
int NumRows;		// number of rows in image
int NumCols;		// number of columns in image

// OpenGL callback function prototypes
void display( void );
void reshape( int w, int h );
void keyboard( unsigned char key, int x, int y );

// other function prototypes
void initOpenGL( void );
bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, unsigned char* &ImagePtr );
void displayImage( int x, int y, int w, int h, byte *image );

/******************************************************************************/

// main() function
int main( int argc, char *argv[] )
{
    if ( argc < 2 )
    {
	cerr << "Usage: bmpRead image.bmp\n";
        return -1;
    }

    if ( !LoadBmpFile( argv[1], NumRows, NumCols, ImagePtr ) )
    {
	cerr << "Error: unable to load file " << argv[1] << endl;
        return -1;
    }

    cerr << "reading " << argv[1] << ": " << NumRows << " x " << NumCols << endl;

    // perform various OpenGL initializations
    FileName = argv[1];
    ScreenWidth = NumCols;
    ScreenHeight = NumRows;
    glutInit( &argc, argv );
    initOpenGL();

    // go into OpenGL/GLUT main loop, never to return
    glutMainLoop();

    // yeah I know, but it keeps compilers from bitching
    return 0;
}

/******************************************************************************/

// various commands to initialize OpenGL and GLUT
void initOpenGL( void )
{
    glutInitDisplayMode( GLUT_RGBA | GLUT_SINGLE );	// 32-bit graphics and single buffering

    glutInitWindowSize( ScreenWidth, ScreenHeight );	// initial window size
    glutInitWindowPosition( 100, 50 );			// initial window  position
    glutCreateWindow( FileName );			// window title

    glClearColor( 0.0, 0.0, 0.0, 0.0 );			// use black for glClear command

    // callback routines
    glutDisplayFunc( display );				// how to redisplay window
    glutReshapeFunc( reshape );				// how to resize window
    glutKeyboardFunc( keyboard );			// how to handle key presses
}

/******************************************************************************/
/*                          OpenGL callback functions                         */
/******************************************************************************/

// callback function that tells OpenGL how to redraw window
void display( void )
{
    // clear the display
    glClear( GL_COLOR_BUFFER_BIT );

    // display image
    displayImage( 0, 0, NumCols, NumRows, ImagePtr );

    // flush graphical output
    glFlush();
}

/******************************************************************************/

// callback function that tells OpenGL how to resize window
// note that this is called when the window is first created
void reshape( int w, int h )
{
    // store new window dimensions globally
    ScreenWidth = w;
    ScreenHeight = h;

    // how to project 3-D scene onto 2-D
    glMatrixMode( GL_PROJECTION );		// use an orthographic projection
    glLoadIdentity();				// initialize transformation matrix
    gluOrtho2D( 0.0, w, 0.0, h );
    glViewport( 0, 0, w, h );			// adjust viewport to new window
}

/******************************************************************************/

// callback function that tells OpenGL how to handle keystrokes
void keyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        // Escape quits program
	case EscapeKey:
	    exit( 0 );
	    break;

        // anything else redraws window
	default:
	    glutPostRedisplay();
	    break;
    }
}

/******************************************************************************/

// function to display image
void displayImage( int x, int y, int w, int h, byte *image )
{
    glRasterPos2i( x, y );
    glDrawPixels( w, h, GL_RGB, GL_UNSIGNED_BYTE, image );
}
