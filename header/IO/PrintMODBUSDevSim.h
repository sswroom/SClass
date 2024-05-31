#ifndef _SM_IO_PRINTMODBUSDEVSIM
#define _SM_IO_PRINTMODBUSDEVSIM
#include "IO/MODBUSDevSim.h"

namespace IO
{
	class PrintMODBUSDevSim : public IO::MODBUSDevSim
	{
	public:
		PrintMODBUSDevSim();
		virtual ~PrintMODBUSDevSim();

		virtual Text::CStringNN GetName();
		virtual UOSInt GetValueCount();
		virtual Text::CString GetValueName(UOSInt index);
		virtual Bool GetValue(UOSInt index, NN<Text::StringBuilderUTF8> sb);
		virtual Bool ToggleValue(UOSInt index);

		virtual Bool ReadCoil(UInt16 coilAddr, Bool *value);
		virtual Bool ReadDescreteInput(UInt16 inputAddr, Bool *value);
		virtual Bool ReadHoldingRegister(UInt16 regAddr, UInt16 *value);
		virtual Bool ReadInputRegister(UInt16 regAddr, UInt16 *value);
		virtual Bool WriteCoil(UInt16 coilAddr, Bool isHigh);
		virtual Bool WriteHoldingRegister(UInt16 regAddr, UInt16 val);
	};
}
#endif
