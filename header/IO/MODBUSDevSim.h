#ifndef _SM_IO_MODBUSDEVSIM
#define _SM_IO_MODBUSDEVSIM
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class MODBUSDevSim
	{
	public:
		virtual ~MODBUSDevSim() {}

		virtual Text::CStringNN GetName() = 0;
		virtual UIntOS GetValueCount() = 0;
		virtual Text::CString GetValueName(UIntOS index) = 0;
		virtual Bool GetValue(UIntOS index, NN<Text::StringBuilderUTF8> sb) = 0;
		virtual Bool ToggleValue(UIntOS index) = 0;

		virtual Bool ReadCoil(UInt16 coilAddr, OutParam<Bool> value) = 0;
		virtual Bool ReadDescreteInput(UInt16 inputAddr, OutParam<Bool> value) = 0;
		virtual Bool ReadHoldingRegister(UInt16 regAddr, OutParam<UInt16> value) = 0;
		virtual Bool ReadInputRegister(UInt16 regAddr, OutParam<UInt16> value) = 0;
		virtual Bool WriteCoil(UInt16 coilAddr, Bool isHigh) = 0;
		virtual Bool WriteHoldingRegister(UInt16 regAddr, UInt16 val) = 0;
	};
}
#endif
