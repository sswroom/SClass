#ifndef _SM_IO_ED527SIM
#define _SM_IO_ED527SIM
#include "IO/MODBUSDevSim.h"

namespace IO
{
	class ED527Sim : public IO::MODBUSDevSim
	{
	private:
		Bool relays[16];
	public:
		ED527Sim();
		virtual ~ED527Sim();

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
