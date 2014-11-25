#include "solar.h"

//drawWireSpheres
//glutWireSphere(GLdouble radius, GLint slices, GLint stacks); slices = longitude, stacks = lattitude
void drawWired(Body body)
{
    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      glColor3fv(body.color);
      glutWireSphere(body.radius, 50, 50);
    glPopMatrix();

    if(body.has_ring == true)
    {
        drawWiredRing(body);
    }
}

//drawSolidScene
//draw the solar system using solid spheres

void drawSmooth(Body body)
{
    GLUquadricObj* sphere;

    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      sphere = gluNewQuadric( );
      gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
      gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
      gluQuadricTexture( sphere, GL_TRUE );
      glColor3fv(body.color);
      gluSphere(sphere, body.radius, 50, 50);
    glPopMatrix();

    if(body.has_ring == true)
    {
        drawSmoothRing(body);
    }
}


//draw textured sphere
//draw textured ring 
//rings with be drawn as a cyllindar with a min and max radius but height of zero
//body.image is texture map for body, body.ring.image for rings, check body.has_ring true if it has rings and false if it does not
void drawTextured(Body body)
{
    GLUquadricObj* sphere;
    if(body.is_asteroid == false)
    {
        glEnable(GL_TEXTURE_2D);
        glTexImage2D( GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, body.image );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        sphere = gluNewQuadric( );
        gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
        gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
        gluQuadricTexture( sphere, GL_TRUE );//creates texture coordinates
    }
    else
    {
        glEnable(GL_TEXTURE_2D);
        glTexImage2D( GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, asteroid_image );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

        sphere = gluNewQuadric( );
        gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
        gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
        gluQuadricTexture( sphere, GL_TRUE );//creates texture coordinates
    }

    if(body.has_ring == true)
    {
        drawTexturedRing(body);
    }
}

//drawOrbit
//draw a dotted circle using the body's orbital_radius
void drawOrbit(Body body)
{
    //body.center is center of orbit
    //body.orbital_radius
    int i;
    
    displayLabel(body);
    glBegin(GL_LINES);
    glColor3fv(White);
    for(i = 0; i < 360; ++i)
    {
        glVertex3f(body.center.x + body.orbital_radius * cos(i * M_PI / 180.0), body.center.y + body.orbital_radius * sin(i * M_PI / 180.0), 0.0);
        glVertex3f(body.center.x + body.orbital_radius * cos((i + 1) * M_PI / 180.0), body.center.y + body.orbital_radius * sin((i + 1) * M_PI / 180.0), 0.0);
    } 
    glEnd();
}

//drawRing
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
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
    
}

void drawWiredRing(Body body)
{
    GLUquadricObj* ring;
    ring = gluNewQuadric();
    gluQuadricDrawStyle(ring, GLU_LINE);
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

void drawSmoothRing(Body body)
{
    GLUquadricObj* ring;
    ring = gluNewQuadric();
    glShadeModel(GL_SMOOTH);
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

void drawFlatRing(Body body)
{
    GLUquadricObj* ring;
    ring = gluNewQuadric();
    glShadeModel(GL_FLAT);
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

void drawFlat(Body body)
{
    GLUquadricObj* sphere;
    
    glPushMatrix();
      glTranslatef(body.position.x, body.position.y, body.position.z);
      sphere = gluNewQuadric( );
      gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
      gluQuadricNormals( sphere, GLU_FLAT );//creates normals for all verticies
      gluQuadricTexture( sphere, GL_TRUE );
      glColor3fv(body.color);
      gluSphere(sphere, body.radius, 50, 50);
    glPopMatrix();
}

void cameraFunction(Camera camera)
{
    gluLookAt(camera.position.x, camera.position.y, camera.position.z, camera.look_at.x, camera.look_at.y, camera.look_at.z, camera.up.dx, camera.up.dy, camera.up.dz);
}

void displayLabel(Body body)
{
    drawBitmapString(body.name.c_str(), body.position.x, body.position.y, -body.radius - 5.0, White);
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
