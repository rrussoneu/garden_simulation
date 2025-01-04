//
// Created by Raphael Russo on 1/2/25.
//

#include "camera.h"


void Camera::setAspectRatio(float ratio) {
    m_aspectRatio = ratio;
}

void Camera::setPosition(const QVector3D &position) {
    m_position = position;
}

void Camera::setTarget(const QVector3D &target) {
    m_target = target;
}

QVector3D Camera::getPosition() {
    return m_position;
}

QVector3D Camera::getTarget() {
    return m_target;
}

float Camera::getDistance() {
    return m_distance;
}



