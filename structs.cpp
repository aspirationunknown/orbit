#include "structs.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <iostream>

int getRand(int min, int max)
{
    return min + rand() % (max - min);
}

double getRandRadians()
{
    return (((double)rand()) / RAND_MAX) * 2 * M_PI;
}

void getTexture(GLubyte* image, const char* filename)
{
    std::string name(filename);
    int num_rows = 256;
    int num_cols = 512;
    // currently not working
    LoadBmpFile( ("./resources/" + name).c_str(), num_rows, num_cols, image );
}

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

void Belt::step( double speed )
{
    for (int i = 0 ; i < this->count; i++)
    {
        asteroids[i].step(speed);
    }
}

void Body::add_rings(const float color[3], double minradius, double maxradius, const char* texture_file)
{
    this->ring.min_radius = minradius;
    this->ring.max_radius = maxradius;

    this->ring.color[0] = color[0];
    this->ring.color[1] = color[1];
    this->ring.color[2] = color[2];

    getTexture(this->ring.image, texture_file);
}

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

void Camera::rotate_left(double r)
{
    
}
void Camera::rotate_right(double r)
{

}
void Camera::rotate_up(double r)
{

}
void Camera::rotate_down(double r)
{

}


void Camera::pan_up(int d)
{
    std::cout << "\n-------------\nx:" << this->position.x;
    std::cout << "\ny:" << this->position.y;
    std::cout << "\nz:" << this->position.z;

    this->position.x += this->up.dx * d;
    this->position.y += this->up.dy * d;

    this->look_at.x += this->up.dx * d;
    this->look_at.y += this->up.dy * d;
}

void Camera::pan_down(int d)
{
    this->pan_up(-1 * d);
}

void Camera::zoom_in(int d)
{
    std::cout << "\n-------------\nx:" << this->position.x;
    std::cout << "\ny:" << this->position.y;
    std::cout << "\nz:" << this->position.z;

    Vector forward;
    forward.dx = look_at.x - position.x;
    forward.dy = look_at.y - position.y;
    forward.dz = look_at.z - position.z;

    // only zoom in if the magnitude of look_at -> position is greater than d
    if(sqrt((forward.dx)*(forward.dx) + (forward.dy)*(forward.dy) + (forward.dz)*(forward.dz)) <= d)
        return;

    forward.normalize();
    
    this->position.x += forward.dx * d;
    this->position.y += forward.dy * d;
    this->position.z += forward.dz * d;
}

void Camera::zoom_out(int d)
{
    this->zoom_in(-1 * d);
}

void Camera::pan_forward(int d)
{
    std::cout << "\n-------------\nx:" << this->position.x;
    std::cout << "\ny:" << this->position.y;
    std::cout << "\nz:" << this->position.z;
    
    Vector forward;
    forward.dx = look_at.x - position.x;
    forward.dy = look_at.y - position.y;
    forward.dz = look_at.z - position.z;

    forward.normalize();
    
    this->position.x += forward.dx * d;
    this->position.y += forward.dy * d;
    this->position.z += forward.dz * d;
    this->look_at.x += forward.dx * d;
    this->look_at.y += forward.dy * d;
    this->look_at.z += forward.dz * d;
}

void Camera::pan_backward(int d)
{
    this->pan_forward(-1 * d);
}
void Camera::pan_left(int d)
{
    std::cout << "\n-------------\nx:" << this->position.x;
    std::cout << "\ny:" << this->position.y;
    std::cout << "\nz:" << this->position.z;

    this->position.x += this->left.dx * d;
    this->position.y += this->left.dy * d;

    this->look_at.x += this->left.dx * d;
    this->look_at.y += this->left.dy * d;
}
void Camera::pan_right(int d)
{
    this->pan_left(-1 * d);
}
void Vector::normalize()
{
    double magnitude = sqrt((this->dx)*(this->dx) + (this->dy)*(this->dy) + (this->dz)*(this->dz));
    this->dx /= magnitude;
    this->dy /= magnitude;
    this->dz /= magnitude;
}
