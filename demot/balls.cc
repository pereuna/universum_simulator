#include <SDL.h>
#include <cmath>
#include <vector>
#include <random>

// Window settings
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 1000;
const int FPS = 6000;
const int GRID_SIZE = 10;

// Colors
SDL_Color WHITE = {255, 255, 255, 255};
SDL_Color RED = {255, 0, 0, 255};
SDL_Color BLUE = {0, 0, 255, 255};

void drawCircle(SDL_Renderer* renderer, int32_t centerX, int32_t centerY, int32_t radius)
{
    const int32_t diameter = (radius * 2);

    int32_t x = (radius - 1);
    int32_t y = 0;
    int32_t tx = 1;
    int32_t ty = 1;
    int32_t error = (tx - diameter);

    while (x >= y)
    {
        //  Each of the following renders an octant of the circle
        SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY - y);
        SDL_RenderDrawPoint(renderer, centerX - x, centerY + y);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX + y, centerY + x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY - x);
        SDL_RenderDrawPoint(renderer, centerX - y, centerY + x);

        if (error <= 0)
        {
            ++y;
            error += ty;
            ty += 2;
        }

        if (error > 0)
        {
            --x;
            tx += 2;
            error += (tx - diameter);
        }
    }
}


class Ball {
public:
    float x, y, vx, vy;
    int radius = 10;
    SDL_Color color = {0, 0, 0, 255};
    bool hit = false;

    Ball(float x, float y, float vx, float vy) : x(x), y(y), vx(vx), vy(vy) {}

    void draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	drawCircle(renderer, x, y, radius);        // Draw the circle here...
    }

    void move() {
        x += vx;
        y += vy;

        // Bounce off walls
        if (x - radius < 0 || x + radius > WINDOW_WIDTH) {
            vx *= -1;
        }
        if (y - radius < 0 || y + radius > WINDOW_HEIGHT) {
            vy *= -1;
        }
    }
};


void resolve_collision(Ball& ball1, Ball& ball2) {
    float dx = ball1.x - ball2.x;
    float dy = ball1.y - ball2.y;
    float collision_angle = std::atan2(dy, dx);

    float speed1 = std::sqrt(ball1.vx * ball1.vx + ball1.vy * ball1.vy);
    float speed2 = std::sqrt(ball2.vx * ball2.vx + ball2.vy * ball2.vy);

    float direction1 = std::atan2(ball1.vy, ball1.vx);
    float direction2 = std::atan2(ball2.vy, ball2.vx);

    float new_speed1x = speed1 * std::cos(direction1 - collision_angle);
    float new_speed1y = speed1 * std::sin(direction1 - collision_angle);
    float new_speed2x = speed2 * std::cos(direction2 - collision_angle);
    float new_speed2y = speed2 * std::sin(direction2 - collision_angle);

    float final_speed1x = ((ball1.radius - ball2.radius) * new_speed1x + (2 * ball2.radius) * new_speed2x) / (ball1.radius + ball2.radius);
    float final_speed2x = ((2 * ball1.radius) * new_speed1x + (ball2.radius - ball1.radius) * new_speed2x) / (ball1.radius + ball2.radius);

    ball1.vx = std::cos(collision_angle) * final_speed1x + std::cos(collision_angle + M_PI/2) * new_speed1y;
    ball1.vy = std::sin(collision_angle) * final_speed1x + std::sin(collision_angle + M_PI/2) * new_speed1y;
    ball2.vx = std::cos(collision_angle) * final_speed2x + std::cos(collision_angle + M_PI/2) * new_speed2y;
    ball2.vy = std::sin(collision_angle) * final_speed2x + std::sin(collision_angle + M_PI/2) * new_speed2y;
}


bool detect_collision(Ball& ball1, Ball& ball2) {
    float dx = ball1.x - ball2.x;
    float dy = ball1.y - ball2.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    if (distance <= ball1.radius + ball2.radius) {
        if (ball1.hit || ball2.hit) {
            return false;
        } else {
            ball2.hit = ball1.hit = true;
            return true;
        }
    } else {
        ball2.hit = ball1.hit = false;
        return false;
    }
}
// The rest of your code (detect_collision and resolve_collision functions) goes here...

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncing Balls", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::vector<Ball> balls;
    for (int i = 20; i < 1000; i += 50) {
        for (int j = 20; j < 1000; j += 50) {
            float x = i;
            float y = j;
            float vx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5) - 0.5;
            float vy = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/0.5) - 0.5;
            balls.push_back(Ball(x, y, vx, vy));
        }
    }
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
        SDL_RenderClear(renderer);

        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                if (detect_collision(balls[i], balls[j])) {
                    resolve_collision(balls[i], balls[j]);
                }
            }
            //balls[i].hit = false;
            balls[i].move();
            balls[i].draw(renderer);
        }

        SDL_RenderPresent(renderer);
        //SDL_Delay(1000 / FPS);
    }
    // Your game loop goes here...

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

