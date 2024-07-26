#include "mesh.h"
#include "../math/vec3.h"
#include <stdlib.h>

void computeMeshNormals(Mesh *mesh) {
    if (mesh->normals == NULL) {
        mesh->normals = malloc(mesh->indexes_count * sizeof(Vec3f));
    }

    for (int i = 0; i < mesh->indexes_count; i += 3) {
        Vec3f *v0 = &mesh->positions[mesh->pos_indices[i]];
        Vec3f *v1 = &mesh->positions[mesh->pos_indices[i + 1]];
        Vec3f *v2 = &mesh->positions[mesh->pos_indices[i + 2]];

        Vec3f edge1 = vec3fsubV(*v1, *v0);
        Vec3f edge2 = vec3fsubV(*v2, *v0);
        Vec3f normal = vec3Cross(edge1, edge2);
        normal = vec3Normalize(normal);

        // Assuming flat shading, apply the same normal to all vertices of the triangle
        mesh->normals[mesh->pos_indices[i]] = normal;
        mesh->normals[mesh->pos_indices[i + 1]] = normal;
        mesh->normals[mesh->pos_indices[i + 2]] = normal;
    }
}

void precomputeLighting(Mesh *mesh, Vec3f light) {
    mesh->diffuseLight = malloc(sizeof(float) * mesh->indexes_count);

    for (int i = 0; i < mesh->indexes_count; i++) {
        Vec3f normal = mesh->normals[i];
        float intensity = (1.0 + vec3Dot(normal, light)) * 0.5;
        intensity = MIN(1.0, MAX(intensity, 0));
        mesh->diffuseLight[i] = intensity;
    }
}

void precomputeTextureCoordinates(Mesh *mesh) {
    mesh->precomputedTCA = malloc(mesh->indexes_count * sizeof(Vec2f));
    mesh->precomputedTCB = malloc(mesh->indexes_count * sizeof(Vec2f));
    mesh->precomputedTCC = malloc(mesh->indexes_count * sizeof(Vec2f));

    for (int i = 0; i < mesh->indexes_count; i += 3) {
        mesh->precomputedTCA[i] = mesh->textCoord[mesh->tex_indices[i]];
        mesh->precomputedTCB[i] = mesh->textCoord[mesh->tex_indices[i + 1]];
        mesh->precomputedTCC[i] = mesh->textCoord[mesh->tex_indices[i + 2]];
    }
}

