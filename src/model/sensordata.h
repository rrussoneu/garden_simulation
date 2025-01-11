//
// Created by Raphael Russo on 1/10/25.
//

#ifndef GARDEN_SIMULATION_SENSORDATA_H
#define GARDEN_SIMULATION_SENSORDATA_H

#include <QDateTime>
#include <QObject>

struct SensorData {
    float temperature;
    float moisture;
    QDateTime timestamp;

    SensorData() : temperature(60.0f), moisture(0.5f), timestamp(QDateTime::currentDateTime()) {}
};

class SensorInterface : public QObject {
Q_OBJECT

public:
    explicit SensorInterface(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~SensorInterface() = default;
    virtual void startReading() = 0;
    virtual void stopReading() = 0;
    virtual bool isReading() const = 0;

signals:
    void dataUpdated(float value);
};

#endif // GARDEN_SIMULATION_SENSORDATA_H
