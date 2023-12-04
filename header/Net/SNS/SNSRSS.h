#ifndef _SM_NET_SNS_SNSRSS
#define _SM_NET_SNS_SNSRSS
#include "Crypto/Hash/CRC32R.h"
#include "Data/FastStringMap.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SNS/SNSControl.h"
#include "Sync/Mutex.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace SNS
	{
		class SNSRSS : public SNSControl
		{
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Text::EncodingFactory *encFact;
			Text::String *userAgent;
			NotNullPtr<Text::String> channelId;
			NotNullPtr<Text::String> chName;
			Text::String *chDesc;
			Data::FastStringMap<SNSItem *> itemMap;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;
			Data::Duration timeout;
			NotNullPtr<IO::LogTool> log;			

			void CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *hashVal);
		public:
			SNSRSS(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::String *userAgent, Text::CString channelId, NotNullPtr<IO::LogTool> log);
			virtual ~SNSRSS();

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
