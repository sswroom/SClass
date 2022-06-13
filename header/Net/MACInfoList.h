#ifndef _SM_NET_MACINFOLIST
#define _SM_NET_MACINFOLIST
#include "Data/ArrayList.h"
#include "Net/MACInfo.h"
namespace Net
{
	class MACInfoList
	{
	private:
		Data::ArrayList<Net::MACInfo::MACEntry*> dataList;
		Bool modified;

	public:
		MACInfoList();
		~MACInfoList();

		UOSInt GetCount();
		const Net::MACInfo::MACEntry *GetItem(UOSInt index);
		OSInt GetIndex(UInt64 macInt);
		const Net::MACInfo::MACEntry *GetEntry(UInt64 macInt);
		const Net::MACInfo::MACEntry *GetEntryOUI(const UInt8 *oui);
		UOSInt SetEntry(UInt64 macInt, Text::CString name);
		UOSInt SetEntry(UInt64 rangeStart, UInt64 rangeEnd, Text::CString name);
		void Load();
		Bool Store();
	};
}
#endif
