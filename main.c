#include <math.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// written by mc_modder
/*
 * if anyone actually sees this, i would
 * like some tips on memory allocation
 * and that sort of stuff
 */

// added custom x and y velocity parameters
// removed unnecessary conversion functions
// added cool π symbol (only here though)

// no longer constants as they can
// be changable by command line
int push_x = 2;
int push_y = 1;
float gravity = 0.25f;
float yaxisdamp = 0.95;
float xaxisdamp = 0.95;

// ball type
typedef struct {
  float x;
  float y;
  float velocityX;
  float velocityY;
} Ball;

// draws the ball at a given location
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

// calculate angle based on x and y velocity
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
  if (ball->x < -2) {
    ball->velocityX *= 0.5;
    ball->x = 2;
  }
  if (ball->x > width + 2) {
    ball->velocityX *= 0.5;
    ball->x = width - 3;
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
    ball->velocityY += gravity;
  }

  // y axis damping
  if (ball->velocityY > 0) {
    ball->velocityY *= yaxisdamp;
  }

  // x axis damping
  if (ball->velocityX < 0 || ball->velocityX > 0) {
    ball->velocityX *= xaxisdamp;
  }
}

void printHelp() {
  printf("Usage: \n");
  printf("    -nyd: --no-y-damping: removes y damping\n");
  printf("    -nxd: --no-x-damping: removes x damping\n");
  printf("    -ng: --no-gravity: removes gravity\n");
  printf("    -h: --help: shows this message\n");
  printf("    -yd <num>: --y-damping <num>: sets custom y axis damping "
         "amount\n");
  printf("    -xd <num>: --x-damping <num: sets custom x axis damping "
         "amount\n");
  printf("    -g <num>: --gravity <num>: sets custom gravity force\n");
  printf("    -px <num>: --push-x <num>: sets custom x velocity\n");
  printf("    -py <num>: --push-y <num>: sets custom y velocity\n");
  printf("  * note: it is recommended to use -ng and -nyd together\n");
}

int main(int argc, char *argv[]) {
  // parse arguments
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-nyd") == 0 ||
        strcmp(argv[i], "--no-y-damping") == 0) {
      yaxisdamp = 1;
    } else if (strcmp(argv[i], "-nxd") == 0 ||
               strcmp(argv[i], "--no-x-damping") == 0) {
      xaxisdamp = 1.0f;
    } else if (strcmp(argv[i], "-ng") == 0 ||
               strcmp(argv[i], "--no-gravity") == 0) {
      gravity = 0;
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      printHelp();
      exit(0);
    } else if (strcmp(argv[i], "-yd") == 0 ||
               strcmp(argv[i], "--y-damping") == 0) {
      yaxisdamp = strtol(argv[i + 1], NULL, 10);
    } else if (strcmp(argv[i], "-g") == 0 ||
               strcmp(argv[i], "--gravity") == 0) {
      gravity = strtol(argv[i + 1], NULL, 10);
    } else if (strcmp(argv[i], "-xd") == 0 ||
               strcmp(argv[i], "--x-damping") == 0) {
      xaxisdamp = strtol(argv[i + 1], NULL, 10);
    } else if (strcmp(argv[i], "-px") == 0 ||
               strcmp(argv[i], "--push-x") == 0) {
      push_x = strtol(argv[i + 1], NULL, 10);
    } else if (strcmp(argv[i], "-py") == 0 ||
               strcmp(argv[i], "--push-y") == 0) {
      push_y = strtol(argv[i + 1], NULL, 10);
    }

    else if (argc == 1 || strcmp(argv[i], argv[0]) == 0) {
      continue;
    }
  }
  // setup ncurses
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
  ball.velocityX = push_x;
  ball.velocityY = push_y;
  // main loop
  while (1) {
    clear();
    updateBall(&ball, HEIGHT, WIDTH);
    drawBall(ball);
    refresh();
    usleep(50000);

    // handle certain keypresses
    int ch = getch();
    if (ch == 'q') {
      break; // quit program
    }
    if (ch == ' ') {
      if (ball.y > HEIGHT - 2 || ball.y < 2) {
        ball.velocityY *= -1;
      }
      if (ball.y > 2) {
        ball.velocityY -= 2; // "jump"
      }
    }
    if (ch == 'w') {
      if (ball.x > 1 && ball.x < WIDTH - 1) {
        ball.velocityX -= 2; // move left
      }
    }
    if (ch == 'r') {
      if (ball.x > 1 && ball.x < WIDTH - 1) {
        ball.velocityX += 2; // move right
      }
    }
  }
  endwin();
}
