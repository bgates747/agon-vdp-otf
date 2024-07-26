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

        // Normalize the normal
        normal = vec3Normalize(normal);

        // Assign the computed normal to all three vertices of the triangle
        mesh->normals[mesh->pos_indices[i]] = normal;
        mesh->normals[mesh->pos_indices[i + 1]] = normal;
        mesh->normals[mesh->pos_indices[i + 2]] = normal;
    }
}

