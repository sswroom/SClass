#ifndef _SM_NET_MACINFOLIST
#define _SM_NET_MACINFOLIST
#include "Data/ArrayListNN.hpp"
#include "Net/MACInfo.h"
namespace Net
{
	class MACInfoList
	{
	private:
		Data::ArrayListNN<Net::MACInfo::MACEntry> dataList;
		Bool modified;

	public:
		MACInfoList();
		~MACInfoList();

		UOSInt GetCount() const;
		NN<const Net::MACInfo::MACEntry> GetItemNoCheck(UOSInt index) const;
		Optional<const Net::MACInfo::MACEntry> GetItem(UOSInt index) const;
		OSInt GetIndex(UInt64 mac64Int);
		Optional<const Net::MACInfo::MACEntry> GetEntry(UInt64 mac64Int);
		Optional<const Net::MACInfo::MACEntry> GetEntryOUI(const UInt8 *oui);
		UOSInt SetEntry(UInt64 mac64Int, Text::CStringNN name);
		UOSInt SetEntry(UInt64 rangeStart, UInt64 rangeEnd, Text::CStringNN name);
		void Load();
		Bool Store();
	};
}
#endif
