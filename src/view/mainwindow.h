//
// Created by Raphael Russo on 1/1/25.
//

#ifndef GARDEN_SIMULATION_MAINWINDOW_H
#define GARDEN_SIMULATION_MAINWINDOW_H
#pragma onceonce
#include <QMainWindow>
#include <QDockWidget>
#include <QLabel>
#include <QSlider>
#include <QBoxLayout>
#include "gardenglwidget.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void handleTemperatureChange(int value);


private:

    QDockWidget *m_toolsDock;
    QDockWidget *m_tempDock;
    QLabel *m_tempLabel;
    QSlider *m_tempSlider;
    GardenGLWidget* m_gardenWidget;

    void createMenus();
    void createDockWindows();
    void createToolbar();
    void setupConnections();



};


#endif //GARDEN_SIMULATION_MAINWINDOW_H
