#ifndef LANDSCAPE_HPP__
#define LANDSCAPE_HPP__
#include "TP1/GameObject.hpp"
#include "Plane.hpp"


class Landscape : public Plane {
public:
    int resolution;
    int size;
    bool randomHeight;

    std::vector<std::vector<float>> heightmap; // La heightmap du terrain

    // Constructeur de Landscape
    Landscape () {}

    Landscape(std::string name, unsigned int res, unsigned int size, int textureID, const char *texturePath, bool randomHeight=false) {
        this->type = LANDSCAPE;
    }

    // Méthode pour obtenir la hauteur du terrain à une position donnée
    float getTerrainHeightAt(float x, float z) const {
        return calculateHeightFromCoordinates(x, z);
    }

private:
    // Méthode pour calculer la hauteur du terrain à partir des coordonnées
    float calculateHeightFromCoordinates(float x, float z) const {
        // Conversion des coordonnées x et z en indices de la heightmap
        float u = (x / resolution) * (heightmap.size() - 1);
        float v = (z / resolution) * (heightmap[0].size() - 1);

        // Interpolation bilinéaire pour obtenir la hauteur du terrain à partir de la heightmap
        int x0 = static_cast<int>(floor(u));
        int z0 = static_cast<int>(floor(v));
        int x1 = x0 + 1;
        int z1 = z0 + 1;

        // Coefficients d'interpolation
        float tx = u - x0;
        float tz = v - z0;

        // Interpolation bilinéaire
        float height = lerp(lerp(heightmap[x0][z0], heightmap[x1][z0], tx),
                            lerp(heightmap[x0][z1], heightmap[x1][z1], tx), tz);
        
        return height;
    }

    // Fonction pour l'interpolation linéaire
    float lerp(float a, float b, float t) const {
        return (1 - t) * a + t * b;
    }
};

#endif