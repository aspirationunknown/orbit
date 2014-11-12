/*
    ***** TexturedSphere.cpp *****

Rotating sphere (GLU quadric object) with texture.

Ref: Angel, OpenGL Primer, 2008

Modifications (JMW):
CSC433/533 demo - Fall 2014
*/

#include <stdlib.h>
#include <GL/freeglut.h>

static GLfloat theta[] = {0.0, 0.0, 0.0};
static GLint axis = 2;
GLUquadricObj* sphere;

// display callback
void display( void )
{
    // clear frame buffer and z buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // rotate sphere and redraw
    glLoadIdentity();
    glRotatef( theta[0], 1.0, 0.0, 0.0 );
    glRotatef( theta[1], 0.0, 1.0, 0.0 );
    glRotatef( theta[2], 0.0, 0.0, 1.0 );
    gluSphere( sphere, 1, 64, 64 );

    // swap buffers (implicitly calls glFlush)
    glutSwapBuffers();
}

// idle callback, spins sphere about selected axis
void spinObject()
{
    theta[axis] += 2.0;
    if ( theta[axis] > 360.0 ) theta[axis] -= 360.0;
    glutPostRedisplay();
}

// window resizing callback
void myReshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    if ( w <= h )
        glOrtho( -2.0, 2.0, -2.0 * ( GLfloat ) h / ( GLfloat ) w,
                 2.0 * ( GLfloat ) h / ( GLfloat ) w, -10.0, 10.0 );
    else
        glOrtho( -2.0 * ( GLfloat ) w / ( GLfloat ) h,
                 2.0 * ( GLfloat ) w / ( GLfloat ) h, -2.0, 2.0, -10.0, 10.0 );
    glMatrixMode( GL_MODELVIEW );
}

// mouse callback, selects axis of rotation
void mouse( int btn, int state, int x, int y )
{
    if ( btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) axis = 0;
    if ( btn == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN ) axis = 1;
    if ( btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) axis = 2;
}

// keyboard callback (press spacebar to start/stop animation)
void key( unsigned char k, int x, int y )
{
    static bool anim = true;

    switch ( k )
    {
        // toggle animation (spacebar)
        case ' ': ( anim = !anim ) ? glutIdleFunc( spinObject ) : glutIdleFunc( NULL ); break;
        // select axis of rotation
        case 'x': case 'X': axis = 0; break;
        case 'y': case 'Y': axis = 1; break;
        case 'z': case 'Z': axis = 2; break;
        // exit on Escape key
        case  27: exit( 0 ); break;
    }
}

// build a checkerboard texture pattern
void makeTexture( GLubyte image[64][64][3] )
{
    int i, j, c;
    for ( i = 0;i < 64;i++ )
    {
        for ( j = 0;j < 64;j++ )
        {
            c = ( ( ( ( i & 0x8 ) == 0 ) ^ ( ( j & 0x8 ) ) == 0 ) ) * 255;
            image[i][j][0] = ( GLubyte ) c;
            image[i][j][1] = ( GLubyte ) c;
            image[i][j][2] = ( GLubyte ) c;
        }
    }
}

// main function
int main( int argc, char **argv )
{
    // GLUT inits: need both double buffering and z buffer
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 500, 500 );
    glutCreateWindow( "teapot" );

    // callbacks
    glutReshapeFunc( myReshape );
    glutDisplayFunc( display );
    glutIdleFunc( spinObject );
    glutMouseFunc( mouse );
    glutKeyboardFunc( key );

    glClearColor( 1.0, 1.0, 1.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );

    // texture mapping
    GLubyte image[64][64][3];
    makeTexture( image );
    glTexImage2D( GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    // building mipmaps is not essential, but can improve mapping
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, image );

    // generate GLU quadric sphere with surface normals and texture coordinates
    sphere = gluNewQuadric( );
    gluQuadricDrawStyle( sphere, GLU_FILL );
    gluQuadricNormals( sphere, GLU_SMOOTH );
    gluQuadricTexture( sphere, GL_TRUE );

    // event loop
    glutMainLoop();
}
