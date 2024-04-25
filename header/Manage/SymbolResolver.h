#ifndef _SM_MANAGE_SYMBOLRESOLVER
#define _SM_MANAGE_SYMBOLRESOLVER
#include "Manage/Process.h"
#include "Manage/AddressResolver.h"
#include "Data/ArrayListStringNN.h"
#include "Data/ArrayListUInt64.h"

namespace Manage
{
	class SymbolResolver : public Manage::AddressResolver
	{
	private:
		Data::ArrayListStringNN modNames;
		Data::ArrayListUInt64 modBaseAddrs;
		Data::ArrayListUInt64 modSizes;
		NN<Manage::Process> proc;

	public:
		SymbolResolver(NN<Manage::Process> proc);
		virtual ~SymbolResolver();

		virtual UTF8Char *ResolveName(UTF8Char *buff, UInt64 address);

		UOSInt GetModuleCount();
		Optional<Text::String> GetModuleName(UOSInt index);
		UInt64 GetModuleAddr(UOSInt index);
		UInt64 GetModuleSize(UOSInt index);
	};
}
#endif
