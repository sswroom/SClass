#ifndef _SM_NET_SNS_SNSTWITTER
#define _SM_NET_SNS_SNSTWITTER
#include "Data/FastMapNN.hpp"
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
			NN<Net::WebSite::WebSiteTwitterControl> ctrl;
			NN<Text::String> channelId;
			NN<Text::String> chName;
			Optional<Text::String> chDesc;
			Bool chError;
			Data::FastMapNN<Int64, SNSItem> itemMap;

		public:
			SNSTwitter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId);
			virtual ~SNSTwitter();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual NN<Text::String> GetChannelId() const;
			virtual NN<Text::String> GetName() const;
			virtual UnsafeArray<UTF8Char> GetDirName(UnsafeArray<UTF8Char> dirName);
			virtual UOSInt GetCurrItems(NN<Data::ArrayListNN<SNSItem>> itemList);
			virtual UnsafeArray<UTF8Char> GetItemShortId(UnsafeArray<UTF8Char> buff, NN<SNSItem> item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
