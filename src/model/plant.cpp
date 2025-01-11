//
// Created by Raphael Russo on 1/4/25.
//

#include "plant.h"

Plant::Plant(Type type, const QString& modelPath, const QString& name, const QString& iconPath)
        : m_type(type)
        , m_name(name)
        , m_icon(QIcon(iconPath))
        , m_model(std::make_unique<Model>())
{
    if (!m_model->loadModel(modelPath)) {
        qDebug() << "Failed to load plant model:" << modelPath;
    }
}