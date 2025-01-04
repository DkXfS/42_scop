#version 430 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex;
uniform int objScale;

void main(){
    vec3 blend = abs(normal);
    blend = normalize(blend);
    float b = (blend.x + blend.y + blend.z);
    blend /= vec3(b, b, b);

    vec4 xCol = texture(tex, pos.yz / objScale);
    vec4 yCol = texture(tex, pos.xz / objScale);
    vec4 zCol = texture(tex, pos.xy / objScale);
    FragColor = xCol * blend.x + yCol * blend.y + zCol * blend.z;
}
