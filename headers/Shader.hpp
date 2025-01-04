#pragma once

#include <fstream>
#include <glad/glad.h>

class Shader{
    unsigned int programID;
    enum ShaderType{ vertex, fragment, program };

    friend std::ostream& operator<<(std::ostream& out, ShaderType type){
        std::string text[]{"vertex", "fragment", "program"};
        out << text[type];
        return out;
    }

    void checkErrors(unsigned int shdrPrgmID, GLenum param, ShaderType type){
        int success;
        char infoLog[1024];
        if(type == program){
            glGetProgramiv(shdrPrgmID, GL_LINK_STATUS, &success);
            if (!success){
                glGetProgramInfoLog(shdrPrgmID, 1024, NULL, infoLog);
                std::cout << "ERROR => " << type << " shader linking failed\n" << infoLog << std::endl;
            }
        }
        else{
            glGetShaderiv(shdrPrgmID, GL_COMPILE_STATUS, &success);
            if (!success){
                glGetShaderInfoLog(shdrPrgmID, 1024, NULL, infoLog);
                std::cout << "ERROR => " << type << " shader compilation failed\n" << infoLog << std::endl;
            }
        }
    }

    unsigned int compileShader(GLenum shaderType, const char* shaderFileLoc){
        std::ifstream shdrfile{shaderFileLoc};
        std::string shdrContents;
        if(shdrfile){
          shdrfile.seekg(0, std::ios::end);
          shdrContents.resize(shdrfile.tellg());
          shdrfile.seekg(0, std::ios::beg);
          shdrfile.read(&shdrContents[0], shdrContents.size());
          shdrfile.close();
        }
        const char* shdrSrc = shdrContents.c_str();

        unsigned int shaderID = glCreateShader(shaderType);
        glShaderSource(shaderID, 1, &shdrSrc, NULL);
        glCompileShader(shaderID);
        checkErrors(shaderID, GL_COMPILE_STATUS, shaderType==GL_VERTEX_SHADER ? vertex : fragment);
        return shaderID;
    }

    unsigned int linkProgram(unsigned int vtxshaderID, unsigned int frgmtshaderID){
        unsigned int programID = glCreateProgram();
        glAttachShader(programID, vtxshaderID);
        glAttachShader(programID, frgmtshaderID);
        glLinkProgram(programID);
        checkErrors(programID, GL_LINK_STATUS, program);
        return programID;
    }

    public:
        operator unsigned int() const{ return programID; }

        Shader(const char* vertexLoc, const char* fragmentLoc){
            unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexLoc);
            unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentLoc);
            programID = linkProgram(vertexShader, fragmentShader);
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        ~Shader(){
            glDeleteProgram(programID);
        }
};
