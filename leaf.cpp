#include "leaf.h"

Leaf::Leaf(float width, float height)
{
    this->width=width;
    this->height=height;
}

void Leaf::initializeGL(){
    setVbo();
    setShaderProgram(":/shaders/simple");
}


void Leaf::makeGLObject(){
    QVector<GLfloat> vertData;
    vertData.append(-width/2);
    vertData.append(0);
    vertData.append(0);

    vertData.append(0.0f);
    vertData.append(0.5f);
    vertData.append(0.05f);

    vertData.append(-width/2);
    vertData.append(height);
    vertData.append(0);

    vertData.append(0.0f);
    vertData.append(0.5f);
    vertData.append(0.05f);

    vertData.append(width/2);
    vertData.append(height);
    vertData.append(0);

    vertData.append(0.0f);
    vertData.append(0.5f);
    vertData.append(0.05f);

    vertData.append(width/2);
    vertData.append(0);
    vertData.append(0);

    vertData.append(0.0f);
    vertData.append(0.5f);
    vertData.append(0.05f);

    vbo->create();
    vbo->bind();
    vbo->allocate(vertData.constData(), vertData.count() * int(sizeof(GLfloat)));
}

void Leaf::display(QMatrix4x4 &projectionMatrix,QMatrix4x4 &viewMatrix){
    vbo->bind();
    shaderProgram->bind(); // active le shader program

    shaderProgram->setUniformValue("projectionMatrix", projectionMatrix);
    shaderProgram->setUniformValue("viewMatrix", viewMatrix);

    shaderProgram->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    shaderProgram->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    shaderProgram->enableAttributeArray("in_position");
    shaderProgram->enableAttributeArray("colAttr");

    for(uint i=0; i < getNbObject() ;i++){
        shaderProgram->setUniformValue("size",size[i]);
        shaderProgram->setUniformValue("modelMatrix", matrice[i]);
        switch (displayMode[i]) {
        case 0 :
        //pas d'affichage
        break;
        case 1 :
        //affichage avec des faces triangle/quad
            glDrawArrays(GL_QUADS, 0, 4);
        break;
        case 2 :
        //affichage en maille
            glLineWidth(2);
            glDrawArrays(GL_LINES, 0, 4);
            glDrawArrays(GL_LINES, 1, 4 -1);
        break;
        case 3 :
        //affichage en sommet
            glPointSize(4);
            glDrawArrays(GL_POINTS, 0, 4);
        break;
        default:;
        }
    }

    shaderProgram->disableAttributeArray("in_position");
    shaderProgram->disableAttributeArray("colAttr");
    shaderProgram->release();
}
