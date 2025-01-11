//
// Created by Raphael Russo on 1/5/25.
//

#ifndef GARDEN_SIMULATION_MOCKSENSOR_H
#define GARDEN_SIMULATION_MOCKSENSOR_H

#include "sensordata.h"
#include <QTimer>
#include <random>

class MockSensor : public SensorInterface {
Q_OBJECT

public:
    explicit MockSensor(float initialValue, float minValue, float maxValue)
            : m_currentValue(initialValue)
            , m_minValue(minValue)
            , m_maxValue(maxValue) {
        connect(&m_updateTimer, &QTimer::timeout, this, &MockSensor::generateReading);
    }

    void startReading() override {
        m_isReading = true;
        m_updateTimer.start(1000);  // Update every second
    }

    void stopReading() override {
        m_isReading = false;
        m_updateTimer.stop();
    }

    bool isReading() const override { return m_isReading; }

private slots:
    void generateReading() {
        // Generate slight random variations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-0.5, 0.5);

        float newValue = m_currentValue + dis(gen);
        newValue = std::clamp(newValue, m_minValue, m_maxValue);
        m_currentValue = newValue;

        emit dataUpdated(m_currentValue);
    }

private:
    QTimer m_updateTimer;
    bool m_isReading = false;
    float m_currentValue;
    float m_minValue;
    float m_maxValue;
};

#endif // GARDEN_SIMULATION_MOCKSENSOR_H