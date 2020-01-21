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

/**
 * @brief The GLArea class used to display our object in a 3D area
 */
class GLArea : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /**
     * @brief GLArea
     * @param parent
     */
    explicit GLArea(QWidget *parent = nullptr);

    /**
    *  @brief ~GLArea
    */
    ~GLArea() override;

    /**
     * @brief raw_texture_load
     * @param filename
     * @param width
     * @param height
     * @return
     */
    GLuint raw_texture_load(const char *filename, int width, int height);


public slots:

    /**
     * @brief setRadius
     * @param radius
     */
    void setRadius(double radius);

    /**
     * @brief parseAndGenerate
     * @param lsystem
     */
    void parseAndGenerate(LSystem* lsystem);

signals:
    /**
     * @brief radiusChanged
     * @param newRadius
     */
    void radiusChanged(double newRadius);

protected slots:
    /**
     * @brief onTimeout
     */
    void onTimeout();

protected:
    /**
     * @brief initializeGL
     */
    void initializeGL() override;

    /**
     * @brief doProjection
     */
    void doProjection();

    /**
     * @brief resizeGL
     * @param w
     * @param h
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief paintGL
     */
    void paintGL() override;

    /**
     * @brief keyPressEvent
     * @param ev
     */
    void keyPressEvent(QKeyEvent *ev) override;

    /**
     * @brief keyReleaseEvent
     * @param event
     */
    void keyReleaseEvent(QKeyEvent *event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief wheelEvent
     * @param event
     */
    void wheelEvent(QWheelEvent *event) override;
private:

    // CAMERA SETTINGS

    /**
     * @brief xRot
     */
    float xRot=20.0f;

    /**
     * @brief yRot
     */
    float yRot=0.0f;

    /**
     * @brief zRot
     */
    float zRot=0.0f;

    /**
     * @brief xPos
     */
    float xPos=0.0f;

    /**
     * @brief yPos
     */
    float yPos=-6.0f;

    /**
     * @brief zPos
     */
    float zPos=-40.0f;

    // BACKGROUND COLOR

    /**
     * @brief bgr
     */
    float bgr=0.2f;

    /**
     * @brief bgg
     */
    float bgg=0.4f;

    /**
     * @brief bgb
     */
    float bgb=1.0f;

    /**
     * @brief bga
     */
    float bga=1.0f;

    // DELTA FOR MOVEMENT

    /**
     * @brief deltaAngle
     */
    float deltaAngle=1;

    /**
     * @brief deltaZoom
     */
    float deltaZoom=1;

    /**
     * @brief lastPos
     */
    QPoint lastPos;

    /**
     * @brief result
     */
    QString result = "";

    /**
     * @brief lsystem
     */
    LSystem* lsystem = nullptr;

    /**
     * @brief timer
     */
    QTimer *timer = nullptr;

    /**
     * @brief elapsedTimer
     */
    QElapsedTimer elapsedTimer;

    /**
     * @brief dt
     */
    float dt = 0;

    /**
     * @brief m_radius
     */
    double m_radius = 0.5;

    /**
     * @brief windowRatio
     */
    double windowRatio = 1.0;

    /**
     * @brief textureFeuille
     */
    GLuint textureFeuille;

    /**
     * @brief makeGLObjects
     */
    void makeGLObjects();

    /**
     * @brief tearGLObjects
     */
    void tearGLObjects();

public:

    /**
     * @brief cy
     */
    Cylindre cy;

    /**
     * @brief leaf
     */
    Leaf leaf;

    /**
     * @brief sol
     */
    Leaf sol;

};

#endif // GLAREA_H
