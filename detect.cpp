#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <windows.h>

namespace Holy {
	constexpr int mines = 99, row = 16, col = 30;

	// Colors of 0 to 7
	constexpr unsigned int color_number[] = { 0xC0C0C0, 0xFF0000, 0x008000, 0x0000FF,
		0x800000, 0x000080, 0x808000, 0x000000 };
	// Color of covered block
	constexpr int color_blank =  0xC0C0C0;
	// Name of the minesweeper program
	const std::string minesweeper_class = "Minesweeper";

	// Helper function that tells if (x, y) is out of bound
	inline bool out_of_bound(int x, int y) noexcept {
		return x < 1 || x > 30 || y < 1 || y > 16;
	}

	inline bool out_of_bound(POINT p) noexcept {
		return out_of_bound(p.x, p.y);
	}


	// Helper function that calculates the hash of a coordinate
	inline constexpr int hash_point(int x, int y) {
		return x + y * col;
	}

	inline constexpr int hash_point(POINT p) {
		return hash_point(p.x, p.y);
	}

	// coordinate of a block
	using Coord = POINT;

	// This class handles clicking and reading
	// So it is called a social butterfly
	class Butterfly {
	private:
		// Window area
		RECT mWindowArea;
		// The left up corner of the game board
		constexpr static POINT mGameBoard{ 15, 100 };
		// The dx and dy for the restart button
		constexpr static POINT mSmileButton{ 254, 74 };
		// The pixels of each block
		constexpr static double x_step = 15.96667, y_step = 15.9375;
		// The handle to the game window
		HWND mWindow;
		// The handle to the DC of whole screen , remember to clean up in ~
		HDC mScreenDC;
		// A DC that is stored in memory, to accelerate reading, clean up!
		HDC mMemoryDC;
		// Bitmaps that are required for memory DC, clean up!
		HBITMAP mBitmap, mOldBitmap;
		// If there is a need to re-screenshot, this is true.
		bool mRefresh = true;
		// Width and height of screen DC
		int mScreenWidth, mScreenHeight;
		// Number of pairs of suitable deltas to ascertain block content
		constexpr static int kDeltaPairs = 6;

		// Helper function that gets a screenshot if there is a need to.
		// Only if there is a need! This function checks mHasClick
		inline void refresh_bitmaps() {
			if (mRefresh) {
				::BitBlt(mMemoryDC, 0, 0, mScreenWidth, mScreenHeight, mScreenDC, 0, 0, SRCCOPY);
				mRefresh = false;
			}
		}

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
		// d is the number of the set of offset
		POINT get_read_point(int x, int y, std::size_t d = 0) const {
			POINT read_point;
			constexpr std::array<int, kDeltaPairs> dx = { 10, 10, 9, 9, 8, 12 };
			constexpr std::array<int, kDeltaPairs> dy = { 9, 8, 5, 4, 5, 13 };
			static_assert(dx.size() == dy.size(), "Array lengths of dx and dy \
				must match");
			if (d >= dx.size())
				throw std::out_of_range("Not valid offset number");
			read_point.x = mWindowArea.left + mGameBoard.x + x_step * x
				- x_step + dx[d];
			read_point.y = mWindowArea.top + mGameBoard.y + y_step * y
				- y_step + dy[d];
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
			if (!(mScreenDC = ::GetDC(NULL)))
				throw std::runtime_error("Failed to get device context!");
			if (!(mMemoryDC = ::CreateCompatibleDC(mScreenDC)))
				throw std::runtime_error("Failed to construct memory DC!");
			mScreenWidth = ::GetDeviceCaps(mScreenDC, HORZRES);
			mScreenHeight = ::GetDeviceCaps(mScreenDC, VERTRES);
			// let's assert these two are positive
			if (mScreenWidth <= 0 || mScreenHeight <= 0)
				throw std::runtime_error("Error getting width and height!");
			// initialize the bitmaps
			mBitmap = ::CreateCompatibleBitmap(mScreenDC, mScreenWidth, mScreenHeight);
			mOldBitmap = static_cast<HBITMAP>(::SelectObject(mMemoryDC, mBitmap));
		}

		// Must not let butterfly copy
		Butterfly(const Butterfly& src) = delete;

