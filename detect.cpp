#include <windows.H>
#include <iostream>
#include <cstdlib>

namespace Holy {
	// The point of left up on the board
	POINT left_up;
	const int mines = 99, row = 16, col = 30;
	double x_step = 15.9, y_step = 15.9;

	// Colors of 0 to 8
	const int colors[] = { 0xC0C0C0, 0x0000FF, 0x008000, 0xFF0000, 0x000080, 0x800000, 0x008080, 0x000000, 0x808080 };
	// Color of undiscovered block
	const int color_blank =  0xC0C0C0;

	void get_edge() {
		POINT right_down;
		std::printf("Move mouse to left up\n");
		std::system("pause");
		::GetCursorPos(&left_up);
		std::printf("Move mouse to right down\n");
		std::system("pause");
		::GetCursorPos(&right_down);
		x_step = double(right_down.x - left_up.x) / col;
		y_step = double(right_down.y - left_up.y) / row;
	}

	void move_to(int x, int y) {
		POINT to_click;
		to_click.x = left_up.x - x_step * 0.5 + x_step * x;
		to_click.y = left_up.y - y_step * 0.5 + y_step * y;
		::SetCursorPos(to_click.x, to_click.y);
	}

	void click(int x, int y, bool is_left = true) {
		move_to(x, y);
		INPUT input[2];
		for (int i = 0; i <= 1; i++) {
			input[i].type = INPUT_MOUSE;
			input[i].mi.dx = input[i].mi.dy = 0;
		}
		input[0].mi.dwFlags = is_left ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
		input[1].mi.dwFlags = is_left ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
		if (::SendInput(2, input, sizeof(INPUT)) != 2) {
			::MessageBeep(MB_ICONERROR);
			std::printf("Failed");
			std::exit(1);
		}
		// std::printf("OK");
		::Sleep(500); // Time required between two clicks
		// TODO: Can I detect this automatically?
	}
}

int main() {
	using namespace Holy;
	Holy::get_edge();
	std::cout << "x_step is " << x_step << "\ny_step is " << y_step << std::endl;
	return 0;
}
