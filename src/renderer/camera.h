//
// Created by Raphael Russo on 1/2/25.
//

#ifndef GARDEN_SIMULATION_CAMERA_H
#define GARDEN_SIMULATION_CAMERA_H


#include <QMatrix4x4>

class Camera {

public:
    Camera(float aspectRatio = 16.0f/9.0f);
    ~Camera() {}

    QMatrix4x4 getViewMatrix() const; // World to camera
    QMatrix4x4 getProjectionMatrix() const; // Perspective proj

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

    // Position and orientation
    QVector3D m_position; // Pos in world coords
    QVector3D m_target; // Point camera is looking at
    QVector3D m_up; // Up vector

    // Perspective projection
    float m_fov; // Field of view
    float m_aspectRatio; // w/h ratio of viewport
    float m_nearPlane; // Near clip plane
    float m_farPlane; // Far clip plane

    float m_distance; // Camera to target distance
    float m_phi; // Horizontal orbital angle
    float m_theta; // Vertical orbital angle

    void updatePosition();
};


#endif //GARDEN_SIMULATION_CAMERA_H
