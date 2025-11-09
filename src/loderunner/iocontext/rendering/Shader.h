#ifndef SHADER_H
#define SHADER_H

#if defined __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include "emscripten.h"
#else 
#include <glad/glad.h>
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <format>
#include <regex>
#include <functional>

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------

    std::string vertexPath;
    std::string fragmentPath;

    Shader(std::string vertexPath, std::string fragmentPath) : Shader(vertexPath.c_str(), fragmentPath.c_str())
    {

    }

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        this->vertexPath = vertexPath;
        this->fragmentPath = fragmentPath;

        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        
        try 
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();		
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            
            unsigned int vertex, fragment;

#if defined __EMSCRIPTEN__ || defined ANDROID_VERSION
            const GLchar* vertex_shader_with_version[2] = { "#version 300 es\nprecision highp float;\n", vertexCode.c_str() };
            const GLchar* fragment_shader_with_version[2] = { "#version 300 es\nprecision highp float;\n", fragmentCode.c_str() };
#else
            const GLchar* vertex_shader_with_version[2] = { "#version 460 core\n", vertexCode.c_str() };
            const GLchar* fragment_shader_with_version[2] = { "#version 460 core\n", fragmentCode.c_str() };

#endif // __EMSCRIPTEN__

            // vertex shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 2, vertex_shader_with_version, NULL);
            glCompileShader(vertex);
            checkCompileErrors(vertex, "VERTEX");
            // fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 2, fragment_shader_with_version, NULL);
            glCompileShader(fragment);
            checkCompileErrors(fragment, "FRAGMENT");
            // if geometry shader is given, compile geometry shader

            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);

            glLinkProgram(ID);
            checkCompileErrors(ID, "PROGRAM");
            // delete the shaders as they're linked into our program now and no longer necessery
            glDeleteShader(vertex);
            glDeleteShader(fragment);

        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
    }
	
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void setIntArray(const std::string& name, int* values, int size)
    {
        glUniform1iv(glGetUniformLocation(ID, name.c_str()), size, values);
    }

    void setInt2Array(const std::string& name, int* values, int size)
    {
        glUniform2iv(glGetUniformLocation(ID, name.c_str()), size, values);
    }

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }

    void setVec2Array(const std::string& name, float* values, float size)
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), size,  values);
    }

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                std::string paths = vertexPath + "/" + fragmentPath;

                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << " - " << paths << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                std::string paths = vertexPath + "/" + fragmentPath;

                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << " - " << paths << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
#endif