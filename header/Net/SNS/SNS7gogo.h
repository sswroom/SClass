#ifndef _SM_NET_SNS_SNS7GOGO
#define _SM_NET_SNS_SNS7GOGO
#include "Data/FastMapNN.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SNS/SNSControl.h"
#include "Net/WebSite/WebSite7gogoControl.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace SNS
	{
		class SNS7gogo : public SNSControl
		{
		private:
			Net::WebSite::WebSite7gogoControl *ctrl;
			NN<Text::String> channelId;
			NN<Text::String> chName;
			Text::String *chDesc;
			Bool chError;
			Data::FastMapNN<Int64, SNSItem> itemMap;

		public:
			SNS7gogo(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CString channelId);
			virtual ~SNS7gogo();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual NN<Text::String> GetChannelId() const;
			virtual NN<Text::String> GetName() const;
			virtual UTF8Char *GetDirName(UTF8Char *dirName);
			virtual UOSInt GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList);
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, NN<SNSItem> item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
