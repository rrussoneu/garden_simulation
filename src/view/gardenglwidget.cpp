//
// Created by Raphael Russo on 1/1/25.
//

#include "gardenglwidget.h"
#include <QMouseEvent>


GardenGLWidget::GardenGLWidget(QWidget* parent)
        : QOpenGLWidget(parent)
        , m_gridShader(nullptr)
        , m_camera(std::make_unique<Camera>())

{

}

void GardenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    // Blueish clear color
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);


    glEnable(GL_DEPTH_TEST);

    initializeShaders();

    initializeGrid();

}

void GardenGLWidget::initializeShaders() {
    // Create and compile grid shader
    m_gridShader = std::make_unique<Shader>("/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/grid.vert", "/Users/raphaelrusso/CLionProjects/garden_simulator/shaders/grid.frag");
    if (!m_gridShader->compile()) {
        qDebug() << "Failed to compile grid shader";
        return;
    }
    qDebug() << "Grid shader compiled successfully";

}

void GardenGLWidget::initializeGrid() {
    std::vector<float> gridVertices;


    const int gridSize = 10;  // Number of cells in each direction

    // Generate grid lines
    for (int i = 0; i <= gridSize; ++i) {
        // Horizontal lines (along X axis)
        // Start point
        gridVertices.push_back(0.0f);     // x = 0 (start)
        gridVertices.push_back(0.0f);     // y = 0 (ground plane)
        gridVertices.push_back(float(i)); // z = i
        gridVertices.push_back(0.0f);     // normal x
        gridVertices.push_back(1.0f);     // normal y
        gridVertices.push_back(0.0f);     // normal z

        // End point
        gridVertices.push_back(float(gridSize)); // x = gridSize (end)
        gridVertices.push_back(0.0f);           // y = 0
        gridVertices.push_back(float(i));       // z = i
        gridVertices.push_back(0.0f);           // normal x
        gridVertices.push_back(1.0f);           // normal y
        gridVertices.push_back(0.0f);           // normal z

        // Vertical lines (along Z axis)
        // Start point
        gridVertices.push_back(float(i)); // x = i
        gridVertices.push_back(0.0f);     // y = 0
        gridVertices.push_back(0.0f);     // z = 0 (start)
        gridVertices.push_back(0.0f);     // normal x
        gridVertices.push_back(1.0f);     // normal y
        gridVertices.push_back(0.0f);     // normal z

        // End point
        gridVertices.push_back(float(i));       // x = i
        gridVertices.push_back(0.0f);           // y = 0
        gridVertices.push_back(float(gridSize)); // z = gridSize (end)
        gridVertices.push_back(0.0f);           // normal x
        gridVertices.push_back(1.0f);           // normal y
        gridVertices.push_back(0.0f);           // normal z
    }

    // Buffers for rendering grid
    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void GardenGLWidget::resizeGL(int w, int h) {
    // Update viewport and camera aspect ratio when widget is resized
    glViewport(0, 0, w, h);

}


void GardenGLWidget::paintGL() {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera matrices for rendering
    QMatrix4x4 view = m_camera->getViewMatrix();
    QMatrix4x4 projection = m_camera->getProjectionMatrix();

    // Shader params
    m_gridShader->bind();
    m_gridShader->setMat4("view", view);
    m_gridShader->setMat4("projection", projection);

    // Lighting params
    QVector3D lightPos(5.0f, 5.0f, 5.0f);
    QVector3D lightColor(1.0f, 1.0f, 1.0f);
    m_gridShader->setVec3("lightPos", lightPos);
    m_gridShader->setVec3("lightColor", lightColor);
    m_gridShader->setVec3("gridColor", QVector3D(0.8f, 0.8f, 0.8f));
    m_gridShader->setFloat("ambientStrength", 0.3f);

    // Draw grid
    QMatrix4x4 model; // Identity matrix for grid in world space
    m_gridShader->setMat4("model", model);
    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, 44); // Draw grid lines

}


void GardenGLWidget::mousePressEvent(QMouseEvent *event) {
    m_lastPos = event->pos();
    if (event->button() == Qt::LeftButton) {
        QVector3D worldPos = screenToWorld(event->pos());
        emit gridClicked(QPoint(worldPos.x(), worldPos.z()));
    }

}
void GardenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    QPoint delta = event->pos() - m_lastPos;

    if (event->buttons() & Qt::RightButton) {
        m_camera->orbit(delta.x(), delta.y());

    } else if (event->buttons() & Qt::MiddleButton) {
        m_camera->pan(delta.x(), delta.y());

    }
    m_lastPos = event->pos();
    update();
}

void GardenGLWidget::wheelEvent(QWheelEvent *event) {
    float delta = event->angleDelta().y() / 120.f;
    m_camera->zoom(delta);
    update();
}

QVector3D GardenGLWidget::screenToWorld(const QPoint &screenPos) {
    // SCreen to normalized device coords (-1 to 1)
    float x = (2.0 * screenPos.x()) / width() - 1.0f;
    float y = 1.0  - (2.0f * screenPos.y()) / height();

    // Ray in clip space
    QVector4D rayClip(x, y, -1.0f, 1.0f);
    // Transform to world space with inverse matrices - backwards through pipeline
    QMatrix4x4 invProjection = m_camera->getProjectionMatrix().inverted();
    QMatrix4x4 invView = m_camera->getViewMatrix().inverted();

    // Ray direction in world space

    // Undo projection to go to view space
    QVector4D rayEye = invProjection * rayClip;
    rayEye.setZ(-1.0f); // Point ray forward
    rayEye.setW(0.0f); // Convert to direction vector
    // Undo view transformation to go to world
    QVector4D rayWorld = invView * rayEye;
    QVector3D rayDir(rayWorld.x(), rayWorld.y(), rayWorld.z());
    rayDir.normalize();

    // Intersection with ground plane (y=0)
    QVector3D camPos = m_camera->getPosition();
    float t = -camPos.y() / rayDir.y();
    return camPos + rayDir * t; // Intersection point
}