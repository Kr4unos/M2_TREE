// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#include "glarea.h"
#include <GL/glu.h>
#include <QDebug>
#include <QSurfaceFormat>
#include "math.h"

GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{

    // Ce n'est pas indispensable
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
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
    textureFeuille = raw_texture_load("/icons/texFeuille.png", 300, 217);

    GLfloat x              = 0.0;
    GLfloat y              = 0.0;
    GLfloat z              = 0.0;
    GLfloat pas            = 0.05;

    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    GLfloat radius           = lsystem->getBranchRadius();
    GLfloat radius_reduction = lsystem->getBranchRadiusReduction();
    GLfloat height           = lsystem->getBranchLengthRandom();
    std::stack<GLfloat> tempRadius;

    glLoadIdentity();
    gluLookAt (3.0, 0.0, 0.0, 0, 0, 0, 0, 1, 0);

    glTranslatef(0,-0.8,0);

    glTranslatef(cam_x,cam_y,cam_z);
    glRotatef(cam_angle_x, 1, 0, 0);
    glRotatef(cam_angle_y, 0, 1, 0);
    glRotatef(cam_angle_z, 0, 0, 1);
//    glEnable(GL_LINE_SMOOTH);
//    glLineWidth(3.0f);

    for(int i = 0; i < result.size(); i++){
        char currentChar = lsystem->getResult().at(i).toLatin1();
        LSystem::Action action = lsystem->getActionFromSymbol(currentChar); //rend l'arbre ne sera plus propotionel

        switch(action){

            case LSystem::DRAW_BRANCH:
                height           = lsystem->getBranchLengthRandom();
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
                glEnable(GL_TEXTURE_2D);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
                glBindTexture(GL_TEXTURE_2D, textureFeuille);

                glBegin(GL_TRIANGLES);
                glNormal3f(0.0, 0.0, 1.0);

                glTexCoord2d(0, 0); glVertex3f(x,y,z);
                glTexCoord2d(0, 1); glVertex3f(x,y+pas,z);
                glTexCoord2d(1, 0); glVertex3f(x+pas,y,z);
                glEnd();
                glFlush();

                glDisable(GL_TEXTURE_2D);

                /*
                glBegin(GL_TRIANGLES);
                    glVertex3f(x,y+pas,z);
                    glVertex3f(x+pas,y,z);
                    glVertex3f(x+pas,y+pas,z);
                glEnd();


*/

                /*
                glBegin(GL_QUAD_STRIP); //cylindre
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
                */
//                glBegin(GL_LINES);
//                  glVertex3f(x, y, z);
//                  glVertex3f(x, y+0.1f, z);
//                glEnd();

                if(radius >= radius_reduction) radius -= radius_reduction;
                break;

            case LSystem::ROTATE_LEFT_X:

                glRotatef(lsystem->getAngleRandom(), 1, 0, 0);
                break;

            case LSystem::ROTATE_RIGHT_X:

                glRotatef(-lsystem->getAngleRandom(), 1, 0, 0);
                break;

            case LSystem::ROTATE_UP_Y:

                glRotatef(lsystem->getAngleRandom(), 0, 0, 1);
                break;

            case LSystem::ROTATE_DOWN_Y:

                glRotatef(-lsystem->getAngleRandom(), 0, 0, 1);
                break;

            case LSystem::TWIST_LEFT_Z:

                glRotatef(lsystem->getAngleRandom(), 0, 1, 0);
                break;

            case LSystem::TWIST_RIGHT_Z:

                glRotatef(-lsystem->getAngleRandom(), 0, 1, 0);
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
    update();
}
