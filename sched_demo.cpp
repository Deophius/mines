// Contains main()
// Demonstrates how the program runs, and tests accio
#include "solvers.h"
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace Holy;

void print(const GameData& game) {
    for (int iy = 1; iy <= row; iy++) {
        for (int ix = 1; ix <= col; ix++) {
            Point p{ ix, iy };
            const Block& b = game[p];
            switch (b.status) {
                case Block::unknown:
                    std::cout << ' ';
                    break;
                case Block::number:
                    std::cout << b.label;
                    break;
                case Block::mine:
                    std::cout << '*';
                    break;
                case Block::semiknown:
                    std::cerr << "Shouldn't have appeared!\n";
                    std::terminate();
            }
            std::cout << ' ';
        }
        std::cout << '\n';
    }
    std::cout.flush();
}

void print(const MineChance& mc) {
    for (int iy = 1; iy <= row; iy++) {
        for (int ix = 1; ix <= col; ix++) {
            int hash = Point{ ix, iy }.hash();
            if (mc[hash])
                std::cout << std::setw(2) << mc[hash] << ' ';
            else
                std::cout << "   ";
        }
        std::cout << '\n';
    }
}

void main_loop(Butterfly& butt) {
    GameData game;
    // std::cerr << "Start first click\n";
    butt.start_game({ 10, 10 });
    game.mark_semiknown({ 10, 10 });
    // std::cerr << "Starting initial accio\n";
    accio(game, butt, true);
    // std::cerr << "Finished initial accio\n";
    std::cout << "Intial position:\n";
    print(game);
    bool has_hope;
    do {
        // std::cerr << "Has hope\n";
        has_hope = false;
        while (roundup(game)) {
            has_hope = true;
            accio(game, butt, true);
        }
        while (felix(game)) {
            has_hope = true;
            accio(game, butt, true);
        }
    } while (has_hope);
    std::cout << "Whether butterfly says we win: " << butt.verify() << std::endl;
    if (butt.verify())
        return;
    std::cout << "Invoking john" << std::endl;
    while (true) {
        auto [guess, mc] = john(game);
        if (mc) {
            // No longer deterministic, break
            std::cout << "Whether john was deterministic: " << (!mc) << '\n';
            std::cout << "Whether butterfly says we win: " << butt.verify() << '\n';
            std::cout << "Whether john says we guess: " << guess << std::endl;
            break;
        }
        roundup(game);
        accio(game, butt, true);
    }
    print(game);
}

int main() {
    Butterfly butt;
    std::cout << std::boolalpha;
    while (true) {
        main_loop(butt);
        std::cin.get();
    }
}