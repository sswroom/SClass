#ifndef _SM_NET_ADDRESSLIST
#define _SM_NET_ADDRESSLIST
#include "Data/List.h"
#include "Net/SocketUtil.h"

namespace Net
{
	class AddressList : public Data::List<const Net::SocketUtil::AddressInfo*>
	{
	private:


	public:
		AddressList();
		virtual ~AddressList();
	};
}
#endif
