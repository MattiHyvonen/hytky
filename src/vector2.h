#pragma once

#include <math.h>
#include <iostream>
#include <vector>

class vec2{
public:
    vec2(float x_=0, float y_=0) : x(x_), y(y_) {}
    
    float x;
    float y;
    
    float length() const {
        return sqrt(lengthSquared() );
    }
    
    float lengthSquared() const {
        return x*x + y*y;
    }
    
    vec2 operator+(const vec2& rhs) const {
        return vec2(x + rhs.x, y + rhs.y);
    }

    void operator+=(const vec2& rhs) {
        x += rhs.x;
        y += rhs.y;
    }
    
    vec2 operator-(const vec2& rhs) const {
        return vec2(x - rhs.x, y - rhs.y);
    }
    
    void operator-=(const vec2& rhs) {
        x-= rhs.x;
        y-= rhs.y;
    }
    
    vec2 operator*(const float& rhs) {
        return vec2(x * rhs, y * rhs);
    }
    
    void operator*=(const float& rhs) {
        x *= rhs;
        y *= rhs;
    }
    
    vec2 operator/(const float& rhs) {
        if(rhs != 0)
            return vec2(x / rhs, y / rhs);
        else {
            std::cout << "vec2: nollalla jako!\n";
            return vec2(0,0);
        }
    }
    
    void operator/=(const float& rhs) {
        if(rhs != 0) {
            x /= rhs;
            y /= rhs;
        }
        else std::cout << "vec2: nollalla jako!\n";
    }
    
    vec2 getNormalized() {
        float l = length();
        if(l == 0) return vec2(0,0);
        else return vec2(x/l, y/l);
    }
    
    vec2 getScaled(const float& l) {
        return getNormalized() * l;
    }
    
    vec2 getSquared() {
        float l = lengthSquared();
        return getNormalized()*l;
    }
    
    vec2 getCubed() {
        return vec2(x*x*x, y*y*y);
    }
    
    vec2 getSqrt() {
        float l = sqrt(length());
        return getNormalized()*l;
    }
};

vec2 getMedian(const std::vector<vec2>& pisteet);