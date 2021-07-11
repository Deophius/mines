#include "solvers.h"
#include <fstream>
#include <chrono>

using namespace Holy;

// Number of games in which john is invoked.
int john_invoked, john_uninvoked, john_det;

// Win rates
int won, lost;

// Total time spent on roundup and felix, in us
long long trivial_total;

// Total time on john, in us
long long john_total;


void main_loop(Butterfly& butt) {
    GameData game;
    butt.start_game({ 10, 10 });
    game.mark_semiknown({ 10, 10 });
    accio(game, butt, true);
    bool has_hope;
    int roundup_cnt = 0, felix_cnt = 0;
    using namespace std::chrono;
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
    trivial_total += duration_cast<microseconds>(end - start).count();
    if (butt.verify()) {
        won++;
        john_uninvoked++;
        return;
    }
    john_invoked++;
    start = high_resolution_clock::now();
    auto [guess, mc] = john(game);
    if (!mc) {
        // Deterministic
        accio(game, butt, true);
        john_det++;
    }
    end = high_resolution_clock::now();
    john_total += duration_cast<microseconds>(end - start).count();
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
    file << "John invoked: " << john_invoked << "    not: " << john_uninvoked << '\n';
    file << "John determined: " << john_det << '\n';
    file << "Total time on john: " << john_total << "us\n";
    file << "Total time on trivial: " << trivial_total << "us\n\n";
    file.flush();
}

int main() {
    std::ofstream file("deter_bench.log", std::ios::out | std::ios::app);
    Butterfly butt;
    while (true) {
        main_loop(butt);
        if (won + lost > 100'000) {
            write_data(file);
            reset_global();
        }
    }
}