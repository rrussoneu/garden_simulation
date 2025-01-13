//
// Created by Raphael Russo on 1/10/25.
//

#include "gardencontroller.h"
GardenController::GardenController(std::unique_ptr<GardenModel> model, QObject* parent)
        : QObject(parent)
        , m_model(std::move(model))
        , m_temperatureSensor(std::make_unique<MockSensor>(60.0f, 30.0f, 90.0f))  // Temperature range: 30-90°F
        , m_moistureSensor(std::make_unique<MockSensor>(0.5f, 0.0f, 1.0f))       // Moisture range: 0-100%
{
    // Forward model signals to controller signals
    connect(m_model.get(), &GardenModel::plantAdded,
            this, &GardenController::plantAdded);
    connect(m_model.get(), &GardenModel::plantRemoved,
            this, &GardenController::plantRemoved);
    connect(m_model.get(), &GardenModel::temperatureChanged,
            this, &GardenController::temperatureChanged);
    connect(m_model.get(), &GardenModel::moistureChanged,
            this, &GardenController::moistureChanged);
    connect(m_model.get(), &GardenModel::gardenLoaded,
            this, &GardenController::gardenLoaded);
    connect(m_model.get(), &GardenModel::gardenSaved,
            this, &GardenController::gardenSaved);
}

bool GardenController::addPlant(Plant::Type type, const QPoint& position) {
    return m_model->addPlant(type, position);
}

bool GardenController::removePlant(const QPoint& position) {
    return m_model->removePlant(position);
}

bool GardenController::canPlacePlant(const QPoint& position) const {
    return m_model->canPlacePlant(position);
}

void GardenController::toggleTemperatureSensor(bool enabled) {
    m_temperatureSensorEnabled = enabled;
    if (enabled) {
        float currentTemp = m_model->getCurrentTemperature();
        m_temperatureSensor = std::make_unique<MockSensor>(currentTemp, 30.0f, 90.0f);
        m_temperatureSensor->startReading();
        m_model->setTemperatureSensor(std::move(m_temperatureSensor));
    } else {
        float lastTemp = m_model->getCurrentTemperature();
        if (auto* sensor = m_model->getTemperatureSensor()) {
            sensor->stopReading();
        }
        // Clear model's ownership

        m_model->setTemperatureSensor(nullptr);
        // Create new sensor
        m_temperatureSensor = std::make_unique<MockSensor>(
                lastTemp,  // Use the previous temperature
                30.0f,       // Min temperature
                90.0f        // Max temperature
        );
        m_model->handleTemperatureUpdate(lastTemp);
    }
    emit temperatureSensorStateChanged(enabled);
}

void GardenController::toggleMoistureSensor(bool enabled) {
    /*
    m_moistureSensorEnabled = enabled;
    if (enabled) {
        m_model->setMoistureSensor(std::move(m_moistureSensor));
        m_moistureSensor->startReading();
    } else {
        m_moistureSensor->stopReading();
        m_model->setMoistureSensor(nullptr);
        m_moistureSensor = std::make_unique<MockSensor>(0.5f, 0.0f, 1.0f);
    }
    emit moistureSensorStateChanged(enabled);
     */
    // For now do nothing, might just leave a display in future
}

void GardenController::setTemperature(float temperature) {
    if (!m_temperatureSensorEnabled) {
        m_model->handleTemperatureUpdate(temperature);
    }
}

void GardenController::setMoisture(float moisture) {
    if (!m_moistureSensorEnabled) {
        m_model->handleMoistureUpdate(moisture);
    }
}

bool GardenController::saveGarden(const QString& filename) {
    return m_model->saveGarden(filename);
}

bool GardenController::loadGarden(const QString& filename) {
    return m_model->loadGarden(filename);
}