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
    int roundup_cnt = 0, felix_cnt = 0;
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    do {
        // std::cerr << "Has hope\n";
        has_hope = false;
        while (++roundup_cnt && roundup(game)) {
            has_hope = true;
            accio(game, butt, true);
        }
        while (++felix_cnt && felix(game)) {
            has_hope = true;
            accio(game, butt, true);
        }
    } while (has_hope);
    auto end = high_resolution_clock::now();
    std::cout << "\nResult:\n";
    print(game);
    std::cout << "Felix-cnt = " << felix_cnt
              << "   roundup cnt = " << roundup_cnt
              << "   mines left = " << game.mines_left;
    std::cout << "\nTime consumed: ";
    std::cout << duration_cast<microseconds>(end - start).count() << "us"
              << std::endl;
    std::cout << "Whether butterfly says we win: " << butt.verify() << std::endl;
    if (butt.verify())
        return;
    std::cout << "Invoking john" << std::endl;
    start = high_resolution_clock::now();
    auto [guess, mc] = john(game);
    if (!mc)
        // Deterministic
        accio(game, butt, true);
    end = high_resolution_clock::now();
    std::cout << "Whether john was deterministic: " << (!mc) << '\n';
    std::cout << "Time consumed: ";
    std::cout << duration_cast<microseconds>(end - start).count() << "us"
              << std::endl;
    std::cout << "Whether butterfly says we win: " << butt.verify() << '\n';
    std::cout << "Whether john says we guess: " << guess << std::endl;
    if (mc && !butt.verify()) {
        print(*mc);
    } else if (!mc) {
        print(game);
    }
}

int main() {
    Butterfly butt;
    std::cout << std::boolalpha;
    while (true) {
        main_loop(butt);
        std::cin.get();
    }
}