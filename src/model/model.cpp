//
// Created by Raphael Russo on 1/4/25.
//

#include <QFile>
#include <QFileInfo>
#include "model.h"

Model::Model() : m_VAO(0), m_VBO(0), m_EBO(0),
    m_position(0.0f,0.0f,0.0f),
    m_rotation(0.0f,0.0f,0.0f),
    m_scale(1.0f,1.0f,1.0f)
{
    QOpenGLFunctions_3_3_Core::initializeOpenGLFunctions();
}

Model::~Model() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void Model::setPosition(const QVector3D &mPosition) {
    m_position = mPosition;
}

void Model::setRotation(const QVector3D &mRotation) {
    m_rotation = mRotation;
}

void Model::setScale(const QVector3D &mScale) {
    m_scale = mScale;
}

bool Model::loadModel(const QString &objPath) {
    QFile file(objPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open OBJ file: " << objPath;
        return false;
    }

    std::vector<QVector3D> positions;
    std::vector<QVector3D> normals;
    std::vector<QVector2D> texCoords;
    QString mtlPath;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);

        if (parts.isEmpty()) continue;

        if (parts[0] == "mtllib") {
            // Get MTL file path relative to OBJ file
            mtlPath = QFileInfo(objPath).path() + "/" + parts[1];
        }
        else if (parts[0] == "v") {
            // Vertex position
            positions.push_back(QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            ));
        }
        else if (parts[0] == "vt") {
            // Texture coordinate
            texCoords.push_back(QVector2D(
                    parts[1].toFloat(),
                    parts[2].toFloat()
            ));
        }
        else if (parts[0] == "vn") {
            // Vertex normal
            normals.push_back(QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            ));
        }
        else if (parts[0] == "f") {
            // Face definition w triangles or quads
            for (int i = 1; i <= 3; ++i) {
                QStringList indices = parts[i].split('/');

                Vertex vertex;
                // OBJ indices are 1-based
                int posIdx = indices[0].toInt() - 1;
                vertex.position = positions[posIdx];

                if (indices.size() > 1 && !indices[1].isEmpty()) {
                    int texIdx = indices[1].toInt() - 1;
                    vertex.texCoords = texCoords[texIdx];
                }

                if (indices.size() > 2) {
                    int normIdx = indices[2].toInt() - 1;
                    vertex.normal = normals[normIdx];
                }

                m_vertices.push_back(vertex);
                m_indices.push_back(m_indices.size());
            }
        }
    }

    file.close();

    // Load material if available
    if (!mtlPath.isEmpty()) {
        parseMTL(mtlPath);
    }

    setupMesh();
    return true;


}

bool Model::parseMTL(const QString& mtlPath) {
    QFile file(mtlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open MTL file:" << mtlPath;
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);

        if (parts.isEmpty()) continue;

        if (parts[0] == "Ka") {
            m_material.ambient = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
        }
        else if (parts[0] == "Kd") {
            m_material.diffuse = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
        }
        else if (parts[0] == "Ks") {
            m_material.specular = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
        }
        else if (parts[0] == "Ns") {
            m_material.shininess = parts[1].toFloat();
        }
        else if (parts[0] == "map_Kd") {
            m_material.diffuseMap = QFileInfo(mtlPath).path() + "/" + parts[1];
        }
    }

    file.close();
    return true;
}

void Model::setupMesh() {
    // Create buffers/arrays
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // Load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex),
                 &m_vertices[0], GL_STATIC_DRAW);

    // Load data into element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int),
                 &m_indices[0], GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

QMatrix4x4 Model::getModelMatrix() const {
    QMatrix4x4 model;
    model.translate(m_position);
    model.rotate(m_rotation.x(), QVector3D(1, 0, 0));
    model.rotate(m_rotation.y(), QVector3D(0, 1, 0));
    model.rotate(m_rotation.z(), QVector3D(0, 0, 1));
    model.scale(m_scale);
    return model;
}

void Model::draw(Shader* shader) {
    shader->bind();

    // Set material properties
    shader->setVec3("material.ambient", m_material.ambient);
    shader->setVec3("material.diffuse", m_material.diffuse);
    shader->setVec3("material.specular", m_material.specular);
    shader->setFloat("material.shininess", m_material.shininess);

    // Set model matrix
    shader->setMat4("model", getModelMatrix());

    // Draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    shader->release();
}
