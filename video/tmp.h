#include "pingo/math/mat4.h"
#include "pingo/math/vec3.h"
#include "pingo/math/vec4.h"
#include "pingo/render/renderer.h"
#include <stdio.h>
#include <math.h>

Renderer renderer;

float pi = 3.14159265359;

float phi = 0;
float theta = 0;
float gamma = 0;
Vec3f object_position = {0, 8, 60}; // Initial object position
Vec3f object_angle = {0, 0, 0}; // Initial object rotation angles
float move_speed = 1.0f;
float rotation_speed = 1.0f * pi / 180.0f;
Vec3f direction = {0, 0, 0};
Mat4 object_transform = mat4Identity(); // Initialize the object transformation matrix

Mat4 object_projection = mat4Perspective(3, 50.0, (float)320 / (float)240, 0.1);

Mat4 object_rotation = mat4Identity(); // Initialize the object rotation matrix

void updateObject() {
    // Apply rotations to the object rotation matrix
    Mat4 rotateX = mat4RotateX(theta);
    Mat4 rotateY = mat4RotateY(phi);
    Mat4 rotateZ = mat4RotateZ(gamma);

    Mat4 new_rotation;
    new_rotation = mat4MultiplyM(&rotateY, &rotateX);
    new_rotation = mat4MultiplyM(&rotateZ, &new_rotation);

    object_rotation = mat4MultiplyM(&new_rotation, &object_rotation); // Combine new rotation with the current object rotation

    direction = mat4MultiplyVec3(&direction, &object_rotation); // Transform direction by the object's rotation matrix
    object_position = vec3fsumV(object_position, direction);

    Mat4 translation = mat4Translate(object_position);
    object_transform = mat4MultiplyM(&object_rotation, &translation); // Combine rotation and translation for the view matrix

    // Reset rotation angles for next frame
    theta = 0;
    phi = 0;
    gamma = 0;
}