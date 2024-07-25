
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
// #include <stdlib.h>

#include "render/material.h"
#include "render/mesh.h"
#include "render/object.h"
#include "render/pixel.h"
#include "render/renderer.h"
#include "render/scene.h"

#include "linux_window_backend.h"

#include "assets/viking.h"

float PI = 3.14159265358979323846;

float radians(float degrees) {
    return degrees * PI / 180;
}

Pixel * loadTexture(char * filename, Vec2i size) {
    Pixel * image = malloc(size.x*size.y*4);
    FILE * file   = fopen(filename, "rb");
    if (file == 0) {
        printf("Error: Could not open file %s\n", filename);
        exit(-1);
    }
    for (int i = 1023; i > 0; i--) {
    for (int j = 0; j < 1024; j++) {
            char r, g, b, a;
            fread(&r, 1, 1, file);
            fread(&g, 1, 1, file);
            fread(&b, 1, 1, file);
            fread(&a, 1, 1, file);
            image[i*1024 + j] = pixelFromRGBA(r, g, b, a);
        }
    }
    fclose(file);
    return image;
}

// // TODO: find out why this segfaults
// Object initRenderableObject() {
//     Object object;
//     object.mesh = &viking_mesh;

//     Pixel * image = loadTexture("assets/viking.rgba", (Vec2i){1024,1024});
// 	Image tex;
// 	texture_init(&tex, (Vec2i){1024, 1024},image);
// 	Material m;
// 	m.texture = &tex;
// 	object.material = &m;

//     return object;
// }

// Function to get the current time in milliseconds
long long timeInMilliseconds(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (((long long)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}

// Your existing main function
int main() {
    Vec2i size = {640, 480};

    LinuxWindowBackend backend;
    linuxWindowBackendInit(&backend, size);

    Renderer renderer;
    rendererInit(&renderer, size, (Backend*)&backend);
    rendererSetCamera(&renderer, (Vec4i){0, 0, size.x, size.y});

    Scene s;
    sceneInit(&s);
    rendererSetScene(&renderer, &s);

    Object object;
    object.mesh = &viking_mesh;
    calculateNormals(object.mesh);
    Pixel *image = loadTexture("assets/viking.rgba", (Vec2i){1024, 1024});
    Texture tex;
    texture_init(&tex, (Vec2i){1024, 1024}, image);
    Material m;
    m.texture = &tex;
    object.material = &m;
    sceneAddRenderable(&s, objectAsRenderable(&object));

    Object object1;
    object1.mesh = &viking_mesh;
    calculateNormals(object1.mesh);
    object1.material = &m;
    sceneAddRenderable(&s, objectAsRenderable(&object1));

    float phi = 0;
    float degrees_per_frame = 1.0;
    Mat4 t;

    // PROJECTION MATRIX - Defines the type of projection used
    renderer.camera_projection = mat4Perspective(1, 2500.0, (float)size.x / (float)size.y, 0.6);

    // VIEW MATRIX - Defines position and orientation of the "camera"
    Mat4 v = mat4Translate((Vec3f){0, 2, -70});
    Mat4 rotateDown = mat4RotateX(radians(-22.9183118052329283507)); // Rotate around origin/orbit
    renderer.camera_view = mat4MultiplyM(&rotateDown, &v);

    // OBJECT TRANSFORM - Defines position and orientation of the object
    object.transform = mat4RotateZ(3.142128);
    t = mat4RotateZ(0);
    object.transform = mat4MultiplyM(&object.transform, &t);

    object1.transform = mat4Translate((Vec3f){20, 0, 0});
    t = mat4Scale((Vec3f){0.5, 0.5, 0.5});
    object1.transform = mat4MultiplyM(&t, &object1.transform);

    long long start_time = timeInMilliseconds();
    int frame_count = 0;

    while (1) {
        // SCENE
        s.transform = mat4RotateY(radians(phi));
        phi += degrees_per_frame;
        frame_count++;

        if (phi >= 360.0) {
            phi = fmod(phi, 360.0);

            long long end_time = timeInMilliseconds();
            long long elapsed = end_time - start_time;
            double millis_per_frame = (double)elapsed / frame_count;
            double frames_per_second = 1000.0 / millis_per_frame;

            printf("%d frames in %lld ms = %.2f ms / frame = %.2f fps\n", frame_count, elapsed, millis_per_frame, frames_per_second);

            // Restart the timer and frame count
            start_time = timeInMilliseconds();
            frame_count = 0;
        }

        rendererRender(&renderer);
        // usleep(40000);
    }
}
