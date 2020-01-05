#include "cylindre.h"

Cylindre::Cylindre(float radius, float height, int subdivision){
    this->radius1=radius;
    this->radius2=radius;
    this->height=height;
    this->subdivision=subdivision;
    if(subdivision<3){
        this->subdivision=3;
        qWarning() << "Warning subdivision of Cylindre must be above or equal 3 to see something";
    }
}

Cylindre::Cylindre(float radius1, float radius2, float height, int subdivision){
    this->radius1=radius1;
    this->radius2=radius2;
    this->height=height;
    this->subdivision=subdivision;
    if(subdivision<3){
        this->subdivision=3;
        qWarning() << "Warning subdivision of Cylindre must be above or equal 3 to see something";
    }
}

void Cylindre::initializeGL(){
    setVbo();
    setShaderProgram(":/shaders/simpleColor");
}


void Cylindre::makeGLObject(){
    QVector<GLfloat> vertData;
    for(double i=0;i<subdivision;i++){
        vertData.append(radius1*cos(2*M_PI*(i/subdivision)));
        vertData.append(0);
        vertData.append(radius1*sin(2*M_PI*(i/subdivision)));

        vertData.append(0.5f);
        vertData.append(0.35f);
        vertData.append(0.05f);

        vertData.append(radius2*cos(2*M_PI*(i/subdivision)));
        vertData.append(height);
        vertData.append(radius2*sin(2*M_PI*(i/subdivision)));

        vertData.append(0.5f);
        vertData.append(0.35f);
        vertData.append(0.05f);

        vertData.append(radius2*cos(2*M_PI*((i+1)/subdivision)));
        vertData.append(height);
        vertData.append(radius2*sin(2*M_PI*((i+1)/subdivision)));

        vertData.append(0.5f);
        vertData.append(0.35f);
        vertData.append(0.05f);

        vertData.append(radius1*cos(2*M_PI*((i+1)/subdivision)));
        vertData.append(0);
        vertData.append(radius1*sin(2*M_PI*((i+1)/subdivision)));

        vertData.append(0.5f);
        vertData.append(0.35f);
        vertData.append(0.05f);
    }
    vbo->create();
    vbo->bind();
    vbo->allocate(vertData.constData(), vertData.count() * int(sizeof(GLfloat)));
}

void Cylindre::display(QMatrix4x4 &projectionMatrix,QMatrix4x4 &viewMatrix){
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
            glDrawArrays(GL_QUADS, 0, subdivision*4);
        break;
        case 2 :
        //affichage en maille
            glLineWidth(2);
            glDrawArrays(GL_LINES, 0, subdivision*4);
            glDrawArrays(GL_LINES, 1, subdivision*4 -1);
        break;
        case 3 :
        //affichage en sommet
            glPointSize(4);
            glDrawArrays(GL_POINTS, 0, subdivision*4);
        break;
        default:;
        }
    }

    shaderProgram->disableAttributeArray("in_position");
    shaderProgram->disableAttributeArray("colAttr");
    shaderProgram->release();
}
