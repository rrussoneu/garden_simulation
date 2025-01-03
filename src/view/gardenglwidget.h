//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_GARDENGLWIDGET_H
#define GARDEN_SIMULATION_GARDENGLWIDGET_H


#include <QOpenGLWidget>
#include <QOpenGLFunctions>


class GardenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit GardenGLWidget(QWidget *parent = nullptr);
    ~GardenGLWidget() { } ;

protected:
    void initializeGL() override;
    //void resizeGL(int w, int h) override;
    //void paintGL() override;


};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
