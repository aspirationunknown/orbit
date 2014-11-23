#include "solar.h"

//drawWireSpheres
//glutWireSphere(GLdouble radius, GLint slices, GLint stacks); slices = longitude, stacks = lattitude
drawWired(Body body)
{
    glPushMatrix();
      glTranslatef(body.center.x, body.center.y, body.center.z);
      glColorfv(body.color);
      glutWireSphere(body.radius, 50, 50);
    glPopMatrix();
}

//drawSolidScene
//draw the solar system using solid spheres

drawSmooth(Body body)
{
    glPushMatrix();
      glTranslatef(body.center.x, body.center.y, body.center.z);
      glColorfv(body.color);
      glutSolidSphere(body.radius, 50, 50);
    glPopMatrix();
}


//draw textured sphere
//draw textured ring 
//rings with be drawn as a cyllindar with a min and max radius but height of zero
//body.image is texture map for body, body.ring.image for rings, check body.has_ring true if it has rings and false if it does not
drawTextured(Body body)
{
    sphere = gluNewQuadric( );
    gluQuadricDrawStyle( sphere, GLU_FILL );//draws the sphere with filled in polygons
    gluQuadricNormals( sphere, GLU_SMOOTH );//creates normals for all verticies
    gluQuadricTexture( sphere, GL_TRUE );//creates texture coordinates

    glEnable(GL_TEXTURE_2D);

    if(body.has_ring == true)
    {
        ring = gluNewQuadric();
        //need to set center of ring to center of body
        gluCylinder(ring, body.ring.min_radius, body.ring.max_radius, 0.0, 20, 20);
        
    }
}

//drawOrbit
//draw a dotted circle using the body's orbital_radius
drawOrbit(Body body)
{

}




