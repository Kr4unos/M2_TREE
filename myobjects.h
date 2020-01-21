#ifndef MYOBJECTS_H
#define MYOBJECTS_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QDebug>

#include <QString>

/**
 * @brief The myObjects class (abstract) used to serialize and store every tables of similar objects
 */
class myObjects
{
public:
    /**
     * @brief myObjects constructor
     */
    myObjects();

    /**
     * @brief ~myObjects destructor
     */
    virtual ~myObjects();

    /**
     * @brief makeGLObject abstract
     */
    virtual void makeGLObject() = 0;

    /**
     * @brief display abstract
     * @param projectionMatrix
     * @param viewMatrix
     */
    virtual void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) = 0;

    /**
     * @brief initializeGL abstract
     */
    virtual void initializeGL() = 0;

    /**
     * @brief tearGLObjects
     */
    virtual void tearGLObjects();

    /**
     * @brief onTimeout
     * @param dt
     */
    virtual void onTimeout(float dt);

    /**
     * @brief addObj
     * @param matrice
     * @param size
     * @param displayMode
     * @return id of the added object
     */
    virtual uint addObj(QMatrix4x4 matrice, float size = 1, int displayMode =1);

protected:
    /**
     * @brief matrice used to keep track of translations and rotations on our object
     */
    std::vector<QMatrix4x4> matrice;

    /**
     * @brief displayMode used to choose the displayMode of the object
     */
    std::vector<int> displayMode;

    /**
     * @brief size size of the object (used by the shader)
     */
    std::vector<float> size;

    /**
     * @brief nbObject count the number of object we have
     */
    uint nbObject=0;

    /**
     * @brief modelMatrix
     */
    QMatrix4x4 modelMatrix;

    /**
     * @brief shaderProgram
     */
    QOpenGLShaderProgram *shaderProgram;

    /**
     * @brief vbo
     */
    QOpenGLBuffer *vbo;

    /**
     * @brief texture
     */
    QOpenGLTexture *texture;

    /**
     * @brief privateVbo
     */
    bool privateVbo=false;

    /**
     * @brief privateTexture
     */
    bool privateTexture=false;

    /**
     * @brief privateShaderProgram
     */
    bool privateShaderProgram=false;

public:

    /**
     * @brief setVbo
     * @param vbo
     */
    virtual void setVbo(QOpenGLBuffer * vbo=nullptr);

    /**
     * @brief setShaderProgram
     * @param shaderProgram
     */
    virtual void setShaderProgram(QOpenGLShaderProgram &shaderProgram);

    /**
     * @brief setShaderProgram
     * @param pathAndName
     */
    virtual void setShaderProgram(char * pathAndName);

    /**
     * @brief setShaderProgram
     * @param pathVertexShader
     * @param pathFragmentShader
     */
    virtual void setShaderProgram(char * pathVertexShader, char * pathFragmentShader);

    /**
     * @brief setTexture
     * @param texture
     */
    virtual void setTexture(QOpenGLTexture &texture);

    /**
     * @brief setTexture
     * @param path
     */
    virtual void setTexture(char * path);

    /**
     * @brief getNbObject
     * @return
     */
    virtual uint getNbObject() const;

    /**
     * @brief setSize
     * @param value
     * @param id
     * @return if id is an existent object and size is successfully set
     */
    bool setSize(float value, uint id=0);

    /**
     * @brief getSize
     * @param id
     * @return 0 if id is not created
     */
    float getSize(uint id=0) const;

    /**
     * @brief setDisplayMode
     * @param displayMode
     * @param id
     * @return if id is an existent object and displayMode is successfully set
     */
    bool setDisplayMode(int displayMode, uint id=0);

    /**
     * @brief getDisplayMode
     * @param id
     * @return 0 if id is not created
     */
    int getDisplayMode(uint id=0) const;

    /**
     * @brief setMatrice
     * @param matrice
     * @param id
     * @return if id is an existent object and matrice is successfully set
     */
    bool setMatrice(QMatrix4x4 matrice, uint id=0);

    /**
     * @brief getMatrice
     * @param id
     * @return if id is an existent object returns the matrix else return identity matrix
     */
    QMatrix4x4 getMatrice(uint id=0) const;
};

#endif // MYOBJECTS_H
