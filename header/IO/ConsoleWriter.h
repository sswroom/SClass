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
		typedef struct
		{
			UInt8 fgColor;
			UInt8 bgColor;
			Int32 currX;
			Int32 currY;
			Int32 consoleWidth;
			Int32 consoleHeight;
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

		WChar *ReadLine(WChar *sbuff, OSInt nChar);

		void SetTextColor(UInt8 fgColor, UInt8 bgColor);
		void ResetTextColor();
		OSInt CalDisplaySize(const WChar *str);
		void EnableCPFix(Bool isEnable);
		void SetAutoFlush(Bool autoFlush);
		Bool GetConsoleState(ConsoleState *state);
		Bool SetCursorPos(Int32 x, Int32 y);
		Bool IsFileOutput();

		void FixWrite(const WChar *str, OSInt displayWidth);
		OSInt GetDisplayWidth(const WChar *str);
	private:
		OSInt GetDisplayCharWidth(WChar c);
	};
}
#endif
