#include "life.h"
#include "naive_life.h"
#include "util.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

static constexpr int WIN_WIDTH = 960;
static constexpr int WIN_HEIGHT = 540;

static constexpr float CELL_SIZE = 30.0F;
static constexpr float LINE_THICK = 3.0F;

static constexpr float ZOOM_SPEED = 0.15F;
static constexpr float MIN_ZOOM = 0.1F;
static constexpr float MAX_ZOOM = 10.0F;

static constexpr float MIN_STEP_DELAY = 0.001F;
static constexpr float MAX_STEP_DELAY = 1.0F;

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
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0F / cam->zoom);
        cam->target = Vector2Add(cam->target, delta);
    }
}

static Coord vec2_to_coord(Vector2 vec)
{
    return (Coord){
        .i = (i64)floorf(vec.y / CELL_SIZE),
        .j = (i64)floorf(vec.x / CELL_SIZE),
    };
}

int main()
{
    InitWindow(WIN_WIDTH, WIN_HEIGHT, "CLife");
    SetTargetFPS(60);

    Camera2D cam = {
        .target = Vector2Zero(),
        .offset = {WIN_WIDTH / 2.0F, WIN_HEIGHT / 2.0F},
        .rotation = 0.0F,
        .zoom = 1.0F,
    };

    auto life = (Life *)naive_life_create();

    Coord last_toggled = {};

    bool step_enabled = false;
    float step_timer = 0.0F;
    float step_delay = 0.1F;

    while (!WindowShouldClose()) {
        process_camera_zoom(&cam);
        process_camera_drag(&cam);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            last_toggled = (Coord){INT64_MAX, INT64_MAX};

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), cam);
            Coord mouse_coord = vec2_to_coord(mouse);

            if (!coord_eq(last_toggled, mouse_coord)) {
                last_toggled = mouse_coord;
                bool cur_val = life_get(life, mouse_coord.i, mouse_coord.j);
                life_set(life, mouse_coord.i, mouse_coord.j, !cur_val);
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            step_enabled = !step_enabled;
            step_timer = step_delay;
        }

        if (IsKeyPressed(KEY_DOWN))
            step_delay =
                Clamp(step_delay * 1.25F, MIN_STEP_DELAY, MAX_STEP_DELAY);

        if (IsKeyPressed(KEY_UP))
            step_delay =
                Clamp(step_delay * 0.75F, MIN_STEP_DELAY, MAX_STEP_DELAY);

        if (step_enabled) {
            step_timer += GetFrameTime();

            if (step_timer >= step_delay) {
                step_timer = 0.0F;
                life_step(life);
            }
        }

        if (IsKeyPressed(KEY_R))
            life_reset(life);

        Vector2 tlv = GetScreenToWorld2D(Vector2Zero(), cam);
        Vector2 brv = GetScreenToWorld2D((Vector2){WIN_WIDTH, WIN_HEIGHT}, cam);
        Coord tl = vec2_to_coord(tlv);
        Coord br = vec2_to_coord(brv);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode2D(cam);

        // Draw tiles
        for (i64 i = tl.i - 1; i <= br.i + 1; ++i) {
            for (i64 j = tl.j - 1; j <= br.j + 1; ++j) {
                if (life_get(life, i, j)) {
                    Vector2 pos = {CELL_SIZE * j, CELL_SIZE * i};
                    Vector2 size = {CELL_SIZE, CELL_SIZE};
                    DrawRectangleV(pos, size, BLACK);
                }
            }
        }

        // Draw horizontal lines
        for (i64 i = tl.i - 1; i <= br.i + 1; ++i) {
            Vector2 from = {tlv.x, CELL_SIZE * i};
            Vector2 to = {brv.x, CELL_SIZE * i};
            DrawLineEx(from, to, LINE_THICK, LIGHTGRAY);
        }

        // Draw vertical lines
        for (i64 j = tl.j - 1; j <= br.j + 1; ++j) {
            Vector2 from = {CELL_SIZE * j, tlv.y};
            Vector2 to = {CELL_SIZE * j, brv.y};
            DrawLineEx(from, to, LINE_THICK, LIGHTGRAY);
        }

        EndMode2D();
        EndDrawing();
    }

    life_destroy(life);
    life = nullptr;

    return 0;
}
