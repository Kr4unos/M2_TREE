// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include "math.h"

GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    qDebug() << "init GLArea" ;

    // Ce n'est pas indispensable
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    setFormat(sf);
    qDebug() << "Depth is"<< format().depthBufferSize();

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
    qDebug() << "destroy GLArea";

    delete m_timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();

    // ici destructions de ressources GL

    doneCurrent();
}


void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);
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

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = w / h;
    doProjection();
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(lsystem == nullptr) return;

    GLfloat x              = 0.0;
    GLfloat y              = 0.0;
    GLfloat z              = 0.0;

    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    GLfloat radius           = lsystem->getBranchRadius();
    GLfloat radius_reduction = lsystem->getBranchRadiusReduction();
    GLfloat height           = lsystem->getBranchLength();
    std::stack<GLfloat> tempRadius;

    glLoadIdentity();
    gluLookAt (3.0, 0.0, 0.0, 0, 0, 0, 0, 1, 0);
    glRotatef (m_angle, 0, 1, 0);
//    glEnable(GL_LINE_SMOOTH);
//    glLineWidth(3.0f);

    for(int i = 0; i < result.size(); i++){
        char currentChar = lsystem->getResult().at(i).toLatin1();
        LSystem::Action action = lsystem->getActionFromSymbol(currentChar);

        switch(action){

            case LSystem::DRAW_BRANCH:

                glColor3f(0.5f, 0.35f, 0.05f);

                glBegin(GL_QUAD_STRIP);
                    angle = 0.0;
                    while( angle < 2 * M_PI ) {
                        GLfloat tempX = radius * cos(angle);
                        GLfloat tempZ = radius * sin(angle);
                        glVertex3f(tempX, height, tempZ);
                        glVertex3f(tempX, y, tempZ);
                        angle += angle_stepsize;
                    }
                    glVertex3f(radius, height, 0.0);
                    glVertex3f(radius, 0.0, 0.0);
                glEnd();

//                glBegin(GL_LINES);
//                  glVertex3f(x, y, z);
//                  glVertex3f(x, y+height, z);
//                glEnd();


                if(radius >= radius_reduction) radius -= radius_reduction;
                glTranslatef (x, y+height, z);
                break;

            case LSystem::DRAW_LEAF:

                glColor3f(0.0f, 1.0f, 0.0f);

                glBegin(GL_QUAD_STRIP);
                    angle = 0.0;
                    while( angle < 2 * M_PI ) {
                        GLfloat tempX = radius * cos(angle);
                        GLfloat tempZ = radius * sin(angle);
                        glVertex3f(tempX, height, tempZ);
                        glVertex3f(tempX, y, tempZ);
                        angle += angle_stepsize;
                    }
                    glVertex3f(radius, height, 0.0);
                    glVertex3f(radius, 0.0, 0.0);
                glEnd();

//                glBegin(GL_LINES);
//                  glVertex3f(x, y, z);
//                  glVertex3f(x, y+0.1f, z);
//                glEnd();

                if(radius >= radius_reduction) radius -= radius_reduction;
                break;

            case LSystem::ROTATE_LEFT_X:

                glRotatef(lsystem->getAngle(), 1, 0, 0);
                break;

            case LSystem::ROTATE_RIGHT_X:

                glRotatef(-lsystem->getAngle(), 1, 0, 0);
                break;

            case LSystem::ROTATE_UP_Y:

                glRotatef(lsystem->getAngle(), 0, 1, 0);
                break;

            case LSystem::ROTATE_DOWN_Y:

                glRotatef(-lsystem->getAngle(), 0, 1, 0);
                break;

            case LSystem::TWIST_LEFT_Z:

                glRotatef(lsystem->getAngle(), 0, 0, 1);
                break;

            case LSystem::TWIST_RIGHT_Z:

                glRotatef(-lsystem->getAngle(), 0, 0, 1);
                break;

            case LSystem::PUSH_BACK:
                tempRadius.push(radius);
                glPushMatrix();
                break;

            case LSystem::POP_BACK:
                radius = tempRadius.top();
                tempRadius.pop();
                glPopMatrix();
                break;

            case LSystem::NO_ACTION:
                break;
        }
    }
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
        case Qt::Key_Space :
            m_angle += 1;
            if (m_angle >= 360) m_angle -= 360;
            update();
            break;
        case Qt::Key_A :
            if (m_timer->isActive())
                m_timer->stop();
            else m_timer->start();
            break;
        case Qt::Key_R :
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
    update();
}
