#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <GLAD/glad.h>
#include "math.hpp"

class Obj{
    struct Vertex{
        Math::Vec3<float> position;
        Math::Vec2<float> texCoord;
        Math::Vec3<float> normal;
        Math::Vec4<float> color;
    };

    std::string name;
    int scale = 1;
    bool missingNormals = false;
    bool validSource = true;

    unsigned int vao, vbo;
    std::vector<Vertex> vtxData;
    Math::Vec3<float> vtxMax;
    Math::Vec3<float> vtxMin;

    void buildNormals(std::vector<Vertex>::iterator va){
        Math::Vec3 v1 = (*va).position - (*(va-1)).position;
        Math::Vec3 v2 = (*va).position - (*(va-2)).position;
        Math::Vec3 normal = (v1 * v2).normalized();
        (*va).normal = normal;
        (*(va-1)).normal = normal;
        (*(va-2)).normal = normal;
    }

    void buildTexCoord(Vertex* v){
        float theta = atan2(v->position.z, v->position.x);
		float phi =
			acos(v->position.y / sqrt(pow(v->position.x, 2) + pow(v->position.y, 2) + pow(v->position.z, 2)));
		v->texCoord.x = (theta + M_PI) / (2.0f * M_PI);
		v->texCoord.y = phi / M_PI;
    }

    Vertex composeVert(std::string desc, float color, std::vector<Math::Vec3<float>>* verts, std::vector<Math::Vec3<float>>* normals, std::vector<Math::Vec2<float>>* texCoords){
        std::stringstream vtxstr(desc);
        std::string idxStr;
        Vertex ret;
        int count = 0;
        while(std::getline(vtxstr, idxStr, '/')){
            if(idxStr.empty()){
                if(count == 1)
                    buildTexCoord(&ret);
                if(count == 2)
                    missingNormals = true;
            }
            else{
                int idx{std::stoi(idxStr) - 1};
                if(idx < 0)
                    idx = verts->size() - idx + 1;
                switch (count){
                    case 0:
                        ret.position = (*verts)[idx];
                        break;
                    case 1:
                        ret.texCoord = (*texCoords)[idx];
                        break;
                    case 2:
                        ret.normal = (*normals)[idx];
                        break;
                }
            }
            count++;
        }
        while(count < 3){
            if(count == 1)
                buildTexCoord(&ret);
            if(count == 2)
                missingNormals = true;
            count++;
        }
        ret.texCoord.y = 1 - ret.texCoord.y;
        ret.color.x = color;
        ret.color.y = color;
        ret.color.z = color;
        ret.color.w = 1;
        return ret;
    }

    void saveMinMax(Math::Vec3<float> v){
        //Saving maximums
        if(v.x > vtxMax.x)
            vtxMax.x = v.x;
        if(v.y > vtxMax.y)
            vtxMax.y = v.y;
        if(v.z > vtxMax.z)
            vtxMax.z = v.z;

        //Saving minimums
        if(v.x < vtxMin.x)
            vtxMin.x = v.x;
        if(v.y < vtxMin.y)
            vtxMin.y = v.y;
        if(v.z < vtxMin.z)
            vtxMin.z = v.z;
    }

    public:
        Obj(std::string file){
            std::vector<Math::Vec3<float>> vertices;
            std::vector<Math::Vec3<float>> vtxNormals;
            std::vector<Math::Vec2<float>> texCoords;
            std::string line, type;
            std::ifstream fileStream{file};
            char fileExtension[5] = ".obj";

            if(!fileStream.is_open()){
                std::cout << "File \"" << file << "\" can't be opened.\n";
                validSource = false;
                return;
            }

            for(int i=0; i<4; i++)
                if(fileExtension[i] != file[file.length()-4+i]){
                    std::cout << "\"" << file << "\" isn't a wavefront OBJ file.\n";
                    validSource = false;
                    return;
                }

            while(std::getline(fileStream, line)){
                std::stringstream ss = std::stringstream(line);
                ss >> type;

                if(type == "v"){
                    float x, y, z;
                    ss >> x >> y >> z;
                    vertices.push_back(Math::Vec3<float>{x, y, z});
                    saveMinMax(vertices.back());
                }
                else if(type == "vt"){
                    float x, y;
                    ss >> x >> y;
                    texCoords.push_back(Math::Vec2<float>{x, y});
                }
                else if(type == "vn"){
                    float x, y, z;
                    ss >> x >> y >> z;
                    vtxNormals.push_back(Math::Vec3<float>{x, y, z});
                }
                else if(type == "f"){
                    std::vector<std::string> faceVtxs;
                    std::string temp;
                    while(ss >> temp)
                        faceVtxs.push_back(temp);

                    short triangleCount = faceVtxs.size() - 2;
                    for(int i=1; i <= triangleCount; i++){
                        float grayScale = ((float) rand() / (RAND_MAX*2.7)) + 0.15f;
                        vtxData.push_back(composeVert(faceVtxs[0], grayScale, &vertices, &vtxNormals, &texCoords));
                        vtxData.push_back(composeVert(faceVtxs[i], grayScale, &vertices, &vtxNormals, &texCoords));
                        vtxData.push_back(composeVert(faceVtxs[i+1], grayScale, &vertices, &vtxNormals, &texCoords));
                        if(missingNormals){
                            buildNormals(vtxData.end()-1);
                            missingNormals = false;
                        }
                    }
                }
                else if(type == "o")
                    ss >> name;
            }

            float maxCoord = std::max(std::max(vtxMax.x, vtxMax.y), vtxMax.z);
            while(maxCoord > 10){
                scale *= 10;
                maxCoord /= 10;
            }

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, vtxData.size()*sizeof(Vertex), &vtxData[0], GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8*sizeof(float)));
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
            glEnableVertexAttribArray(0);
            glEnableVertexAttribArray(1);
            glEnableVertexAttribArray(2);
            glEnableVertexAttribArray(3);
        }

        ~Obj(){
            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
        }

        operator bool() const{ return validSource; }

        std::string* getName(){ return &name; }

        float getScale(){ return scale; }

        void draw(){
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, vtxData.size());
        }

        Math::Vec3<float> getMidPoint(){
            return Math::Vec3<float>{(vtxMax.x+vtxMin.x)/2, (vtxMax.y+vtxMin.y)/2, (vtxMax.z+vtxMin.z)/2};
        }

        float boundingSphereRadius(){
            return sqrt(pow(vtxMax.x - vtxMin.x, 2) + pow(vtxMax.y - vtxMin.y, 2) + pow(vtxMax.z - vtxMin.z, 2)) / 2.0f;
        }
};
