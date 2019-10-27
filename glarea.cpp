// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include "math.h"

static const char *vertexShaderSource = ":/shaders/basic.vsh";
static const char *fragmentShaderSource = ":/shaders/basic.fsh";

GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{

    // Ce n'est pas indispensable
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);
    setEnabled(true);  // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus); // accepte focus
    setFocus();                      // donne le focus

    m_timer = new QTimer(this);
    m_timer->setInterval(50);  // msec
    connect (m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect (this, SIGNAL(radiusChanged(double)), this, SLOT(setRadius(double)));

}

GLArea::~GLArea()
{
    delete m_timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();

    tearGLObjects();
    // ici destructions de ressources GL
    doneCurrent();
}


void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();


    glEnable(GL_DEPTH_TEST);
    initTexture();
    /*QMatrix4x4 first;
    first.setToIdentity();
    position.push_back(first);*/
    makeGLObjects();

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->setUniformValue("texture", 0);

    if(!m_program->link()){
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    m_matrixUniform = m_program->uniformLocation("matrix");
}

void GLArea::doProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat hr = m_radius, wr = hr * m_ratio;
    glFrustum(-wr, wr, -hr, hr, 1.0f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
}

void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;
    doProjection();
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_program->bind();

    QMatrix4x4 matrix;
    GLfloat hr = m_radius, wr = hr * m_ratio;
    matrix.frustum(-wr, wr, -hr, hr, 1.0, 5.0);
    matrix.translate(0,0, -3.0);
    matrix.rotate(cam_angle_y, 0, 1, 0);
    m_program->setUniformValue(m_matrixUniform, matrix);


    m_program->setAttributeBuffer("posAttr", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer("texAttr", GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer("posGlObject", GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));

    m_program->enableAttributeArray("posAttr");
    m_program->enableAttributeArray("texAttr");
    m_program->enableAttributeArray("posGlObject");


    for(int i = 0; i < (sizeVertData/3); i = i+3){
            int textureID = texturePoint.at(i/3);
            m_textures[textureID]->bind();

            //Z-BUFFER
            if(textureID==0) glDepthMask(GL_FALSE);
            m_program->setUniformValue("posGlObject", position.at((i)/3));

            glDrawArrays(GL_TRIANGLES, i, 3);
            if(textureID==0) glDepthMask(GL_TRUE);
            m_textures[textureID]->release();
    }

    m_program->disableAttributeArray("posAttr");
    m_program->disableAttributeArray("texAttr");
    m_program->disableAttributeArray("posGlObject");

    m_program->release();
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {        
    case Qt::Key_9 :
        cam_angle_y += 1;
        if (cam_angle_y >= 360) cam_angle_y -= 360;
        update();
        break;
    case Qt::Key_7 :
        cam_angle_y -= 1;
        if (cam_angle_y <= -1) cam_angle_y += 360;
        update();
        break;
    case Qt::Key_4 :
        cam_angle_x += 1;
        if (cam_angle_x >= 360) cam_angle_x -= 360;
        update();
        break;
    case Qt::Key_6 :
        cam_angle_x -= 1;
        if (cam_angle_x <= -1) cam_angle_x += 360;
        update();
        break;
    case Qt::Key_8 :
        cam_angle_z += 1;
        if (cam_angle_z >= 360) cam_angle_z -= 360;
        update();
        break;
    case Qt::Key_2 :
        cam_angle_z -= 1;
        if (cam_angle_z <= -1) cam_angle_z += 360;
        update();
        break;
    case Qt::Key_D :
        cam_z+=0.1f;
        update();
        break;
    case Qt::Key_Q :
        cam_z-=0.1f;
        update();
        break;
    case Qt::Key_Z :
        cam_x+=0.1f;
        update();
        break;
    case Qt::Key_S :
        cam_x-=0.1f;
        update();
        break;
    case Qt::Key_R :
        cam_y-=0.1f;
        update();
        break;
    case Qt::Key_F :
        cam_y+=0.1f;
        update();
        break;
    case Qt::Key_Return :
        cam_x=0;
        cam_y=0;
        cam_z=0;
        cam_angle_x=0;
        cam_angle_y=0;
        cam_angle_z=0;
        update();
        break;
    case Qt::Key_Escape:
        qDebug() <<"m_x" << cam_x
                 <<"m_y" << cam_y
                 <<"m_z" << cam_z
                 <<"a_x" << cam_angle_x
                 <<"a_y" << cam_angle_y
                 <<"a_z" << cam_angle_z;
        break;
    case Qt::Key_A :
        if (m_timer->isActive())
            m_timer->stop();
        else m_timer->start();
        break;
    case Qt::Key_W :
        if (ev->text() == "r")
             setRadius(m_radius-0.10);
        else setRadius(m_radius+0.10);
        break;
    }
}

