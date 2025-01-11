//
// Created by Raphael Russo on 1/5/25.
//

#include "gardenmodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

GardenModel::GardenModel(int gridSize)
        : m_gridSize(gridSize)
{
    m_grid.resize(gridSize);

    for (auto& row : m_grid) {
        row.resize(gridSize);
    }
}
GardenModel::~GardenModel() = default;

bool GardenModel::addPlant(Plant::Type type, const QPoint& position) {
    if (!canPlacePlant(position)) {
        return false;
    }

    QString modelPath = QString("/Users/raphaelrusso/CLionProjects/garden_simulation/models/plants/%1.obj")
            .arg(getPlantTypeName(type).toLower());

    auto plant = std::make_unique<Plant>(type, modelPath, getPlantTypeName(type), "");
    plant->setGridPosition(position);

    m_grid[position.x()][position.y()] = std::move(plant);
    emit plantAdded(position, type);
    return true;
}

bool GardenModel::removePlant(const QPoint& position) {
    if (!isValidGridPosition(position) || !m_grid[position.x()][position.y()]) {
        return false;
    }

    m_grid[position.x()][position.y()].reset();
    emit plantRemoved(position);
    return true;
}

bool GardenModel::canPlacePlant(const QPoint& position) const {
    return isValidGridPosition(position) && !m_grid[position.x()][position.y()];
}

Plant* GardenModel::getPlant(const QPoint& position) {
    if (!isValidGridPosition(position)) {
        return nullptr;
    }
    return m_grid[position.x()][position.y()].get();
}

bool GardenModel::isValidGridPosition(const QPoint& position) const {
    return position.x() >= 0 && position.x() < m_gridSize &&
           position.y() >= 0 && position.y() < m_gridSize;
}

void GardenModel::setTemperatureSensor(std::unique_ptr<SensorInterface> sensor) {
    if (m_temperatureSensor) {
        m_temperatureSensor->stopReading();
    }
    m_temperatureSensor = std::move(sensor);
    if (m_temperatureSensor) {
        connect(m_temperatureSensor.get(), &SensorInterface::dataUpdated,
                this, &GardenModel::handleTemperatureUpdate);
    }
}

void GardenModel::setMoistureSensor(std::unique_ptr<SensorInterface> sensor) {
    if (m_moistureSensor) {
        m_moistureSensor->stopReading();
    }
    m_moistureSensor = std::move(sensor);
    if (m_moistureSensor) {
        connect(m_moistureSensor.get(), &SensorInterface::dataUpdated,
                this, &GardenModel::handleMoistureUpdate);
    }
}

void GardenModel::handleTemperatureUpdate(float value) {
    m_sensorData.temperature = value;
    m_sensorData.timestamp = QDateTime::currentDateTime();
    emit temperatureChanged(value);
}

void GardenModel::handleMoistureUpdate(float value) {
    m_sensorData.moisture = value;
    m_sensorData.timestamp = QDateTime::currentDateTime();
    emit moistureChanged(value);
}

QString GardenModel::getPlantTypeName(Plant::Type type) {
    switch (type) {
        case Plant::Type::Carrot: return "Carrot";
        case Plant::Type::Pumpkin: return "Pumpkin";
        case Plant::Type::Tomato: return "Tomato";
        default: return "Unknown";
    }
}

bool GardenModel::saveGarden(const QString& filename) {
    QJsonObject garden;
    QJsonArray plants;

    for (int x = 0; x < m_gridSize; ++x) {
        for (int y = 0; y < m_gridSize; ++y) {
            if (m_grid[x][y]) {
                QJsonObject plant;
                plant["type"] = static_cast<int>(m_grid[x][y]->getType());
                plant["x"] = x;
                plant["y"] = y;
                plants.append(plant);
            }
        }
    }

    garden["plants"] = plants;
    garden["gridSize"] = m_gridSize;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonDocument doc(garden);
    file.write(doc.toJson());
    emit gardenSaved();
    return true;
}

bool GardenModel::loadGarden(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject garden = doc.object();

    // Update grid size
    m_gridSize = garden["gridSize"].toInt();

    // Clear the existing grid and resize
    m_grid.clear();
    m_grid.resize(m_gridSize);

    // Initialize each row with empty vectors
    for (auto& row : m_grid) {
        row.resize(m_gridSize);
    }

    // Now load the plants from the JSON
    QJsonArray plants = garden["plants"].toArray();
    for (const auto& plantRef : plants) {
        QJsonObject plantObj = plantRef.toObject();
        QPoint pos(plantObj["x"].toInt(), plantObj["y"].toInt());
        Plant::Type type = static_cast<Plant::Type>(plantObj["type"].toInt());
        addPlant(type, pos);  // Create and move the plant into position
    }

    emit gardenLoaded();
    return true;
}