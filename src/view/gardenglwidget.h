//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_GARDENGLWIDGET_H
#define GARDEN_SIMULATION_GARDENGLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "../renderer/shader.h"
#include "renderer/camera.h"
#include <QOpenGLFunctions_3_3_Core>



class GardenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    explicit GardenGLWidget(QWidget *parent = nullptr);
    ~GardenGLWidget() { } ;


signals:
    // Signal for grid click events
    void gridClicked(QPoint gridPosition);


protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Shaders
    std::unique_ptr<Shader> m_gridShader;

    std::unique_ptr<Camera> m_camera;

    // Buffers
    GLuint  m_gridVAO, m_gridVBO;

    void initializeShaders();
    void initializeGrid();

    // Mouse tracking
    QPoint m_lastPos;

    QVector3D screenToWorld(const QPoint& screenPos);  // Convert screen to world coordinates





};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
