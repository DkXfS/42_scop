#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec4 vertexColor;
out vec2 texCoord;
out vec3 normal;
out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	pos = aPos;
	normal = aNormal;
	vertexColor = aColor;
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	//texCoord = vec2(aTexCoord.x, aTexCoord.y);
	texCoord = aTexCoord;
}
