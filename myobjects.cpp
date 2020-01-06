#include "myobjects.h"

myObjects::myObjects(){
    nbObject=0;
}

myObjects::~myObjects(){
    tearGLObjects();
}

void myObjects::tearGLObjects(){
    if(privateVbo){
        vbo->destroy();
    }
    vbo=nullptr;
    if(privateTexture){
        delete texture;
    }
    texture=nullptr;
    if(privateShaderProgram){
        delete shaderProgram;
    }
    shaderProgram=nullptr;
}

/*
void myObjects::initializeGL(){
    setVbo();
    setShaderProgram(":/shaders/simple.vsh", ":/shaders/simple.fsh");
    setTexture(":/textures/poisson1.png")
}
*/

/*
void myObjects::makeGLObject(){
    QVector<GLfloat> vertData;

    vbo->create();
    vbo->bind();
    vbo->allocate(vertData.constData(), vertData.count() * int(sizeof(GLfloat)));
}
*/

/*
void UVSphere::display(QMatrix4x4 &projectionMatrix,QMatrix4x4 &viewMatrix){
    vbo->bind();
    shaderProgram->bind(); // active le shader program

    shaderProgram->setUniformValue("projectionMatrix", projectionMatrix);
    shaderProgram->setUniformValue("viewMatrix", viewMatrix);

    shaderProgram->setAttributeBuffer("in_position", GL_FLOAT, 0, 3, 6 * sizeof(GLfloat));
    shaderProgram->setAttributeBuffer("colAttr", GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
    shaderProgram->enableAttributeArray("in_position");
    shaderProgram->enableAttributeArray("colAttr");

    for(uint i=0; i < getNbObject() ;i++){
        modelMatrix.setToIdentity();
        modelMatrix.translate(position[i]);
        modelMatrix.rotate(rotation[i].x(),1,0,0);
        modelMatrix.rotate(rotation[i].y(),0,1,0);
        modelMatrix.rotate(rotation[i].z(),0,0,1);
        shaderProgram->setUniformValue("size",size[i]);
        shaderProgram->setUniformValue("modelMatrix", modelMatrix);
        switch (displayMode[i]) {
        case 0 :
        //pas d'affichage
        break;
        case 1 :
        //affichage avec des faces triangle/quad
            glDrawArrays(GL_QUADS, 0, subdivision*subdivision*4*2);
        break;
        case 2 :
        //affichage en maille
            glLineWidth(2);
            glDrawArrays(GL_LINES, 0, subdivision*subdivision*4*2);
            glDrawArrays(GL_LINES, 1, subdivision*subdivision*4*2 -1);
        break;
        case 3 :
        //affichage en sommet
            glPointSize(4);
            glDrawArrays(GL_POINTS, 0, subdivision*subdivision*4*2);
        break;
        default:;
        }
    }

    shaderProgram->disableAttributeArray("in_position");
    shaderProgram->disableAttributeArray("colAttr");
    shaderProgram->release();
}
*/

void myObjects::setVbo(QOpenGLBuffer * vbo){
    if(vbo != nullptr){
        this->vbo=vbo;
        privateVbo=false;
    }
    else{
        this->vbo = new QOpenGLBuffer();
        privateVbo=true;
    }
}

void myObjects::setShaderProgram(QOpenGLShaderProgram &shaderProgram){
    this->shaderProgram=&shaderProgram;
    privateShaderProgram=false;
}

void myObjects::setTexture(QOpenGLTexture &texture){
    this->texture=&texture;
    privateTexture=false;
}

void myObjects::setShaderProgram(char * pathAndName){
    this->shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex,   QString(pathAndName) + ".vsh");
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, QString(pathAndName) + ".fsh");
    if (! shaderProgram->link()) {                  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << shaderProgram->log();
    }
    this->privateShaderProgram=true;
}

void myObjects::setShaderProgram(char * pathVertexShader, char * pathFragmentShader){
    this->shaderProgram = new QOpenGLShaderProgram();
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, pathVertexShader);
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, pathFragmentShader);
    if (! shaderProgram->link()) {                  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << shaderProgram->log();
    }
    this->privateShaderProgram=true;
}

void myObjects::setTexture(char * path){
    QImage image_poisson(path);
    if (image_poisson.isNull()){
        qDebug() << "load image " << path << " failed";
        return;
    }
    this->texture = new QOpenGLTexture(image_poisson);
    this->privateTexture=true;
}

uint myObjects::addObj(QMatrix4x4 matrice, float size, int displayMode){
    this->matrice.push_back(matrice);
    this->displayMode.push_back(displayMode);
    this->size.push_back(size);

    nbObject++;

    return getNbObject()-1;
}

uint myObjects::getNbObject() const{
    return this->nbObject;
}

bool myObjects::setSize(float value, uint id){
    if(id < getNbObject()){
        this->size[id] = value;
        return true;
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return false;
    }
}

float myObjects::getSize(uint id) const{
    if(id < getNbObject()){
        return this->size[id];
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return 0;
    }
}

bool myObjects::setDisplayMode(int displayMode, uint id){
    if(id < getNbObject()){
        this->displayMode[id]=displayMode;
        return true;
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return false;
    }
}

int myObjects::getDisplayMode(uint id) const{
    if(id < getNbObject()){
        return this->displayMode[id];
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return 0;
    }
}


bool myObjects::setMatrice(QMatrix4x4 matrice, uint id){
    if(id < getNbObject()){
        this->matrice[id]=matrice;
        return true;
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return false;
    }
}

QMatrix4x4 myObjects::getMatrice(uint id) const{
    if(id < getNbObject()){
        return this->matrice[id];
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        QMatrix4x4 tmp;
        return tmp;
    }
}

void myObjects::onTimeout(float dt){

}
