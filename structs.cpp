#include "structs.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <iostream>


  /***************************************************************************//**
 * GetRand
 * Authors - Derek Stotz, Charles Parsons
 *
 * Returns a random number from min to max
 ******************************************************************************/
int getRand(int min, int max)
{
    return min + rand() % (max - min);
}


  /***************************************************************************//**
 * GetRandRadians
 * Authors - Derek Stotz, Charles Parsons
 *
 * Returns a random angle in radians, for the asteroid generation
 ******************************************************************************/
double getRandRadians()
{
    return (((double)rand()) / RAND_MAX) * 2 * M_PI;
}


  /***************************************************************************//**
 * Get Texture
 * Authors - Derek Stotz, Charles Parsons
 *
 * Loads a bmp file given by a filename string and stores it into a given byte array.
 ******************************************************************************/
void getTexture(GLubyte* image, const char* filename)
{
    std::string name(filename);
    int num_rows = 256;
    int num_cols = 512;
    if (!LoadBmpFile( ("./resources/" + name).c_str(), num_rows, num_cols, image ))
        std::cout << "Failed to load" << "./resources/" + name << std::endl;
}


  /***************************************************************************//**
 * Add Moon
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates and adds a single body to the moon list.  Similar to the Add Body function
    found in solar.cpp
 ******************************************************************************/
void Body::add_moon(const float color[3], double emissivity, double radius, double orbital_radius, double orbital_period, double rotation_period, const char* texture_file, const char* name)
{
    Body newBody;
    newBody.name = std::string(name);

    newBody.center.x = this->position.x;
    newBody.center.y = this->position.y;
    newBody.center.z = this->position.z;

    newBody.color[0] = color[0];
    newBody.color[1] = color[1];
    newBody.color[2] = color[2];

    newBody.emissivity = emissivity;
    newBody.orbital_period = orbital_period / 365;
    newBody.rotation_period = rotation_period / (365 * 24);
    newBody.orbital_radius = orbital_radius * 10;
    newBody.radius = radius / 1000;

    this->moons[this->num_moons] = newBody;
    this->num_moons++;
}


  /***************************************************************************//**
 * Step
 * Authors - Derek Stotz, Charles Parsons
 *
 * Moves a body a distance defined by a given speed, updating the current position
    in orbit and doing so for all of its moons as well.
 ******************************************************************************/
void Body::step( double speed )
{
    // since the framerate is 60 fps
    double increment = speed * (2 * M_PI) / this->orbital_period / 60;
    this-> orbital_rotation += increment;

    increment = speed * (2 * M_PI) / this->rotation_period / 60;
    this-> rotation += increment;

    // update the position

    this->position.x = this->center.x + cos( this->orbital_rotation ) * this->orbital_radius;
    this->position.y = this->center.y + sin( this->orbital_rotation ) * this->orbital_radius;

    // update the centers of the moons
    for (int i = 0; i < this->num_moons; i++)
    {
        this->moons[i].center = this->position;
        this->moons[i].step(speed);
    }    
}


  /***************************************************************************//**
 * Step
 * Authors - Derek Stotz, Charles Parsons
 *
 * Steps a given amount for every asteroid in the belt
 ******************************************************************************/
void Belt::step( double speed )
{
    for (int i = 0 ; i < this->count; i++)
    {
        asteroids[i].step(speed);
    }
}


  /***************************************************************************//**
 * Add Rings
 * Authors - Derek Stotz, Charles Parsons
 *
 * Adds a ring to a body with given color information and a shape defined by a min and max radius.
 ******************************************************************************/
void Body::add_rings(const float color[3], double minradius, double maxradius, const char* texture_file)
{
    this->ring.min_radius = minradius;
    this->ring.max_radius = maxradius;

    this->ring.color[0] = color[0];
    this->ring.color[1] = color[1];
    this->ring.color[2] = color[2];

    getTexture(this->ring.image, texture_file);
}


  /***************************************************************************//**
 * Create Asteroids
 * Authors - Derek Stotz, Charles Parsons
 *
 * Creates randomly generated asteroids and adds them to the asteroid belt
 ******************************************************************************/
