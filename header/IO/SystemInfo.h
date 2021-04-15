#ifndef _SM_IO_SYSTEMINFO
#define _SM_IO_SYSTEMINFO
#include "Data/ArrayList.h"

namespace IO
{
	class SystemInfo
	{
	public:
		typedef struct
		{
			const UTF8Char *deviceLocator;
			const UTF8Char *manufacturer;
			const UTF8Char *partNo;
			const UTF8Char *sn;
			OSInt defSpdMHz;
			OSInt confSpdMHz;
			Int32 dataWidth;
			Int32 totalWidth;
			Int64 memorySize;
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
		void *clsData;
	public:
		SystemInfo();
		~SystemInfo();

		UTF8Char *GetPlatformName(UTF8Char *sbuff);
		UTF8Char *GetPlatformSN(UTF8Char *sbuff);
		UInt64 GetTotalMemSize();
		UInt64 GetTotalUsableMemSize();
		ChassisType GetChassisType();

		UOSInt GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList);
		void FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList);
	};
}

#endif
