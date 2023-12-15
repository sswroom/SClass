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

		void AddItem(const UTF8Char *item, UOSInt itemLen);
		UOSInt GetCount();
		Optional<Text::String> GetItem(UOSInt index);

		UTF8Char *GetNetworkName(UTF8Char *buff);
		UTF8Char *GetCountryCode(UTF8Char *buff);
		UInt32 GetStartIP();
		UInt32 GetEndIP();
	};
}
#endif
