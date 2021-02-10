#include <iostream>
#include <fstream>
#include <sstream>

const std::string colors[] = { "c0c0c0", "ff0000", "8000", "ff",
	"800000", "80", "808000", "0", "808080" };

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "Bad call!" << std::endl;
        std::terminate();
    }
    int to_check = argv[1][0] - '0';
    std::cout << "Testing for " << to_check << std::endl;
    std::ostringstream filename;
    filename << to_check << ".ok";
    std::ofstream fout(filename.str());
    while (std::cin) {
        int dx, dy;
        std::string color;
        std::cin >> dx >> dy >> color;
        if (color == colors[to_check])
            fout << dx << ' ' << dy << std::endl;
    }
}