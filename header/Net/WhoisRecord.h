#ifndef _SM_NET_WHOISRECORD
#define _SM_NET_WHOISRECORD
#include "Data/ArrayListStrUTF8.h"
namespace Net
{
	class WhoisRecord
	{
	private:
		UInt32 recordIP;
		Data::ArrayListStrUTF8 *items;
		UInt32 startIP;
		UInt32 endIP;

	public:
		WhoisRecord(UInt32 recordIP);
		~WhoisRecord();

		void AddItem(const UTF8Char *item);
		OSInt GetCount();
		const UTF8Char *GetItem(OSInt index);

		UTF8Char *GetNetworkName(UTF8Char *buff);
		UTF8Char *GetCountryCode(UTF8Char *buff);
		UInt32 GetStartIP();
		UInt32 GetEndIP();
	};
};
#endif
