#ifndef _SM_IO_VBOXVMINFO
#define _SM_IO_VBOXVMINFO
#include "Data/Timestamp.h"
#include "Text/String.h"

namespace IO
{
	class VBoxVMInfo
	{
	public:
		enum class State
		{
			Unknown,
			PoweredOff
		};
	private:
		Text::String *name;
		State state;
		Data::Timestamp stateSince;

	public:
		VBoxVMInfo(Text::CString name);
		~VBoxVMInfo();

		Text::String *GetName() const;
		State GetState() const;
		Data::Timestamp GetStateSince() const;

		void SetState(State state);
		void SetStateSince(Data::Timestamp stateSince);

		static Text::CString StateGetName(State val);
	};
}
#endif
