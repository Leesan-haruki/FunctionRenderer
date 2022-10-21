/*
    Boolean operation is available for these models.
    Reference: https://iquilezles.org/articles/distfunctions/
*/

#include "model.h"

class Union : public Model {
public:
    Union(Model* m1, Model* m2): m1(m1), m2(m2) {}
    double sdf(Vector3 v){
        return std::min(m1->sdf(v), m2->sdf(v));
    }
    Vector3 normal(Vector3 v){
        if(m1->sdf(v) > m2->sdf(v)) return m2->normal(v);
        else return m1->normal(v);
    }
private:
    Model *m1, *m2;
};

class Intersection : public Model {
public:
    Intersection(Model* m1, Model* m2): m1(m1), m2(m2) {}
    double sdf(Vector3 v){
        return std::max(m1->sdf(v), m2->sdf(v));
    }
    Vector3 normal(Vector3 v){
        if(m1->sdf(v) > m2->sdf(v)) return m2->normal(v);
        else return m1->normal(v);
    }
private:
    Model *m1, *m2;
};

class Difference : public Model {
    // m1 - m2
public:
    Difference(Model* m1, Model* m2): m1(m1), m2(m2) {}
    double sdf(Vector3 v){
        return std::max(m1->sdf(v), -m2->sdf(v));
    }
    Vector3 normal(Vector3 v){
        if(m1->sdf(v) > m2->sdf(v)) return m2->normal(v);
        else return m1->normal(v);
    }
private:
    Model *m1, *m2;
};

