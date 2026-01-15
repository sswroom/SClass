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

		virtual UnsafeArrayOpt<UTF8Char> ResolveName(UnsafeArray<UTF8Char> buff, UInt64 address);

		UIntOS GetModuleCount();
		Optional<Text::String> GetModuleName(UIntOS index);
		UInt64 GetModuleAddr(UIntOS index);
		UInt64 GetModuleSize(UIntOS index);
	};
}
#endif
