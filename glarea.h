// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QOpenGLTexture>

#include "lsystem.h"

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = nullptr);
    ~GLArea() override;

public slots:
    void setRadius(double radius);
    void startTimer();
    void stopTimer();
    void modifyTimeoutInterval(int value);
    void setAngle(double angle);

protected slots:
    void onTimeout();
    void parseAndGenerate(LSystem* lsystem);

protected:
    void initializeGL() override;
    void doProjection();
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *ev) override;

private:

    QMatrix4x4 matrix;
    QMatrix4x4 save_matrix;

    double m_angle = 0;
    QTimer *m_timer = nullptr;
    double m_anim = 0;
    double m_radius = 0.5;
    double m_ratio = 1;
    double m_upper = 0;
    bool flag_sens = false;
    QOpenGLShaderProgram *m_program;

    void modifyVbo();
    void makeGLObjects();
    void tearGLObjects();
    QOpenGLBuffer m_vbo;
};

#endif // GLAREA_H
