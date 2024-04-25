#ifndef _SM_IO_VBOXMANAGER
#define _SM_IO_VBOXMANAGER
#include "Data/ArrayList.h"
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
		Data::ArrayList<VMId*> vms;
		Text::String *progPath;
		Text::String *version;

		UOSInt GetVMList(Data::ArrayList<VMId*> *vms);
	public:
		VBoxManager();
		~VBoxManager();

		Text::String *GetVersion() const;
		const Data::ArrayList<VMId*> *GetVMS() const;
		VBoxVMInfo *GetVMInfo(VMId *vm) const;
	};
}
#endif
