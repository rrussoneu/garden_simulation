//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_GARDENGLWIDGET_H
#define GARDEN_SIMULATION_GARDENGLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include "../renderer/shader.h"
#include "../renderer/camera.h"
#include "../model/model.h"
#include "src/model/plant.h"
#include "controller/gardencontroller.h"
#include <memory>

struct GridCell {
    bool hasBed;
    QVector3D position;
    float moisture;  // Will be used later for soil moisture visualization
    std::shared_ptr<Plant> plant;
};

class GardenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
Q_OBJECT

public:
    explicit GardenGLWidget(GardenController* controller, QWidget* parent = nullptr);
    ~GardenGLWidget() override = default;

    bool canPlacePlant(const QPoint& gridPos) const;
    bool addPlant(Plant::Type type, const QPoint& gridPos);
    void removePlant(const QPoint& gridPos);
    void setDeleteMode(bool enabled);

signals:
    void gridClicked(QPoint gridPosition);

private slots:
    void onPlantAdded(const QPoint& position, Plant::Type type);
    void onPlantRemoved(const QPoint& position);
    void onTemperatureChanged(float temperature);
    void onMoistureChanged(float moisture);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    static const int GRID_SIZE = 10;

    GardenController* m_controller;

    // OpenGL objects
    std::unique_ptr<Shader> m_gridShader;
    std::unique_ptr<Shader> m_modelShader;
    std::unique_ptr<Model> m_bedModel;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Shader> m_sunShader;

    // Grid rendering
    GLuint m_gridVAO, m_gridVBO;
    std::vector<std::vector<GridCell>> m_grid;

    // Sun rendering
    GLuint m_sunVAO, m_sunVBO;
    QVector3D m_sunPosition;
    void initializeSun();
    QVector3D calculateSunColor(float temperature);
    QVector3D interpolateColors(const QVector3D& color1, const QVector3D& color2, float t);
    void renderSun(const QMatrix4x4 &view, const QMatrix4x4 &projection);
    void drawCube(GLuint &vao, GLuint &vbo);

    // Environmental parameters
    float m_temperature;  // Will control light color
    float m_moisture;     // Will control bed darkness

    // Mouse tracking
    QPoint m_lastPos;

    // Initialize helpers
    void initializeShaders();
    void initializeGridLines();
    void initializeModels();
    void initializeGridCells();

    // Utility functions
    QVector3D screenToWorld(const QPoint& screenPos);
    void handleGridClick(const QPoint& gridPos);

    bool m_isPreviewActive = false;
    Plant::Type m_previewPlantType;
    std::unique_ptr<Model> m_previewModel;
    QVector3D m_previewPosition;

    // Helper method to handle preview model creation and updates
    void updatePreviewModel(Plant::Type type);



    QPoint screenToGrid(const QPoint& screenPos);


    // Deletion
    bool m_deleteModeActive = false;
    QPoint m_hoveredCell = QPoint(-1, -1);
    void renderPlantHighlight(const QPoint& position, const QVector3D& color);
    void handleDeleteModeClick(const QPoint& gridPos);



};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
