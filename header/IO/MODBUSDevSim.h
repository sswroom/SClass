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

		virtual Text::CString GetName() = 0;
		virtual UOSInt GetValueCount() = 0;
		virtual Text::CString GetValueName(UOSInt index) = 0;
		virtual Bool GetValue(UOSInt index, NN<Text::StringBuilderUTF8> sb) = 0;
		virtual Bool ToggleValue(UOSInt index) = 0;

		virtual Bool ReadCoil(UInt16 coilAddr, Bool *value) = 0;
		virtual Bool ReadDescreteInput(UInt16 inputAddr, Bool *value) = 0;
		virtual Bool ReadHoldingRegister(UInt16 regAddr, UInt16 *value) = 0;
		virtual Bool ReadInputRegister(UInt16 regAddr, UInt16 *value) = 0;
		virtual Bool WriteCoil(UInt16 coilAddr, Bool isHigh) = 0;
		virtual Bool WriteHoldingRegister(UInt16 regAddr, UInt16 val) = 0;
	};
}
#endif
