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

		UTF8Char *GetModuleFileName(UTF8Char *buff);
		Bool GetModuleAddress(OSInt *baseAddr, UInt32 *size);
	};
};
#endif
