#ifndef _SM_IO_BTCONTROLLER
#define _SM_IO_BTCONTROLLER
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "Text/String.h"

namespace IO
{
	class BTController
	{
	public:
		typedef void (__stdcall *LEScanHandler)(void *userObj, UInt64 mac, Int32 rssi, const Char *name);

		class BTDevice
		{
		private:
			void *internalData;
			void *hRadio;
			UInt8 *devInfo;
			void *clsData;
		public:
			BTDevice(void *internalData, void *hRadio, void *devInfo);
			~BTDevice();

			const UTF8Char *GetName();
			UInt8 *GetAddress();
			UInt32 GetDevClass();
			Bool IsConnected();
			Bool IsRemembered();
			Bool IsAuthenticated();
			void GetLastSeen(Data::DateTime *dt);
			void GetLastUsed(Data::DateTime *dt);

			Bool Pair(const UTF8Char *key);
			Bool Unpair();

			UOSInt QueryServices(Data::ArrayList<void*> *guidList);
			void FreeServices(Data::ArrayList<void*> *guidList);
			Bool EnableService(void *guid, Bool toEnable);
		};
	private:
		void *internalData;
		void *hand;
		UInt8 addr[6];
		Text::String *name;
		UInt32 devClass;
		UInt16 subversion;
		UInt16 manufacturer;

		Bool leScanning;
		Bool leScanToStop;
		LEScanHandler leHdlr;
		void *leHdlrObj;

		static UInt32 __stdcall LEScanThread(void *userObj);
	public:
		BTController(void *internalData, void *hand);
		~BTController();

		OSInt CreateDevices(Data::ArrayList<BTDevice*> *devList, Bool toSearch);

		UInt8 *GetAddress();
		Text::String *GetName();
		UInt32 GetDevClass();
		UInt16 GetManufacturer();
		UInt16 GetSubversion();

		void LEScanHandleResult(LEScanHandler leHdlr, void *leHdlrObj);
		Bool LEScanBegin();
		Bool LEScanEnd();
	};
}
#endif
