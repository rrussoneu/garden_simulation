//
// Created by Raphael Russo on 1/2/25.
//

#ifndef GARDEN_SIMULATION_CAMERA_H
#define GARDEN_SIMULATION_CAMERA_H


#include <QMatrix4x4>

class Camera {
    Camera(float aspectRatio = 16.0f/9.0f);

    QMatrix4x4 getViewMatrix() const;
    QMatrix4x4 getProjectionMatrix() const;

    void orbit(float deltaX, float deltaY);
    void zoom(float delta);
    void pan(float deltaX, float deltaY);

    void setAspectRatio(float ratio);
    void setPosition(const QVector3D &position);
    void setTarget(const QVector3D &target);

    QVector3D getPosition();
    QVector3D getTarget();
    float getDistance();

private:
    QVector3D m_position;
    QVector3D m_target;
    QVector3D m_up;


    float m_pov;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    float m_distance;
    float m_phi;
    float m_theta;

    void updatePosition();
};


#endif //GARDEN_SIMULATION_CAMERA_H
