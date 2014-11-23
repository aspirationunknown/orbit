void Body::add_moon(const float color[3], double emissivity, double radius, double orbital_radius, double orbital_period, double rotation_period, const char* texture_file)
{
    Body newBody;
    newBody.center.x = this.position.x;
    newBody.center.y = this.position.y;
    newBody.center.z = this.position.z;

    newBody.color[0] = color[0];
    newBody.color[1] = color[1];
    newBody.color[2] = color[2];

    newBody.emissivity = emissivity;
    newBody.orbital_period = orbital_period / 365;
    newBody.rotation_period = rotation_period / (365 * 24);
    newBody.orbital_radius = orbital_radius * 10;
    newBody.radius = radius / 1000;

    this.moons[this.num_moons] = newBody;
    this.num_moons++;
}

void Body::step( double speed )
{

}
