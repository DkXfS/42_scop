#pragma once

#include <iostream>
#include <string_view>
#include <string>
#include <glad/glad.h>
#include <vector>
#include "Math.hpp"

struct Light{
    Math::Vec3<float> position;
    Math::Vec4<float> color;
};

void setLights(std::vector<Light> lights, unsigned int shader){
    for(int i=0; i < lights.size(); i++){
        std::string UniformName[3] = {"lights[", "].position", "].color"};
        glUniform3fv(glGetUniformLocation(shader, (UniformName[0] + std::to_string(i) + UniformName[1]).c_str()), 1, &(lights[i].position.x));
        glUniform4fv(glGetUniformLocation(shader, (UniformName[0] + std::to_string(i) + UniformName[2]).c_str()), 1, &(lights[i].color.x));
    }
}
