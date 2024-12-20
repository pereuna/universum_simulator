#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 1000
#define NUM_BALLS 24 // Lisää palloja suorituskyvyn mittaamiseksi
#define BALL_RADIUS 5
#define NUM_TRIANGLES 24
#define NUM_THREADS 24

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float x, y;
    float vx, vy;
    float radius;
    float r, g, b;
    float vertices[NUM_TRIANGLES + 2][2];
} Ball;

Ball balls[NUM_BALLS];
pthread_t threads[NUM_THREADS];
pthread_mutex_t ball_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int start_idx;
    int end_idx;
} ThreadData;

typedef struct {
    pthread_t *threads;
    int num_threads;
    pthread_mutex_t queue_mutex;
    pthread_cond_t queue_cond;
    void (*task)(void *);
    void *task_arg;
    int stop;
} ThreadPool;

void handle_signal(int signal) {
    exit(0);
}
void init_balls() {
    srand(time(NULL));
    for (int i = 0; i < NUM_BALLS; i++) {
        balls[i].x = rand() % (WINDOW_WIDTH - 2 * BALL_RADIUS) + BALL_RADIUS;
        balls[i].y = rand() % (WINDOW_HEIGHT - 2 * BALL_RADIUS) + BALL_RADIUS;
        balls[i].vx = ((float)rand() / RAND_MAX) * 2 - 1;
        balls[i].vy = ((float)rand() / RAND_MAX) * 2 - 1;
        balls[i].radius = BALL_RADIUS;
        balls[i].r = (float)rand() / RAND_MAX;
        balls[i].g = (float)rand() / RAND_MAX;
        balls[i].b = (float)rand() / RAND_MAX;

        balls[i].vertices[0][0] = 0.0f;
        balls[i].vertices[0][1] = 0.0f;
        for (int j = 0; j <= NUM_TRIANGLES; j++) {
            float angle = j * (360.0f / NUM_TRIANGLES) * M_PI / 180.0f;
            balls[i].vertices[j + 1][0] = cos(angle) * balls[i].radius;
            balls[i].vertices[j + 1][1] = sin(angle) * balls[i].radius;
        }
    }
}

void draw_ball(Ball *ball) {
    glColor3f(ball->r, ball->g, ball->b);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= NUM_TRIANGLES + 1; i++) {
        glVertex2f(ball->x + ball->vertices[i][0], ball->y + ball->vertices[i][1]);
    }
    glEnd();
}

void move_ball(Ball *ball) {
    ball->x += ball->vx;
    ball->y += ball->vy;

    if (ball->x - ball->radius < 0 && ball->vx < 0) ball->vx *= -1;
    if (ball->x + ball->radius > WINDOW_WIDTH && ball->vx > 0) ball->vx *= -1;
    if (ball->y - ball->radius < 0 && ball->vy < 0) ball->vy *= -1;
    if (ball->y + ball->radius > WINDOW_HEIGHT && ball->vy > 0) ball->vy *= -1;
}