		Butterfly& operator= (const Butterfly& src) = delete;

		virtual ~Butterfly() noexcept {
			// Let's assume the release will succeed, or the program crashes
			::ReleaseDC(mWindow, mScreenDC);
			::DeleteDC(mMemoryDC);
		}

		// x, y -- the coordinate of the block from left top
		// left top is marked as (1, 1), right bottom as (30, 16)
		void left_click(int x, int y) {
			if (out_of_bound(x, y))
				throw std::out_of_range("Position not defined");
			POINT actual = get_click_point(x, y);
			::SetCursorPos(actual.x, actual.y);
			::mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			::mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
			mRefresh = true;
		}

		// Gets the focus for minesweeper
		void get_focus() {
			POINT actual = get_click_point(0, 0);
			::SetCursorPos(actual.x, actual.y);
			::mouse_event(MOUSEEVENTF_LEFTDOWN, actual.x, actual.y, 0, 0);
			::mouse_event(MOUSEEVENTF_LEFTUP, actual.x, actual.y, 0, 0);
		}

		// Read the number at (x, y)
		// return 0 if the block is unopened or really is 0
		int read_block(int x, int y) {
			if (out_of_bound(x, y))
				throw std::out_of_range("Position not defined!");
			refresh_bitmaps();
			for (int d = 0; d < kDeltaPairs; d++) {
				POINT left_top = get_read_point(x, y, d);
				COLORREF color =
					::GetPixel(mMemoryDC, left_top.x, left_top.y);
				for (int num = 1; num <= 7; num++) {
					if (color == color_number[num])
						return num;
				}
			}
			return 0;
		}

		// Restart the game by clicking the smiling token
		void restart() {
			int x = mWindowArea.left + mSmileButton.x;
			int y = mWindowArea.top + mSmileButton.y;
			::SetCursorPos(x, y);
			::mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
			::mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
			mRefresh = true;
		}
	};

	// This struct stores the basic data of a block
	struct Block {
		// status of block
		enum Status {
			unknown, mine, number
		};
		Status status = unknown;
		// label read from screen (0 if status != number)
		int label = 0;
		// tags whether second hand data is initialized
		bool second_init = false;
		// effective label (0 if status != number)
		int elabel = 0;
		// the number of vacant neighbors
		int vacant_nei = 0;
	};

	// This class stores the basic data of the game
	struct GameData {
		// Array of the blocks
		std::array<std::array<Block, row + 1>, col + 1> blocks;
		// The mines left unprobed
		int mines_left = mines;

		// This function resets *this to initial state
		[[maybe_unused]] void init() {
			for (auto& i : blocks) {
				for (auto& block : i) {
					block.elabel = block.label = 0;
					block.status = Block::unknown;
				}
			}
			mines_left = mines;
		}

		// A shorthand for accessing a given Block
		// Does not check for out_of_bound errors, to make noexcept promise
		// According to language standard, only one argument
		Block& operator[] (Coord p) noexcept {
			return blocks[p.x][p.y];
		}

		const Block& operator[] (Coord p) const noexcept {
			return blocks[p.x][p.y];
		}

	private:
		// Recounts the elabel of block p
		// Does not check for out_of_range, make sure of that!
		// Does not check that p is a number!
		void recount(Coord p) noexcept {
			(*this)[p].elabel = (*this)[p].label;
			(*this)[p].vacant_nei = 0;
			constexpr int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
			constexpr int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
			for (int i = 0; i < 8; i++) {
				Coord np{ p.x + dx[i], p.y + dy[i] };
				if (out_of_bound(np))
					continue;
				// Only decrease if np is mine
				if ((*this)[np].status == Block::mine)
					(*this)[p].elabel--;
				if ((*this)[np].status == Block::unknown)
					(*this)[p].vacant_nei++;
			}
			(*this)[p].second_init = true;
		}

	public:
		// Recount all the elabels, does not throw
		void recount() noexcept {
			for (int ix = 1; ix <= col; ix++) {
				for (int iy = 1; iy <= row; iy++)
					if ((*this)[{ ix, iy }].status == Block::number
						&& !(*this)[{ ix, iy }].second_init)
						recount({ ix, iy });
			}
		}
	};

