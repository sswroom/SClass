#ifndef _SM_NET_MACINFOLIST
#define _SM_NET_MACINFOLIST
#include "Data/ArrayList.h"
#include "Net/MACInfo.h"
namespace Net
{
	class MACInfoList
	{
	private:
		Data::ArrayList<Net::MACInfo::MACEntry*> *dataList;
		Bool modified;

	public:
		MACInfoList();
		~MACInfoList();

		UOSInt GetCount();
		const Net::MACInfo::MACEntry *GetItem(UOSInt index);
		OSInt GetIndex(UInt64 macInt);
		const Net::MACInfo::MACEntry *GetEntry(UInt64 macInt);
		UOSInt SetEntry(UInt64 macInt, const UTF8Char *name);
		void Load();
		Bool Store();
	};
}
#endif
