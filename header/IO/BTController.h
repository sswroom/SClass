#ifndef _SM_IO_BTCONTROLLER
#define _SM_IO_BTCONTROLLER
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/DateTime.h"
#include "Data/UUID.h"
#include "Text/String.h"

namespace IO
{
	class BTController
	{
	public:
		typedef void (CALLBACKFUNC LEScanHandler)(AnyType userObj, UInt64 mac, Int32 rssi, Text::CString name);

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

			NN<Text::String> GetName() const;
			UInt8 *GetAddress();
			UInt32 GetDevClass();
			Bool IsConnected();
			Bool IsRemembered();
			Bool IsAuthenticated();
			void GetLastSeen(Data::DateTime *dt);
			void GetLastUsed(Data::DateTime *dt);

			Bool Pair(const UTF8Char *key);
			Bool Unpair();

			UOSInt QueryServices(NN<Data::ArrayListNN<Data::UUID>> guidList);
			void FreeServices(NN<Data::ArrayListNN<Data::UUID>> guidList);
			Bool EnableService(NN<Data::UUID> guid, Bool toEnable);
		};
	private:
		void *internalData;
		void *hand;
		UInt8 addr[6];
		NN<Text::String> name;
		UInt32 devClass;
		UInt16 subversion;
		UInt16 manufacturer;

		Bool leScanning;
		Bool leScanToStop;
		LEScanHandler leHdlr;
		AnyType leHdlrObj;

		static UInt32 __stdcall LEScanThread(AnyType userObj);
	public:
		BTController(void *internalData, void *hand);
		~BTController();

		OSInt CreateDevices(NN<Data::ArrayListNN<BTDevice>> devList, Bool toSearch);

		UInt8 *GetAddress();
		NN<Text::String> GetName() const;
		UInt32 GetDevClass();
		UInt16 GetManufacturer();
		UInt16 GetSubversion();

		void LEScanHandleResult(LEScanHandler leHdlr, AnyType leHdlrObj);
		Bool LEScanBegin();
		Bool LEScanEnd();
	};
}
#endif
