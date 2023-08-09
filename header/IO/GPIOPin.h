#ifndef _SM_IO_GPIOPIN
#define _SM_IO_GPIOPIN
#include "Data/ArrayList.h"
#include "IO/GPIOControl.h"
#include "IO/IOPin.h"

namespace IO
{
	class GPIOPin : public IO::IOPin
	{
	private:
		UInt16 pinNum;
		NotNullPtr<IO::GPIOControl> gpio;
	public:
		GPIOPin(NotNullPtr<IO::GPIOControl> gpio, UInt16 pinNum);
		virtual ~GPIOPin();

		virtual Bool IsError();

		virtual Bool IsPinHigh();
		virtual Bool IsPinOutput();
		virtual void SetPinOutput(Bool isOutput);
		virtual void SetPinState(Bool isHigh);
		virtual Bool SetPullType(PullType pt);
		virtual UTF8Char *GetName(UTF8Char *buff);

		virtual void SetEventOnHigh(Bool enable);
		virtual void SetEventOnLow(Bool enable);
		virtual void SetEventOnRaise(Bool enable);
		virtual void SetEventOnFall(Bool enable);
		virtual Bool HasEvent();
		virtual void ClearEvent();
	};
}
#endif
