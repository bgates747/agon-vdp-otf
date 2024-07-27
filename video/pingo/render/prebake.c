#include <stdlib.h>

#include "_.h"
#include "prebake.h"
#include "mesh.h"
#include "../math/vec3.h"
#include "../math/mat4.h"


#define MIN(a, b)(((a) < (b)) ? (a) : (b))
#define MAX(a, b)(((a) > (b)) ? (a) : (b))

void copyPositions(Prebake *prebake, Mesh *mesh) {
    for (int i = 0; i < mesh->indexes_count; i++) {
        prebake[i].position = mesh->positions[mesh->pos_indices[i]];
    }
}

void copyNormals(Prebake *prebake, Mesh *mesh) {
    for (int i = 0; i < mesh->indexes_count; i++) {
        prebake[i].normal = mesh->normals[mesh->nor_indices[i]];
    }
}

void copyTexCoords(Prebake *prebake, Mesh *mesh) {
    for (int i = 0; i < mesh->indexes_count; i++) {
        prebake[i].textCoord = mesh->textCoord[mesh->tex_indices[i]];
    }
}

void prebakePositions(Prebake *prebake, Mesh *mesh, Mat4 *transform) {
    for (int i = 0; i < mesh->indexes_count; i++) {
        prebake[i].position = mat4MultiplyVec3(&prebake[i].position, transform);
    }
}

void prebakeNormals(Prebake *prebake, Mesh *mesh) {
    for (int i = 0; i < mesh->indexes_count; i += 3) {
        Vec3f *v0 = &mesh->positions[mesh->pos_indices[i]];
        Vec3f *v1 = &mesh->positions[mesh->pos_indices[i + 1]];
        Vec3f *v2 = &mesh->positions[mesh->pos_indices[i + 2]];

        Vec3f edge1 = vec3fsubV(*v1, *v0);
        Vec3f edge2 = vec3fsubV(*v2, *v0);
        Vec3f normal = vec3Cross(edge1, edge2);
        normal = vec3Normalize(normal);

        prebake[mesh->pos_indices[i]].normal = normal;
        prebake[mesh->pos_indices[i + 1]].normal = normal;
        prebake[mesh->pos_indices[i + 2]].normal = normal;
    }
}

void prebakeLight(Prebake *prebake, Mesh *mesh) {
    Vec3f light = vec3Normalize(DIFFUSE_LIGHT);
    for (int i = 0; i < mesh->indexes_count; i++) {
        Vec3f normal = prebake[i].normal;
        float intensity = (1.0 + vec3Dot(normal, light)) * 0.5;
        intensity = MIN(1.0, MAX(intensity, 0));
        prebake[i].light = intensity;
    }
}

void prebakeTexCoords(Prebake *prebake, Mesh *mesh, Vec2f *textCoord) {
    // Your implementation here, if needed
}