void Belt::create_asteroids()
{
    srand (time(NULL));
    
    for (int i = 0; i < this->count; i++)
    {
        Body newBody;
        
        newBody.color[0] = this->color[0];
        newBody.color[1] = this->color[1];
        newBody.color[2] = this->color[2];

        newBody.emissivity = 0;
        newBody.orbital_period = getRand(this->min_orbital_period, this->max_orbital_period) * orbital_period_scale;
        newBody.rotation_period = getRand(this->min_rotation_period, this->max_rotation_period) * rotation_period_scale;
        newBody.orbital_radius = getRand(this->min_orbital_radius, this->max_orbital_radius) *orbital_radius_scale + orbital_radius_offset;
        newBody.radius = getRand(this->min_radius, this->max_radius) *radius_scale;

        newBody.rotation = getRandRadians();
        newBody.orbital_rotation = getRandRadians();

        newBody.is_asteroid = true; // use the static texture

        this->asteroids[i] = newBody;
    }
}

  /***************************************************************************//**
 * Rotate Up
 * Authors - Derek Stotz, Charles Parsons
 *
 * Raises the view angle by a distance d
 ******************************************************************************/
void Camera::rotate_up(double d)
{
    this->rotate_down(-1 * d);
}

  /***************************************************************************//**
 * Rotate Down
 * Authors - Derek Stotz, Charles Parsons
 *
 * Lowers the view angle by a distance d
 ******************************************************************************/
void Camera::rotate_down(double d)
{
    if(this->position.z > d)
    {
        this->position.z -= d;
        this->position.y += d;
    }
}

  /***************************************************************************//**
 * Pan Up
 * Authors - Derek Stotz, Charles Parsons
 *
 * Pans the camera up by distance d
 ******************************************************************************/
void Camera::pan_up(int d)
{
    this->position.x += this->up.dx * d;
    this->position.y += this->up.dy * d;

    this->look_at.x += this->up.dx * d;
    this->look_at.y += this->up.dy * d;
}

  /***************************************************************************//**
 * Pan Down
 * Authors - Derek Stotz, Charles Parsons
 *
 * Pans the camera down by distance d
 ******************************************************************************/
void Camera::pan_down(int d)
{
    this->pan_up(-1 * d);
}

  /***************************************************************************//**
 * Zoom In
 * Authors - Derek Stotz, Charles Parsons
 *
 * Moves the camera towards the look_at point by a distance d
 ******************************************************************************/
void Camera::zoom_in(int d)
{
    Vector forward;
    forward.dx = look_at.x - position.x;
    forward.dy = look_at.y - position.y;
    forward.dz = look_at.z - position.z;

    // only zoom in if the magnitude of look_at -> position is greater than d
    if(sqrt((forward.dx)*(forward.dx) + (forward.dy)*(forward.dy) + (forward.dz)*(forward.dz)) <= d || (forward.dz+1) >= this->position.z)
        return;

    forward.normalize();
    
    this->position.x += forward.dx * d;
    this->position.y += forward.dy * d;
    this->position.z += forward.dz * d;
}

  /***************************************************************************//**
 * Zoom Out
 * Authors - Derek Stotz, Charles Parsons
 *
 * Zooms the camera away from the look_at point by distance d
 ******************************************************************************/
void Camera::zoom_out(int d)
{
    this->zoom_in(-1 * d);
}

  /***************************************************************************//**
 * Pan Left
 * Authors - Derek Stotz, Charles Parsons
 *
 * Pans the camera left by a distance d
 ******************************************************************************/
void Camera::pan_left(int d)
{
    this->position.x += this->left.dx * d;
    this->position.y += this->left.dy * d;

    this->look_at.x += this->left.dx * d;
    this->look_at.y += this->left.dy * d;
}

  /***************************************************************************//**
 * Pan Right
 * Authors - Derek Stotz, Charles Parsons
 *
 * Pans the camera to the right by a distance d
 ******************************************************************************/
void Camera::pan_right(int d)
{
    this->pan_left(-1 * d);
}

  /***************************************************************************//**
 * Normalize
 * Authors - Derek Stotz, Charles Parsons
 *
 * Normalizes a vector
 ******************************************************************************/
void Vector::normalize()
{
    double magnitude = sqrt((this->dx)*(this->dx) + (this->dy)*(this->dy) + (this->dz)*(this->dz));
    this->dx /= magnitude;
    this->dy /= magnitude;
    this->dz /= magnitude;
}
