#ifndef BOUNDING_BOX_HPP__
#define BOUNDING_BOX_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

#include "variables.hpp"

class BoundingBox {

private:
    glm::vec3 min; // Coordonnées minimales
    glm::vec3 max; // Coordonnées maximales

    GLuint vao = 0; // Vertex Array Object
    GLuint vbo = 0; // Vertex Buffer Object
    GLuint ebo = 0; // Element Buffer Object (facultatif si indices)
    GLuint colorULoc;

    std::vector<glm::vec3> vertices; // Sommets de la boîte englobante
    std::vector<GLuint> indices;     // Indices pour dessiner les lignes

    bool initialized = false;

public:
    // --- Constructeurs ---
    BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
    BoundingBox() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}

    // --- Accesseurs ---
    glm::vec3 getMin() const { return min; }
    glm::vec3 getMax() const { return max; }
    GLuint getVAO() const {
        return vao;
    }
    size_t getIndicesCount() const {
        return indices.size();
    }

    // --- Méthodes principales ---

    // Initialisation des buffers OpenGL
    void initBuffers() {
        if (initialized) return;

        // Générer les sommets de la bounding box
        generateVertices();

        // Création des VAO, VBO, et éventuellement EBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // Charger les sommets dans le VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Configurer les attributs de vertex
        glEnableVertexAttribArray(0); // Layout 0 : position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // Charger les indices dans l'EBO (si nécessaire)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0); // Delink le VAO

        colorULoc = glGetUniformLocation(programID, "color");
        initialized = true;
    }

    // Générer les sommets et les indices pour la boîte englobante
    void generateVertices() {
        vertices = {
            // Bottom face
            {min.x, min.y, min.z}, {max.x, min.y, min.z},
            {max.x, min.y, min.z}, {max.x, max.y, min.z},
            {max.x, max.y, min.z}, {min.x, max.y, min.z},
            {min.x, max.y, min.z}, {min.x, min.y, min.z},

            // Top face
            {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {max.x, min.y, max.z}, {max.x, max.y, max.z},
            {max.x, max.y, max.z}, {min.x, max.y, max.z},
            {min.x, max.y, max.z}, {min.x, min.y, max.z},

            // Connect bottom to top
            {min.x, min.y, min.z}, {min.x, min.y, max.z},
            {max.x, min.y, min.z}, {max.x, min.y, max.z},
            {min.x, max.y, min.z}, {min.x, max.y, max.z},
            {max.x, max.y, min.z}, {max.x, max.y, max.z}
        };

        // Les indices pour dessiner les lignes de la boîte
        for (GLuint i = 0; i < vertices.size(); ++i) {
            indices.push_back(i);
        }
    }

    void draw() const {
        if (!initialized) {
            std::cerr << "BoundingBox not initialized! Call initBuffers() first." << std::endl;
            return;
        }
        // Activer le VAO de la bounding box
        glBindVertexArray(vao);
        glm::vec4 color {1.f, 0.f, 0.f, 1.f};
        glUniform4fv(colorULoc, 1, &color[0]); // Couleur
        glLineWidth(2.f);

        // Dessiner les lignes
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    // Mettre à jour la bounding box après une transformation
    void updateAfterTransformation(const std::vector<glm::vec3>& vertices, const glm::mat4& transform) {
        std::vector<glm::vec3> transformedVertices;

        for (const auto& vertex : vertices) {
            glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
            transformedVertices.push_back(glm::vec3(transformedVertex));
        }

        init(transformedVertices);
    }

    // Initialiser avec des sommets
    void init(const std::vector<glm::vec3>& vertices) {
        if (vertices.empty()) return;

        glm::vec3 minVertex = vertices[0];
        glm::vec3 maxVertex = vertices[0];

        for (const auto& vertex : vertices) {
            minVertex = glm::min(minVertex, vertex);
            maxVertex = glm::max(maxVertex, vertex);
        }

        min = minVertex;
        max = maxVertex;

        // Régénérer les sommets et indices
        generateVertices();
        initBuffers();
    }

    // Destructeur
    ~BoundingBox() {
        if (initialized) {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }
    }
};

#endif
