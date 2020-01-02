#ifndef LEAF_H
#define LEAF_H
#include "myobjects.h"


class Leaf : public myObjects
{
public:
    Leaf(float width=1, float height=2);

    void makeGLObject() override;
    void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override;
    void initializeGL() override;

    float height,width;
};

#endif // LEAF_H
