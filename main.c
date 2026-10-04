#include <stdio.h>
#include "stl_loader.h"
#include "ray_triangle.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define WIDTH 30
#define HEIGHT 40
#define DELAY 50000

char chars[] = {'.', ',', '\'', '`', '-', '^', ':', ';', '~', '+', '=', '*',
                'i', '|', 'i', 'l', 'x', 'c', 'o', 'O', '0',
                '8', '&', '%', '#', '@'};

Box box;

static void restore_terminal(void) {
    printf("\033[?25h\033[?1049l");
    fflush(stdout);
}

static void handle_sigint(int sig) {
    (void)sig;
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s MODEL_NAME.stl\n", argv[0]);
        return 1;
    }

    TriangleArray mesh = stl_load(argv[1]);

    if (!mesh.triangles) {
        return 1;
    }

    normalize_vertices(&mesh, &box);

    TriangleArray mesh_to_rotate = stl_load(argv[1]);
    normalize_vertices(&mesh_to_rotate, &box);

    float angle = 0.f;

    atexit(restore_terminal);
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);

    printf("\033[?1049h\033[2J\033[?25l");
    fflush(stdout);

    char buffer[HEIGHT * (WIDTH + 1) + 1];

    while(true){
        int buf_idx = 0;

        for(int z = 0; z < HEIGHT; z++){
            for(int y = 0; y < WIDTH; y++){
                if(isInside((float)(z - HEIGHT / 2), (float)(y - WIDTH / 2), &box)){
                    float lightLevel = getLightLevel((float)(z - HEIGHT / 2), (float)(y - WIDTH / 2), &mesh_to_rotate);
                    if(lightLevel == -1.0f){
                        buffer[buf_idx++] = ' ';
                        continue;
                    }

                    buffer[buf_idx++] = chars[(int)lightLevel];
                } else {
                    buffer[buf_idx++] = ' ';
                }
            }
            buffer[buf_idx++] = '\n';
        }
        buffer[buf_idx] = '\0';

        printf("\033[H%s", buffer);
        fflush(stdout);

        rotate_z(&mesh, &mesh_to_rotate, angle);
        update_bounding_box(&mesh_to_rotate, &box);
        angle += 0.05f;
        if(angle >= 2 * 3.14159f)
            angle -= 2 * 3.14159f;
        usleep(DELAY);
    }

    stl_free(&mesh);
    stl_free(&mesh_to_rotate);

    return 0;
}
