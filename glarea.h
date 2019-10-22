// CC-BY Edouard.Thiel@univ-amu.fr - 22/01/2019

#ifndef GLAREA_H
#define GLAREA_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QKeyEvent>
#include <QTimer>
#include <stack>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

#include "lsystem.h"

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
private:
    //cam setting
    double cam_angle_x = 0;
    double cam_angle_y = 0;
    double cam_angle_z = 0;
    double cam_x = 0;
    double cam_y = 0;
    double cam_z = 0;


    QString result = "";
    LSystem* lsystem = nullptr;


    QTimer *m_timer = nullptr;
    double m_alpha = 0;
    double m_radius = 0.5;
    double m_ratio = 1;
    GLuint textureFeuille;

    /* VBO */
    void makeLeafObject(QVector<GLfloat> &vertData);

    QOpenGLShaderProgram *m_program;
    QOpenGLBuffer m_vbo;
    QOpenGLTexture *m_textures[2];
    int m_matrixUniform;
    int m_posAttr;
    int m_colAttr;

    void makeGLObjects();
    void tearGLObjects();
};

#endif // GLAREA_H
