#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif
#include <windows.h>
#include <iostream>
#include <string_view>

namespace Holy {
	constexpr int mines = 99, row = 16, col = 30;

	// Colors of 0 to 8
	constexpr int colors[] = { 0xC0C0C0, 0x0000FF, 0x008000, 0xFF0000,
		0x000080, 0x800000, 0x008080, 0x000000, 0x808080 };
	// Color of covered block
	constexpr int color_blank =  0xC0C0C0;
	// Name of the minesweeper program
	const std::string_view minesweeper_class = "Minesweeper";

	class Clicker {
	private:
		// Window area
		RECT mWindowArea;
		// The left up corner of the game board
		constexpr static POINT mGameBoard{ 15, 100 };
		// The pixels of each block
		constexpr static double x_step = 15.9, y_step = 15.9;
		// The handle to the game window
		HWND mWindow;

		// Helper function that calculates the actual point for clicking
		// Does not check for out of range errors
		POINT get_actual_point(int x, int y) const noexcept {
			POINT click_point;
			// x is made up of three parts: client left to window left to game
			// board left to the block's center
			click_point.x = mWindowArea.left + mGameBoard.x + x_step * x
				- x_step / 2.0;
			click_point.y = mWindowArea.top + mGameBoard.y + y_step * y
				- y_step / 2.0;
			return click_point;
		}
	public:
		// hwnd: provided handle to game window
		Clicker(HWND hwnd = NULL) {
			if (!hwnd) {
				// Find the window by myself.
				hwnd = ::FindWindow(minesweeper_class.data(), NULL);
				if (!hwnd)
					throw std::runtime_error("Cannot find minesweeper window!");
			}
			mWindow = hwnd;
			if (::GetWindowRect(hwnd, &mWindowArea) == 0)
				throw std::runtime_error("Failed to get window area!");
		}

		// x, y -- the coordinate of the block from left top
		// left top is marked as (1, 1), right bottom as (30, 16)
		void left_click(int x, int y) const {
			if (x < 1 || x > 30 || y < 1 || y > 16)
				throw std::out_of_range("Position not defined");
			POINT actual = get_actual_point(x, y);
			SetCursorPos(actual.x, actual.y);
			mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
		}

		// Gets the focus for minesweeper
		void get_focus() {
			POINT actual = get_actual_point(0, 0);
			SetCursorPos(actual.x, actual.y);
			mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
		}
	};
}

int main() {
	Holy::Clicker clicker;
	clicker.get_focus();
	clicker.left_click(3, 3);
}