//
// Created by Raphael Russo on 1/1/25.
//

#include "gardenglwidget.h"
#include <QMouseEvent>


GardenGLWidget::GardenGLWidget(QWidget* parent)
        : QOpenGLWidget(parent)
        , m_camera(std::make_unique<Camera>())
        , m_temperature(60.0f)  // Default temperature (°F)
        , m_moisture(0.5f)      // Default moisture (50%)
{
    setFocusPolicy(Qt::StrongFocus);  // Enable key events
}

void GardenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    initializeShaders();
    initializeGridLines();
    initializeModels();
    initializeGridCells();
}

void GardenGLWidget::initializeShaders() {
    // Create and compile grid shader
    m_gridShader = std::make_unique<Shader>(
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/grid.vert",
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/grid.frag"
    );
    if (!m_gridShader->compile()) {
        qDebug() << "Failed to compile grid shader";
        return;
    }
    qDebug() << "Grid shader compiled successfully";

    // Create and compile model shader
    m_modelShader = std::make_unique<Shader>(
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/model.vert",
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/model.frag"
    );
    if (!m_modelShader->compile()) {
        qDebug() << "Failed to compile model shader";
        return;
    }
    qDebug() << "Model shader compiled successfully";
}

void GardenGLWidget::initializeGridLines() {
    std::vector<float> gridVertices;
    for (int i = 0; i <= GRID_SIZE; ++i) {
        // Horizontal lines
        gridVertices.push_back(0.0f);      // x start
        gridVertices.push_back(0.0f);      // y
        gridVertices.push_back(float(i));  // z
        gridVertices.push_back(0.0f);      // normal x
        gridVertices.push_back(1.0f);      // normal y
        gridVertices.push_back(0.0f);      // normal z

        gridVertices.push_back(float(GRID_SIZE));  // x end
        gridVertices.push_back(0.0f);             // y
        gridVertices.push_back(float(i));         // z
        gridVertices.push_back(0.0f);             // normal x
        gridVertices.push_back(1.0f);             // normal y
        gridVertices.push_back(0.0f);             // normal z

        // Vertical lines
        gridVertices.push_back(float(i));  // x
        gridVertices.push_back(0.0f);      // y
        gridVertices.push_back(0.0f);      // z start
        gridVertices.push_back(0.0f);      // normal x
        gridVertices.push_back(1.0f);      // normal y
        gridVertices.push_back(0.0f);      // normal z

        gridVertices.push_back(float(i));         // x
        gridVertices.push_back(0.0f);             // y
        gridVertices.push_back(float(GRID_SIZE)); // z end
        gridVertices.push_back(0.0f);             // normal x
        gridVertices.push_back(1.0f);             // normal y
        gridVertices.push_back(0.0f);             // normal z
    }

    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers(1, &m_gridVBO);

    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float),
                 gridVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void GardenGLWidget::initializeModels() {
    m_bedModel = std::make_unique<Model>();
    if (!m_bedModel->loadModel("/Users/raphaelrusso/CLionProjects/garden_simulation/models/bed.obj")) {
        qDebug() << "Failed to load garden bed model";
        return;
    }
}

void GardenGLWidget::initializeGridCells() {
    m_grid.resize(GRID_SIZE, std::vector<GridCell>(GRID_SIZE));
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int z = 0; z < GRID_SIZE; ++z) {
            m_grid[x][z].hasBed = true;
            m_grid[x][z].position = QVector3D(x, 0, z);
            m_grid[x][z].moisture = m_moisture;
        }
    }
}

void GardenGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    if (m_camera) {
        m_camera->setAspectRatio(float(w) / float(h));
    }
}


void GardenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view = m_camera->getViewMatrix();
    QMatrix4x4 projection = m_camera->getProjectionMatrix();

    // Draw grid
    m_gridShader->bind();
    m_gridShader->setMat4("view", view);
    m_gridShader->setMat4("projection", projection);

    QVector3D lightPos(5.0f, 5.0f, 5.0f);  // Will be updated with sun position later
    QVector3D lightColor(1.0f, 1.0f, 1.0f); // Will be temperature-based later
    m_gridShader->setVec3("lightPos", lightPos);
    m_gridShader->setVec3("lightColor", lightColor);
    m_gridShader->setVec3("gridColor", QVector3D(0.8f, 0.8f, 0.8f));
    m_gridShader->setFloat("ambientStrength", 0.3f);

    QMatrix4x4 model;
    m_gridShader->setMat4("model", model);
    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, (GRID_SIZE + 1) * 4);

    // Draw garden beds
    m_modelShader->bind();
    m_modelShader->setMat4("view", view);
    m_modelShader->setMat4("projection", projection);
    m_modelShader->setVec3("lightPos", lightPos);
    m_modelShader->setVec3("lightColor", lightColor);
    m_modelShader->setVec3("viewPos", m_camera->getPosition());

    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int z = 0; z < GRID_SIZE; ++z) {
            const GridCell& cell = m_grid[x][z];
            if (cell.hasBed) {
                // Center the bed in the grid cell
                QVector3D position(x + 0.5f, 0.0f, z + 0.5f);  // Add 0.5 to center in cell
                m_bedModel->setPosition(position);

                // If needed, add rotation to align with grid
                m_bedModel->setRotation(QVector3D(0.0f, 0.0f, 0.0f));

                // Scale if needed to fit grid
                m_bedModel->setScale(QVector3D(1.0f, 1.0f, 1.0f));

                m_bedModel->draw(m_modelShader.get());
            }
        }
    }
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

