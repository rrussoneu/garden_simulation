//
// Created by Raphael Russo on 1/5/25.
//

#ifndef GARDEN_SIMULATION_MOCKSENSOR_H
#define GARDEN_SIMULATION_MOCKSENSOR_H


#include "sensordata.h"
#include <QTimer>
#include <QObject>
#include <random>

class MockSensor : public SensorInterface {
Q_OBJECT

public:
    explicit MockSensor(float initialValue, float minValue, float maxValue);

    void startReading() override;
    void stopReading() override;
    bool isReading() const override { return m_isReading; }

private slots:
    void generateReading();

private:
    QTimer m_updateTimer;
    bool m_isReading = false;
    float m_currentValue;
    float m_minValue;
    float m_maxValue;
};

#endif // GARDEN_SIMULATION_MOCKSENSOR_H