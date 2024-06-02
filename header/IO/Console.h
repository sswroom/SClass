#ifndef _SM_IO_CONSOLE
#define _SM_IO_CONSOLE
namespace IO
{
	class Console
	{
	public:
		typedef void (__stdcall *ConsoleHandler)(void *userObj);
	private:
		static ConsoleHandler exitCmdHdlr;
		static void *exitCmdObj;
		static Int32 __stdcall ConsoleHdlr(UInt32 dwCtrlType);
	public:
		static UTF32Char GetChar();
		static void PutChar(UTF32Char c);
		static UnsafeArray<UTF8Char> GetLine(UnsafeArray<UTF8Char> buff); //Without newline
		static void PrintStrO(const UTF8Char *str1);
		static UOSInt WriteStdOut(UInt8 *buff, UOSInt size);
		static void PreventCtrlC();
		static void HandleExitCmd(ConsoleHandler exitCmdHdlr, void *userObj);
		static void SetCodePage(UInt32 codePage);
		static Int32 GetKey();
	};
}
#endif
