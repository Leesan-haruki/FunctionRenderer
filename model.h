#include "vector3.h"

#ifndef _MODEL_H_
#define _MODEL_H_

class Model {
public:
    Model(){}
    virtual double sdf(Vector3 v) {}
    virtual Vector3 normal(Vector3 v) {}
};

class Sphere : public Model {
public:
    Sphere(double x, double y, double z, double r);
    Sphere(Vector3 c, double r);
    double sdf(Vector3 v) override;
    Vector3 normal(Vector3 v) override;
private:
    Vector3 c;
    double r;
};

class Rectangular : public Model {
// axis aligned only
public:
    Rectangular(Vector3 o, double len);
    Rectangular(Vector3 o, double w, double d, double h);
    double sdf(Vector3 v) override;
    Vector3 normal(Vector3 v) override;
private:
    Vector3 o, a, b, c;
};

class Cylinder : public Model {
public:
    Cylinder(Vector3 c1, Vector3 c2, double r);
    double sdf(Vector3 v) override;
    Vector3 normal(Vector3 v) override;
private:
    Vector3 c1, c2;
    double r;
};

class Torus : public Model {
// origin center is only available
public:
    Torus(double R, double r);
    double sdf(Vector3 v) override;
    Vector3 normal(Vector3 v) override;
private:
    double R, r;
};

class Quadric : public Model {
// f(x, y, z) = ax^2 + by^2 + cz^2 + dxy + eyz + fzx + gx + hy + iz + j = 0
public:
    Quadric(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j);
    Quadric(std::array<double, 10> arr);
    double sdf(Vector3 v) override;
    Vector3 normal(Vector3 v) override;
private:
    double a, b, c, d, e, f, g, h, i, j;
};

#endif
