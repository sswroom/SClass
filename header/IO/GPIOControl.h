#ifndef _SM_IO_GPIOCONTROL
#define _SM_IO_GPIOCONTROL
#include "AnyType.h"
#include "IO/IOPin.h"
#include "Text/CString.h"

namespace IO
{
	class GPIOControl
	{
	public:
		typedef void (CALLBACKFUNC InterruptHandler)(AnyType userObj);	
	private:
		struct ClassData;
		ClassData *clsData;
	public:
		GPIOControl();
		~GPIOControl();

		Bool IsError();
		UIntOS GetPinCount();

		Bool IsPinHigh(UIntOS pinNum);
		Bool IsPinOutput(UIntOS pinNum);
		UIntOS GetPinMode(UIntOS pinNum);
		Bool SetPinOutput(UIntOS pinNum, Bool isOutput);
		Bool SetPinState(UIntOS pinNum, Bool isHigh);
		Bool SetPullType(UIntOS pinNum, IO::IOPin::PullType pt);

		Bool InterruptEnable(UIntOS pinNum, Bool enable);
		void HandleInterrupt(InterruptHandler hdlr, AnyType userObj);
		void UnhandleInterrupt(InterruptHandler hdlr, AnyType userObj);

		void SetEventOnHigh(UIntOS pinNum, Bool enable);
		void SetEventOnLow(UIntOS pinNum, Bool enable);
		void SetEventOnRaise(UIntOS pinNum, Bool enable);
		void SetEventOnFall(UIntOS pinNum, Bool enable);
		Bool HasEvent(UIntOS pinNum);
		void ClearEvent(UIntOS pinNum);

		static Text::CStringNN PinModeGetName(UIntOS pinNum, UIntOS pinMode);
	};
}
#endif
