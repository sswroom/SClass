#ifndef _SM_SSWR_DATASYNC_SYNCCLIENTDATAMGR
#define _SM_SSWR_DATASYNC_SYNCCLIENTDATAMGR
#include "Data/ArrayListObj.hpp"
#include "Sync/Mutex.h"

namespace SSWR
{
	namespace DataSync
	{
		class SyncClientDataMgr
		{
		private:
			Sync::Mutex mut;
			Data::ArrayListObj<UInt8*> dataList;

		public:
			SyncClientDataMgr();
			~SyncClientDataMgr();

			void AddUserData(const UInt8 *data, UOSInt dataSize);
			UOSInt GetCount();
			const UInt8 *GetData(UOSInt index, UOSInt *dataSize);
			void RemoveData(UOSInt cnt);
		};
	}
}
#endif
