#ifndef BOUNDING_BOX_HPP__
#define BOUNDING_BOX_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <vector>

class BoundingBox {

private:
    glm::vec3 min;
    glm::vec3 max;

public:

    //Constructor : 
    BoundingBox(glm::vec3 min, glm::vec3 max){
        this->min = min;
        this->max = max;
    }

    BoundingBox(){}

    // Methods
    glm::vec3 getMin() {return this->min;}
    glm::vec3 getMax() {return this->max;}

    bool intersect(const BoundingBox& other) const {        // Vérifiez s'il y a intersection sur chaque axe (x, y, z)
        bool collisionX = (max.x >= other.min.x) && (min.x <= other.max.x);
        bool collisionY = (max.y >= other.min.y) && (min.y <= other.max.y);
        bool collisionZ = (max.z >= other.min.z) && (min.z <= other.max.z);

        // Il y a intersection si les boîtes se chevauchent sur tous les axes
        return collisionX && collisionY && collisionZ;
    }
    
    void init(const std::vector<glm::vec3>& vertices) {
        if (vertices.empty())
            return;

        glm::vec3 minVertex = vertices[0];
        glm::vec3 maxVertex = vertices[0];

        // Trouver les coordonnées minimales et maximales dans le tableau de vertices
        for (const auto& vertex : vertices) {
            // Trouver les valeurs minimales sur chaque axe
            minVertex.x = std::min(minVertex.x, vertex.x);
            minVertex.y = std::min(minVertex.y, vertex.y);
            minVertex.z = std::min(minVertex.z, vertex.z);

            // Trouver les valeurs maximales sur chaque axe
            maxVertex.x = std::max(maxVertex.x, vertex.x);
            maxVertex.y = std::max(maxVertex.y, vertex.y);
            maxVertex.z = std::max(maxVertex.z, vertex.z);
        }

        // Assigner les coordonnées minimales et maximales
        min = minVertex;
        max = maxVertex;
        // std::cout << "Initialisation of bounding box done : min(" << min.x << "; " << min.y << "; " << min.z << ") / max(" << max.x << "; " << max.y << "; " << max.z << ")" << std::endl;
    }

    void updateAfterTransformation(std::vector<glm::vec3> vertices, glm::mat4 transform) {
        std::vector<glm::vec3> transformedVertices;
        for (const auto& vertex : vertices) {
            // Appliquer la matrice de transformation à chaque vertex
            glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
            transformedVertices.push_back(glm::vec3(transformedVertex));
        }

        // Mettre à jour la bounding box avec les nouveaux vertices transformés
        init(transformedVertices);
    }

    void draw(glm::vec3 color) const {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_LINES);

        // Bottom face
        glVertex3d(min[0], min[1], min[2]);
        glVertex3d(max[0], min[1], min[2]);

        glVertex3d(min[0], min[1], min[2]);
        glVertex3d(min[0], max[1], min[2]);

        glVertex3d(max[0], min[1], min[2]);
        glVertex3d(max[0], max[1], min[2]);

        glVertex3d(min[0], max[1], min[2]);
        glVertex3d(max[0], max[1], min[2]);

        // Top face
        glVertex3d(min[0], min[1], max[2]);
        glVertex3d(max[0], min[1], max[2]);

        glVertex3d(min[0], min[1], max[2]);
        glVertex3d(min[0], max[1], max[2]);

        glVertex3d(max[0], min[1], max[2]);
        glVertex3d(max[0], max[1], max[2]);

        glVertex3d(min[0], max[1], max[2]);
        glVertex3d(max[0], max[1], max[2]);

        // Connect the corners
        glVertex3d(min[0], min[1], min[2]);
        glVertex3d(min[0], min[1], max[2]);

        glVertex3d(max[0], min[1], min[2]);
        glVertex3d(max[0], min[1], max[2]);

        glVertex3d(min[0], max[1], min[2]);
        glVertex3d(min[0], max[1], max[2]);

        glVertex3d(max[0], max[1], min[2]);
        glVertex3d(max[0], max[1], max[2]);

        glEnd();
    }

    // Opérateurs
    BoundingBox& operator=(const BoundingBox& other) {
        if (this != &other) { // Vérifie si l'objet n'est pas le même que celui passé en argument
            min = other.min;
            max = other.max;
        }
        return *this;
    }

    bool operator==(const BoundingBox& other) const {
        return min == other.min && max == other.max;
    }

    glm::vec3 getMinOverlap(const BoundingBox& other) const {
        glm::vec3 overlap;

        glm::vec3 thisMin = min;
        glm::vec3 thisMax = max;
        glm::vec3 otherMin = other.min;
        glm::vec3 otherMax = other.max;

        // Calcul du chevauchement sur chaque axe
        overlap.x = std::min(thisMax.x - otherMin.x, otherMax.x - thisMin.x);
        overlap.y = std::min(thisMax.y - otherMin.y, otherMax.y - thisMin.y);
        overlap.z = std::min(thisMax.z - otherMin.z, otherMax.z - thisMin.z);

        return overlap;
    }


};

#endif