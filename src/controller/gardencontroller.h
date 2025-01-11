//
// Created by Raphael Russo on 1/10/25.
//

#ifndef GARDEN_SIMULATION_GARDENCONTROLLER_H
#define GARDEN_SIMULATION_GARDENCONTROLLER_H

#pragma once
#include "model/gardenmodel.h"
#include "model/mocksensor.h"
#include <QObject>
#include <memory>

class GardenController : public QObject {
Q_OBJECT

public:
    explicit GardenController(std::unique_ptr<GardenModel> model, QObject* parent = nullptr);

    bool addPlant(Plant::Type type, const QPoint& position);
    bool removePlant(const QPoint& position);
    bool canPlacePlant(const QPoint& position) const;

    bool saveGarden(const QString& filename);
    bool loadGarden(const QString& filename);

    const GardenModel* getModel() const { return m_model.get(); }

public slots:
    // Sensor control slots
    void toggleTemperatureSensor(bool enabled);
    void toggleMoistureSensor(bool enabled);

    // Environment control slots
    void setTemperature(float temperature);
    void setMoisture(float moisture);

signals:
    void plantAdded(const QPoint& position, Plant::Type type);
    void plantRemoved(const QPoint& position);
    void temperatureChanged(float temperature);
    void moistureChanged(float moisture);
    void gardenLoaded();
    void gardenSaved();

    // Sensor state signals
    void temperatureSensorStateChanged(bool enabled);
    void moistureSensorStateChanged(bool enabled);

private:
    std::unique_ptr<GardenModel> m_model;
    std::unique_ptr<MockSensor> m_temperatureSensor;
    std::unique_ptr<MockSensor> m_moistureSensor;
    bool m_temperatureSensorEnabled = false;
    bool m_moistureSensorEnabled = false;
};



#endif //GARDEN_SIMULATION_GARDENCONTROLLER_H
