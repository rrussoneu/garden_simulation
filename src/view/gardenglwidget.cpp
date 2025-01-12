//
// Created by Raphael Russo on 1/1/25.
//

#include "gardenglwidget.h"
#include <QMouseEvent>
#include <QMimeData>


GardenGLWidget::GardenGLWidget(GardenController* controller, QWidget* parent)
        : QOpenGLWidget(parent)
        , m_controller(controller)
        , m_camera(std::make_unique<Camera>())
        , m_gridShader(nullptr)
        , m_modelShader(nullptr)
        , m_sunShader(nullptr)
        , m_temperature(60.0f)  // Default temperature (°F)
        , m_moisture(0.5f)      // Default moisture (50%)
{
    setFocusPolicy(Qt::StrongFocus);  // Enable key events
    setAcceptDrops(true);

    // Connect to controller signals
    connect(controller, &GardenController::plantAdded,
            this, &GardenGLWidget::onPlantAdded);
    connect(controller, &GardenController::plantRemoved,
            this, &GardenGLWidget::onPlantRemoved);
    connect(controller, &GardenController::temperatureChanged,
            this, &GardenGLWidget::onTemperatureChanged);
    connect(controller, &GardenController::moistureChanged,
            this, &GardenGLWidget::onMoistureChanged);
}



void GardenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initializeShaders();
    initializeGridLines();
    initializeModels();
    initializeGridCells();
    initializeSun();
}

void GardenGLWidget::initializeSun() {
    // Create VAO and VBO for the sun
    glGenVertexArrays(1, &m_sunVAO);
    glGenBuffers(1, &m_sunVBO);
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

    m_sunShader = std::make_unique<Shader>(
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/sun.vert",
            "/Users/raphaelrusso/CLionProjects/garden_simulation/shaders/sun.frag"
    );
    if (!m_sunShader->compile()) {
        qDebug() << "Failed to compile sun shader";
        return;
    }
    qDebug() << "Plant shader compiled successfully";
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

    // Grid isn't affected by the sun position or temperature / color
    QVector3D lightPos(5.0f, 5.0f, 5.0f);
    QVector3D lightColor(1.0f, 1.0f, 1.0f);
    m_gridShader->setVec3("lightPos", lightPos);
    m_gridShader->setVec3("lightColor", lightColor);
    m_gridShader->setVec3("gridColor", QVector3D(0.8f, 0.8f, 0.8f));
    m_gridShader->setFloat("ambientStrength", 0.3f);

    QMatrix4x4 model;
    m_gridShader->setMat4("model", model);
    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, (GRID_SIZE + 1) * 4);

    renderSun(view, projection);

    // Update light properties based on sun
    lightColor = calculateSunColor(m_temperature);

    // Use sun position and color for lighting other objects
    m_modelShader->bind();
    m_modelShader->setMat4("view", view);
    m_modelShader->setMat4("projection", projection);
    m_modelShader->setVec3("lightPos", m_sunPosition);
    m_modelShader->setVec3("lightColor", lightColor);
    m_modelShader->setVec3("viewPos", m_camera->getPosition());
    m_modelShader->setBool("isPreview", false); // Beds and placed plants don't have any change with the preview state

    const GardenModel* gardenModel = m_controller->getModel();
    for (int x = 0; x < gardenModel->getGridSize(); ++x) {
        for (int z = 0; z < gardenModel->getGridSize(); ++z) {
            QPoint pos(x, z);

            // Draw bed
            QVector3D position(x + 0.5f, 0.0f, z + 0.5f);
            m_bedModel->setPosition(position);
            m_bedModel->draw(m_modelShader.get());

            // Draw plant if present
            Plant* plant = gardenModel->getPlant(pos);
            if (plant) {
                plant->getModel()->setPosition(position);
                plant->getModel()->draw(m_modelShader.get());
            }
        }
    }

    // Draw preview model if active
    if (m_isPreviewActive && m_previewModel) {
        // Temporarily modify depth testing for transparent preview
        glDepthFunc(GL_LEQUAL);  // Change depth function
        glDepthMask(GL_FALSE);   // Don't write to depth buffer

        m_modelShader->bind();
        m_modelShader->setBool("isPreview", true);

        // Convert world position to grid position for validity check
        QPoint gridPos(std::floor(m_previewPosition.x()),
                       std::floor(m_previewPosition.z()));

        // Check if position is within grid bounds
        bool isWithinGrid = (gridPos.x() >= 0 && gridPos.x() < GRID_SIZE &&
                             gridPos.y() >= 0 && gridPos.y() < GRID_SIZE);


        // Check if position is valid for placement
        bool isValidPlacement = isWithinGrid &&
                                //m_grid[gridPos.x()][gridPos.y()].hasBed &&
                                //!m_grid[gridPos.x()][gridPos.y()].plant;
                                gardenModel->canPlacePlant(gridPos);


        QVector3D highlightColor = isValidPlacement ?
                                   QVector3D(0.0f, 1.0f, 0.0f) :  // Valid placement
                                   QVector3D(1.0f, 0.0f, 0.0f);   // Invalid placement

        m_modelShader->setVec3("previewColor", highlightColor);
        m_modelShader->setFloat("previewAlpha", 0.7f);

        m_previewModel->setPosition(m_previewPosition);
        m_previewModel->draw(m_modelShader.get());

        // Reset depth testing to normal
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        m_modelShader->setBool("isPreview", false);
    }
}

