#include "butterfly.h"
#include <iostream>
#include <chrono>
#include "solvers.h"

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

bool invoke_roundup(Holy::Butterfly& butt) {
	using namespace Holy;
	GameData game;
	for (int ix = 1; ix <= col; ix++) {
		for (int iy = 1; iy <= row; iy++) {
			if (flagged[ix][iy])
				game[{ ix, iy }].status = Block::mine;
			else if (auto read = butt.read({ ix, iy }); read) {
				game[{ ix, iy }].status = Block::number;
				game[{ ix, iy }].label = *read;
			}
		}
	}
	game.recount();
	bool ret = roundup(game, butt);
	for (int ix = 1; ix <= col; ix++) {
		for (int iy = 1; iy <= row; iy++) {
			if (game[{ ix, iy }].status == Block::mine)
				flagged[ix][iy] = true;
		}
	}
	return ret;
}

bool main_loop(Holy::Butterfly& butt) {
	std::cout << "1) Start new game\n";
	std::cout << "2) Click point\n";
	std::cout << "3) Mark as flagged\n";
	std::cout << "4) Call roundup once\n";
	std::cout << "5) Call roundup recursively\n";
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
		case 4:
			if (!butt.in_game())
				throw std::logic_error("Not in game");
			std::cout << "Roundup returned " << invoke_roundup(butt) << '\n';
			return true;
		case 5:
			if (!butt.in_game())
				throw std::logic_error("Not in game");
			{
				using namespace std::chrono;
				auto start = high_resolution_clock::now();
				while (invoke_roundup(butt))
					;
				auto end = high_resolution_clock::now();
				std::cout << duration_cast<microseconds>(end - start).count()
					<< "us" << std::endl;
			}
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
