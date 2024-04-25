#include <opencv2/opencv.hpp> // Подключение библиотеки OpenCV для обработки изображений
#include <iostream> 
#include <vector> // Подключение библиотеки контейнеров векторов
#include <cstdlib> // Подключение стандартной библиотеки для различных утилит, таких как random
#include <ctime> 
#include <ncurses.h> // Подключение библиотеки ncurses для функциональности GUI в терминале
#include <fstream> // Подключение библиотеки потоков файлов
#include <sstream> // Подключение библиотеки строковых потоков
#include <map> // Подключение библиотеки контейнеров map
#include <utility> // Подключение утилит различных функций

class GameOfLife {
private:
    std::vector<std::vector<int>> grid; // Двумерный вектор для хранения игрового поля
    int height; 
    int width; 
    std::map<std::pair<int, int>, int> rules; //  хранения правил игры

public:
    GameOfLife(int h, int w) : height(h), width(w) {
        grid.resize(height, std::vector<int>(width, 0)); // Инициализация игрового поля
    }

    void initializeFromImage(const std::string& imagePath) {
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE); // Загрузка изображения 
        if (image.empty()) {
            std::cerr << "Не удалось загрузить изображение." << std::endl;
            return;
        }

        cv::threshold(image, image, 127, 1, cv::THRESH_BINARY_INV); // Конвертация изображения в бинарный формат

        height = image.rows;
        width = image.cols;
        grid.resize(height, std::vector<int>(width, 0)); // Изменение размера игрового поля в соответствии с изображением

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                grid[i][j] = image.at<uint8_t>(i, j); // Заполнение поля по данным изображения
            }
        }
    }

    void loadRules(const std::string& fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            std::cerr << "Не удалось открыть файл с правилами: " << fileName << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line[0] == '#' || line.empty()) continue; // Пропуск комментариев 

            std::istringstream iss(line);
            std::string state;
            int neighbors, resultState;

            if (!(iss >> state >> neighbors >> resultState)) {
                std::cerr << "Неверный формат правил: " << line << std::endl;
                continue;
            }

            int cellState = (state == "alive") ? 1 : 0;
            rules[{cellState, neighbors}] = resultState; // Загрузка правил
            std::cout << "Правило загружено: " << state << " с " << neighbors << " соседями -> " << (resultState ? "Живой" : "Мертвый") << std::endl;
        }
    }

    void display() {
        clear();
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                move(i, j);
                addch(grid[i][j] ? '*' : ' '); // Отображение клеток
            }
        }
        refresh();
    }

    void update() {
        std::vector<std::vector<int>> newGrid = grid;
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                int liveNeighbors = countNeighbors(i, j);
                std::pair<int, int> ruleKey = {grid[i][j], liveNeighbors};

                if (rules.find(ruleKey) != rules.end()) {
                    newGrid[i][j] = rules[ruleKey]; // Применение правил
                } else {
                    newGrid[i][j] = grid[i][j]; // Сохранение текущего состояния, если правило не применяется
                }
            }
        }
        grid = newGrid; // Обновление поля
    }

private:
    int countNeighbors(int x, int y) {
        int count = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if ((i == 0 && j == 0) || x+i < 0 || y+j < 0 || x+i >= height || y+j >= width) {
                    continue; // Пропуск самой клетки и клеток за пределами поля
                }
                count += grid[x+i][y+j]; // Подсчет живых соседей
            }
        }
        return count;
    }
};

int main() {
    initscr(); // Инициализация ncurses
    cbreak();
    noecho();
    curs_set(0); // Скрытие курсора

    GameOfLife game(0, 0); // Инициализация с нулевыми размерами, размер будет задан на основе изображения
    game.loadRules("rules.txt"); // Загрузка правил из файла
    game.initializeFromImage("path_to_your_image.jpg"); //  путь к вашему изображению
    game.display();

    nodelay(stdscr, TRUE); // Неблокирующий ввод
    scrollok(stdscr, TRUE); // Разрешение прокрутки
    keypad(stdscr, TRUE); // Включение специальных клавиш

    bool running = true;
    while (running) {
        int ch = getch();
        if (ch == 'q' or ch == 'Q') { // Выход по 'q' или 'Q'
            running = false;
        } else {
            game.update();
            game.display();
            napms(200); // Пауза между обновлениями состояния
        }
    }

    endwin(); // Закрытие режима ncurses
    return 0;
}
