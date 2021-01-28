#ifndef _SM_IO_NULLIOPIN
#define _SM_IO_NULLIOPIN
#include "IO/IOPin.h"

namespace IO
{
	class NullIOPin : public IOPin
	{
	public:
		NullIOPin();
		virtual ~NullIOPin();

		virtual Bool IsPinHigh();
		virtual Bool IsPinOutput();
		virtual void SetPinOutput(Bool isOutput);
		virtual void SetPinState(Bool isHigh);
		virtual Bool SetPullType(PullType pt);
		virtual UTF8Char *GetName(UTF8Char *buff);
	};
}
#endif
