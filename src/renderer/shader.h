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
private:
    std::unique_ptr<QOpenGLShaderProgram> m_program;
    QString m_vertexPath;
    QString m_fragmentPath;
    bool m_isCompiled;
};


#endif //GARDEN_SIMULATION_SHADER_H
