#include "solvers.h"
#include <chrono>
#include <fstream>
#include <iostream>

using namespace Holy;

// Number of games in which john is invoked.
int john_invoked, john_uninvoked, john_det;

// Win rates
int won, lost;

// Total time spent on roundup and felix, in ms
long long trivial_total;

// Total time on john, in ms
long long john_total;

void main_loop(Butterfly& butt) {
    GameData game;
    butt.start_game({ 10, 10 });
    game.mark_semiknown({ 10, 10 });
    accio(game, butt, true);
    bool has_hope;
    bool first = true;
    int roundup_cnt = 0, felix_cnt = 0;
    using namespace std::chrono;
trivial_start:
    auto start = high_resolution_clock::now();
    do {
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
    trivial_total += duration_cast<milliseconds>(end - start).count();
    if (butt.verify()) {
        won++;
        if (first)
            john_uninvoked++;
        return;
    }
    if (first)
        john_invoked++;
    start = high_resolution_clock::now();
    auto [guess, mc] = john(game);
    if (!mc) {
        // Deterministic
        accio(game, butt, true);
        john_det++;
        end = high_resolution_clock::now();
        john_total += duration_cast<milliseconds>(end - start).count();
        first = false;
        goto trivial_start;
    }
    end = high_resolution_clock::now();
    john_total += duration_cast<milliseconds>(end - start).count();
    if (butt.verify())
        won++;
    else
        lost++;
}

void reset_global() {
    john_det = john_invoked = john_uninvoked = 0;
    won = lost = 0;
    trivial_total = john_total = 0;
}

void write_data(std::ostream& file) {
    file << "Won: " << won << "    lost: " << lost << '\n';
    file << "John invoked: " << john_invoked << "    not: " << john_uninvoked
         << '\n';
    file << "John determined: " << john_det << '\n';
    file << "Total time on john: " << john_total << "ms\n";
    file << "Total time on trivial: " << trivial_total << "ms\n\n";
    file.flush();
}

int main() {
    std::ofstream file("deter_bench.log", std::ios::out | std::ios::app);
    Butterfly butt;
    int exit = 0;
    std::cout << "Exit after 100 games? (1 or 0)\n";
    std::cin >> exit;
    while (true) {
        const int won_before = won;
        main_loop(butt);
        if (won == won_before + 1)
            file << "w";
        else
            file << "l";
        if (won + lost == 100) {
            file << '\n';
            write_data(file);
            reset_global();
            if (exit)
                break;
        }
    }
}