void GardenGLWidget::renderSun(const QMatrix4x4& view, const QMatrix4x4& projection) {
    // Bind sun shader
    m_sunShader->bind();
    m_sunPosition = QVector3D(GRID_SIZE/2.0f, 8.0f, GRID_SIZE/2.0f);  // Center above garden

    // Create model matrix for sun
    QMatrix4x4 model;
    model.translate(m_sunPosition);
    model.scale(1.0f);

    // Set shader uniforms
    m_sunShader->setMat4("model", model);
    m_sunShader->setMat4("view", view);
    m_sunShader->setMat4("projection", projection);

    // Calculate sun color based on temperature
    QVector3D sunColor = calculateSunColor(m_temperature);
    float intensity = 1.0f + (m_temperature - 60.0f) / 30.0f;  // Brighter when hotter

    // Clamp intensity
    intensity = std::max(0.5f, std::min(2.0f, intensity));

    m_sunShader->setVec3("sunColor", sunColor);
    m_sunShader->setFloat("intensity", intensity);

    // Draw the sun cube
    drawCube(m_sunVAO, m_sunVBO);

    m_sunShader->release();
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

void GardenGLWidget::dragEnterEvent(QDragEnterEvent* event) {
    // Check if the drag contains plant data
    if (event->mimeData()->hasFormat("application/x-plant")) {
        // Extract the plant type from the mime data
        Plant::Type type = static_cast<Plant::Type>(
                event->mimeData()->data("application/x-plant").toInt());

        // Create the preview model for this plant type
        updatePreviewModel(type);

        // Activate preview mode
        m_isPreviewActive = true;

        // Accept the drag operation
        event->acceptProposedAction();
    }
}

void GardenGLWidget::dragMoveEvent(QDragMoveEvent* event) {
    // Get world position
    QVector3D worldPos = screenToWorld(event->pos());

    // Calculate the grid cell position (for snapping)
    float gridX = std::floor(worldPos.x());
    float gridZ = std::floor(worldPos.z());

    // Calculate the position within the cell (0-1)
    float cellX = worldPos.x() - gridX;
    float cellZ = worldPos.z() - gridZ;

    // Update preview position with snapping
    if (m_isPreviewActive) {
        // Snap to grid cell centers
        m_previewPosition = QVector3D(
                gridX + 0.5f,  // Snap X to cell center
                0.0f,         // Ground level
                gridZ + 0.5f  // Snap Z to cell center
        );
    }

    event->acceptProposedAction();
    update();
}

void GardenGLWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    // Deactivate preview when drag leaves the widget
    m_isPreviewActive = false;
    event->accept();
    update();
}

void GardenGLWidget::dropEvent(QDropEvent* event) {
    // Convert drop position to grid coordinates
    QPoint gridPos = screenToGrid(event->pos());

    if (m_controller->canPlacePlant(gridPos)) {
        Plant::Type type = static_cast<Plant::Type>(
                event->mimeData()->data("application/x-plant").toInt());
        m_controller->addPlant(type, gridPos);
    }

    // Clean up preview state
    m_isPreviewActive = false;
    event->acceptProposedAction();
    update();
}


QPoint GardenGLWidget::screenToGrid(const QPoint& screenPos) {
    QVector3D worldPos = screenToWorld(screenPos);

    // Floor the world coordinates to make sure within bounds
    int x = static_cast<int>(std::floor(worldPos.x()));
    int z = static_cast<int>(std::floor(worldPos.z()));

    x = std::clamp(x, 0, GRID_SIZE - 1);
    z = std::clamp(z, 0, GRID_SIZE - 1);

    return QPoint(x, z);
}

bool GardenGLWidget::canPlacePlant(const QPoint& gridPos) const {
    if (gridPos.x() < 0 || gridPos.x() >= GRID_SIZE ||
        gridPos.y() < 0 || gridPos.y() >= GRID_SIZE) {
        return false;
    }

    // Extra safety check for the preview position
    if (m_previewPosition.x() >= GRID_SIZE ||
        m_previewPosition.z() >= GRID_SIZE) {
        return false;
    }

    const GridCell& cell = m_grid[gridPos.x()][gridPos.y()];
    return cell.hasBed && !cell.plant;
}



