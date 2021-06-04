#ifndef _SM_IO_CONSOLEWRITER
#define _SM_IO_CONSOLEWRITER
#include "IO/Writer.h"

namespace Text
{
	class Encoding;
}
namespace Sync
{
	class Mutex;
}

namespace IO
{
	class ConsoleWriter : public IO::Writer
	{
	public:
		typedef enum
		{
			CC_BLACK = 0,
			CC_DARK_BLUE = 1,
			CC_DARK_GREEN = 2,
			CC_DARK_CYAN = 3,
			CC_DARK_RED = 4,
			CC_DARK_MAGENTA = 5,
			CC_DARK_YELLOW = 6,
			CC_GRAY = 7,
			CC_DARK_GRAY = 8,
			CC_BLUE = 9,
			CC_GREEN = 10,
			CC_CYAN = 11,
			CC_RED = 12,
			CC_MAGENTA = 13,
			CC_YELLOW = 14,
			CC_WHITE = 15
		} ConsoleColor;
		typedef struct
		{
			ConsoleColor fgColor;
			ConsoleColor bgColor;
			UInt32 currX;
			Int32 currY;
			UInt32 consoleWidth;
			UInt32 consoleHeight;
		} ConsoleState;
	private:
		void *hand;
		Text::Encoding *enc;
		Sync::Mutex *mut;
		Bool autoFlush;
		Bool fileOutput;

	public:
		ConsoleWriter();
		virtual ~ConsoleWriter();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();

		WChar *ReadLine(WChar *sbuff, UOSInt nChar);

		void SetTextColor(ConsoleColor fgColor, ConsoleColor bgColor);
		void ResetTextColor();
		OSInt CalDisplaySize(const WChar *str);
		void EnableCPFix(Bool isEnable);
		void SetAutoFlush(Bool autoFlush);
		Bool GetConsoleState(ConsoleState *state);
		Bool SetCursorPos(UInt32 x, Int32 y);
		Bool IsFileOutput();

		void FixWrite(const WChar *str, OSInt displayWidth);
		OSInt GetDisplayWidth(const WChar *str);
	private:
		OSInt GetDisplayCharWidth(WChar c);
	};
}
#endif
