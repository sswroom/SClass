#ifndef _SM_IO_ED588SIM
#define _SM_IO_ED588SIM
#include "IO/MODBUSDevSim.h"

namespace IO
{
	class ED588Sim : public IO::MODBUSDevSim
	{
	private:
		Bool diValues[8];
		UInt16 diCounts[8];
		Bool relays[8];
	public:
		ED588Sim();
		virtual ~ED588Sim();

		virtual Text::CString GetName();
		virtual UOSInt GetValueCount();
		virtual Text::CString GetValueName(UOSInt index);
		virtual Bool GetValue(UOSInt index, Text::StringBuilderUTF8 *sb);
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
