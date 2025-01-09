#version 430 core
out vec4 FragColor;

in vec4 vertexColor;
in vec4 vertexGrey;
in vec2 texCoord;
in vec3 fragPos;
in vec3 localNormal;
in vec3 pos;

struct Light{
    vec3 position;
    vec4 color;
};

uniform sampler2D tex;
uniform int objScale;
uniform float percentage;
uniform ivec2 chosenShaders;
uniform int lightCount;
uniform Light lights[3];
uniform mat4 model;

vec4 triplanarMapping(){
    vec3 blend = abs(localNormal);
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
            return vec4(localNormal, 1.0f);
        case 2:
            return triplanarMapping();
        case 3:
            return vertexGrey;
        case 4:
            return vertexColor;
    }
}

void main(){
    vec4 objColor = mix(getColor(chosenShaders.x), getColor(chosenShaders.y), percentage);

    if(lightCount > 0){
        vec4 lightSum = vec4(0.2, 0.2, 0.2, 1.0);     // Setting the fixed ambient light value
        for(int i=0; i < lightCount; i++){
            vec3 lightVec = normalize(lights[i].position - fragPos);
            vec3 worldNormal = vec3(model * vec4(localNormal, 0.0f));
            float lightImpact = max(dot(worldNormal, lightVec), 0.0);
            lightSum += lightImpact * lights[i].color;
        }
        lightSum.a = 1;
        FragColor = objColor * lightSum;
    }
    else
        FragColor = objColor;
}
