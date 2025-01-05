//
// Created by Raphael Russo on 1/4/25.
//

#ifndef GARDEN_SIMULATION_MODEL_H
#define GARDEN_SIMULATION_MODEL_H

#pragma once
#include <QVector3D>
#include <QOpenGLFunctions_3_3_Core>
#include "renderer/shader.h"

struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D texCoords;
};

struct Material {
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    float shininess;
    QString diffuseMap;
};

class Model: protected QOpenGLFunctions_3_3_Core {

public:
    Model();
    ~Model();

    bool loadModel(const QString &objPath);

    void draw(Shader *shader);

    QMatrix4x4 getModelMatrix() const;

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;
    Material m_material;

    GLuint  m_VAO, m_VBO, m_EBO;

    QVector3D m_position;
public:
    void setPosition(const QVector3D &mPosition);

    void setRotation(const QVector3D &mRotation);

    void setScale(const QVector3D &mScale);

private:
    QVector3D m_rotation;
    QVector3D m_scale;

    bool parseMTL(const QString &mtlPath);
    void setupMesh();


};


#endif //GARDEN_SIMULATION_MODEL_H
