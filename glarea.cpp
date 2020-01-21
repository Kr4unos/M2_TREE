// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include "math.h"

/**
 * @brief GLArea::GLArea
 * @param parent
 */
GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{

    // Ce n'est pas indispensable
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    setFormat(sf);
    setEnabled(true);                // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus); // accepte focus
    setFocus();                      // donne le focus

    timer = new QTimer(this);
    timer->setInterval(20);           // msec
    connect (timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
    elapsedTimer.start();
    connect (this, SIGNAL(radiusChanged(double)), this, SLOT(setRadius(double)));
}

/**
 * @brief GLArea::~GLArea
 */
GLArea::~GLArea()
{
    delete timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();

    // ici destructions de ressources GL
    tearGLObjects();
    doneCurrent();
}

/**
 * @brief GLArea::initializeGL
 */
void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.6f,0.6f,1.0f,1.0f);
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();
}

/**
 * @brief GLArea::doProjection
 */
void GLArea::doProjection()
{
    qDebug() << __func__ ;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat hr = m_radius, wr = hr * windowRatio;
    glFrustum(-wr, wr, -hr, hr, 1.0f, 10.0f);
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief GLArea::tearGLObjects
 */
void GLArea::tearGLObjects()
{
    cy.tearGLObjects();
    leaf.tearGLObjects();
    sol.tearGLObjects();
}

/**
 * @brief GLArea::makeGLObjects
 */
void GLArea::makeGLObjects()
{
    if(lsystem == nullptr) return;
    qDebug() << __func__ ;

    GLfloat rotationAngle=lsystem->getAngleRandom();

    GLfloat radius          = lsystem->getBranchRadiusRandom()*10 ;
    GLfloat radius_reduction = lsystem->getBranchRadiusReductionRandom()*10;
    GLfloat height,size  = 1;
    std::stack<GLfloat> tempRadius;

    height=lsystem->getBranchLengthRandom()*10 ;
    cy=Cylindre(radius,radius-1*radius_reduction,height,8);
    cy.initializeGL();
    cy.setTexture(":/icons/texTroncArbre.png");
    leaf=Leaf(0.5f,0.8f);
    leaf.initializeGL();
    leaf.setTexture(":/icons/texFeuille.png");
    float groundSize=30;
    sol=Leaf(groundSize,groundSize);
    sol.initializeGL();
    sol.setTexture(":/icons/texGround.png");

    QMatrix4x4 actualMatrice;
    std::vector<QMatrix4x4> vMatrice;
    std::vector<QVector3D> vNextRotation;
    std::vector<GLfloat> vSize;
    actualMatrice.rotate(90,1,0,0);
    actualMatrice.translate(0,-groundSize/2,0);
    sol.addObj(actualMatrice);
    actualMatrice.setToIdentity();
    for(int i = 0; i < result.size(); i++){
        char currentChar = lsystem->getResult().at(i).toLatin1();
        LSystem::Action action = lsystem->getActionFromSymbol(currentChar);

        switch (action) {
            case LSystem::DRAW_BRANCH:
                //position= actualMatrice * QVector3D(0,0,0);
                cy.addObj(actualMatrice,size);
                size*=(1-1*radius_reduction/radius);
                //actualMatrice.rotate(1,1,0,0);
                actualMatrice.translate(QVector3D(0,height,0));

            break;
            case LSystem::DRAW_LEAF:
                leaf.addObj(actualMatrice);
            break;
            case LSystem::ROTATE_LEFT_X:
            actualMatrice.rotate(rotationAngle,0,0,1);
            break;
            case LSystem::ROTATE_RIGHT_X:
            actualMatrice.rotate(-rotationAngle,0,0,1);
            break;
            case LSystem::ROTATE_UP_Y:
            actualMatrice.rotate(rotationAngle,1,0,0);
            break;
            case LSystem::ROTATE_DOWN_Y:
            actualMatrice.rotate(-rotationAngle,1,0,0);
            break;
            case LSystem::TWIST_LEFT_Z:
                actualMatrice.rotate(rotationAngle,0,1,0);
            break;
            case LSystem::TWIST_RIGHT_Z:
                actualMatrice.rotate(-rotationAngle,0,1,0);
            break;
            case LSystem::PUSH_BACK:
                vMatrice.push_back(actualMatrice);
                vSize.push_back(size);
            break;
            case LSystem::POP_BACK:
                actualMatrice = vMatrice.back();
                vMatrice.pop_back();
                size = vSize.back();
                vSize.pop_back();
            break;
            case LSystem::NO_ACTION:
            break;

        }
    }

    cy.makeGLObject();
    leaf.makeGLObject();
    sol.makeGLObject();
}

/**
 * @brief GLArea::paintGL
 */
void GLArea::paintGL()
{
    glClearColor(bgr,bgg,bgb,bga);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrice de projection
    QMatrix4x4 projectionMatrix;
    projectionMatrix.perspective(45.0f, windowRatio, 1.0f, 1000.0f);

    // Matrice de vue (caméra)
    QMatrix4x4 viewMatrix;
    viewMatrix.translate(xPos, yPos, zPos);
    viewMatrix.rotate(xRot, 1, 0, 0);
    viewMatrix.rotate(yRot, 0, 1, 0);
    viewMatrix.rotate(zRot, 0, 0, 1);
    if(lsystem == nullptr) return;

    cy.display(projectionMatrix,viewMatrix);
    sol.display(projectionMatrix,viewMatrix);
    glDepthMask(GL_FALSE);
    leaf.display(projectionMatrix,viewMatrix);
    glDepthMask(GL_TRUE);
}

/**
 * @brief GLArea::raw_texture_load
 * @param filename
 * @param width
 * @param height
 * @return
 */
GLuint GLArea::raw_texture_load(const char *filename, int width, int height)
 {
     GLuint texture;
     unsigned char *data;
     FILE *file;

     // open texture data
     file = fopen(filename, "rb");
     if (file == NULL) return 0;

     // allocate buffer
     data = (unsigned char*) malloc(width * height * 4);

     // read texture data
     fread(data, width * height * 4, 1, file);
     fclose(file);

     // allocate a texture name
     glGenTextures(1, &texture);

     // select our current texture
     glBindTexture(GL_TEXTURE_2D, texture);

     // select modulate to mix texture with color for shading
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_DECAL);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_DECAL);

     // when texture area is small, bilinear filter the closest mipmap
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
     // when texture area is large, bilinear filter the first mipmap
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

     // texture should tile
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

     // build our texture mipmaps
     gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);


     // free buffer
     free(data);

     return texture;
}

