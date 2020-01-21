#ifndef LEAF_H
#define LEAF_H
#include "myobjects.h"

/**
 * @brief The Leaf class used to generate a leaf for the 3D model of our tree
 */
class Leaf : public myObjects
{
public:
    /**
     * @brief Leaf constructor
     * @param width
     * @param height
     */
    Leaf(float width=1, float height=2);

    /**
     * @brief makeGLObject to create the vertices and triangles composing the leaf
     */
    void makeGLObject() override;

    /**
     * @brief display the leaf in vbo
     * @param projectionMatrix
     * @param viewMatrix
     */
    void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override;

    /**
     * @brief initializeGL
     */
    void initializeGL() override;

    /**
     * @brief height
     */
    float height;

    /**
     * @brief width
     */
    float width;
};

#endif // LEAF_H
