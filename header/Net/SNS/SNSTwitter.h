#ifndef _SM_NET_SNS_SNSTWITTER
#define _SM_NET_SNS_SNSTWITTER
#include "Data/FastMap.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SNS/SNSControl.h"
#include "Net/WebSite/WebSiteTwitterControl.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace SNS
	{
		class SNSTwitter : public SNSControl
		{
		private:
			Net::WebSite::WebSiteTwitterControl *ctrl;
			NotNullPtr<Text::String> channelId;
			NotNullPtr<Text::String> chName;
			Text::String *chDesc;
			Bool chError;
			Data::FastMap<Int64, SNSItem *> itemMap;

		public:
			SNSTwitter(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent, Text::CString channelId);
			virtual ~SNSTwitter();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual NotNullPtr<Text::String> GetChannelId() const;
			virtual NotNullPtr<Text::String> GetName() const;
			virtual UTF8Char *GetDirName(UTF8Char *dirName);
			virtual UOSInt GetCurrItems(NotNullPtr<Data::ArrayList<SNSItem*>> itemList);
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
