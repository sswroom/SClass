#ifndef _SM_WIN32_WINRTCORE
#define _SM_WIN32_WINRTCORE
namespace Win32
{
	class WinRTCore
	{
	private:
		static Bool inited;
	public:
		static void Init();
	};
}
#endif
