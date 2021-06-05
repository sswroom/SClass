#ifndef _SM_NET_SNS_SNSINSTAGRAM
#define _SM_NET_SNS_SNSINSTAGRAM
#include "Data/StringUTF8Map.h"
#include "Net/SocketFactory.h"
#include "Net/SNS/SNSControl.h"
#include "Net/WebSite/WebSiteInstagramControl.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace SNS
	{
		class SNSInstagram : public SNSControl
		{
		private:
			Net::WebSite::WebSiteInstagramControl *ctrl;
			const UTF8Char *channelId;
			const UTF8Char *chName;
			const UTF8Char *chDesc;
			Bool chError;
			Data::StringUTF8Map<SNSItem *> *itemMap;

		public:
			SNSInstagram(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *channelId);
			virtual ~SNSInstagram();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual const UTF8Char *GetChannelId();
			virtual const UTF8Char *GetName();
			virtual UTF8Char *GetDirName(UTF8Char *dirName);
			virtual UOSInt GetCurrItems(Data::ArrayList<SNSItem*> *itemList);
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
