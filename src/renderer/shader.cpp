//
// Created by Raphael Russo on 1/2/25.
//

#include <QFile>
#include "shader.h"


Shader::Shader(const QString &vertexPath, const QString &fragmentPath) :
    m_vertexPath(vertexPath)
    , m_fragmentPath(fragmentPath)
    , m_isCompiled(false)
{
    m_program = std::make_unique<QOpenGLShaderProgram>();
}

Shader::~Shader() {
    if (m_program) {
        m_program->release();
    }
}

bool Shader::compile() {
    if (m_isCompiled) return true;

    // Load and compile vertex shader
    QFile vertexFile(m_vertexPath);
    if (!vertexFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open vertex shader: " << m_vertexPath;
        return false;
    }

    QString vertexCode = QTextStream(&vertexFile).readAll();
    vertexFile.close();

    // And fragment shader
    QFile fragmentFile(m_fragmentPath);
    if (!fragmentFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open fragment shader: " << m_fragmentPath;
        return false;
    }
    QString fragmentCode = QTextStream(&fragmentFile).readAll();
    fragmentFile.close();

    // Add shaders to program
    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexCode)) {
        qDebug() << "Vertex shader compilation failed";
        return false;
    }

    if (!m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentCode)) {
        qDebug() << "Fragment shader compilation failed";
        return false;
    }

    // Link
    if (!m_program->link()) {
        qDebug() << "Shader program linking failed";
        return false;
    }

    m_isCompiled = true;
    return true;
}

void Shader::bind() {
    if (m_isCompiled) {
        m_program->bind();
    }
}

void Shader::release() {
    m_program->release();
}

void Shader::setMat4(const QString &name, const QMatrix4x4 &matrix) {
    m_program->setUniformValue(name.toStdString().c_str(), matrix);
}

void Shader::setVec3(const QString &name, const QVector3D &vector) {
    m_program->setUniformValue(name.toStdString().c_str(), vector);

}
void Shader::setFloat(const QString& name, float value) {
    m_program->setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setInt(const QString& name, int value) {
    m_program->setUniformValue(name.toStdString().c_str(), value);
}

void Shader::setBool(const QString& name, bool value) {
    m_program->setUniformValue(name.toStdString().c_str(), value ? 1 : 0);

}
