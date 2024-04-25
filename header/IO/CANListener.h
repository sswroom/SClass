#ifndef _SM_IO_CANLISTENER
#define _SM_IO_CANLISTENER
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class CANListener
	{
	public:
		virtual ~CANListener(){};

		virtual void CANStop() = 0;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const = 0;
	};
}
#endif
