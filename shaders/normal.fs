#version 430 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex;

void main(){
	FragColor = vec4(normal, 1.0f);
}
