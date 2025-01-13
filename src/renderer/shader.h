//
// Created by Raphael Russo on 1/2/25.
//

#ifndef GARDEN_SIMULATION_SHADER_H
#define GARDEN_SIMULATION_SHADER_H
#pragma once

#include <OpenGL/gl.h>
#include <QOpenGLFunctions>
#include <QString>
#include <QOpenGLShaderProgram>



class Shader {

public:
    // Takes path to vert and frag shaders
    Shader(const QString &vertexPath, const QString &fragmentPath);
    ~Shader();

    bool compile();
    void bind();
    void release();

    void setMat4(const QString &name, const QMatrix4x4 &matrix);
    void setVec3(const QString &name, const QVector3D &vector);
    void setFloat(const QString &name, float value);
    void setInt(const QString &name, int value);
    void setBool(const QString& name, bool value);

    bool hasUniform(const QString& name) const {
        return m_program->uniformLocation(name.toStdString().c_str()) != -1;
    }

    bool getBoolUniform(const QString& name) const {
        // Get the location of the uniform
        GLint location = m_program->uniformLocation(name.toStdString().c_str());
        if (location == -1) return false;

        // Get the value using OpenGL
        GLint value;
        glGetUniformiv(m_program->programId(), location, &value);
        return value != 0;
    }


    template<typename T>
    T getUniformValue(const QString& name) const {
        static_assert(false, "Unsupported uniform type");
    }
    GLint getUniformLocation(const QString& name) const;

private:
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    QString m_vertexPath;
    QString m_fragmentPath;
    bool m_isCompiled;
};

template<>
inline bool Shader::getUniformValue<bool>(const QString& name) const {
    int location = m_program->uniformLocation(name.toStdString().c_str());
    GLint value;
    glGetUniformiv(m_program->programId(), location, &value);
    return value != 0;
}

template<>
inline float Shader::getUniformValue<float>(const QString& name) const {
    int location = m_program->uniformLocation(name.toStdString().c_str());
    GLfloat value;
    glGetUniformfv(m_program->programId(), location, &value);
    return value;
}

template<>
inline QVector3D Shader::getUniformValue<QVector3D>(const QString& name) const {
    int location = m_program->uniformLocation(name.toStdString().c_str());
    GLfloat values[3];
    glGetUniformfv(m_program->programId(), location, values);
    return QVector3D(values[0], values[1], values[2]);
}

template<>
inline QMatrix4x4 Shader::getUniformValue<QMatrix4x4>(const QString& name) const {
    int location = m_program->uniformLocation(name.toStdString().c_str());
    GLfloat values[16];
    glGetUniformfv(m_program->programId(), location, values);
    return QMatrix4x4(values);
}

#endif //GARDEN_SIMULATION_SHADER_H
