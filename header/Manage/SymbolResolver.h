#ifndef _SM_MANAGE_SYMBOLRESOLVER
#define _SM_MANAGE_SYMBOLRESOLVER
#include "Manage/Process.h"
#include "Manage/AddressResolver.h"
#include "Data/ArrayListString.h"
#include "Data/ArrayListUInt64.h"

namespace Manage
{
	class SymbolResolver : public Manage::AddressResolver
	{
	private:
		Data::ArrayListString modNames;
		Data::ArrayListUInt64 modBaseAddrs;
		Data::ArrayListUInt64 modSizes;
		Manage::Process *proc;

	public:
		SymbolResolver(Manage::Process *proc);
		virtual ~SymbolResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address);

		UOSInt GetModuleCount();
		Text::String *GetModuleName(UOSInt index);
		UInt64 GetModuleAddr(UOSInt index);
		UInt64 GetModuleSize(UOSInt index);
	};
}
#endif
