//
// Created by Raphael Russo on 1/4/25.
//

#ifndef GARDEN_SIMULATION_PLANT_H
#define GARDEN_SIMULATION_PLANT_H

#pragma once
#include <QString>
#include <QPoint>
#include <QIcon>
#include "model/model.h"

class Plant {
public:
    enum Type {
        Carrot,
        Pumpkin,
        Tomato
    };

    Plant(Type type, const QString& modelPath, const QString& name, const QString& iconPath);
    ~Plant() = default;

    Type getType() const { return m_type; }
    QString getName() const { return m_name; }
    QIcon getIcon() const { return m_icon; }
    Model* getModel() const { return m_model.get(); }
    bool isPlaced() const { return m_isPlaced; }
    QPoint getGridPosition() const { return m_gridPosition; }


    void setGridPosition(const QPoint& pos) {
        m_gridPosition = pos;
        m_isPlaced = true;
    }
    void removePlacement() { m_isPlaced = false; }

private:
    Type m_type;
    QString m_name;
    QIcon m_icon;
    std::unique_ptr<Model> m_model;
    bool m_isPlaced = false;
    QPoint m_gridPosition;
};


#endif //GARDEN_SIMULATION_PLANT_H
