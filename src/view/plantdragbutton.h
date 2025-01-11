//
// Created by Raphael Russo on 1/4/25.
//

#ifndef GARDEN_SIMULATION_PLANTDRAGBUTTON_H
#define GARDEN_SIMULATION_PLANTDRAGBUTTON_H
#pragma once
#include <QPushButton>
#include <QDrag>
#include <QMimeData>
#include "src/model/plant.h"

class PlantDragButton : public QPushButton {
Q_OBJECT

public:
    PlantDragButton(Plant::Type type, const QString& text, QWidget* parent = nullptr);

protected:
    //void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    Plant::Type m_plantType;
    QPoint m_dragStartPosition;
};



#endif //GARDEN_SIMULATION_PLANTDRAGBUTTON_H
