#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

class GameOfLife {
private:
    std::vector<std::vector<int>> grid;
    int height;
    int width;

public:
    GameOfLife(int h, int w) : height(h), width(w) {
        grid.resize(height, std::vector<int>(width, 0));
    }

    void initialize() {
        srand(time(0));
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                grid[i][j] = rand() % 2;
            }
        }
    }

    void display() {
        clear();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                move(i, j * 2); // Учет двух позиций для отображения
                if (grid[i][j] == 1)
                    addch('*');
                else
                    addch('.');
            }
        }
        refresh();
    }

    void update() {
        std::vector<std::vector<int>> newGrid = grid;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int liveNeighbors = countNeighbors(i, j);
                if (grid[i][j] == 1 && (liveNeighbors < 2 || liveNeighbors > 3))
                    newGrid[i][j] = 0;
                else if (grid[i][j] == 0 && liveNeighbors == 3)
                    newGrid[i][j] = 1;
            }
        }
        grid = newGrid;
    }

private:
    int countNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int nx = x + i;
                int ny = y + j;
                if (nx >= 0 && nx < height && ny >= 0 && ny < width)
                    count += grid[nx][ny];
            }
        }
        return count;
    }
};

int main() {
    initscr(); // Инициализация ncurses
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0); // Скрыть курсор

    GameOfLife game(20, 40);
    game.initialize();
    game.display();

    int ch;
    bool running = true;
    while (running) {
        ch = getch();
        switch (ch) {
            case 'q':  // Выход
                running = false;
                break;
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
                // Логика перемещения курсора или других действий
                break;
            default:
                game.update();
                game.display();
                break;
        }
    }

    endwin(); // Закрытие ncurses
    return 0;
}
