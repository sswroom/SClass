#ifndef _SM_MANAGE_SYMBOLRESOLVER
#define _SM_MANAGE_SYMBOLRESOLVER
#include "Manage/Process.h"
#include "Manage/AddressResolver.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/ArrayListUInt32.h"
#include "Data/ArrayListUInt64.h"

namespace Manage
{
	class SymbolResolver : public Manage::AddressResolver
	{
	private:
		Data::ArrayListStrUTF8 *modNames;
		Data::ArrayListUInt64 *modBaseAddrs;
		Data::ArrayListUInt32 *modSizes;
		Manage::Process *proc;

	public:
		SymbolResolver(Manage::Process *proc);
		virtual ~SymbolResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address);

		UOSInt GetModuleCount();
		const UTF8Char *GetModuleName(UOSInt index);
		UInt64 GetModuleAddr(UOSInt index);
		UInt32 GetModuleSize(UOSInt index);
	};
}
#endif
