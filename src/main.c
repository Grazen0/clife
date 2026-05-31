#include "life.h"
#include "naive_life.h"
#include "util.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

static constexpr int WIN_WIDTH = 960;
static constexpr int WIN_HEIGHT = 540;

static constexpr float CELL_SIZE = 30.0F;
static constexpr float LINE_THICK = 2.0F;

static constexpr float ZOOM_SPEED = 0.2F;
static constexpr float MIN_ZOOM = 0.1F;
static constexpr float MAX_ZOOM = 10.0F;

static void process_camera_zoom(Camera2D *cam)
{
    // https://www.raylib.com/examples/core/loader.html?name=core_2d_camera_mouse_zoom
    float wheel = GetMouseWheelMove();
    if (wheel == 0.0F)
        return;

    Vector2 mouse = GetMousePosition();
    Vector2 mouse_world_pos = GetScreenToWorld2D(mouse, *cam);

    cam->offset = mouse;
    cam->target = mouse_world_pos;

    float scale = ZOOM_SPEED * wheel;
    cam->zoom = Clamp(expf(logf(cam->zoom) + scale), MIN_ZOOM, MAX_ZOOM);
}

static void process_camera_drag(Camera2D *cam)
{
    // https://www.raylib.com/examples/core/loader.html?name=core_2d_camera_mouse_zoom
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0F / cam->zoom);
        cam->target = Vector2Add(cam->target, delta);
    }
}

typedef struct {
    i64 i;
    i64 j;

} Coord;

static Coord vec2_to_coord(Vector2 vec)
{
    return (Coord){
        .i = (i64)floorf(vec.y / CELL_SIZE),
        .j = (i64)floorf(vec.x / CELL_SIZE),
    };
}

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "CLife");
    SetTargetFPS(60);

    Camera2D cam = {
        .target = Vector2Zero(),
        .offset = {WIN_WIDTH / 2.0F, WIN_HEIGHT / 2.0F},
        .rotation = 0.0F,
        .zoom = 1.0F,
    };

    auto life = (Life *)naive_life_create();

    while (!WindowShouldClose()) {
        process_camera_zoom(&cam);
        process_camera_drag(&cam);

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), cam);
            i64 i = (i64)floorf(mouse.y / CELL_SIZE);
            i64 j = (i64)floorf(mouse.x / CELL_SIZE);

            bool cur_val = life_get(life, i, j);
            life_set(life, i, j, !cur_val);
        }

        Vector2 tlv = GetScreenToWorld2D(Vector2Zero(), cam);
        Vector2 brv = GetScreenToWorld2D((Vector2){WIN_WIDTH, WIN_HEIGHT}, cam);
        Coord tl = vec2_to_coord(tlv);
        Coord br = vec2_to_coord(brv);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(cam);

        // Draw horizontal lines
        for (i64 i = tl.i - 1; i <= br.i + 1; ++i) {
            Vector2 from = {tlv.x, CELL_SIZE * i};
            Vector2 to = {brv.x, CELL_SIZE * i};
            DrawLineEx(from, to, LINE_THICK, BLACK);
        }

        // Draw vertical lines
        for (i64 j = tl.j - 1; j <= br.j + 1; ++j) {
            Vector2 from = {CELL_SIZE * j, tlv.y};
            Vector2 to = {CELL_SIZE * j, brv.y};
            DrawLineEx(from, to, LINE_THICK, BLACK);
        }

        for (i64 i = tl.i - 1; i <= br.i + 1; ++i) {
            for (i64 j = tl.j - 1; j <= br.j + 1; ++j) {

                if (life_get(life, i, j)) {
                    Vector2 pos = {CELL_SIZE * j, CELL_SIZE * i};
                    Vector2 size = {CELL_SIZE, CELL_SIZE};
                    DrawRectangleV(pos, size, BLACK);
                }
            }
        }

        EndMode2D();
        EndDrawing();
    }

    life_destroy(life);
    life = nullptr;

    return 0;
}
