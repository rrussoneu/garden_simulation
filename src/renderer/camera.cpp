//
// Created by Raphael Russo on 1/2/25.
//

#include "camera.h"

Camera::Camera(float aspectRatio)
        : m_up(0.0f, 1.0f, 0.0f)
        , m_fov(45.0f)
        , m_aspectRatio(aspectRatio)
        , m_nearPlane(0.1f)
        , m_farPlane(1000.0f)
        , m_distance(15.0f)
        , m_phi(M_PI / 4.0f)
        , m_theta(M_PI / 4.0f)
{
    // Set target to center of grid
    m_target = QVector3D(5.0f, 0.0f, 5.0f);  // Center of 10x10 grid
    updatePosition();
}

void Camera::updatePosition() {
    // Convert spherical coordinates (distance, phi, theta) to Cartesian coordinates
    // Place the camera at the correct position relative to the target
    float x = m_distance * qCos(m_phi) * qCos(m_theta);
    float y = m_distance * qSin(m_theta);
    float z = m_distance * qSin(m_phi) * qCos(m_theta);

    // Update camera position relative to target
    m_position = m_target + QVector3D(x, y, z);
}

QMatrix4x4 Camera::getViewMatrix() const {
    // Create view matrix for world space to camera space
    QMatrix4x4 view;
    view.lookAt(m_position, m_target, m_up); // Position and orient camera
    return view;
}

QMatrix4x4 Camera::getProjectionMatrix() const {
    // Perspective projection matrix
    QMatrix4x4 projection;
    projection.perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    return projection;
}


void Camera::orbit(float deltaX, float deltaY) {
    // Update orbital angles based on mouse movement
    // deltaX: phi (horizontal)
    // deltaY: theta (vertical)

    constexpr float sensitivity = 0.01f;
    m_phi += deltaX * sensitivity;
    m_theta += deltaY * sensitivity;

    // Clamp vertical angle to prevent camera flipping
    // Restrict theta
    constexpr float epsilon = 0.1f;
    m_theta = qBound(epsilon, m_theta, M_PI_2 - epsilon);

    // Update camera position with new orbital angles
    updatePosition();
}

void Camera::zoom(float delta) {
    float zoomSensitivity = 0.1f;
    float minDistance = 2.0f;
    float maxDistance = 50.0f;

    // Update distance with scroll delta
    m_distance += delta * zoomSensitivity;

    // Clamp distance based on above
    m_distance = qBound(minDistance, m_distance, maxDistance);

    // Update camera position with new distance
    updatePosition();
}

void Camera::pan(float deltaX, float deltaY) {
    // Calculate right vector for horizontal panning
    QVector3D right = QVector3D::crossProduct(m_target - m_position, m_up).normalized();

    // Scale pan sensitivity with distance from target
    float panSensitivity = m_distance * 0.001f;

    // Move target and position together for panning
    QVector3D offset = (right * -deltaX + m_up * deltaY) * panSensitivity;
    m_target += offset;
    m_position += offset;
}

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



