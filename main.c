#include <math.h>
#include <ncurses.h>
#include <unistd.h>

// some constants
const float PIOVER180 = 0.01745329251;
const int STARTING_VELOCITY_X = 1;
const int STARTING_VELOCITY_Y = 1;

// ball type
typedef struct {
  float x;
  float y;
  float velocityX;
  float velocityY;
} Ball;

// draw the ball at a given location
void drawBall(Ball ball) {
  mvprintw(ball.y + 1, ball.x - 1, "*");
  mvprintw(ball.y + 1, ball.x, "*");
  mvprintw(ball.y + 1, ball.x + 1, "*");
  mvprintw(ball.y, ball.x - 1, "*");
  mvprintw(ball.y, ball.x, "*");
  mvprintw(ball.y, ball.x + 1, "*");
  mvprintw(ball.y - 1, ball.x - 1, "*");
  mvprintw(ball.y - 1, ball.x, "*");
  mvprintw(ball.y - 1, ball.x + 1, "*");
}

// simple conversions
float toRadians(float degrees) { return degrees * PIOVER180; }
float toDegrees(float radians) { return radians / PIOVER180; }

// calulate angle based on x and y velocity
float calculateAngle(Ball ball) {
  return atan2(ball.velocityY, ball.velocityX);
}

// add direction using sine and cosine to x and y
float calculateX(Ball *ball) { return ball->x * cos(calculateAngle(*ball)); }
float calculateY(Ball *ball) { return ball->y * sin(calculateAngle(*ball)); }

// update the ball
void updateBall(Ball *ball, int height, int width) {

  ball->x += ball->velocityX;
  ball->y += ball->velocityY;

  // run bounds checks
  /*
  if (ball->y == height - 2 || ball->y == 2) {
    ball->velocityY *= -1;
  }
  */
  if (ball->x > width - 2 || ball->x < 1) {
    ball->velocityX *= -1;
  }
  if (ball->y > height - 2 || ball->y < 1) {
    ball->velocityY *= -1;
  }
  if (ball->y < -10) {
    ball->velocityY *= 0.5;
    ball->y = 2;
  }

  // gravity
  if (ball->y + 0.4 < height - 2 || ball->y + 0.4 < 2 && ball->velocityY < 2) {
    ball->velocityY += 0.25f;
  }

  // y axis damping
  if (ball->velocityY > 0) {
    ball->velocityY *= 0.95;
  }

  /* x axis damping
  if (ball->velocityX < 0 || ball->velocityX > 0) {
    ball->velocityX -= 0.002;
  }
  */
}

int main(int argc, char *argv[]) {
  initscr();
  curs_set(0);
  noecho();
  cbreak();
  nodelay(stdscr, TRUE);
  // width and height constants
  const int HEIGHT = getmaxy(stdscr);
  const int WIDTH = getmaxx(stdscr);

  // ball initialization
  Ball ball;
  ball.x = 10;
  ball.y = 10;
  ball.velocityX = STARTING_VELOCITY_X;
  ball.velocityY = STARTING_VELOCITY_Y;

  // main loop
  while (1) {
    clear();
    updateBall(&ball, HEIGHT, WIDTH);
    drawBall(ball);
    refresh();
    usleep(50000);
    int ch = getch();
    if (ch == 'q') {
      break; // quit program
    }
    if (ch == ' ') {
      if (ball.y > HEIGHT - 2 || ball.y < 2) {
        ball.velocityY *= -1;
      }
      if (ball.y > 2) {
        ball.velocityY -= 2;
      }
    }
  }
  endwin();
}
