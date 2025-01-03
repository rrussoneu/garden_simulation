//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_GARDENGLWIDGET_H
#define GARDEN_SIMULATION_GARDENGLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "../renderer/shader.h"
#include <QOpenGLFunctions_3_3_Core>



class GardenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

public:
    explicit GardenGLWidget(QWidget *parent = nullptr);
    ~GardenGLWidget() { } ;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    // Shaders
    std::unique_ptr<Shader> m_gridShader;

    // Buffers
    GLuint  m_gridVAO, m_gridVBO;

    void initializeShaders();
    void initializeGrid();





};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
