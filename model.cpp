/*
    Each model has
        parameters (member variable) to determine the shape
        constructor to register paramters  
        SDF (member function) used in sphere tracing 
        normal (member function) used in texturing
    Torus and Quadric are difficult to compute SDF analytically, so first-order approximation is implemented
    Reference: https://iquilezles.org/articles/distfunctions/
               Taubin 1994
*/

#include <array>
#include <assert.h>
#include <vector>

#include "model.h"

Sphere::Sphere(double x, double y, double z, double r) : 
    c(Vector3(x, y, z)), r(r) {}
Sphere::Sphere(Vector3 c, double r) : c(c), r(r) {}
double Sphere::sdf(Vector3 v) {
    return v.dist(c) - r;
}
Vector3 Sphere::normal(Vector3 v) {
    // analytical
    return Vector3(2 * (v.x - c.x), 2 * (v.y - c.y), 2 * (v.z - c.z)).normalize();
}

Rectangular::Rectangular(Vector3 o, double len): o(o), a(Vector3(o.x + len, o.y, o.z)),
    b(Vector3(o.x, o.y + len, o.z)), c(Vector3(o.x, o.y, o.z + len)) {}
Rectangular::Rectangular(Vector3 o, double w, double d, double h): o(o), a(Vector3(o.x + w, o.y, o.z)),
    b(Vector3(o.x, o.y + d, o.z)), c(Vector3(o.x, o.y, o.z + h)) {}
double Rectangular::sdf(Vector3 v) {
    double d_x = std::max(v.x - a.x, o.x - v.x);
    double d_y = std::max(v.y - b.y, o.y - v.y);
    double d_z = std::max(v.z - c.z, o.z - v.z);
    if(d_x < 0.0 && d_y < 0.0 && d_z < 0.0){  // inside
        return std::max(std::max(d_x, d_y), d_z);
    }
    else{  // outside
        d_x = std::max(d_x, 0.0);
        d_y = std::max(d_y, 0.0);
        d_z = std::max(d_z, 0.0);
        return Vector3(d_x, d_y, d_z).norm();
    }
}
Vector3 Rectangular::normal(Vector3 v) {
    if(std::abs(v.z - c.z) < EPSILON) return Vector3(0, 0, 1);
    else if(std::abs(v.z - o.z) < EPSILON) return Vector3(0, 0, -1);
    else if(std::abs(v.y - b.y) < EPSILON) return Vector3(0, 1, 0);
    else if(std::abs(v.y - o.y) < EPSILON) return Vector3(0, -1, 0);
    else if(std::abs(v.x - a.x) < EPSILON) return Vector3(1, 0, 0);
    else if(std::abs(v.x - o.x) < EPSILON) return Vector3(-1, 0, 0);
    else return Vector3(0, 0, 0);
}

Cylinder::Cylinder(Vector3 c1, Vector3 c2, double r): c1(c1), c2(c2), r(r) {}
double Cylinder::sdf(Vector3 v) {
    Vector3 dir = c2 - c1;
    double height = dir.norm();
    /* compute the intersection of line and plane
        line: x = c1 + t * d
        plane: d・(v - x) = 0 */
    double t = dir.dot(v - c1) / (dir.dot(dir));
    double side_dist = v.dist(c1 + dir * t) - r;
    
    if(t >= 0 && t <= 1){
        return side_dist >= 0 ? side_dist : std::min(side_dist, std::min(height * t, height * (1-t)));
    }
    else{
        if(t > 1){
            return side_dist >= 0 ? std::sqrt(side_dist * side_dist + height * (t-1) * height * (t-1)) : height * (t-1);
        }
        else{
            return side_dist >= 0 ? std::sqrt(side_dist * side_dist + height * (0-t) * height * (0-t)) : height * (0-t);
        }
    }
}
Vector3 Cylinder::normal(Vector3 v) {
    // analytical
    Vector3 dir = c2 - c1;
    double t = dir.dot(v - c1) / (dir.dot(dir));
    if(t >= 1)
        return dir.normalize();
    else if(t <= 0)
        return (-dir).normalize();
    else{
        Vector3 n1 = dir.cross(Vector3(1, 0, 0)).normalize();
        Vector3 n2 = n1.cross(dir).normalize();
        double x2, y2, z2, xy, yz, zx;
        x2 = n1.x * n1.x + n2.x * n2.x;
        y2 = n1.y * n1.y + n2.y * n2.y;
        z2 = n1.z * n1.z + n2.z * n2.z;
        xy = 2 * n1.x * n1.y + 2 * n2.x * n2.y;
        yz = 2 * n1.y * n1.z + 2 * n2.y * n2.z;
        zx = 2 * n1.z * n1.x + 2 * n2.z * n2.x;
        
        return Vector3(
            2 * x2 * v.x + xy * v.y + zx * v.z,
            2 * y2 * v.y + yz * v.z + xy * v.x,
            2 * z2 * v.z + zx * v.x + yz * v.y
        ).normalize();
    }
}

Torus::Torus(double R, double r): R(R), r(r) {}
double Torus::sdf(Vector3 v) {
    double p = (v.x * v.x + v.y * v.y + v.z * v.z + R * R - r * r);
    double f0 = p * p - 4 * R * R * (v.x * v.x + v.y * v.y);
    double dx = 4 * v.x * p - 8 * R * R * v.x;
    double dy = 4 * v.y * p - 8 * R * R * v.y;
    double dz = 4 * v.z * p;
    double f1 = std::sqrt(dx * dx + dy * dy + dz * dz);
    return f0 / f1;
}
Vector3 Torus::normal(Vector3 v) {
    double p = (v.x * v.x + v.y * v.y + v.z * v.z + R * R - r * r);
    double dx = 4 * v.x * p - 8 * R * R * v.x;
    double dy = 4 * v.y * p - 8 * R * R * v.y;
    double dz = 4 * v.z * p;
    return Vector3(dx, dy, dz).normalize();
}

Quadric::Quadric(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j):
    a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h), i(i), j(j) {}
Quadric::Quadric(std::array<double, 10> arr): a(arr[0]), b(arr[1]), c(arr[2]), d(arr[3]), e(arr[4]),
    f(arr[5]), g(arr[6]), h(arr[7]), i(arr[8]), j(arr[9]) {}
double Quadric::sdf(Vector3 v) {
    // first-order approximation
    double f0 = a * v.x * v.x + b * v.y * v.y + c * v.z * v.z + d * v.x * v.y + 
        e * v.y * v.z + f * v.z * v.x + g * v.x + h * v.y + i * v.z + j;
    double dx = 2*a*v.x + d*v.y + f*v.z + g;
    double dy = 2*b*v.y + e*v.z + d*v.x + h;
    double dz = 2*c*v.z + f*v.x + e*v.y + i;
    double f1 = std::sqrt(dx * dx + dy * dy + dz * dz);
    return f0 / f1;
}
Vector3 Quadric::normal(Vector3 v) {
    return Vector3(
        2*a*v.x + d*v.y + f*v.z + g,
        2*b*v.y + e*v.z + d*v.x + h,
        2*c*v.z + f*v.x + e*v.y + i
    ).normalize();
}
