#include "util/CommonIncludes.h"
#include "view.h"
#include <QApplication>
#include <QKeyEvent>

#include "game/coggame.h"

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    for (int i = 0; i < FRAMES_TO_AVERAGE; i++) {
        frameTimes[i] = 0;
    }
    frameIndex = 0;

    GAME = new COG::COGGame();
}

View::~View()
{
    delete GAME;
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    //initialize glew
    GLenum err = glewInit();
    if ( GLEW_OK != err ) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // general gl setup
    // Enable depth testing, so that objects are occluded based on depth instead of drawing order.
    glEnable(GL_DEPTH_TEST);

    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);

    GAME->initializeGL();
}

void View::paintGL()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    GAME->draw();

    glPopMatrix();
    glPopAttrib();

    //glColor3f(1,1,1);
    //renderText(10, 20, "FPS: " + QString::number((int) (fps)), this->font());
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    GAME->resizeEvent(w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    GAME->mousePressEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;
    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    GAME->mouseMoveEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    GAME->mouseReleaseEvent(event);
}

void View::wheelEvent(QWheelEvent *event)
{
    GAME->wheelEvent(event);
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    GAME->keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    GAME->keyReleaseEvent(event);
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    frameTimes[frameIndex] = seconds;
    frameIndex = (frameIndex + 1) % FRAMES_TO_AVERAGE;
    fps = 0;
    for (int i = 0; i < FRAMES_TO_AVERAGE; i++) {
        fps += frameTimes[i];
    }
    fps /= FRAMES_TO_AVERAGE;
    fps = 1.f / fps;

    QString title = "Children of Gaia";
    title += " (FPS: ";
    title += QString::number((int)fps);
    title += ")";
    window()->setWindowTitle(title);

    GAME->tick(seconds);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
