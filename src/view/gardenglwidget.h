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
#include <memory>

struct GridCell {
    bool hasBed;
    QVector3D position;
    float moisture;  // Will be used later for soil moisture visualization
};

class GardenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
Q_OBJECT

public:
    explicit GardenGLWidget(QWidget *parent = nullptr);
    ~GardenGLWidget() override = default;

signals:
    void gridClicked(QPoint gridPosition);

public slots:
    // These will be connected to UI controls later
    void setTemperature(float temp) { m_temperature = temp; update(); }
    void setMoisture(float moisture) { m_moisture = moisture; update(); }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    static const int GRID_SIZE = 10;

    // OpenGL objects
    std::unique_ptr<Shader> m_gridShader;
    std::unique_ptr<Shader> m_modelShader;
    std::unique_ptr<Model> m_bedModel;
    std::unique_ptr<Camera> m_camera;

    // Grid rendering
    GLuint m_gridVAO, m_gridVBO;
    std::vector<std::vector<GridCell>> m_grid;

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
};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
