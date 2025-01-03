//
// Created by Raphael Russo on 1/1/25.
//

#include "gardenglwidget.h"

GardenGLWidget::GardenGLWidget(QWidget* parent)
        : QOpenGLWidget(parent)
        , m_gridShader(nullptr)

{

}

void GardenGLWidget::initializeGL() {
    initializeOpenGLFunctions();

    // Blueish clear color
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);


    glEnable(GL_DEPTH_TEST);

    initializeGrid();
    initializeShaders();
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

    // Create and bind Vertex Array Object
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



}