void GLArea::onTimeout()
{
    m_alpha += 1;
    if (m_alpha >= 360) m_alpha = 0;
    update();
}

void GLArea::setRadius(double radius)
{
    if (radius != m_radius && radius > 0.01 && radius <= 50) {
        m_radius = radius;
        emit radiusChanged(radius);
        makeCurrent();
        doProjection();
        doneCurrent();
        update();
    }
}

void GLArea::parseAndGenerate(LSystem *lsystem)
{
    this->result = lsystem->getResult();
    this->lsystem = lsystem;

    if(lsystem != nullptr){
        makeGLObjects();
    }
    update();
}


/*** vbo test ***/

void GLArea::initTexture(){

    QImage imageFeuille(QString(":/icons/texFeuille.png"));
    m_textures[0] = new QOpenGLTexture(imageFeuille);

    QImage imageTroncArbre(QString(":/icons/texTroncArbre.png"));
    m_textures[1] = new QOpenGLTexture(imageTroncArbre);
}

void GLArea::makeLeafObject(QVector<GLfloat> &vertData, float taille, QMatrix4x4 posGlObject){

    GLfloat vertices[] = {
         0.0,  0.0,  0.0,
         0.0,  taille,  0.0,
         taille,  0.0,  0.0,
         0.0,  taille,  0.0,
         taille,  0.0,  0.0,
         taille,  taille,  0.0
    };

    //pour ces 2 triangles, on veut la texture 0
    for(int a = 0; a < 2 ; a++){
        texturePoint.push_back(0);
        position.push_back(posGlObject); //glPushMatrix();
    }
    GLfloat texCoords[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
    };

    for(int i = 0; i < 6; i++){
        for(int j = 0; j < 3; j++){
            vertData.append(vertices[i*3+j]);
        }
        for(int j = 0; j < 2; j++)
            vertData.append(texCoords[i*2+j]);
    }
    sizeVertData += 18;
}


void GLArea::makeBranchObject(QVector<GLfloat> &vertData, GLfloat radius, GLfloat height, QMatrix4x4 posGlObject){

    GLfloat angle_stepsize = 0.5;
    GLfloat angle = 0.0;
    std::vector <GLfloat> vertices;
    std::vector <GLfloat> texCoords;
    GLfloat y = 0.0;

    while( angle < 2 * M_PI ) {
        GLfloat tempX = radius * cos(angle);
        GLfloat tempZ = radius * sin(angle);
        angle += angle_stepsize;
        GLfloat tmp_tempX = radius * cos(angle);
        GLfloat tmp_tempZ = radius * sin(angle);

        //longueur de la branche
        vertices.push_back(tempX); vertices.push_back(y); vertices.push_back(tempZ);
        vertices.push_back(tempX); vertices.push_back(height); vertices.push_back(tempZ);
        vertices.push_back(tmp_tempX); vertices.push_back(height); vertices.push_back(tmp_tempZ);

        vertices.push_back(tempX); vertices.push_back(y); vertices.push_back(tempZ);
        vertices.push_back(tmp_tempX); vertices.push_back(y); vertices.push_back(tmp_tempZ);
        vertices.push_back(tmp_tempX); vertices.push_back(height); vertices.push_back(tmp_tempZ);

        texCoords.push_back(0.0); texCoords.push_back(0.0);
        texCoords.push_back(0.0); texCoords.push_back(1.0);
        texCoords.push_back(1.0); texCoords.push_back(0.0);

        texCoords.push_back(0.0); texCoords.push_back(1.0);
        texCoords.push_back(1.0); texCoords.push_back(0.0);
        texCoords.push_back(1.0); texCoords.push_back(1.0);

        //épaisseur branche
        vertices.push_back(tempX); vertices.push_back(y); vertices.push_back(tempZ);
        vertices.push_back(tmp_tempX); vertices.push_back(y); vertices.push_back(tmp_tempZ);
        vertices.push_back(0.0); vertices.push_back(y); vertices.push_back(0.0);

        vertices.push_back(tempX); vertices.push_back(height); vertices.push_back(tempZ);
        vertices.push_back(tmp_tempX); vertices.push_back(height); vertices.push_back(tmp_tempZ);
        vertices.push_back(0.0); vertices.push_back(height); vertices.push_back(0.0);

        texCoords.push_back(0.0); texCoords.push_back(0.0);
        texCoords.push_back(0.0); texCoords.push_back(1.0);
        texCoords.push_back(1.0); texCoords.push_back(0.0);

        texCoords.push_back(0.0); texCoords.push_back(1.0);
        texCoords.push_back(1.0); texCoords.push_back(0.0);
        texCoords.push_back(1.0); texCoords.push_back(1.0);
    }

    //pour ces n triangles, on veut la texture 1
    for(int a = 0; a < vertices.size()/3/3; a++){
        texturePoint.push_back(1);
        position.push_back(posGlObject); //glPushMatrix();
    }

    for(int i = 0; i < vertices.size()/3; i++){
        for(int j = 0; j < 3; j++){
            vertData.append(static_cast<GLfloat>(vertices[i*3+j]));
        }
        for(int j = 0; j < 2; j++)
            vertData.append(static_cast<GLfloat>(texCoords[i*2+j]));
    }

    sizeVertData += vertices.size();
}


