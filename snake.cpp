#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <vector>

using namespace std;

// DEFINITIONS
#define goup "\033[A"
#define FPS 3
#define DEFAULT_VIEWPORT_SIZE 10

struct Location {
    int x, y;
};

struct GameObject {
    char c;
    Location location;
};

struct Food : GameObject {
};

struct Snake {
    char c;
    vector<Location> locations;
};

// GAME LOGIC
int WIDTH;
int HEIGHT;
Location direction = {0, 1};
Location nextDirection = {0, 1};
Food food;
Snake snake;
bool isGameOver = false;
bool isFirstFrame = true;
char** matrix;

void initMatrix (int width, int height) {
    matrix = new char*[width];
    for(int i = 0; i < width; i ++)
        matrix[i] = new char[height];
}

void respawnFood () {
    bool intersectsSnake = true;

    while(intersectsSnake)
    {
        if (snake.locations.size() == WIDTH * HEIGHT)
        {
            isGameOver = true;
            break;
        }
        intersectsSnake = false;
        food.location.x = rand() % WIDTH;
        food.location.y = rand() % HEIGHT;
        vector<Location> tail = snake.locations;
        for (auto i = tail.begin(); i != tail.end(); i++)
            if ((*i).x == food.location.x && (*i).y == food.location.y)
                intersectsSnake = true;
    }
}

void initGameObjects () {
    food.c = '*';
    snake.c = 'O';
    Location snakeLocation = {rand() % WIDTH, rand() % HEIGHT};
    snakeLocation = {rand() % WIDTH, rand() % HEIGHT};
    snake.locations.push_back(snakeLocation);
    respawnFood();
}

char checkInput () {
    char cr;
    if (_kbhit()) {
        cr = getch();
        cr = tolower(cr);
        switch (cr) {
            case 'w':
                if(!direction.y)
                {
                    nextDirection.x = 0;
                    nextDirection.y = -1;
                }
            break;
            case 'a':
                if(!direction.x)
                {
                    nextDirection.x = -1;
                    nextDirection.y = 0;
                }
            break;
            case 's':
                if(!direction.y)
                {
                    nextDirection.x = 0;
                    nextDirection.y = 1;
                }
            break;
            case 'd':
                if(!direction.x)
                {
                    nextDirection.x = 1;
                    nextDirection.y = 0;
                }
            break;
        }
    }
    return cr;
}

void update (long dt) {
    direction.x = nextDirection.x;
    direction.y = nextDirection.y;
    // Move da snek
    Location last;
    
    last.x = snake.locations.back().x;
    last.y = snake.locations.back().y;
    
    Location next;
    next.x = last.x;
    next.y = last.y;
    next.x += direction.x;
    next.y += direction.y;
    if (next.x < 0) next.x += WIDTH;
    if (next.y < 0) next.y += HEIGHT;
    next.x %= WIDTH;
    next.y %= HEIGHT;

    // Check for head intersection with every snake body part
    for (auto i = snake.locations.begin(); i != snake.locations.end(); i++)
        if (next.x == (*i).x && next.y == (*i).y)
            isGameOver = true;

    snake.locations.push_back(next);
    
    // Check for head intersection with the food
    if (food.location.x != next.x || food.location.y != next.y)
        snake.locations.erase(snake.locations.begin());
    else
        respawnFood();
}

void render () {
    
    string instructions = "[Use English Keyboard Layout] Use WASD to control snake. Q to exit";
    // CLear the matrix;
    for (int j = 0; j < HEIGHT; j ++)
        for (int i = 0; i < WIDTH; i ++)
            matrix[i][j] = ' ';
    
    // Render the objects
    // food
    matrix[food.location.x][food.location.y] = food.c;
    // snake
    for (auto i = snake.locations.begin(); i != snake.locations.end(); i++)
        matrix[(*i).x][(*i).y] = snake.c;
    matrix[snake.locations.back().x][snake.locations.back().y] = '@';
    if(!isFirstFrame)
    {
        for (int i = 0; i < HEIGHT+2; i ++)
            cout << goup;
        cout << '\r';
    }
    
    // Render the matrix to console
    for (int i = 0; i < (WIDTH*2+3); i ++)
        cout << "-";
    cout << endl;
    for (int j = 0; j < HEIGHT; j ++)
    {
        cout << "| ";
        for (int i = 0; i < WIDTH; i ++)
            cout << matrix[i][j] << ' ';
        cout << "|" << endl;
    }
    for (int i = 0; i < (WIDTH*2+3); i ++)
        cout << "-";
    cout << endl;

    isFirstFrame = false;
}


int main (int argc, char** argv) {
    cout << "\e[?25l";
    if(argc > 3 || argc == 2) {
        cerr << "Invalid argument count. Expected arguments width and height";
        return 1;
    }
    

    int width, height; 
    if (argc > 1) {
        width = stoi(argv[1]);
        height = stoi(argv[2]);
    } else {
        width = height = DEFAULT_VIEWPORT_SIZE;
    }
    WIDTH = width;
    HEIGHT = height;

    float uspf = 1000000 / FPS; // microseconds per frame

    initMatrix(width, height);
    initGameObjects();
 
    long totalTimeMicroSeconds = 0;
    timeval lastTick, currentTick;
    gettimeofday(&lastTick, 0);

    while(true)
    {
        
        gettimeofday(&currentTick, 0);
        if((checkInput() == 'q') || isGameOver) break;
        
        totalTimeMicroSeconds += 1000000 * (currentTick.tv_sec - lastTick.tv_sec) + currentTick.tv_usec - lastTick.tv_usec;
        
        int framesToEval = floor(totalTimeMicroSeconds / uspf);
        for (int i = 0; i < framesToEval; i ++)
        {
            totalTimeMicroSeconds -= uspf;
            update(uspf);
        }

        if (framesToEval) render();

        lastTick.tv_sec = currentTick.tv_sec;
        lastTick.tv_usec = currentTick.tv_usec;
    }
}
