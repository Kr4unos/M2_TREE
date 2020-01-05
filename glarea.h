// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <stack>

#include "lsystem.h"
#include "cylindre.h"
#include "leaf.h"

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = nullptr);
    ~GLArea() override;
    GLuint raw_texture_load(const char *filename, int width, int height);


public slots:
    void setRadius(double radius);
    void parseAndGenerate(LSystem* lsystem);

signals:  // On ne les implémente pas, elles seront générées par MOC ;
          // les paramètres seront passés aux slots connectés.
    void radiusChanged(double newRadius);

protected slots:
    void onTimeout();

protected:
    void initializeGL() override;
    void doProjection();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    //cam setting
    float xRot=20.0f, yRot=0.0f, zRot=0.0f;
    float xPos=0.0f,  yPos=-6.0f, zPos=-40.0f;

    float bgr=0.2f, bgg=0.4f, bgb=1.0f, bga=1.0f;   //background rgba
    float deltaAngle=1, deltaZoom=1;                //delta for mouvement

    QPoint lastPos;
    QString result = "";
    LSystem* lsystem = nullptr;


    QTimer *timer = nullptr;
    QElapsedTimer elapsedTimer;
    float dt = 0;
    double m_radius = 0.5;
    double windowRatio = 1.0;
    GLuint textureFeuille;


    Cylindre cy;
    Leaf leaf;
    Leaf sol;

    void makeGLObjects();
    void tearGLObjects();
};

#endif // GLAREA_H