bool GardenGLWidget::addPlant(Plant::Type type, const QPoint& gridPos) {
    if (!canPlacePlant(gridPos)) {
        return false;
    }

    QString basePath = "/Users/raphaelrusso/CLionProjects/garden_simulation/models/plants/";
    QString modelName;
    QString plantName;

    switch (type) {
        case Plant::Type::Carrot:
            modelName = "carrot.obj";
            plantName = "Carrot";
            break;
        case Plant::Type::Pumpkin:
            modelName = "pumpkin.obj";
            plantName = "Pumpkin";
            break;
        case Plant::Type::Tomato:
            modelName = "tomato.obj";
            plantName = "Tomato";
            break;
    }

    auto newPlant = std::make_shared<Plant>(
            type,
            basePath + modelName,
            plantName,
            ""
    );

    QVector3D position(gridPos.x() + 0.5f, 0.0f, gridPos.y() + 0.5f);
    m_grid[gridPos.x()][gridPos.y()].plant = newPlant;
    m_grid[gridPos.x()][gridPos.y()].position = position;

    newPlant->getModel()->setPosition(position);
    newPlant->setGridPosition(gridPos);

    qDebug() << "Added plant:" << plantName << "at position:" << position;

    update();
    return true;
}

void GardenGLWidget::removePlant(const QPoint& gridPos) {
    if (gridPos.x() >= 0 && gridPos.x() < GRID_SIZE &&
        gridPos.y() >= 0 && gridPos.y() < GRID_SIZE) {
        m_grid[gridPos.x()][gridPos.y()].plant.reset();
        update();
    }
}

void GardenGLWidget::updatePreviewModel(Plant::Type type) {
    QString basePath = "/Users/raphaelrusso/CLionProjects/garden_simulation/models/plants/";
    QString modelName;

    switch (type) {
        case Plant::Type::Carrot:
            modelName = "carrot.obj";
            break;
        case Plant::Type::Pumpkin:
            modelName = "pumpkin.obj";
            break;
        case Plant::Type::Tomato:
            modelName = "tomato.obj";
            break;
    }

    m_previewModel = std::make_unique<Model>();
    if (!m_previewModel->loadModel(basePath + modelName)) {
        qDebug() << "Failed to load preview model:" << modelName;
        return;
    }

    m_previewPlantType = type;
    qDebug() << "Successfully loaded preview model:" << modelName;
}


QVector3D GardenGLWidget::calculateSunColor(float temperature) {
    // Normalize temperature to 0-1 range (30F to 90F)
    float t = (temperature - 30.0f) / 60.0f;
    t = std::max(0.0f, std::min(1.0f, t));  // Clamp to 0-1

    // Define color ranges for different temperatures
    QVector3D coldColor(0.6f, 0.7f, 1.0f);    // Cool blueish
    QVector3D midColor(1.0f, 0.95f, 0.8f);    // Neutral warm
    QVector3D hotColor(1.0f, 0.6f, 0.4f);     // Warm orangey red

    // Interpolate between colors
    if (t < 0.5f) {
        return interpolateColors(coldColor, midColor, t * 2.0f);
    } else {
        return interpolateColors(midColor, hotColor, (t - 0.5f) * 2.0f);
    }
}

QVector3D GardenGLWidget::interpolateColors(const QVector3D& color1, const QVector3D& color2, float t) {
    // Clamp t to [0,1] range
    t = std::max(0.0f, std::min(1.0f, t));

    // Linearly interpolate each component
    return QVector3D(
            color1.x() + (color2.x() - color1.x()) * t,
            color1.y() + (color2.y() - color1.y()) * t,
            color1.z() + (color2.z() - color1.z()) * t
    );
}

void GardenGLWidget::drawCube(GLuint& vao, GLuint& vbo) {
    // Define vertices for a complete cube
    static const float vertices[] = {
            // Front face         // Normal
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // Bottom left
            0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  // Bottom right
            0.5f, 0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  // Top right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,  // Bottom left
            0.5f, 0.5f, -0.5f,   0.0f, 0.0f, -1.0f,  // Top right
            -0.5f, 0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  // Top left

            // Back face
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,

            // Left face
            -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f,

            // Right face
            0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f,    1.0f, 0.0f, 0.0f,

            // Top face
            -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f,    0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,

            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f,   0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  0.0f, -1.0f, 0.0f
    };

    // Bind and update buffer data
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Draw the cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
}



// Model update handlers
void GardenGLWidget::onPlantAdded(const QPoint& position, Plant::Type type) {
    update();
}

void GardenGLWidget::onPlantRemoved(const QPoint& position) {
    update();
}

void GardenGLWidget::onTemperatureChanged(float temperature) {
    m_temperature = temperature;
    update();
}

void GardenGLWidget::onMoistureChanged(float moisture) {
    m_moisture = moisture;
    update();
}