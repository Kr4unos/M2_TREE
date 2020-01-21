#include "myobjects.h"

/**
 * @brief myObjects::myObjects
 */
myObjects::myObjects(){
    nbObject=0;
}

/**
 * @brief myObjects::~myObjects
 */
myObjects::~myObjects(){
    tearGLObjects();
}

/**
 * @brief myObjects::tearGLObjects
 */
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

/**
 * @brief myObjects::setVbo
 * @param vbo
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

/**
 * @brief myObjects::setShaderProgram
 * @param shaderProgram
 */
void myObjects::setShaderProgram(QOpenGLShaderProgram &shaderProgram){
    this->shaderProgram=&shaderProgram;
    privateShaderProgram=false;
}

/**
 * @brief myObjects::setTexture
 * @param texture
 */
void myObjects::setTexture(QOpenGLTexture &texture){
    this->texture=&texture;
    privateTexture=false;
}

/**
 * @brief myObjects::setShaderProgram
 * @param pathAndName
 */
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

/**
 * @brief myObjects::setShaderProgram
 * @param pathVertexShader
 * @param pathFragmentShader
 */
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

/**
 * @brief myObjects::setTexture
 * @param path
 */
void myObjects::setTexture(char * path){
    QImage image_poisson(path);
    if (image_poisson.isNull()){
        qDebug() << "load image " << path << " failed";
        return;
    }
    this->texture = new QOpenGLTexture(image_poisson);
    this->privateTexture=true;
}

/**
 * @brief myObjects::addObj
 * @param matrice
 * @param size
 * @param displayMode
 * @return
 */
uint myObjects::addObj(QMatrix4x4 matrice, float size, int displayMode){
    this->matrice.push_back(matrice);
    this->displayMode.push_back(displayMode);
    this->size.push_back(size);

    nbObject++;

    return getNbObject()-1;
}

/**
 * @brief myObjects::getNbObject
 * @return
 */
uint myObjects::getNbObject() const{
    return this->nbObject;
}

/**
 * @brief myObjects::setSize
 * @param value
 * @param id
 * @return
 */
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

/**
 * @brief myObjects::getSize
 * @param id
 * @return
 */
float myObjects::getSize(uint id) const{
    if(id < getNbObject()){
        return this->size[id];
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return 0;
    }
}

/**
 * @brief myObjects::setDisplayMode
 * @param displayMode
 * @param id
 * @return
 */
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

/**
 * @brief myObjects::getDisplayMode
 * @param id
 * @return
 */
int myObjects::getDisplayMode(uint id) const{
    if(id < getNbObject()){
        return this->displayMode[id];
    }
    else{
        qWarning() << __FUNCTION__ << " error id " << id << " not created";
        return 0;
    }
}

/**
 * @brief myObjects::setMatrice
 * @param matrice
 * @param id
 * @return
 */
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

/**
 * @brief myObjects::getMatrice
 * @param id
 * @return
 */
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

/**
 * @brief myObjects::onTimeout
 * @param dt
 */
void myObjects::onTimeout(float dt){

}
