/******************************************************************************
* Name: structs.h
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: This file has all of the structs we used for the solar system
*              program as well as color global constants and orbital scaling
*              global constants.
*
******************************************************************************/
#include <GL/freeglut.h>
#include <string>

// colors {R, G, B}
const float Black[]	= { 0.0, 0.0, 0.0 };
const float Red[]	= { 1.0, 0.0, 0.0 };
const float Green[]	= { 0.0, 1.0, 0.0 };
const float Blue[]	= { 0.0, 0.0, 1.0 };
const float Magenta[]	= { 1.0, 0.0, 1.0 };
const float Cyan[]	= { 0.0, 1.0, 1.0 };
const float Yellow[]	= { 1.0, 1.0, 0.0 };
const float White[]	= { 1.0, 1.0, 1.0 };

// scalings
const double orbital_period_scale = 1.0/365;
const double rotation_period_scale = 1.0/(365 * 24);
const double orbital_radius_scale =  1.0;
const double orbital_radius_offset = 0;
const double radius_scale = 1.0/1000.0;

/******************************************************************************
* Name: Point
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents a 3D point.
*
******************************************************************************/
struct Point
{
    int x = 0;
    int y = 0;
    int z = 0;
};

/******************************************************************************
* Name: Vector
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents a 3D vector.
*
******************************************************************************/
struct Vector
{
    double dx = 0;
    double dy = 0;
    double dz = 0;

    void normalize();
};

/******************************************************************************
* Name: Ring
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents a planetary ring
*
******************************************************************************/
struct Ring
{
    float color[3];
    GLubyte* image;
    
    double min_radius;
    double max_radius;
};

/******************************************************************************
* Name: Body
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents a planet, moon, asteroid, or the sun.
*
******************************************************************************/
struct Body
{
    Point center;
    Point position;
    float color[3];
    double emissivity = 0.0;
    Body *moons;

    GLubyte* image;
    std::string name;

    bool is_asteroid = false;

    Ring ring;    
    bool has_ring = false;

    int num_moons = 0;

    double orbital_period;
    double rotation_period;
    
    double orbital_radius;
    double radius;

    double orbital_rotation = 0;
    double rotation = 0;

    void step(double speed);
    void add_moon(const float color[3], double emissivity, double radius, double orbital_radius, double orbital_period, double rotation_period, const char* texture_file, const char* name);
    void add_rings(const float color[3], double minradius, double maxradius, const char* texture_file);
};

/******************************************************************************
* Name: Camera
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents the camera position.
*
******************************************************************************/
struct Camera
{
    Point look_at;
    Point position;

    Vector left;
    Vector up;

    void rotate_left(double r);
    void rotate_right(double r);
    void rotate_up(double r);
    void rotate_down(double r);
 
    void pan_down(int d);
    void pan_up(int d);
    void pan_forward(int d);
    void pan_backward(int d);
    void pan_left(int d);
    void pan_right(int d);
    void zoom_in(int d);
    void zoom_out(int d);
};

/******************************************************************************
* Name: Belt
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: represents an asteroid belt.
*
******************************************************************************/
struct Belt
{
    int count;
    Point center;
    float color[3];    
    Body asteroids[500];

    double min_orbital_period;
    double max_orbital_period;
    double min_rotation_period;
    double max_rotation_period;
    
    double min_orbital_radius;
    double max_orbital_radius;
    double min_radius;
    double max_radius;

    void step(double speed);
    void create_asteroids();
};

/******************************************************************************
* Name: getTexture
*
* Authors: Derek Stotz and Charles Parsons
*
* Description: Gets the texture map from a file and stores it in the Body
*              struct.
*
******************************************************************************/
void getTexture(GLubyte* image, const char* filename);

/******************************************************************************
* Name: LoadBmpFile
*
* Authors: Dr. Weiss
*
* Description: Reads in a bitmap file and stores it in an array.
*
******************************************************************************/
bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, GLubyte* &ImagePtr );
