#ifndef _SM_SSWR_VAMS_VAMSBTLIST
#define _SM_SSWR_VAMS_VAMSBTLIST
#include "Data/ArrayListNN.hpp"
#include "Data/FastMap.hpp"
#include "Data/FastMapNN.hpp"
#include "Data/FastStringMapNN.hpp"
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
    			NN<Text::String> avlNo;
				Int32 progId;
    			Int64 lastDevTS;
    			Int16 rssi;
				Int64 lastProcTS;
			};
		private:
			Sync::Mutex mut;
			Data::FastMapNN<Int32, Data::FastStringMapNN<AvlBleItem>> itemMap;
			Data::FastMap<Int32, Int64> kaMap;

		public:
			VAMSBTList();
			~VAMSBTList();

			void AddItem(NN<Text::String> avlNo, Int32 progId, Int64 ts, Int16 rssi);
			UOSInt QueryByProgId(NN<Data::ArrayListNN<AvlBleItem>> itemList, Int32 progId, Int32 timeoutIntervalMs);
			void KARecv(Int32 progId);
			Int64 GetLastKeepAlive(Int32 progId);
			Bool HasProg(Int32 progId);
			UOSInt GetProgList(NN<Data::ArrayList<Int32>> progList);
		};
	}
}
#endif
