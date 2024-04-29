#ifndef _SM_IO_VBOXMANAGER
#define _SM_IO_VBOXMANAGER
#include "Data/ArrayListNN.h"
#include "Data/UUID.h"
#include "IO/VBoxVMInfo.h"
#include "Text/String.h"

namespace IO
{
	class VBoxManager
	{
	public:
		struct VMId
		{
			NN<Text::String> name;
			Data::UUID uuid;
		};
	private:
		Data::ArrayListNN<VMId> vms;
		Text::String *progPath;
		Text::String *version;

		UOSInt GetVMList(NN<Data::ArrayListNN<VMId>> vms);
	public:
		VBoxManager();
		~VBoxManager();

		Text::String *GetVersion() const;
		NN<const Data::ArrayListNN<VMId>> GetVMS() const;
		Optional<VBoxVMInfo> GetVMInfo(NN<VMId> vm) const;
	};
}
#endif
