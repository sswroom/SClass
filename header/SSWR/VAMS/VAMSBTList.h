#ifndef _SM_SSWR_VAMS_VAMSBTLIST
#define _SM_SSWR_VAMS_VAMSBTLIST
#include "Data/FastStringMap.h"
#include "Data/Int32Map.h"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace VAMS
	{
		class VAMSBTList
		{
		public:
			struct AvlBleItem
			{
    			Text::String *avlNo;
				Int32 progId;
    			Int64 lastDevTS;
    			Int16 rssi;
				Int64 lastProcTS;
			};
		private:
			Sync::Mutex mut;
			Data::Int32Map<Data::FastStringMap<AvlBleItem*>*> itemMap;
			Data::Int32Map<Int64> kaMap;

		public:
			VAMSBTList();
			~VAMSBTList();

			void AddItem(Text::String *avlNo, Int32 progId, Int64 ts, Int16 rssi);
			UOSInt QueryByProgId(Data::ArrayList<AvlBleItem *> *itemList, Int32 progId, Int32 timeoutIntervalMs);
			void KARecv(Int32 progId);
			Int64 GetLastKeepAlive(Int32 progId);
			Bool HasProg(Int32 progId);
			UOSInt GetProgList(Data::ArrayList<Int32> *progList);
		};
	}
}
#endif
