#version 430 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;

uniform sampler2D tex;
uniform int objScale;
uniform float percentage;
uniform ivec2 chosenShaders;

vec4 triplanarMapping(){
    vec3 blend = abs(normal);
    blend = normalize(blend);
    float b = (blend.x + blend.y + blend.z);
    blend /= vec3(b, b, b);

    vec4 xCol = texture(tex, pos.yz / objScale);
    vec4 yCol = texture(tex, pos.xz / objScale);
    vec4 zCol = texture(tex, pos.xy / objScale);
    return xCol * blend.x + yCol * blend.y + zCol * blend.z;
}

vec4 getColor(int choice){
    switch(choice){
        case 0:
            return texture(tex, texCoord);
        case 1:
            return vec4(normal, 1.0f);
        case 2:
            return triplanarMapping();
        case 3:
            return vertexColor;
    }
}

void main(){
    FragColor = mix(getColor(chosenShaders.x), getColor(chosenShaders.y), percentage);
}
