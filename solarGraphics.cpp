/******************************************************************************
* Name: solarGraphics.cpp
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: This file has all of the functions for drawing objects (planets,
*              moons, asteroids, and orbital paths.
*
******************************************************************************/
#include "solar.h"

//externs

// global texture used for asteroids
extern GLubyte* asteroid_image;

// fidelity of wireframes
extern int fidelity;

// initialize the light model
void initLightModel()
{
    glEnable(GL_COLOR_MATERIAL);

    GLfloat mat_specular[] = { .9, .9, .9, 1.0 };
    GLfloat mat_diffuse[] = { .9, .9, .9, 1.0 };
    GLfloat mat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat mat_shininess = { 0.0 };

    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse );
    glMaterialf( GL_FRONT, GL_SHININESS, mat_shininess);

    GLfloat light_ambient[] = { 0.4, 0.4, 0.4, 0.1 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { .9, .9, .9, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };

    glLightfv( GL_LIGHT0, GL_POSITION, light_position );
    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

    glEnable( GL_DEPTH_TEST );
    glColor3f ( 1.0, 1.0, 1.0 );
    glEnable( GL_NORMALIZE );    // automatic normalization of normals
    glEnable( GL_CULL_FACE );    // eliminate backfacing polygons
    glCullFace( GL_BACK );
}

/******************************************************************************
* Name: drawWired
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the wireframe planets, moons, and asteroids.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawWired(Body body)
{
    GLfloat mat_emissive[] = { .8, .8, .8, 1.0 };
    GLfloat mat_nonemissive[] = {0, 0, 0, 1.0};

    if(body.emissivity > 0)
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_emissive );

    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      glRotatef(360 * body.rotation / (2 * M_PI), 0 , 0, 1 );
      glColor3fv(body.color);
      glutWireSphere(body.radius, fidelity, fidelity);
    glPopMatrix();

    if(body.has_ring == true)
    {
        drawWiredRing(body);
    }

    glMaterialfv( GL_FRONT, GL_EMISSION, mat_nonemissive);
}

/******************************************************************************
* Name: drawSmooth
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the smooth shaded planets, moons, and asteroids.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawSmooth(Body body)
{
    GLfloat mat_emissive[] = { .8, .8, .8, 1.0 };
    GLfloat mat_nonemissive[] = {0, 0, 0, 1.0};

    if(body.emissivity > 0)
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_emissive );

    GLUquadricObj* sphere;

    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      sphere = gluNewQuadric( );
      gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
      gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
      gluQuadricTexture( sphere, GL_TRUE );
      glColor3fv(body.color);
      gluSphere(sphere, body.radius, fidelity, fidelity);
    glPopMatrix();

    if(body.has_ring == true)
    {
        drawSmoothRing(body);
    }

    glMaterialfv( GL_FRONT, GL_EMISSION, mat_nonemissive);

}


/******************************************************************************
* Name: drawTextured
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the texture mapped planets, moons, and asteroids.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawTextured(Body body)
{
    GLUquadricObj* sphere;
    if(body.is_asteroid == false)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, body.image );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, 512, 256, GL_RGB, GL_UNSIGNED_BYTE, body.image );

        sphere = gluNewQuadric( );
        gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
        gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
        gluQuadricTexture( sphere, GL_TRUE );//creates texture coordinates
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glTexImage2D( GL_TEXTURE_2D, 0, 3, 512, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, asteroid_image );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        sphere = gluNewQuadric( );
        gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
        gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
        gluQuadricTexture( sphere, GL_TRUE );//creates texture coordinates
        glDisable(GL_TEXTURE_2D);
    }
/*
    if(body.has_ring == true)
    {
        drawTexturedRing(body);
    }
*/
}

