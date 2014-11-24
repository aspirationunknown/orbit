#include "solar.h"

//drawWireSpheres
//glutWireSphere(GLdouble radius, GLint slices, GLint stacks); slices = longitude, stacks = lattitude
void drawWired(Body body)
{
    glPushMatrix();
      glTranslatef(body.center.x, body.center.y, body.center.z);
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
    glPushMatrix();
      glTranslatef(body.center.x, body.center.y, body.center.z);
      glColor3fv(body.color);
      glutSolidSphere(body.radius, 50, 50);
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

    glBegin(GL_LINES);
    glColor3fv(White);
    for(i = 0; i < 360; ++i)
    {
        glVertex3f(body.orbital_radius * cos(i * M_PI / 360.0), body.orbital_radius * sin(i * M_PI / 360.0), 0.0);
        glVertex3f(body.orbital_radius * cos((i + 1) * M_PI / 360.0), body.orbital_radius * sin((i + 1) * M_PI / 360.0), 0.0);
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
    //need to set center of ring to center of body
    gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
}

void drawFlat(Body body)
{
    glPushMatrix();
      glTranslatef(body.center.x, body.center.y, body.center.z);
      glColor3fv(body.color);
      glutSolidSphere(body.radius, 50, 50);
    glPopMatrix();
}