	// Helper function that does something for every neighbor of a specific block
	// in 8 directions
	// p - The coordinate of the block
	// fn -- the function to be called. fn should be invocable with fn(Coord)
	template <typename Fn>
	void for_each_nei8(Coord p, Fn&& fn) {
		static_assert(std::is_invocable_v<Fn, Coord>, "Should meet type req!");
		constexpr int dx[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
		constexpr int dy[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
		for (int i = 0; i < 8; i++) {
			Coord np{ p.x + dx[i], p.y + dy[i] };
			if (out_of_bound(np))
				continue;
			fn(np);
		}
	}

	// Helper function that does something for every neighbor of a specific block
	// in 4 directions
	// p - The coordinate of the block
	// fn -- the function to be called. fn should be invocable with fn(Coord)
	template <typename Fn>
	void for_each_nei4(Coord p, Fn&& fn) {
		static_assert(std::is_invocable_v<Fn, Coord>, "Should meet type req!");
		constexpr int dx[] = { 0, 0, 1, -1 };
		constexpr int dy[] = { 1, -1, 0, 0 };
		for (int i = 0; i < 4; i++) {
			Coord np{ p.x + dx[i], p.y + dy[i] };
			if (out_of_bound(np))
				continue;
			fn(np);
		}
	}

	// This struct contains find_homo and its helper functions
	struct HOMOfinder {
	public:
		// The type that's used to keep track of what's visited and what's not
		using Checklist = std::array<bool, hash_point(col, row) + 5>;

		// Vector to represent HOMO
		using HOMO = std::vector<Coord>;
	private:
		// Helper function that uses DFS to find out the exterior HOMO
		// When not called recursively, expects that currp is valid
		// and vis is all false, output is empty
		// When called recursively, does not assume vis and output are empty,
		// but currp is valid and contains a number, and not in vis
		static void find_exterior(Coord currp, Checklist& vis, HOMO& output,
			const GameData& game_data) {
			// Prerequesite says that currp is a number, and not enlisted
			output.push_back(currp);
			vis[hash_point(currp)] = true;
			for_each_nei4(currp, [&](Coord nextp) {
				if (vis[hash_point(nextp)])
					return;
				if (game_data[nextp].status == Block::number)
					find_exterior(nextp, vis, output, game_data);
			});
		}

		// Recursively searches for HOMO (helper)
		// output written to output
		// Assumes that currp is valid
		static void search_recurse(Coord currp, Checklist& vis,
			std::vector<HOMO>& output, const GameData& game_data) {
			// Have visited this one, applies also in find_exterior()
			vis[hash_point(currp)] = true;
			// If the current block is a number, then we can start finding one
			const Block& currb = game_data[currp];
			if (currb.status == currb.number) {
				output.emplace_back();
				find_exterior(currp, vis, output.back(), game_data);
			}
			// Not a number, empty (can't be mine), advance to each direction
			// It doesn't matter if the current one is a number, because we share
			// the vis with find_exterior()
			for_each_nei4(currp, [&](Coord nextp) {
				if (!vis[hash_point(nextp)])
					search_recurse(nextp, vis, output, game_data);
			});
		}

		// Helper function that marks points read 0 as blank
		// currp is a point that's known to be blank, expands in 8 directions
		// On first call, requires that vis is all false
		// output written to game_data
		// I think this function can make the noexcept promise
		static void mark_empty(Coord currp, Checklist& vis, GameData& game) noexcept {
			vis[hash_point(currp.x, currp.y)] = true;
			auto& currb = game[currp];
			currb.status = currb.number;
			currb.label = currb.elabel = 0;
			for_each_nei8(currp, [&](Coord nextp) {
				if (vis[hash_point(nextp)])
					return;
				if (game[nextp].label == 0
					&& game[nextp].label == game[nextp].unknown)
					mark_empty(nextp, vis, game);
			});
		}

	public:
		// Searches for HOMOs starting from specified point.
		// currp is the current point under investigation
		// returns a vector of the HOMOs found
		static std::vector<HOMO> search(Coord currp, const GameData& game_data) {
			std::vector<HOMO> result;
			result.reserve(1);
			Checklist vis{ false };
			search_recurse(currp, vis, result, game_data);
			return result;
		}

		// Try and find all the HOMOs by specifying various starting points
		static std::vector<HOMO> search(const GameData& game_data) {
			std::vector<HOMO> result;
			result.reserve(1);
			Checklist vis{ false };
			for (int ix = 1 ; ix <= col; ix++) {
				for (int iy = 1; iy <= row; iy++) {
					if (!vis[hash_point(ix, iy)])
						search_recurse({ ix, iy }, vis, result, game_data);
				}
			}
			return result;
		}

		// porcelain function to mark empty blocks that are newly discovered
		// startp is a block that is known to be empty. Such confidence usually
		// comes from a previous click in Butterfly
		// game_data is the game. Output parameter!
		static void mark_empty(Coord startp, GameData& game_data) noexcept {
			Checklist vis;
			vis.fill(false);
			mark_empty(startp, vis, game_data);
		}
	};

	// This function tells if we have lost, by means of more than 5 sevens
	// butterfly is the butterfly used to communicate
	// game_data should be a reference.
	// While checking for whether we lost, newly read data will be written
	// into game_data. If we lost, game_data will NOT be initialized.
	// Does NOT calculate effective label along the way.
	// Returns true if we have lost, false if we have not.
	bool have_lost(Butterfly& butterfly, GameData& game_data) {
		int sevens_cnt = 0;
		for (int ix = 1; ix <= col; ix++) {
			for (int iy = 1; iy <= row; iy++) {
				auto& block = game_data[{ ix, iy }];
				if (block.status == block.unknown) {
					block.label = butterfly.read_block(ix, iy);
					if (block.label != 0)
						block.status = block.number;
				}
				if (block.label == 7) // 7 is the magic number for mine
					sevens_cnt++;
				if (sevens_cnt == 2)
					return true;
			}
		}
		return false;
	}

	// Start the game with some recursive mad clicking
	// butterfly and game_data with usual meanings.
	// Does not calculates elabels along the way (that is more conveniently
	// done in another separate func)
	// Returns only when we have probed a zero or a mine
	// Returns true if have not lost. false if lost
	// named after su-zhi-san-lian
	bool civil_clicks(Butterfly& butterfly, GameData& game_data) {
		// Randomness generator
		using std::chrono::steady_clock;
		std::mt19937 mt1(steady_clock::now().time_since_epoch().count());
		using Uintd = std::uniform_int_distribution<int>;
		auto ran = std::bind(Uintd(hash_point(1, 1), hash_point(col, row)), mt1);
		// Now start the game ...
		while (true) {
			int r = ran();
			int x = r % 30 + 1;
			int y = r / 30;
			butterfly.left_click(x, y);
			// To be cautious, check for losing or winning
			// FIXME: Is there a way to improve accuracy?
			if (have_lost(butterfly, game_data)) {
				// std::cerr << "Lost!" << std::endl;
				return false;
			}
			// Now data has been pulled in by have_lost()
			else if (game_data[{ x, y }].label == 0) {
				HOMOfinder::mark_empty({ x, y }, game_data);
				return true;
			}
		}
	}
}

int main() {
	// The point to start search
	Holy::Butterfly butterfly;
	Holy::GameData game_data;
	std::cout << std::boolalpha;
	butterfly.get_focus();
	butterfly.restart();
	bool result = Holy::civil_clicks(butterfly, game_data);
	std::cout << result << std::endl;
	if (!result)
		::MessageBeep(MB_ICONASTERISK);
	game_data.recount();
	std::cout << "Table of elabel:\n\n";
	using namespace Holy;
	for (int iy = 1; iy <= row; iy++) {
		for (int ix = 1; ix <= col; ix++) {
			if (game_data[{ix,iy}].status == Block::number)
				std::cout << game_data[{ ix, iy }].elabel;
			else
				std::cout << ' ';
			std::cout << ' ';
		}
		std::cout << '\n';
	}
	std::cout << "\nThe table of vacant_nei:\n\n";
	for (int iy = 1; iy <= row; iy++) {
		for (int ix = 1; ix <= col; ix++) {
			if (game_data[{ix,iy}].status == Block::number)
				std::cout << game_data[{ ix, iy }].vacant_nei;
			else
				std::cout << ' ';
			std::cout << ' ';
		}
		std::cout << '\n';
	}
}