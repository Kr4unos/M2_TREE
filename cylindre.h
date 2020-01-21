#ifndef CYLINDRE_H
#define CYLINDRE_H
#include <cmath>
#include "myobjects.h"

/**
 * @brief The Cylindre class used to generate log and branches for our trees
 */
class Cylindre : public myObjects
{
public:
    /**
     * @brief Cylindre constructor
     * @param radius
     * @param height
     * @param subdivision
     */
    Cylindre(float radius=1, float height=2, int subdivision=16);

    /**
     * @brief Cylindre constructor if variable radius
     * @param radius1
     * @param radius2
     * @param height
     * @param subdivision
     */
    Cylindre(float radius1, float radius2, float height, int subdivision);

    /**
     * @brief makeGLObject
     */
    void makeGLObject() override;

    /**
     * @brief display
     * @param projectionMatrix
     * @param viewMatrix
     */
    void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) override;

    /**
     * @brief initializeGL
     */
    void initializeGL() override;

    /**
     * @brief subdivision
     */
    int subdivision;

    /**
     * @brief height
     */
    float height;

    /**
     * @brief radius1
     */
    float radius1;

    /**
     * @brief radius2
     */
    float radius2;
};

#endif // CYLINDRE_H
