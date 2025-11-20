#ifndef _SM_NET_ADDRESSLIST
#define _SM_NET_ADDRESSLIST
#include "Data/ArrayListNN.hpp"
#include "Net/SocketUtil.h"

namespace Net
{
	class AddressList : public Data::ReadingListNN<const Net::SocketUtil::AddressInfo>
	{
	private:
		Data::ArrayListNN<const Net::SocketUtil::AddressInfo> addrList;

	public:
		AddressList();
		virtual ~AddressList();

		virtual UOSInt Add(NN<const Net::SocketUtil::AddressInfo> val);
		virtual UOSInt AddRange(UnsafeArray<NN<const Net::SocketUtil::AddressInfo>> arr, UOSInt cnt);
		virtual Bool Remove(NN<const Net::SocketUtil::AddressInfo> val);
		virtual Optional<const Net::SocketUtil::AddressInfo> RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, NN<const Net::SocketUtil::AddressInfo> val);
		virtual UOSInt IndexOf(NN<const Net::SocketUtil::AddressInfo> val); //-1 = not found
		virtual void Clear();

		virtual UOSInt GetCount() const;
		virtual UOSInt GetCapacity() const;

		virtual NN<const Net::SocketUtil::AddressInfo> GetItemNoCheck(UOSInt index) const;
		virtual Optional<const Net::SocketUtil::AddressInfo> GetItem(UOSInt index) const;
		virtual void SetItem(UOSInt index, NN<const Net::SocketUtil::AddressInfo> val);

		static NN<const Net::SocketUtil::AddressInfo> CloneItem(NN<const Net::SocketUtil::AddressInfo> addr);
		static void FreeItem(NN<const Net::SocketUtil::AddressInfo> addr);
	};
}
#endif
