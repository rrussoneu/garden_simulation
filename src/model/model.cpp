//
// Created by Raphael Russo on 1/4/25.
//

#include <QFile>
#include <QFileInfo>
#include <QtGui/QImage>
#include "model.h"
#include <QDir>


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
                // OBJ indices are 1 based
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

    QVector3D min(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
    );

    QVector3D max(
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
    );

    for (const auto &vertex : m_vertices) {
        min.setX(qMin(min.x(), vertex.position.x()));
        min.setY(qMin(min.y(), vertex.position.y()));
        min.setZ(qMin(min.z(), vertex.position.z()));

        max.setX(qMax(max.x(), vertex.position.x()));
        max.setY(qMax(max.y(), vertex.position.y()));
        max.setZ(qMax(max.z(), vertex.position.z()));
    }

    QVector3D dimensions = max - min;
    QVector3D center     = (min + max) * 0.5f;

    qDebug() << "Model dimensions:" << dimensions;
    qDebug() << "Model center:"     << center;


    return true;


}

bool Model::parseMTL(const QString& mtlPath) {
    QFile file(mtlPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open MTL file:" << mtlPath;
        return false;
    }

    qDebug() << "Loading material from:" << mtlPath;



    // Set default material values
    m_material.ambient = QVector3D(0.1f, 0.1f, 0.1f);   // Darker ambient by default
    m_material.diffuse = QVector3D(0.6f, 0.4f, 0.2f);   // Default to wooden brown color
    m_material.specular = QVector3D(0.1f, 0.1f, 0.1f);
    m_material.shininess = 4.0f;

    // Get the directory containing the MTL file and handle relative texture paths
    QFileInfo mtlFileInfo(mtlPath);
    QString mtlDir = mtlFileInfo.absolutePath();

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(' ', Qt::SkipEmptyParts);

        if (parts.isEmpty()) continue;

        // Handle standard material properties
        if (parts[0] == "Ka") {
            m_material.ambient = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
            qDebug() << "Ambient color:" << m_material.ambient;
        }
        else if (parts[0] == "Kd") {
            m_material.diffuse = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
            qDebug() << "Diffuse color:" << m_material.diffuse;
        }
        else if (parts[0] == "Ks") {
            m_material.specular = QVector3D(
                    parts[1].toFloat(),
                    parts[2].toFloat(),
                    parts[3].toFloat()
            );
            qDebug() << "Specular color:" << m_material.specular;
        }
        else if (parts[0] == "Ns") {
            m_material.shininess = parts[1].toFloat();
            qDebug() << "Shininess:" << m_material.shininess;
        }
            // Handle texture maps
        else if (parts[0] == "map_Kd") {
            // Convert texture path to absolute path if it's relative
            QString texPath = parts[1];
            QFileInfo texFileInfo(texPath);
            if (texFileInfo.isRelative()) {
                texPath = QDir(mtlDir).absoluteFilePath(texPath);
            }

            // Load the diffuse texture
            GLuint texID = loadTexture(texPath);
            if (texID) {
                m_textures.push_back({texID, "diffuse", texPath});
                qDebug() << "Loaded diffuse texture:" << texPath;
            } else {
                qDebug() << "Failed to load diffuse texture:" << texPath;
            }
        }
        else if (parts[0] == "map_Bump" || parts[0] == "bump") {
            // Handle normal map parameters (-bm intensity value)
            int pathIndex = 1;
            float bumpMultiplier = 1.0f;

            if (parts.size() > 2 && parts[1] == "-bm") {
                bumpMultiplier = parts[2].toFloat();
                pathIndex = 3;
            }

            // Get and resolve the texture path
            QString texPath = parts[pathIndex];
            QFileInfo texFileInfo(texPath);
            if (texFileInfo.isRelative()) {
                texPath = QDir(mtlDir).absoluteFilePath(texPath);
            }

            // Load the normal map texture
            GLuint texID = loadTexture(texPath);
            if (texID) {
                m_textures.push_back({texID, "normal", texPath});
                qDebug() << "Loaded normal map:" << texPath
                         << "with bump multiplier:" << bumpMultiplier;
            } else {
                qDebug() << "Failed to load normal map:" << texPath;
            }
        }
    }

    file.close();

    // Debugging
    qDebug() << "Final material properties:";
    qDebug() << "  Ambient:" << m_material.ambient;
    qDebug() << "  Diffuse:" << m_material.diffuse;
    qDebug() << "  Specular:" << m_material.specular;
    qDebug() << "  Shininess:" << m_material.shininess;
    qDebug() << "  Number of textures loaded:" << m_textures.size();

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

    // Handle textures
    int diffuseNr = 0;
    int normalNr = 0;
    bool hasDiffuse = false;
    bool hasNormal = false;

    for (unsigned int i = 0; i < m_textures.size(); i++) {
        // Activate texture
        glActiveTexture(GL_TEXTURE0 + i);

        // Get texture type number
        QString number;
        QString name = m_textures[i].type;
        if (name == "diffuse") {
            number = QString::number(diffuseNr++);
            hasDiffuse = true;
            shader->setInt("diffuseMap", i);  // Diffuse map
        }
        else if (name == "normal") {
            number = QString::number(normalNr++);
            hasNormal = true;
            shader->setInt("normalMap", i);   // Normal map
        }

        // Bind the texture
        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }


    shader->setBool("hasDiffuseMap", hasDiffuse);
    shader->setBool("hasNormalMap", hasNormal);

    // Draw mesh
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    // Cleanup
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);  // Reset active texture

    shader->release();
}

GLuint Model::loadTexture(const QString& path) {
    qDebug() << "Starting texture load from:" << path;

    QImage image(path);
    if (image.isNull()) {
        qDebug() << "Failed to load texture image from:" << path;
        qDebug() << "Current working directory:" << QDir::currentPath();
        return 0;
    }

    // Before conversion
    qDebug() << "Original image format:" << image.format();
    qDebug() << "Original image size:" << image.size();

    // Convert and flip image
    image = image.convertToFormat(QImage::Format_RGBA8888);
    image = image.mirrored();  // OpenGL needs textures flipped vertically

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        qDebug() << "OpenGL error after texture load:" << err;
    }

    qDebug() << "Successfully created texture with ID:" << textureID;
    return textureID;
}
