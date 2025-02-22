#pragma once

#include "raylib.h"
#include "raymath.h"

class Triangle
{
public:

    Vector3 p1;
    Vector3 p2;
    Vector3 p3;
    Color color;

    Triangle(Vector3 p1, Vector3 p2, Vector3 p3, Color color){
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
        this->color = color;
    };    

    void draw(){
        DrawTriangle3D(p2, p1, p3, color);
    };
};
