#ifndef _SM_WIN32_SMBIOS
#define _SM_WIN32_SMBIOS
#include "Data/ArrayList.h"
#include "Text/StringBuilderUTF.h"

namespace Win32
{
	class SMBIOS
	{
	public:
		typedef enum
		{
			MFF_OTHER = 1,
			MFF_UNKNOWN = 2,
			MFF_SIMM = 3,
			MFF_SIP = 4,
			MFF_CHIP = 5,
			MFF_DIP = 6,
			MFF_ZIP = 7,
			MFF_PROPRIETARY_CARD = 8,
			MFF_DIMM = 9,
			MFF_TSOP = 10,
			MFF_ROW_OF_CHIPS = 11,
			MFF_RIMM = 12,
			MFF_SODIMM = 13,
			MFF_SRIMM = 14,
			MFF_FB_DIMM = 15
		} MemoryFormFactor;

		typedef enum
		{
			MT_OTHER = 1,
			MT_UNKNOWN = 2,
			MT_DRAM = 3,
			MT_EDRAM = 4,
			MT_VRAM = 5,
			MT_SRAM = 6,
			MT_RAM = 7,
			MT_ROM = 8,
			MT_FLASH = 9,
			MT_EEPROM = 10,
			MT_FEPROM = 11,
			MT_EPROM = 12,
			MT_CDRAM = 13,
			MT_3DRAM = 14,
			MT_SDRAM = 15,
			MT_SGRAM = 16,
			MT_RDRAM = 17,
			MT_DDR = 18,
			MT_DDR2 = 19,
			MT_DDR2_FBDIMM = 20,
			MT_DDR3 = 24,
			MT_FBD2 = 25,
			MT_DDR4 = 26,
			MT_LPDDR = 27,
			MT_LPDDR2 = 28,
			MT_LPDDR3 = 29,
			MT_LPDDR4 = 30
		} MemoryType;

		typedef struct
		{
			UInt16 memArrayHandle;
			UInt16 memErrHandle;
			UInt16 totalWidthBits;
			UInt16 dataWidthBits;
			UInt64 memorySize;
			MemoryFormFactor formFactor;
			UInt8 deviceSet;
			const Char *deviceLocator;
			const Char *bankLocator;
			MemoryType memType;
			UInt16 maxSpeedMTs; //ddr4-2400 = 2400 (Transfer Speed, Clock Speed = 1200MHz)
			const Char *manufacturer;
			const Char *sn;
			const Char *assetTag;
			const Char *partNo;
			UInt8 attributes;
			UInt16 confSpeedMTs;
			Double minVolt;
			Double maxVolt;
			Double confVolt;
		} MemoryDeviceInfo;
	private:
		const UInt8 *smbiosBuff;
		UOSInt smbiosBuffSize;
		UInt8 *relPtr;

		void GetDataType(Data::ArrayList<const UInt8 *> *dataList, UInt8 dataType);
	public:
		SMBIOS(const UInt8 *smbiosBuff, UOSInt smbiosBuffSize, UInt8 *relPtr);
		~SMBIOS();

		UOSInt GetMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList);
		void FreeMemoryInfo(Data::ArrayList<MemoryDeviceInfo*> *memList);

		UTF8Char *GetPlatformName(UTF8Char *buff);
		UTF8Char *GetPlatformSN(UTF8Char *buff);
		Int32 GetChassisType();
		Bool ToString(Text::StringBuilderUTF *sb);
		static void GetProcessorFamily(Text::StringBuilderUTF *sb, Int32 family);
		static void GetConnectorType(Text::StringBuilderUTF *sb, UInt8 type);
	};
}
#endif
