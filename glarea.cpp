#include "glarea.h"

static const QString vertexShaderFile   = ":/shaders/basic.vsh";
static const QString fragmentShaderFile = ":/shaders/basic.fsh";

GLArea::GLArea(QWidget *parent) : QOpenGLWidget(parent)
{
    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);
    setFormat(sf);

    setEnabled(true);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    setAngle(180);
    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect (m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}
GLArea::~GLArea()
{
    delete m_timer;

    makeCurrent();
    tearGLObjects();
    doneCurrent();
}
void GLArea::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    makeGLObjects();

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }
}
void GLArea::makeGLObjects()
{
    m_vbo.create();
    m_vbo.bind();
    modifyVbo();
}
void GLArea::tearGLObjects()
{
    m_vbo.destroy();
}
void GLArea::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_ratio = (double) w / h;
}

void GLArea::modifyVbo() {
    QVector<GLfloat> vertData;



    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

void GLArea::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    GLfloat hr = m_radius, wr = hr * m_ratio;
    matrix.frustum(-wr, wr, -hr, hr, 1.0, 5.0);
    matrix.rotate(m_angle, 0, 1, 0);

    m_program->release();
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    switch(ev->key()) {
        case Qt::Key_Space :
            this->setAngle(m_angle+1);
            break;
        case Qt::Key_D:
            this->startTimer();
            break;
        case Qt::Key_S:
            this->stopTimer();
            break;
        case Qt::Key_R :
            if (ev->text() == "r")
                 setRadius(m_radius-0.05);
            else setRadius(m_radius+0.05);
            break;
        case Qt::Key_A:
            if (ev->text() == "a")
                 this->modifyTimeoutInterval(5);
            else this->modifyTimeoutInterval(-5);
            break;
    }
}
void GLArea::onTimeout()
{
    if(flag_sens) {
        m_anim += 0.01;
        if(m_anim >= 1) flag_sens = !flag_sens;
    }
    else {
        m_anim -= 0.01;
        if(m_anim <= 0) flag_sens = !flag_sens;
    }
    update();
}

void GLArea::parseAndGenerate(LSystem *lsystem)
{
    for(int i = 0; i < lsystem->getResult(); i++){
        char currentChar = lsystem->getResult().at(i).toLatin1();
        LSystem::Action action = lsystem->getActionFromSymbol(currentChar);
        if(action = LSystem::NO_ACTION) continue;

        switch(action){
            case LSystem::DRAW_FORWARD:
                break;
            case LSystem::TURN_LEFT:
                break;
            case LSystem::TURN_RIGHT:
                break;
            case LSystem::PUSH_BACK:
                break;
            case LSystem::POP_BACK:
                break;
        }
    }
}

void GLArea::setRadius(double radius)
{
    if (radius != m_radius && radius > 0.01 && radius <= 10) {
        m_radius = radius;
        update();
    }
}
void GLArea::startTimer(){
    if(!this->m_timer->isActive())
        this->m_timer->start();
}
void GLArea::stopTimer(){
    if(this->m_timer->isActive())
        this->m_timer->stop();
}
void GLArea::modifyTimeoutInterval(int value){
    if(value < 0 && m_timer->interval()+value < 0) return;
    this->m_timer->setInterval(m_timer->interval()+value);
}
void GLArea::setAngle(double angle){
    m_angle = angle;
    if (m_angle >= 360) m_angle -= 360;
    update();
}
