#ifndef SHADER_H
#define SHADER_H

//#include <glad/glad.h>
//#include <glm/glm.hpp>
#include "NDKHelper.h"

#include "Helper.h"

class Shader
{
public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(const char* vertexPath, const char* fragmentPath, std::map<std::string, std::string>& shaderParams) {
        GLuint vertShader, fragShader;

        // Create shader program
        ID = glCreateProgram();
        Helper::log("Created Shader "+ std::to_string(ID));

        // Create and compile vertex shader
        if (!ndk_helper::shader::CompileShader(&vertShader, GL_VERTEX_SHADER, vertexPath,
                                               shaderParams)) {

            Helper::log("Failed to compile vertex shader: " + std::string(vertexPath));
            checkCompileErrors(vertShader, "VERTEX");
            glDeleteProgram(ID);
            //return false;
        }

        // Create and compile fragment shader
        if (!ndk_helper::shader::CompileShader(&fragShader, GL_FRAGMENT_SHADER,
                                               fragmentPath, shaderParams)) {
            Helper::log("Failed to compile fragment shader: " + std::string(fragmentPath));
            checkCompileErrors(fragShader, "FRAGMENT");
            glDeleteProgram(ID);
            //return false;
        }

        // Attach vertex shader to program
        glAttachShader(ID, vertShader);

        // Attach fragment shader to program
        glAttachShader(ID, fragShader);

        // Link program
        if (!ndk_helper::shader::LinkProgram(ID)) {
            Helper::log("Failed to link program: " + std::to_string(ID));
            checkCompileErrors(ID, "PROGRAM");

            if (vertShader) {
                glDeleteShader(vertShader);
                vertShader = 0;
            }
            if (fragShader) {
                glDeleteShader(fragShader);
                fragShader = 0;
            }
            if (ID) {
                glDeleteProgram(ID);
            }
            //return false;
        }

        // Release vertex and fragment shaders
        if (vertShader) glDeleteShader(vertShader);
        if (fragShader) glDeleteShader(fragShader);
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() const
    { 
        glUseProgram(ID); 
    }

    void terminate() {
        glDeleteProgram(ID);
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
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Helper::log("ERROR::SHADER_COMPILATION_ERROR of type: " + type +"\n" + infoLog + "\n -- --------------------------------------------------- -- ");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Helper::log("ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog + "\n -- --------------------------------------------------- -- ");
            }
        }
    }
};
#endif

