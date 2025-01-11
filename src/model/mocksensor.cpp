//
// Created by Raphael Russo on 1/10/25.
//

#include "mocksensor.h"

MockSensor::MockSensor(float initialValue, float minValue, float maxValue)
        : m_currentValue(initialValue)
        , m_minValue(minValue)
        , m_maxValue(maxValue)
{
    connect(&m_updateTimer, &QTimer::timeout, this, &MockSensor::generateReading);
}

void MockSensor::startReading() {
    m_isReading = true;
    m_updateTimer.start(1000);  // Update every second
}

void MockSensor::stopReading() {
    m_isReading = false;
    m_updateTimer.stop();
}

void MockSensor::generateReading() {
    // Generate slight random variations
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.5, 0.5);

    float newValue = m_currentValue + dis(gen);
    newValue = std::clamp(newValue, m_minValue, m_maxValue);
    m_currentValue = newValue;

    emit dataUpdated(m_currentValue);
}