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
        srand(time(0)); // Инициализация генератора случайных чисел
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                grid[i][j] = rand() % 2; // Начальное заполнение случайными значениями 0 или 1
            }
        }
    }

    void display() {
        clear();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                move(i, j); // Перемещаем курсор на позицию клетки
                addch(grid[i][j] ? '*' : ' '); // Отображаем клетку
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
                if ((i == 0 && j == 0) || x+i < 0 || y+j < 0 || x+i >= height || y+j >= width) {
                    continue; // Пропускаем себя и клетки за пределами поля
                }
                count += grid[x+i][y+j];
            }
        }
        return count;
    }
};

int main() {
    initscr(); // Инициализация ncurses
    cbreak();
    noecho();
    curs_set(0); // Скрыть курсор

    int height = 20, width = 40;
    GameOfLife game(height, width);
    game.initialize();
    game.display();

    nodelay(stdscr, TRUE); // Неблокирующий ввод
    scrollok(stdscr, TRUE); // Разрешить прокрутку
    keypad(stdscr, TRUE); // Разрешить специальные клавиши

    bool running = true;
    while (running) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') { // Выход по 'q' или 'Q'
            running = false;
        } else {
            game.update();
            game.display();
        }
        napms(200); // Пауза между обновлениями состояния
    }

    endwin(); // Выход из режима ncurses
    return 0;
}
