#ifndef _SM_NET_ADDRESSLIST
#define _SM_NET_ADDRESSLIST
#include "Data/ArrayList.h"
#include "Data/List.h"
#include "Net/SocketUtil.h"

namespace Net
{
	class AddressList : public Data::List<const Net::SocketUtil::AddressInfo*>
	{
	private:
		Data::ArrayList<const Net::SocketUtil::AddressInfo *> *addrList;

	public:
		AddressList();
		virtual ~AddressList();

		virtual UOSInt Add(const Net::SocketUtil::AddressInfo *val);
		virtual UOSInt AddRange(const Net::SocketUtil::AddressInfo **arr, UOSInt cnt);
		virtual Bool Remove(const Net::SocketUtil::AddressInfo *val);
		virtual const Net::SocketUtil::AddressInfo *RemoveAt(UOSInt index);
		virtual void Insert(UOSInt index, const Net::SocketUtil::AddressInfo *val);
		virtual OSInt IndexOf(const Net::SocketUtil::AddressInfo *val); //-1 = not found
		virtual void Clear();

		virtual UOSInt GetCount();
		virtual UOSInt GetCapacity();

		virtual const Net::SocketUtil::AddressInfo *GetItem(UOSInt index);
		virtual void SetItem(UOSInt index, const Net::SocketUtil::AddressInfo *val);

		static const Net::SocketUtil::AddressInfo *CloneItem(const Net::SocketUtil::AddressInfo *addr);
		static void FreeItem(const Net::SocketUtil::AddressInfo *addr);
	};
}
#endif
