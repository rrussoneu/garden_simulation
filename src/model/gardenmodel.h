//
// Created by Raphael Russo on 1/5/25.
//

#ifndef GARDEN_SIMULATION_GARDENMODEL_H
#define GARDEN_SIMULATION_GARDENMODEL_H

#include "plant.h"
#include "sensordata.h"
#include <QObject>
#include <QVector>
#include <QPoint>
#include <memory>
#include <vector>

class GardenModel : public QObject {
Q_OBJECT

public:
    explicit GardenModel(int gridSize = 10);
    ~GardenModel();

    // Plant management
    bool addPlant(Plant::Type type, const QPoint& position);
    bool removePlant(const QPoint& position);
    bool canPlacePlant(const QPoint& position) const;
    Plant* getPlant(const QPoint& position) const;

    // Grid management
    int getGridSize() const { return m_gridSize; }
    bool isValidGridPosition(const QPoint& position) const;

    // Sensor management
    void setTemperatureSensor(std::unique_ptr<SensorInterface> sensor);
    void setMoistureSensor(std::unique_ptr<SensorInterface> sensor);

    // Current state accessors
    float getCurrentTemperature() const { return m_sensorData.temperature; }
    float getCurrentMoisture() const { return m_sensorData.moisture; }

    // Save/Load functionality
    bool saveGarden(const QString& filename);
    bool loadGarden(const QString& filename);

signals:
    void plantAdded(const QPoint& position, Plant::Type type);
    void plantRemoved(const QPoint& position);
    void temperatureChanged(float temperature);
    void moistureChanged(float moisture);
    void gardenLoaded();
    void gardenSaved();

public slots:
    void handleTemperatureUpdate(float value);
    void handleMoistureUpdate(float value);

private:
    int m_gridSize;
    std::vector<std::vector<std::unique_ptr<Plant>>> m_grid;
    SensorData m_sensorData;
    std::unique_ptr<SensorInterface> m_temperatureSensor;
    std::unique_ptr<SensorInterface> m_moistureSensor;
    QString getPlantTypeName(Plant::Type type);
};

#endif //GARDEN_SIMULATION_GARDENMODEL_H
