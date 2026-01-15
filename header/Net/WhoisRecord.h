#ifndef _SM_NET_WHOISRECORD
#define _SM_NET_WHOISRECORD
#include "Data/ArrayListStringNN.h"
namespace Net
{
	class WhoisRecord
	{
	private:
		UInt32 recordIP;
		Data::ArrayListStringNN items;
		UInt32 startIP;
		UInt32 endIP;

	public:
		WhoisRecord(UInt32 recordIP);
		~WhoisRecord();

		void AddItem(UnsafeArray<const UTF8Char> item, UIntOS itemLen);
		UIntOS GetCount() const;
		Optional<Text::String> GetItem(UIntOS index) const;
		Data::ArrayIterator<NN<Text::String>> Iterator() const;

		UnsafeArrayOpt<UTF8Char> GetNetworkName(UnsafeArray<UTF8Char> buff);
		UnsafeArrayOpt<UTF8Char> GetCountryCode(UnsafeArray<UTF8Char> buff);
		UInt32 GetStartIP();
		UInt32 GetEndIP();
	};
}
#endif
