#version 330 core

layout(points) in;                  // Entrée : points (voxels)
layout(triangle_strip, max_vertices = 24) out; // Sortie : triangles pour un cube

in VS_OUT {
    vec3 center;      // Centre du voxel
    float halfSize;   // HalfSize du voxel
} gs_in[];

void emitCubeFace(vec3 center, vec3 offset1, vec3 offset2) {
    vec3 p0 = center + offset1 + offset2;
    vec3 p1 = center + offset1 - offset2;
    vec3 p2 = center - offset1 + offset2;
    vec3 p3 = center - offset1 - offset2;

    gl_Position = vec4(p0, 1.0); EmitVertex();
    gl_Position = vec4(p1, 1.0); EmitVertex();
    gl_Position = vec4(p2, 1.0); EmitVertex();
    gl_Position = vec4(p3, 1.0); EmitVertex();
    EndPrimitive();
}

void main() {
    vec3 center = gs_in[0].center;   // Lire le centre transmis
    float halfSize = gs_in[0].halfSize; // Lire la halfSize

    // Générer les 6 faces du cube (chaque face est un quad)
    emitCubeFace(center, vec3(halfSize, 0, 0), vec3(0, halfSize, 0)); // Face +X
    emitCubeFace(center, vec3(-halfSize, 0, 0), vec3(0, halfSize, 0)); // Face -X
    emitCubeFace(center, vec3(0, halfSize, 0), vec3(0, 0, halfSize)); // Face +Y
    emitCubeFace(center, vec3(0, -halfSize, 0), vec3(0, 0, halfSize)); // Face -Y
    emitCubeFace(center, vec3(0, 0, halfSize), vec3(halfSize, 0, 0)); // Face +Z
    emitCubeFace(center, vec3(0, 0, -halfSize), vec3(halfSize, 0, 0)); // Face -Z
}
