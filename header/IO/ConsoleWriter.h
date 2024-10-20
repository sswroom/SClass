#ifndef _SM_IO_CONSOLEWRITER
#define _SM_IO_CONSOLEWRITER
#include "Text/StyledTextWriter.h"

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
	class ConsoleWriter : public Text::StyledTextWriter
	{
	public:
		typedef struct
		{
			Text::StandardColor fgColor;
			Text::StandardColor bgColor;
			UInt32 currX;
			Int32 currY;
			UInt32 consoleWidth;
			UInt32 consoleHeight;
		} ConsoleState;

		struct ClassData;
	private:
		ClassData *clsData;
		Text::StandardColor bgColor;

	public:
		ConsoleWriter();
		virtual ~ConsoleWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteLine();
		virtual Bool WriteChar(UTF8Char c);

		void SetBGColor(Text::StandardColor bgColor);
		virtual void SetTextColor(Text::StandardColor fgColor);
		virtual void ResetTextColor();

		UOSInt CalDisplaySize(const WChar *str);

		UnsafeArrayOpt<WChar> ReadLine(UnsafeArray<WChar> sbuff, UOSInt nChar);

		void EnableCPFix(Bool isEnable);
		void SetAutoFlush(Bool autoFlush);
		Bool GetConsoleState(ConsoleState *state);
		Bool SetCursorPos(UInt32 x, Int32 y);
		Bool IsFileOutput();

		void FixWrite(const WChar *str, UOSInt displayWidth);
		UOSInt GetDisplayWidth(const WChar *str);
	private:
		UOSInt GetDisplayCharWidth(WChar c);
	};
}
#endif
