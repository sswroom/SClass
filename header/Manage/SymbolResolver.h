#ifndef _SM_MANAGE_SYMBOLRESOLVER
#define _SM_MANAGE_SYMBOLRESOLVER
#include "Manage/Process.h"
#include "Manage/AddressResolver.h"
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStrUTF8.h"

namespace Manage
{
	class SymbolResolver : public Manage::AddressResolver
	{
	private:
		Data::ArrayListStrUTF8 *modNames;
		Data::ArrayListInt64 *modBaseAddrs;
		Data::ArrayListInt32 *modSizes;
		Manage::Process *proc;

	public:
		SymbolResolver(Manage::Process *proc);
		virtual ~SymbolResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address);

		OSInt GetModuleCount();
		const UTF8Char *GetModuleName(OSInt index);
		Int64 GetModuleAddr(OSInt index);
		Int32 GetModuleSize(OSInt index);
	};
};
#endif