/******************************************************************************
* Name: drawOrbit
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the orbital paths of planets and moons.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawOrbit(Body body)
{
    //body.center is center of orbit
    //body.orbital_radius
    int i;
    glBegin(GL_LINES);
    glColor3fv(White);
    for(i = 0; i < 360; ++i)
    {
        glVertex3f(body.center.x + body.orbital_radius * cos(i * M_PI / 180.0), body.center.y + body.orbital_radius * sin(i * M_PI / 180.0), 0.0);
        glVertex3f(body.center.x + body.orbital_radius * cos((i + 1) * M_PI / 180.0), body.center.y + body.orbital_radius * sin((i + 1) * M_PI / 180.0), 0.0);
    } 
    glEnd();
}

/******************************************************************************
* Name: drawTexturedRing
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the texture mapped rings of planets.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawTexturedRing(Body body)
{
    GLUquadricObj* ring;
    glEnable(GL_TEXTURE_2D);
    glTexImage2D( GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, body.ring.image );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    ring = gluNewQuadric();
    gluQuadricDrawStyle( ring, GLU_FILL );//draws the sphere with filled in polygons
    gluQuadricNormals( ring, GLU_SMOOTH );//creates normals for all verticies
    gluQuadricTexture( ring, GL_TRUE );//creates texture coordinates
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, fidelity, fidelity);
    
}

/******************************************************************************
* Name: drawWiredRing
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the wireframe rings of planets.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawWiredRing(Body body)
{
    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      glRotatef(360 * body.rotation / (2 * M_PI), 0 , 0, 1 );
      glColor3fv(body.color);
      glutWireCylinder(body.ring.max_radius, 0.0, fidelity, fidelity);
    glPopMatrix();
}

/******************************************************************************
* Name: drawSmoothRing
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the smooth shaded rings of planets.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawSmoothRing(Body body)
{
    GLUquadricObj* ring;
    ring = gluNewQuadric();
    glShadeModel(GL_SMOOTH);
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

/******************************************************************************
* Name: drawFlatRing
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the flat shaded rings of planets.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawFlatRing(Body body)
{
    GLUquadricObj* ring;
    ring = gluNewQuadric();
    glShadeModel(GL_FLAT);
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

/******************************************************************************
* Name: drawFlat
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the flat shaded planets, moons, and asteroids.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void drawFlat(Body body)
{

    GLfloat mat_emissive[] = { .8, .8, .8, 1.0 };
    GLfloat mat_nonemissive[] = {0, 0, 0, 1.0};

    if(body.emissivity > 0)
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_emissive );
    
    GLUquadricObj* sphere;
    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      sphere = gluNewQuadric( );
      gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
      gluQuadricNormals( sphere, GLU_FLAT );//creates normals for all verticies
      gluQuadricTexture( sphere, GL_TRUE );
      glColor3fv(body.color);
      gluSphere(sphere, body.radius, fidelity, fidelity);
    glPopMatrix();

    glMaterialfv( GL_FRONT, GL_EMISSION, mat_nonemissive);
}

/******************************************************************************
* Name: cameraFunction
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Sets the look at position of the camera
*
* Param[in] camera - a Camera object.
*
******************************************************************************/
void cameraFunction(Camera camera)
{
    gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.look_at.x, camera.look_at.y, camera.look_at.z, camera.up.dx, camera.up.dy, camera.up.dz);
}

/******************************************************************************
* Name: displayLabel
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Draws the names of the different planets, moons, and the sun.
*
* Param[in] body - a Body object that is either a planet, moon, or an asteroid
*
******************************************************************************/
void displayLabel(Body body)
{
    glPushMatrix();
        drawBitmapString(body.name.c_str(), body.position.x, body.position.y, body.radius + 5.0, White);
    glPopMatrix();
}

 /***************************************************************************//**
 * DrawBitmapString
 * Authors - Dr. John Weiss
 *
 * Draws a stroke string for a game menu
 * Parameters -
    string - the string to draw
    x - the x location to draw the string on the screen
    y - the y location to draw the string on the screen
    color -the color the draw the string, stored in a 3-element array
 ******************************************************************************/
void drawBitmapString( const char *string, float x, float y, float z, const float color[] )
{
    glColor3f( color[0], color[1], color[2] );
    glRasterPos3f( x, y, z );
    // while ( *string ) glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, *string++ );
    glutBitmapString( GLUT_BITMAP_HELVETICA_18, (const unsigned char *)string );
}
