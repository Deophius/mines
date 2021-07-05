#include "butterfly.h"
#include "solvers.h"
#include <chrono>
#include <iostream>

// Contains main()

std::array<std::bitset<Holy::row + 1>, Holy::col + 1> flagged;

// This function prints the content of game onto screen
void print_game(const Holy::Butterfly& butt) {
    using namespace Holy;
    for (int iy = 1; iy <= row; iy++) {
        for (int ix = 1; ix <= col; ix++) {
            if (flagged[ix][iy])
                std::cout << '*';
            else {
                auto res = butt.read({ ix, iy });
                if (!res)
                    std::cout << '.';
                else if (*res) // res contains something because elif
                    std::cout << *res;
                else
                    std::cout << '0';
            }
            std::cout << ' ';
        }
        std::cout << '\n';
    }
}

void init_game(Holy::Butterfly& butt, Holy::Point p) {
    for (auto& row : flagged)
        row = 0;
    butt.start_game(p);
}

bool main_loop(Holy::Butterfly& butt) {
    std::cout << "1) Start new game\n";
    std::cout << "2) Click point\n";
    std::cout << "3) Mark as flagged\n";
    std::cout << "Your choice? (Three ints please)";
    // operands
    int option, x = 0, y = 0;
    std::cin >> option;
    try {
        switch (option) {
            case 1:
                std::cin >> x >> y;
                init_game(butt, { x, y });
                return true;
            case 2: {
                std::cin >> x >> y;
                auto res = butt.click({ x, y });
                if (!res) {
                    std::cout << "Sorry, you lost!" << std::endl;
                    return false;
                }
                return true;
            }
            case 3:
                std::cin >> x >> y;
                if (!butt.in_game())
                    throw std::logic_error("Not in game");
                flagged[x][y] = true;
                return true;
            default:
                std::cout << "Unrecognized option, try again!" << std::endl;
                return false;
        }
    } catch (const std::out_of_range& ex) {
        std::cout << "The x, y you specified was out of range!\n";
    } catch (const std::logic_error& ex) {
        std::cout << "Please start a new game first!\n";
    }
    return false;
}

int main() {
    Holy::Butterfly butt;
    std::cout << std::boolalpha;
    while (true) {
        if (main_loop(butt))
            print_game(butt);
        if (std::cin.eof())
            break;
    }
}
