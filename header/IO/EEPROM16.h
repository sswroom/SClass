#ifndef _SM_IO_EEPROM16
#define _SM_IO_EEPROM16

namespace IO
{
	class EEPROM16
	{
	public:
		static UInt16 GetSize();
		static UInt8 ReadByte(UInt16 addr);
		static Bool WriteByte(UInt16 addr, UInt8 data);
	};
};
#endif
