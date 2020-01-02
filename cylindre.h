#ifndef CYLINDRE_H
#define CYLINDRE_H
#include <cmath>

#include "myobjects.h"

class Cylindre : public myObjects
{
public:
    Cylindre(float radius=1, float height=2, int subdivision=16);
    Cylindre(float radius1, float radius2, float height, int subdivision);

    void makeGLObject() override;
    void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override;
    void initializeGL() override;

    int subdivision;
    float height,radius1,radius2;
};

#endif // CYLINDRE_H
