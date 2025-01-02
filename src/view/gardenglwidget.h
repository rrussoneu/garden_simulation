//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_GARDENGLWIDGET_H
#define GARDEN_SIMULATION_GARDENGLWIDGET_H


#include <QOpenGLWidget>

class GardenGLWidget : public QOpenGLWidget {
    Q_OBJECT

public:
    explicit GardenGLWidget(QWidget *parent = nullptr);
    ~GardenGLWidget() { } ;


};


#endif //GARDEN_SIMULATION_GARDENGLWIDGET_H