void GLArea::makeGLObjects(){

    QVector<GLfloat> vertData;

    QMatrix4x4 posGlObject;

    m_vbo.create();
    texturePoint.clear();
    posGlObject.setToIdentity();

    if(lsystem != nullptr){
        GLfloat x              = 0.0;
        GLfloat y              = 0.0;
        GLfloat z              = 0.0;
        GLfloat pas            = 0.05;

        GLfloat angle          = 0.0;
        GLfloat angle_stepsize = 0.1;

        GLfloat tailleFeuille = 0.1;

        GLfloat radius           = lsystem->getBranchRadius();
        GLfloat radius_reduction = lsystem->getBranchRadiusReduction();
        GLfloat height,treeSize  = sqrt(lsystem->getBranchLengthRandom());
        std::stack<GLfloat> tempRadius;



        for(int i = 0; i < result.size(); i++){
            char currentChar = lsystem->getResult().at(i).toLatin1();
            LSystem::Action action = lsystem->getActionFromSymbol(currentChar);

            switch(action){

                case LSystem::DRAW_BRANCH:

                   height = treeSize*sqrt(lsystem->getBranchLengthRandom());//l'arbre ne sera plus propotionel

                   makeBranchObject(vertData, radius, height, posGlObject);
                   //position.push_back(posGlObject); //glPushMatrix();


                   if(radius >= radius_reduction) radius -= radius_reduction;
                   posGlObject.translate(x, y+height, z);//glTranslatef (x, y+height, z);
                   break;

                case LSystem::DRAW_LEAF:

                    makeLeafObject(vertData, tailleFeuille, posGlObject);
                    //position.push_back(posGlObject); //glPushMatrix();

                    if(radius >= radius_reduction) radius -= radius_reduction;
                    break;

                case LSystem::ROTATE_LEFT_X:

                    posGlObject.rotate(lsystem->getAngleRandom(), 1, 0, 0);//glRotatef(lsystem->getAngleRandom(), 1, 0, 0);
                    break;

                case LSystem::ROTATE_RIGHT_X:

                    posGlObject.rotate(-lsystem->getAngleRandom(), 1, 0, 0);//glRotatef(-lsystem->getAngleRandom(), 1, 0, 0);
                    break;

                case LSystem::ROTATE_UP_Y:

                    posGlObject.rotate(lsystem->getAngleRandom(), 0, 0, 1);//glRotatef(lsystem->getAngleRandom(), 0, 0, 1);
                    break;

                case LSystem::ROTATE_DOWN_Y:

                    posGlObject.rotate(-lsystem->getAngleRandom(), 0, 0, 1); //glRotatef(-lsystem->getAngleRandom(), 0, 0, 1);
                    break;

                case LSystem::TWIST_LEFT_Z:

                    posGlObject.rotate(lsystem->getAngleRandom(), 0, 1, 0);//glRotatef(lsystem->getAngleRandom(), 0, 1, 0);
                    break;

                case LSystem::TWIST_RIGHT_Z:

                    posGlObject.rotate(-lsystem->getAngleRandom(), 0, 1, 0); //glRotatef(-lsystem->getAngleRandom(), 0, 1, 0);
                    break;

                case LSystem::PUSH_BACK:

                    tempRadius.push(radius);
                    //position.push_back(posGlObject); //glPushMatrix();
                    break;

                case LSystem::POP_BACK:

                    radius = tempRadius.top();
                    tempRadius.pop();
                    //position.pop_back();//glPopMatrix();
                    break;

                case LSystem::NO_ACTION:
                    break;
            }

        }
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(), vertData.count()*sizeof(GLfloat));
    initTexture();
}

void GLArea::tearGLObjects(){

    m_vbo.destroy();

    for(auto i : m_textures){
        i->destroy();
    }
}
