#include <GL/freeglut.h>

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
const double orbital_radius_offset = 20;
const double radius_scale = 1.0/1000;

struct Point
{
    int x;
    int y;
    int z;
};

struct Vector
{
    double dx;
    double dy;
    double dz;

    void normalize();
};

struct Ring
{
    float color[3];
    GLubyte* image;
    
    double min_radius;
    double max_radius;
};

struct Body
{
    Point center;
    Point position;
    float color[3];
    double emissivity;
    Body *moons;

    GLubyte* image;

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
    void add_moon(const float color[3], double emissivity, double radius, double orbital_radius, double orbital_period, double rotation_period, const char* texture_file);
    void add_rings(const float color[3], double minradius, double maxradius, const char* texture_file);
};

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
 
    void pan_forward(int d);
    void pan_backward(int d);
    void pan_left(int d);
    void pan_right(int d);
};

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

void getTexture(GLubyte* image, const char* filename);

// function by Dr. Weiss
bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, GLubyte* &ImagePtr );
