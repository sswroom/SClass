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

		virtual UIntOS Add(NN<const Net::SocketUtil::AddressInfo> val);
		virtual UIntOS AddRange(UnsafeArray<NN<const Net::SocketUtil::AddressInfo>> arr, UIntOS cnt);
		virtual Bool Remove(NN<const Net::SocketUtil::AddressInfo> val);
		virtual Optional<const Net::SocketUtil::AddressInfo> RemoveAt(UIntOS index);
		virtual void Insert(UIntOS index, NN<const Net::SocketUtil::AddressInfo> val);
		virtual UIntOS IndexOf(NN<const Net::SocketUtil::AddressInfo> val); //-1 = not found
		virtual void Clear();

		virtual UIntOS GetCount() const;
		virtual UIntOS GetCapacity() const;

		virtual NN<const Net::SocketUtil::AddressInfo> GetItemNoCheck(UIntOS index) const;
		virtual Optional<const Net::SocketUtil::AddressInfo> GetItem(UIntOS index) const;
		virtual void SetItem(UIntOS index, NN<const Net::SocketUtil::AddressInfo> val);

		static NN<const Net::SocketUtil::AddressInfo> CloneItem(NN<const Net::SocketUtil::AddressInfo> addr);
		static void FreeItem(NN<const Net::SocketUtil::AddressInfo> addr);
	};
}
#endif
