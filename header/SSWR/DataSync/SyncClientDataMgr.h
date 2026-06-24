#ifndef _SM_SSWR_DATASYNC_SYNCCLIENTDATAMGR
#define _SM_SSWR_DATASYNC_SYNCCLIENTDATAMGR
#include "Data/ArrayListArr.hpp"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncClientDataMgr
		{
		private:
			Sync::Mutex mut;
			Data::ArrayListArr<UInt8> dataList;

		public:
			SyncClientDataMgr();
			~SyncClientDataMgr();

			void AddUserData(UnsafeArray<const UInt8> data, UIntOS dataSize);
			UIntOS GetCount();
			UnsafeArrayOpt<const UInt8> GetData(UIntOS index, OutParam<UIntOS> dataSize);
			void RemoveData(UIntOS cnt);
		};
	}
}
#endif