/**
 * @brief GLArea::resizeGL
 * @param w
 * @param h
 */
void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    windowRatio = double(w) / h;
    doProjection();
}

/**
 * @brief GLArea::keyPressEvent
 * @param ev
 */
void GLArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << QKeySequence(ev->key()).toString();
    switch(ev->key()) {
        case Qt::Key_A :
            xRot -= deltaAngle/9;
        break;

        case Qt::Key_Q :
            xRot += deltaAngle/9;
        break;

        case Qt::Key_Z :
            yRot -= deltaAngle/9;
        break;

        case Qt::Key_S :
            yRot += deltaAngle/9;
        break;

        case Qt::Key_E :
            zRot -= deltaAngle/9;
        break;

        case Qt::Key_D :
            zRot += deltaAngle/9;
        break;

        case Qt::Key_Plus :
            deltaAngle*=2;
            deltaZoom*=2;
            qDebug() << "deltaMouvment = " << deltaAngle;
        break;

        case Qt::Key_Minus :
            deltaAngle/=2;
            deltaZoom/=2;
            qDebug() << "deltaMouvment = " << deltaAngle;
        break;

        case Qt::Key_Return :
            xRot=20.0f, yRot=0.0f, zRot=0.0f;
            xPos=0.0f,  yPos=-6.0f, zPos=-40.0f;
        break;

        case Qt::Key_Escape:
            qDebug() <<"xPos" << xPos
                     <<"yPos" << yPos
                     <<"zPos" << zPos
                     <<"xRot" << xRot
                     <<"yRot" << yRot
                     <<"zRot" << zRot;
        break;
        case Qt::Key_Backspace:
            makeGLObjects();
        break;
        case Qt::Key_W :
        if (ev->text() == "w")
             setRadius(m_radius-0.10);
        else setRadius(m_radius+0.10);
        break;

    }
    update();
}

/**
 * @brief GLArea::keyReleaseEvent
 * @param ev
 */
void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();
}

/**
 * @brief GLArea::mousePressEvent
 * @param ev
 */
void GLArea::mousePressEvent(QMouseEvent *ev)
{
    lastPos = ev->pos();
}

/**
 * @brief GLArea::wheelEvent
 * @param ev
 */
void GLArea::wheelEvent(QWheelEvent *ev){
    zPos += static_cast<float>(ev->delta() * deltaZoom/100);
    update();
}

/**
 * @brief GLArea::mouseReleaseEvent
 * @param ev
 */
void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

/**
 * @brief GLArea::mouseMoveEvent
 * @param ev
 */
void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    int dx = ev->x() - lastPos.x();
    int dy = ev->y() - lastPos.y();

    if (ev->buttons() & Qt::MidButton || (ev->buttons() & Qt::LeftButton && ev->buttons() & Qt::RightButton)) {
        xPos += dx/10.0f;
        zPos += dy;
        update();
    } else if (ev->buttons() & Qt::LeftButton) {
        xRot += dy;
        yRot += dx;
        update();
    } else if (ev->buttons() & Qt::RightButton) {
        xPos += dx/10.0f;
        yPos -= dy/10.0f;
        update();
    }

    lastPos = ev->pos();
}

/**
 * @brief GLArea::onTimeout
 */
void GLArea::onTimeout(){
    static qint64 old_chrono = elapsedTimer.elapsed(); // static : initialisation la première fois et conserve la dernière valeur
    qint64 chrono = elapsedTimer.elapsed();
    dt = (chrono - old_chrono) / 1000.0f;
    old_chrono = chrono;

    update();
}

/**
 * @brief GLArea::setRadius
 * @param radius
 */
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

/**
 * @brief GLArea::parseAndGenerate
 * @param lsystem
 */
void GLArea::parseAndGenerate(LSystem *lsystem)
{
    this->result = lsystem->getResult();
    this->lsystem = lsystem;
    makeGLObjects();
    update();
}
