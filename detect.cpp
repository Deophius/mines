#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif
#include <windows.h>
#include <iostream>
#include <sstream>
#include <string_view>

/** @file Final actions that happen during scope cleanup */
/// @author Deophius
#include <type_traits>
#include <utility>

namespace Holy
{
    /// @brief A Final action that needs to be done during scope cleanup.
    ///
    /// We know that you need to clean up if a exception is thrown.
    /// Classes like lock_guard, unique_ptr are like that.
    /// But sometimes you cannot afford a new type for every action, so you can
    /// take advantage of this FinalAction support class. Like this: <pre> void
    /// f(int p)
    /// {
    ///     int* a = new int(3);
    ///     auto cleanup = Misc::finally([a]{ delete p; });
    ///     if (p == 0)
    ///         throw std::invalid_argument("f you p!");
    /// }
    /// </pre>
    /// This class is not designed to take the place of smart pointers
    /// and RAII. Just for convenience sometimes.
    ///
    /// @attention Requires: NothrowInvocable<Action> && StrippedDown<Action>
    /// && CopyConstructible<Action>
    /// @param Action -- the type of the Action. Perhaps it is a lambda
    /// expression.
    template <class Action>
    // Final action class
    class FinalAction
    {
    private:
        /// @brief This is the action that's executed at the destructor.
        Action mAction;

    public:
        /// @brief Constructs a final action bound to act.
        /// @param act -- the action to perform.
        /// @exception I_DONT_KNOW Depends on the copy constructor of Action.
        /// @exception None if std::is_nothrow_copy_constructible_v<Action>
        /// @note Usually this is not called to construct a FinalAction
        /// @see Misc::finally<Action>(Action&&)
        explicit FinalAction(const Action& act) noexcept(
            std::is_nothrow_copy_constructible_v<Action>) :
            mAction(act)
        {
            // Type requirements.
            static_assert(
                std::is_copy_constructible_v<Action>,
                "Action class should be copy constructible.");
            static_assert(
                std::is_invocable_v<Action>,
                "Action class should be nullary invocable.");
        }

        /// @brief Nontrivial dtor that invokes action.
        ///
        /// @exception None.
        virtual ~FinalAction() noexcept
        {
            mAction();
        }
    };

    template <typename Action>
    /// @fn finally
    /// @brief Preferred way to create a FinalAction object.
    ///
    /// @param Action -- the TYPE of act, (with some twich from C++)
    /// @param act -- the action to be carried out at scope exit.
    /// @exceptions see FinalAction::FinalAction.
    /// @note Action&& is a forwarding reference! Be careful!
    auto finally(Action&& act)
        -> FinalAction<std::remove_cv_t<std::remove_reference_t<Action>>>
    {
        return FinalAction<std::remove_cv_t<std::remove_reference_t<Action>>>{
            act
        };
    }
} // namespace Holy

namespace Holy {
	constexpr int mines = 99, row = 16, col = 30;

	// Colors of 0 to 7
	constexpr unsigned int color_number[] = { 0xC0C0C0, 0xFF0000, 0x008000, 0x0000FF,
		0x800000, 0x000080, 0x808000, 0x000000 };
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
		HDC mScreenDC;

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
			constexpr int dx[] = { 10, 10, 9, 9, 8, 12 };
			constexpr int dy[] = { 9, 8, 5, 4, 5, 13 };
			if (d >= sizeof(dx))
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
		}

		virtual ~Butterfly() noexcept {
			// Let's assume the release will succeed, or the program crashes
			::ReleaseDC(mWindow, mScreenDC);
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
			// First try to get a compatible DC
			HDC memoryDC = ::CreateCompatibleDC(mScreenDC);
			int width = ::GetDeviceCaps(mScreenDC, HORZRES);
			int height = ::GetDeviceCaps(mScreenDC, VERTRES);
			// let's assert these two are positive
			if (width <= 0 || height <= 0)
				throw std::runtime_error("Error getting width and height!");
			// compatible bitmap
			HBITMAP bitmap = ::CreateCompatibleBitmap(mScreenDC, width, height);
			// old bitmap
			auto old_bitmap = static_cast<HBITMAP>(::SelectObject(memoryDC, bitmap));
			// Copy the screen content into our bitmap
			::BitBlt(memoryDC, 0, 0, width, height, mScreenDC, 0, 0, SRCCOPY);
			// Declare a cleanup action
			auto cleanup = finally([&bitmap, old_bitmap, memoryDC]() {
				bitmap = static_cast<HBITMAP>(::SelectObject(memoryDC, old_bitmap));
				::DeleteDC(memoryDC);
			});
			try {
				for (int d = 0;; d++) {
					POINT left_top = get_read_point(x, y, d);
					COLORREF color =
						::GetPixel(memoryDC, left_top.x, left_top.y);
					for (int num = 1; num <= 7; num++) {
						if (color == color_number[num])
							return num;
					}
				}
			} catch (const std::out_of_range& ex) {
				// It seems as if all things didn't work, give 0
				return 0;
			}
			// Should never have reached here
			std::terminate();
		}
	};
}

int main() {
	Holy::Butterfly butterfly;
	for (int iy = 1; iy <= Holy::row; iy++) {
		for (int ix = 1; ix <= Holy::col; ix++) {
			std::cout << butterfly.read_block(ix, iy) << ' ';
		}
		std::cout << std::endl;
	}
}