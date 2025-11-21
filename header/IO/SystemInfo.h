#ifndef _SM_IO_SYSTEMINFO
#define _SM_IO_SYSTEMINFO
#include "Data/ArrayListNN.hpp"
#include "Text/String.h"

namespace IO
{
	class SystemInfo
	{
	public:
		typedef struct
		{
			Optional<Text::String> deviceLocator;
			Optional<Text::String> manufacturer;
			Optional<Text::String> partNo;
			Optional<Text::String> sn;
			UOSInt defSpdMHz;
			UOSInt confSpdMHz;
			UInt32 dataWidth;
			UInt32 totalWidth;
			UInt64 memorySize;
		} RAMInfo;

		typedef enum
		{
			CT_UNKNOWN,
			CT_DESKTOP,
			CT_LPDESKTOP,
			CT_PIZZABOX,
			CT_MINITOWER,
			CT_TOWER,
			CT_PORTABLE,
			CT_LAPTOP,
			CT_NOTEBOOK,
			CT_HANDHELD,
			CT_DOCKINGSTATION,
			CT_AIO,
			CT_SUBNOTEBOOk,
			CT_SPACESAVING,
			CT_LUNCHBOX,
			CT_MAINSERVER,
			CT_EXPANSION,
			CT_SUBCHASSIS,
			CT_BUSEXPANSION,
			CT_PERIPHERAL,
			CT_RAID,
			CT_RACKMOUNT,
			CT_SEALED_CASE_PC,
			CT_MULTISYSTEM,
			CT_COMPACT_PCI,
			CT_ADVANCEDTCA,
			CT_BLADE,
			CT_BLADEENCLOSURE,
			CT_TABLET,
			CT_CONVERTIBLE,
			CT_DETACHABLE,
			CT_IOT_GATEWAY,
			CT_EMBEDDEDPC,
			CT_MINI_PC,
			CT_STICK_PC
		} ChassisType;
	public:
		struct ClassData;
		NN<ClassData> clsData;
	public:
		SystemInfo();
		~SystemInfo();

		UnsafeArrayOpt<UTF8Char> GetPlatformName(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetPlatformSN(UnsafeArray<UTF8Char> sbuff);
		UInt64 GetTotalMemSize();
		UInt64 GetTotalUsableMemSize();
		ChassisType GetChassisType();

		UOSInt GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList);
		void FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList);
	};
}

#endif
