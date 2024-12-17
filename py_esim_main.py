import pygame
import math
import random
import numpy as np
import sys
import time

# Pygame specific settings
WINDOW_WIDTH = 1000
WINDOW_HEIGHT = 1000
FPS = 1000
GRID_SIZE = 10

# Colors
WHITE = (255, 255, 255)
RED = (255, 0, 0)
BLUE = (0, 0, 255)



class Ball:
    id_ = 0
    def __init__(self, x, y, vx, vy):
        self.id = Ball.id_
        Ball.id_ += 1
        self.x = x
        self.y = y
        self.vx = vx
        self.vy = vy
        self.radius = 10
        self.color = (0, 0, 0)
        self.prev_x = x
        self.prev_y = y

    def draw(self, screen):
        if self.id == 0:
            try:
                pygame.draw.circle(screen, self.color, (int(self.x), int(self.y)), self.radius)
            except TypeError:
                print("x:" + str(self.x) + "y:" + str(self.y))
                pass

    def move(self, t, screen):
        dx = self.vx * t 
        dy = self.vy * t 
        self.prev_x = self.x
        self.prev_y = self.y
        self.x += dx
        self.y += dy
        if self.id == 0:
            pygame.draw.line(screen, (0, 0, 0), (int(self.prev_x), int(self.prev_y)), (int(self.x), int(self.y)), 3)


def main():
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_WIDTH, WINDOW_HEIGHT))
    clock = pygame.time.Clock()
    screen.fill(WHITE)

    balls = []
    for i in range(30):
        x = random.uniform(100, 900)
        y = random.uniform(100, 900)
        vx = random.uniform(-0.5, 0.5)
        vy = random.uniform(-0.5, 0.5)
        ball = Ball(x, y, vx, vy)
        balls.append(ball)
        #ball.move(t_min[0], screen)
        ball.color = (0, 0, 255)
        ball.draw(screen)

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                return

        t_min = (sys.maxsize, balls[0], balls[1])

        for i in range(len(balls)):
            for j in range(i+1, len(balls)):    
                t = calculate_time_of_collision(balls[i], balls[j])
                if (t > 0) and not math.isinf(t):
                    if t < t_min[0]:
                        t_min = (t, balls[i], balls[j])
            tw = calculate_time_to_wall(balls[i])
            if tw < t_min[0]:
                t_min = (tw, balls[i], False)
         
        if t_min[0] < sys.maxsize and t_min[2]:
            t_min[1].move(t_min[0], screen)
            t_min[2].move(t_min[0], screen)
            t_min[1].color = (255, 0, 0)
            t_min[2].color = (255, 0, 0)
            t_min[1].draw(screen)
            t_min[2].draw(screen)
        else:
            t_min[1].move(t_min[0], screen)
            t_min[1].color = (0, 255, 0)
            t_min[1].draw(screen)

        if t_min[2]:
            resolve_collision(t_min[1], t_min[2])
        else:
            # Bounce off walls
            if t_min[1].x - t_min[1].radius < 0 and t_min[1].vx < 0:
                t_min[1].vx *= -1
            if t_min[1].x + t_min[1].radius > WINDOW_WIDTH and t_min[1].vx > 0:
                t_min[1].vx *= -1
            if t_min[1].y - t_min[1].radius < 0 and t_min[1].vy < 0:
                t_min[1].vy *= -1
            if t_min[1].y + t_min[1].radius > WINDOW_HEIGHT and t_min[1].vy > 0:
                t_min[1].vy *= -1

        pygame.display.flip()
        clock.tick(FPS)



def calculate_time_of_collision(ball1, ball2):
    # Calculate relative position and velocity
    dx = ball1.x - ball2.x
    dy = ball1.y - ball2.y
    dvx = ball1.vx - ball2.vx
    dvy = ball1.vy - ball2.vy

    # Calculate a, b, c for the quadratic equation
    a = dvx**2 + dvy**2
    b = 2 * (dx * dvx + dy * dvy)
    c = dx**2 + dy**2 - (ball1.radius + ball2.radius)**2

    # Calculate the discriminant
    discriminant = b**2 - 4*a*c

    # If the discriminant is negative, the balls will not collide
    if discriminant < 0 or a == 0:
        return float('inf')

    # Calculate the time of collision
    t = (-b - math.sqrt(discriminant)) / (2*a)

    return t

def calculate_time_to_wall(ball):

    if ball.vx > 0:
        dx = WINDOW_WIDTH - ball.x
        tx = dx / ball.vx 
    elif ball.vx < 0:
        dx = ball.x
        tx = dx / abs(ball.vx) 

    if ball.vy > 0:
        dy =  WINDOW_HEIGHT - ball.y
        ty = dy / ball.vy 
    elif ball.vy < 0:
        dy = ball.y
        ty = dy / abs(ball.vy) 

    if ty < tx:
        return ty
    else:
        return tx

def resolve_collision(ball1, ball2):
    dx = ball1.x - ball2.x
    dy = ball1.y - ball2.y
    collision_angle = math.atan2(dy, dx)

    speed1 = math.sqrt(ball1.vx * ball1.vx + ball1.vy * ball1.vy)
    speed2 = math.sqrt(ball2.vx * ball2.vx + ball2.vy * ball2.vy)

    direction1 = math.atan2(ball1.vy, ball1.vx)
    direction2 = math.atan2(ball2.vy, ball2.vx)

    new_speed1x = speed1 * math.cos(direction1 - collision_angle)
    new_speed1y = speed1 * math.sin(direction1 - collision_angle)
    new_speed2x = speed2 * math.cos(direction2 - collision_angle)
    new_speed2y = speed2 * math.sin(direction2 - collision_angle)

    final_speed1x = ((ball1.radius - ball2.radius) * new_speed1x + (2 * ball2.radius) * new_speed2x) / (ball1.radius + ball2.radius)
    final_speed2x = ((2 * ball1.radius) * new_speed1x + (ball2.radius - ball1.radius) * new_speed2x) / (ball1.radius + ball2.radius)

    ball1.vx = math.cos(collision_angle) * final_speed1x + math.cos(collision_angle + math.pi/2) * new_speed1y
    ball1.vy = math.sin(collision_angle) * final_speed1x + math.sin(collision_angle + math.pi/2) * new_speed1y
    ball2.vx = math.cos(collision_angle) * final_speed2x + math.cos(collision_angle + math.pi/2) * new_speed2y
    ball2.vy = math.sin(collision_angle) * final_speed2x + math.sin(collision_angle + math.pi/2) * new_speed2y

if __name__ == "__main__":
    main()
