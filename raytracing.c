#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#define WIDTH 1200
#define HEIGHT 800
#define COLOR_WHITE 0xFFFFFF
#define COLOR_BLACK 0x000000
#define COLOR_GRAY 0xefefef
#define COLOR_RAY 0xffff66
#define RAYS_NUMBER 500
#define RAY_THICKNESS 1
#define OBSTACLE_SPEED 5

typedef struct Circle {
  double x;
  double y;
  double r;
} Circle;

typedef struct Ray {
  double x_start, y_start;
  double angle;
  double x_end, y_end;
} Ray;

void FillCircle(SDL_Surface* surface, Circle circle, Uint32 color) {
  double radius_squared = pow(circle.r, 2);
  int i = 0;
  for (double x = circle.x - circle.r; x <= circle.x + circle.r; x++) {
    for (double y = circle.y - circle.r; y <= circle.y + circle.r; y++) {
      double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);
      if (distance_squared < radius_squared) {
        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
        SDL_FillRect(surface, &pixel, color);
      }
    }
  }
}

void FillRays(SDL_Surface* surface, Ray rays[RAYS_NUMBER], Uint32 color, Circle object) {
  double radius_squared = pow(object.r, 2);
  for (int i = 0; i < RAYS_NUMBER; i++) {
    Ray ray = rays[i];
    int end_of_screen = 0;
    int object_hit = 0;

    double step = 1;
    double x_draw = ray.x_start;
    double y_draw = ray.y_start;

    while (!end_of_screen && !object_hit) {
      x_draw += step * cos(ray.angle);
      y_draw += step * sin(ray.angle);
      SDL_Rect ray_point = (SDL_Rect){x_draw, y_draw, RAY_THICKNESS, RAY_THICKNESS};
      SDL_FillRect(surface, &ray_point, COLOR_RAY);
      if (x_draw < 0 || x_draw > WIDTH) end_of_screen = 1;
      if (y_draw < 0 || y_draw > HEIGHT) end_of_screen = 1;

      double distance_squared = pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);
      if (distance_squared < radius_squared) {
        break;
      }
    }
  }
}

void generate_rays(Circle circle, Ray rays[RAYS_NUMBER]) {
  for (int i = 0; i < RAYS_NUMBER; i++) {
    double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
    Ray ray = {circle.x, circle.y, angle, 0, 0};
    rays[i] = ray;
  }
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

  SDL_Surface* surface = SDL_GetWindowSurface(window);

  Circle circle = {200, 200, 40};
  Circle shadow_circle = {500, 300, 140};
  SDL_Rect erase_rect = {0, 0, WIDTH, HEIGHT};

  Ray rays[RAYS_NUMBER];
  generate_rays(circle, rays);

  double obstacle_speed_y = OBSTACLE_SPEED;

  int simulation_running = 1;
  SDL_Event event;

  while (simulation_running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        simulation_running = 0;
      }
      if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
        circle.x = event.motion.x;
        circle.y = event.motion.y;
        generate_rays(circle, rays);
      }
    }
    SDL_FillRect(surface, NULL, COLOR_BLACK);
    FillRays(surface, rays, COLOR_RAY, shadow_circle);
    FillCircle(surface, circle, COLOR_RAY);

    FillCircle(surface, shadow_circle, COLOR_WHITE);

    shadow_circle.y += obstacle_speed_y;
    if ((shadow_circle.y - shadow_circle.r < 0) || (shadow_circle.y + shadow_circle.r > HEIGHT)) {
      obstacle_speed_y = -obstacle_speed_y;
    }
    SDL_UpdateWindowSurface(window);

    SDL_Delay(10);
  }
}
