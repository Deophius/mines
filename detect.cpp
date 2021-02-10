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
	constexpr int colors[] = { 0xC0C0C0, 0xFF0000, 0x008000, 0x0000FF,
		0x800000, 0x000080, 0x808000, 0x000000, 0x808080 };
	// Color of covered block
	constexpr int color_blank =  0xC0C0C0;
	// Name of the minesweeper program
	const std::string_view minesweeper_class = "Minesweeper";

	// This class handles clicking and reading
	// So it is called a social butterfly
	class Butterfly {
	private:
		// Window area
		RECT mWindowArea;
		// The left up corner of the game board
		constexpr static POINT mGameBoard{ 15, 100 };
		// The pixels of each block
		constexpr static double x_step = 15.9, y_step = 15.9;
		// The handle to the game window
		HWND mWindow;
		// The handle to the DC of whole screen , remember to clean up in ~
		HDC mDeviceContext;

		// Helper function that calculates the actual point for clicking
		// Does not check for out of range errors
		POINT get_click_point(int x, int y) const noexcept {
			POINT click_point;
			// x is made up of three parts: client left to window left to game
			// board left to the block's center
			click_point.x = mWindowArea.left + mGameBoard.x + x_step * x
				- x_step / 2.0;
			click_point.y = mWindowArea.top + mGameBoard.y + y_step * y
				- y_step / 2.0;
			return click_point;
		}

		// Helper function that calculates the point for reading
		// does not check for out of range errors
		POINT get_read_point(int x, int y) const noexcept {
			POINT read_point;
			read_point.x = mWindowArea.left + mGameBoard.x + x_step * x - x_step;
			read_point.y = mWindowArea.top + mGameBoard.y + y_step * y - y_step;
			return read_point;
		}
	public:
		// hwnd: provided handle to game window
		Butterfly(HWND hwnd = NULL) {
			if (!hwnd) {
				// Find the window by myself.
				hwnd = ::FindWindow(minesweeper_class.data(), NULL);
				if (!hwnd)
					throw std::runtime_error("Cannot find minesweeper window!");
			}
			mWindow = hwnd;
			if (::GetWindowRect(hwnd, &mWindowArea) == 0)
				throw std::runtime_error("Failed to get window area!");
			if (!(mDeviceContext = ::GetDC(NULL)))
				throw std::runtime_error("Failed to get device context!");
		}

		virtual ~Butterfly() noexcept {
			// Let's assume the release will succeed, or the program crashes
			::ReleaseDC(mWindow, mDeviceContext);
		}

		// x, y -- the coordinate of the block from left top
		// left top is marked as (1, 1), right bottom as (30, 16)
		void left_click(int x, int y) const {
			if (x < 1 || x > 30 || y < 1 || y > 16)
				throw std::out_of_range("Position not defined");
			POINT actual = get_click_point(x, y);
			SetCursorPos(actual.x, actual.y);
			mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
		}

		// Gets the focus for minesweeper
		void get_focus() {
			POINT actual = get_click_point(0, 0);
			SetCursorPos(actual.x, actual.y);
			mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
		}

		// Read the number at (x, y)
		// return 0 if the block is unopened or really is 0
		int read_block(int x, int y) const {
			if (x < 1 || x > 30 || y < 1 || y > 16)
				throw std::out_of_range("Position not defined!");
			POINT left_top = get_read_point(x, y);
			for (double dx = 0; dx <= x_step; dx += 1) {
				for (double dy = 0; dy <= y_step; dy += 1) {
					double x = left_top.x + dx;
					double y = left_top.y + dy;
					COLORREF color = GetPixel(mDeviceContext, x, y);
					if (color == CLR_INVALID)
						continue;
					std::cout << dx << " " << dy << " " << color << '\n';
				}
			}
			return 0;
		}
	};
}

int main() {
	Holy::Butterfly butterfly;
	std::cout << std::hex;
	butterfly.read_block(2, 3);
	// butterfly.read_block(11, 7);
}