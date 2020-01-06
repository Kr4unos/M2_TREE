#ifndef MYOBJECTS_H
#define MYOBJECTS_H


#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QDebug>

#include <QString>

class myObjects
{
public:
    myObjects();
    virtual ~myObjects();

    virtual void makeGLObject() =0;
    virtual void display(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix) =0;
    virtual void initializeGL() =0;

    virtual void tearGLObjects();
    virtual void onTimeout(float dt);

    virtual uint addObj(QMatrix4x4 matrice, float size = 1, int displayMode =1);
        //return id

protected:
    std::vector<QMatrix4x4> matrice;
    std::vector<int> displayMode;
    std::vector<float> size;
    uint nbObject=0;


    QMatrix4x4 modelMatrix;
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLBuffer *vbo;
    QOpenGLTexture *texture;
    bool privateVbo=false,privateTexture=false,privateShaderProgram=false;

public:
    //get set zone
    virtual void setVbo(QOpenGLBuffer * vbo=nullptr);
    virtual void setShaderProgram(QOpenGLShaderProgram &shaderProgram);
    virtual void setShaderProgram(char * pathAndName);
    virtual void setShaderProgram(char * pathVertexShader, char * pathFragmentShader);
    virtual void setTexture(QOpenGLTexture &texture);
    virtual void setTexture(char * path);

    virtual uint getNbObject() const;

    bool setSize(float value, uint id=0);
    float getSize(uint id=0) const; //0 if id not created

    bool setDisplayMode(int displayMode, uint id=0);
    int getDisplayMode(uint id=0) const; //0 if id not created

    bool setMatrice(QMatrix4x4 matrice, uint id=0);
    QMatrix4x4 getMatrice(uint id=0) const; //matrice id if id not created



};

#endif // MYOBJECTS_H