void resolve_collision(Ball *b1, Ball *b2) {
    float dx = b1->x - b2->x;
    float dy = b1->y - b2->y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance < b1->radius + b2->radius) {
        float collision_angle = atan2(dy, dx);

        float speed1 = sqrt(b1->vx * b1->vx + b1->vy * b1->vy);
        float speed2 = sqrt(b2->vx * b2->vx + b2->vy * b2->vy);

        float direction1 = atan2(b1->vy, b1->vx);
        float direction2 = atan2(b2->vy, b2->vx);

        float new_speed1x = speed1 * cos(direction1 - collision_angle);
        float new_speed1y = speed1 * sin(direction1 - collision_angle);
        float new_speed2x = speed2 * cos(direction2 - collision_angle);
        float new_speed2y = speed2 * sin(direction2 - collision_angle);

        float final_speed1x = ((b1->radius - b2->radius) * new_speed1x + (2 * b2->radius) * new_speed2x) / (b1->radius + b2->radius);
        float final_speed2x = ((2 * b1->radius) * new_speed1x + (b2->radius - b1->radius) * new_speed2x) / (b1->radius + b2->radius);

        b1->vx = cos(collision_angle) * final_speed1x + cos(collision_angle + M_PI / 2) * new_speed1y;
        b1->vy = sin(collision_angle) * final_speed1x + sin(collision_angle + M_PI / 2) * new_speed1y;
        b2->vx = cos(collision_angle) * final_speed2x + cos(collision_angle + M_PI / 2) * new_speed2y;
        b2->vy = sin(collision_angle) * final_speed2x + sin(collision_angle + M_PI / 2) * new_speed2y;
    }
}
/*
void *update_balls(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->start_idx; i < data->end_idx; i++) {
        move_ball(&balls[i]);
    }

    for (int i = data->start_idx; i < data->end_idx; i++) {
        for (int j = i + 1; j < NUM_BALLS; j++) {
            pthread_mutex_lock(&ball_mutex);
            resolve_collision(&balls[i], &balls[j]);
            pthread_mutex_unlock(&ball_mutex);
        }
    }

    return NULL;
}
*/
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < NUM_BALLS; i++) {
        draw_ball(&balls[i]);
    }
    glutSwapBuffers();
}
/*
void update() {
    ThreadData thread_data[NUM_THREADS];
    int balls_per_thread = NUM_BALLS / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_idx = i * balls_per_thread;
        thread_data[i].end_idx = (i == NUM_THREADS - 1) ? NUM_BALLS : (i + 1) * balls_per_thread;
        pthread_create(&threads[i], NULL, update_balls, &thread_data[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    glutPostRedisplay();
}
*/
void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
}

//here start paste
void *thread_pool_worker(void *arg) {
    ThreadPool *pool = (ThreadPool *)arg;
    while (1) {
        pthread_mutex_lock(&pool->queue_mutex);
        while (pool->task == NULL && !pool->stop) {
            pthread_cond_wait(&pool->queue_cond, &pool->queue_mutex);
        }
        if (pool->stop) {
            pthread_mutex_unlock(&pool->queue_mutex);
            break;
        }
        void (*task)(void *) = pool->task;
        void *task_arg = pool->task_arg;
        pool->task = NULL;
        pthread_mutex_unlock(&pool->queue_mutex);
        task(task_arg);
    }
    return NULL;
}

void thread_pool_init(ThreadPool *pool, int num_threads) {
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pthread_mutex_init(&pool->queue_mutex, NULL);
    pthread_cond_init(&pool->queue_cond, NULL);
    pool->task = NULL;
    pool->stop = 0;
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool);
    }
}

void thread_pool_submit(ThreadPool *pool, void (*task)(void *), void *arg) {
    pthread_mutex_lock(&pool->queue_mutex);
    pool->task = task;
    pool->task_arg = arg;
    pthread_cond_signal(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_mutex);
}
void thread_pool_shutdown(ThreadPool *pool) {
    pthread_mutex_lock(&pool->queue_mutex);
    pool->stop = 1;
    pthread_cond_broadcast(&pool->queue_cond);
    pthread_mutex_unlock(&pool->queue_mutex);
    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);
    pthread_mutex_destroy(&pool->queue_mutex);
    pthread_cond_destroy(&pool->queue_cond);
}

void *update_balls(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->start_idx; i < data->end_idx; i++) {
        move_ball(&balls[i]);
    }
    for (int i = data->start_idx; i < data->end_idx; i++) {
        for (int j = i + 1; j < NUM_BALLS; j++) {
            pthread_mutex_lock(&ball_mutex);
            resolve_collision(&balls[i], &balls[j]);
            pthread_mutex_unlock(&ball_mutex);
        }
    }
    return NULL;
}
void update() {
    ThreadPool pool;
    thread_pool_init(&pool, NUM_THREADS);
    ThreadData thread_data[NUM_THREADS];
    int balls_per_thread = NUM_BALLS / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_idx = i * balls_per_thread;
        thread_data[i].end_idx = (i == NUM_THREADS - 1) ? NUM_BALLS : (i + 1) * balls_per_thread;
        thread_pool_submit(&pool, update_balls, &thread_data[i]);
    }

    thread_pool_shutdown(&pool);
    glutPostRedisplay();
}

int main(int argc, char **argv) {
    // Register signal handler
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Multithreaded Ball Collision Simulation");
    init();
    init_balls();
    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutMainLoop();
    return 0;
}

