#ifndef _SM_MANAGE_MODULEINFO
#define _SM_MANAGE_MODULEINFO
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ModuleInfo
	{
	private:
		void *hMod;
		void *hProc;

	public:
		ModuleInfo(void *hProc, void *hMod);
		~ModuleInfo();

		UnsafeArray<UTF8Char> GetModuleFileName(UnsafeArray<UTF8Char> buff);
		Bool GetModuleAddress(OutParam<UOSInt> baseAddr, OutParam<UOSInt> size);
	};
}
#endif
