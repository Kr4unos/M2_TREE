// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>

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

    float x = 0.0f, y = 0.0f, z = 0.0f;

    glLoadIdentity();
    gluLookAt (0, 0, 3.0, 0, 0, 0, 0, 1, 0);
    glRotatef (m_angle, 0, 1, 0);
    glPushMatrix();
    for(int i = 0; i < result.size(); i++){
        char currentChar = lsystem->getResult().at(i).toLatin1();
        LSystem::Action action = lsystem->getActionFromSymbol(currentChar);
        if(action == LSystem::NO_ACTION) continue;
        //qDebug() << "action dodu en cours : "<< static_cast<int>(action) << "avec un angle moulu de : "<< lsystem->getAngle();
        switch(action){
            case LSystem::DRAW_FORWARD:
                glBegin(GL_LINES);
                  glColor3f(1, 1, 1);
                  glVertex3f(x, y, z);
                  glVertex3f(x, y+(lsystem->getLength()/10), z);
                glEnd();
                //y += (lsystem->getLength()/10);
                qDebug() << "DRAW_FORWARD";

                break;
            case LSystem::TURN_LEFT:
                qDebug() << "glRotatef(" << -lsystem->getAngle() << ", 1, 0, 0)";
                glTranslatef (x,y+(lsystem->getLength()/10), z);

                glRotatef(-lsystem->getAngle(), 0, 0, 1);
                break;
            case LSystem::TURN_RIGHT:
                qDebug() << "glRotatef(" << lsystem->getAngle() << ", 1, 0, 0)";
                glTranslatef (x,y+(lsystem->getLength()/10), z);

                glRotatef(lsystem->getAngle(), 0, 0, 1);
                break;
            case LSystem::PUSH_BACK:
                qDebug() << "glPushMatrix();";
                glPushMatrix();
                break;
            case LSystem::POP_BACK:
                qDebug() << "glPopMatrix();";
                glPopMatrix();
                break;
        }
    }
    glPopMatrix();
}


void GLArea::keyPressEvent(QKeyEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->text();

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
                 setRadius(m_radius-0.05);
            else setRadius(m_radius+0.05);
            break;
    }
}

void GLArea::onTimeout()
{
    qDebug() << __FUNCTION__ ;
    m_alpha += 1;
    if (m_alpha >= 360) m_alpha = 0;
    update();
}

void GLArea::setRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    if (radius != m_radius && radius > 0.01 && radius <= 10) {
        m_radius = radius;
        qDebug() << "  emit radiusChanged()";
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
