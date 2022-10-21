/*
    Basic vector operation, overloading some operators, implementing dot product, cross product, and so on.
*/

#include <algorithm>
#include <cmath>
#include <iostream>

#ifndef _VECTOR3_H_
#define _VECTOR3_H_

const double EPSILON = 1.0e-3;

class Vector3{
public:
    Vector3(double x, double y, double z): x(x), y(y), z(z) {}
    
    Vector3 operator+(const Vector3 &other){
        Vector3 ret = Vector3(x + other.x, y+other.y, z+other.z);
        return ret;
    };
    Vector3& operator+=(const Vector3 &other){
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }
    Vector3 operator-(const Vector3 &other){
        Vector3 ret = Vector3(x - other.x, y-other.y, z-other.z);
        return ret;
    }
    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }
    Vector3& operator-=(const Vector3 &other){
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }
    Vector3 operator*(const double c){
        return Vector3(x * c, y * c, z * c);
    }
    Vector3 operator/(const double c){
        if(c == 0){
            // std::cout << "Warning: 0 division is done." << std::endl;
            return Vector3(0, 0, 0);
        } 
        return Vector3(x / c, y / c, z / c);
    }
    bool operator==(const Vector3 &other) const {
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON && std::abs(z - other.z) < EPSILON;
    }
    Vector3 cross(const Vector3 &other){
        Vector3 ret = Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
        return ret;
    };
    double dot(const Vector3 &other){
        double ret = x * other.x + y * other.y + z * other.z;
        return ret;
    }
    double norm(){
        return std::sqrt(x * x + y * y + z * z);
    };
    double dist(const Vector3 &other){
        Vector3 diff = *this - other;
        return diff.norm();
    }
    Vector3 normalize(){
        double length = this->norm();
        return *this / length;
    }
    void print(){
        std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl;
    }

    double x, y, z;
};

#endif
