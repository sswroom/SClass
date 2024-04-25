#ifndef _SM_NET_SNS_SNSINSTAGRAM
#define _SM_NET_SNS_SNSINSTAGRAM
#include "Data/FastStringMap.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
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
			NN<Text::String> channelId;
			NN<Text::String> chName;
			Text::String *chDesc;
			Bool chError;
			Data::FastStringMap<SNSItem *> itemMap;

		public:
			SNSInstagram(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CString channelId);
			virtual ~SNSInstagram();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual NN<Text::String> GetChannelId() const;
			virtual NN<Text::String> GetName() const;
			virtual UTF8Char *GetDirName(UTF8Char *dirName);
			virtual UOSInt GetCurrItems(NN<Data::ArrayList<SNSItem*>> itemList);
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
