#ifndef _SM_IO_GPIOCONTROL
#define _SM_IO_GPIOCONTROL
#include "IO/IOPin.h"

namespace IO
{
	class GPIOControl
	{
	public:
		typedef void (__stdcall *InterruptHandler)(void *userObj);	
	private:
		void *clsData;
	public:
		GPIOControl();
		~GPIOControl();

		Bool IsError();
		UOSInt GetPinCount();

		Bool IsPinHigh(UOSInt pinNum);
		Bool IsPinOutput(UOSInt pinNum);
		UOSInt GetPinMode(UOSInt pinNum);
		Bool SetPinOutput(UOSInt pinNum, Bool isOutput);
		Bool SetPinState(UOSInt pinNum, Bool isHigh);
		Bool SetPullType(UOSInt pinNum, IO::IOPin::PullType pt);

		Bool InterruptEnable(UOSInt pinNum, Bool enable);
		void HandleInterrupt(InterruptHandler hdlr, void *userObj);
		void UnhandleInterrupt(InterruptHandler hdlr, void *userObj);

		void SetEventOnHigh(UOSInt pinNum, Bool enable);
		void SetEventOnLow(UOSInt pinNum, Bool enable);
		void SetEventOnRaise(UOSInt pinNum, Bool enable);
		void SetEventOnFall(UOSInt pinNum, Bool enable);
		Bool HasEvent(UOSInt pinNum);
		void ClearEvent(UOSInt pinNum);

		static const UTF8Char *PinModeGetName(UOSInt pinNum, UOSInt pinMode);
	};
}
#endif
