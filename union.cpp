#include <string>
#include <iostream>
#include <unordered_map>
#include <fstream>

struct Coordinate {
    int x, y;
};

namespace std {
    template <>
    struct hash<Coordinate> {
        int operator() (const Coordinate& a) const noexcept {
            return a.x * 30 + a.y;
        }
    };
}

bool operator== (const Coordinate& lhs, const Coordinate& rhs) noexcept {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

std::unordered_map<Coordinate, int> stat;

void read_file(std::ifstream& file) {
    Coordinate curr;
    while (file) {
        file >> curr.x >> curr.y;
        stat[curr]++;
    }
}

int main() {
    std::string name = "1.ok";
    while (name[0] <= '8') {
        std::ifstream file(name, std::ios::in);
        read_file(file);
        name[0]++;
    }
    for (auto [pos, freq] : stat) {
        if (freq == 8)
            std::cout << pos.x << ' '  << pos.y << '\n';
    }
}