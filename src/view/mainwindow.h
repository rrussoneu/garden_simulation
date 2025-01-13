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
#include <QCheckBox>
#include "gardenglwidget.h"
#include "controller/gardencontroller.h"
#include "model/gardenmodel.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

signals:
    void temperatureChanged(float value);

private slots:
    void handleTemperatureChange(int value);
    void handleMoistureChange(int value);
    void handleTemperatureSensorToggle(bool enabled);
    void handleMoistureSensorToggle(bool enabled);
    void handleSaveGarden();
    void handleLoadGarden();


private:

    void createMenus();
    void createDockWindows();
    void createToolbar();
    void setupConnections();

    QDockWidget *m_toolsDock;
    QDockWidget *m_environmentDock;
    QLabel *m_tempLabel;
    QSlider *m_tempSlider;
    QLabel *m_moistureLabel;
    QSlider *m_moistureSlider;
    GardenGLWidget* m_gardenWidget;
    QCheckBox *m_moistureSensorCheck;
    QCheckBox *m_tempSensorCheck;

    std::unique_ptr<GardenController> m_controller;
    std::unique_ptr<GardenModel> m_model;



};


#endif //GARDEN_SIMULATION_MAINWINDOW_H
