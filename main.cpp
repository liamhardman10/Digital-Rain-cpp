#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

#define SHOW   "\033[?25h"
#define HIDE   "\033[?25l"
#define CLEAR  "\033[2J\033[H"
#define RESET  "\033[0m"
#define BRIGHT "\033[1;92m"
#define GREEN  "\033[32m"

struct CharBank {
    std::string bank = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#$%^&*(){}:'><?:`~-=+_";

    std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist{0, bank.size() - 1};
    
    char random() { return bank[dist(rng)]; }

    int range (int lo, int hi) {
        std::uniform_int_distribution<int> d(lo, hi);
        return d(rng);
    }
};

struct Column {
    int head;
    int length;
    int speed;
    int tick;
};

int main() {
    const int WIDTH = 80;
    const int HEIGHT = 30;
    
    CharBank cb;

    std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));
    std::vector<Column> cols(WIDTH);
    for (int x = 0; x < WIDTH; ++x) {
        cols[x].head = cb.range(-HEIGHT, 0);
        cols[x].length = cb.range(4, 20);
        cols[x].speed = cb.range(1, 3);
        cols[x].tick = cb.range(0, cols[x].speed - 1);
    }

    std::cout << HIDE << CLEAR;

    while (true) {
        for (int x = 0; x < WIDTH; ++x) {
            Column& c = cols[x];
            
            if (++c.tick < c.speed) continue;
            c.tick = 0;
            c.head++;
            
            if (c.head >= 0 && c.head < HEIGHT) {
                grid[c.head][x] = cb.random();
            }

            int tail = c.head - c.length;
            if (tail >= 0 && tail < HEIGHT) {
                grid[tail][x] = ' ';
            }

            if (tail >= HEIGHT) {
                c.head = cb.range(-10, 0);
                c.length = cb.range(4, 20);
                c.speed = cb.range(1, 3);
                c.tick = 0;
            }
        }

        for (int i = 0; i < 15; ++i) {
            int x = cb.range(0, WIDTH  - 1);
            int y = cb.range(0, HEIGHT - 1);
            if (grid[y][x] != ' ')
                grid[y][x] = cb.random();
        }

        std::string frame;
        frame.reserve(WIDTH * HEIGHT + HEIGHT * 12);
        frame += "\033[H";

        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                char ch = grid[y][x];
                if (ch == ' ') {
                    frame += ' ';
                } else {
                    bool isHead = false;
                    for (int cx = 0; cx < WIDTH; ++cx) {
                        if (cols[cx].head == y && cx == x) { isHead = true; break; }
                    }
                    frame += isHead ? BRIGHT : GREEN;
                    frame += ch;
                    frame += RESET;
                }
            }
            frame += '\n';
        }

        std::cout << frame << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << SHOW;
    return 0;
}