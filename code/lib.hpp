#ifndef LIB_HPP_
#define LIB_HPP_

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace glm;

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/texture.hpp>
#include "variables.hpp"
// #include "Camera.hpp"

/* ------------------ Plane Generation ------------------ */

void genPlane(std::vector<unsigned short> &indices, std::vector<std::vector<unsigned short>> &triangles, std::vector<glm::vec3> &indexed_vertices,std::vector<glm::vec2> &uv, int resolution, int size, bool randomheight)
{
    indices.clear();
    triangles.clear();
    indexed_vertices.clear();

    float step = size / (float)resolution;
    float x = 0, y = 0, z = 0;

    for (int i = 0; i <= resolution; i++) {
        for (int j = 0; j <= resolution; j++) {
            x = j * step;
            if(randomheight) {
                y = std::max((float) rand() / (RAND_MAX),0.f);
            } else {
                y = 0.f;
            }
            z = i * step;
            indexed_vertices.push_back(glm::vec3(x - size/2.f, y, z - size/2.f));
        }
    }

    for (int i = 0; i < resolution; i++) {
        for (int j = 0; j < resolution; j++) {
            unsigned short bottomLeft = j + i * (resolution + 1);
            unsigned short bottomRight = bottomLeft + 1;
            unsigned short topLeft = bottomLeft + (resolution + 1);
            unsigned short topRight = topLeft + 1;

            triangles.push_back({bottomLeft, bottomRight, topLeft});
            triangles.push_back({bottomRight, topRight, topLeft});
        }
    }
    for (unsigned short i = 0; i < triangles.size(); i++) {
        indices.push_back(triangles[i][0]);
        indices.push_back(triangles[i][1]);
        indices.push_back(triangles[i][2]);
    }

}

void computePlaneUV(std::vector<glm::vec2> &uv, int resolution) 
{
    uv.clear();

    float UvStep = 1.0f / (float)resolution;
    for (int i = 0; i <= resolution; i++)
    {
        for (int j = 0; j <= resolution; j++)
        {
            float u = j * UvStep;
            float v = i * UvStep;
            uv.push_back(glm::vec2(u, v));
        }
    }
}

/* ------------------ MESH ------------------ */

void targetMesh(std::vector<glm::vec3> &indexed_vertices )
{
    glm::vec3 centroid(0.0f);
    for (const auto& vertex : indexed_vertices) {
        centroid += vertex;
    }
    centroid /= static_cast<float>(indexed_vertices.size());

    // camera.setFront(centroid - camera.getPosition());
}

void computeUV(std::vector<glm::vec2> &uv, std::vector<glm::vec3> &indexed_vertices) 
{
    uv.clear();

    for (const auto& vertex : indexed_vertices) {
        glm::vec2 uv_coord(vertex.x, 1 - vertex.y);
        uv.push_back(uv_coord);
    }
}

#endif