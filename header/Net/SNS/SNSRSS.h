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
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Text::EncodingFactory *encFact;
			Text::String *userAgent;
			Text::String *channelId;
			Text::String *chName;
			Text::String *chDesc;
			Data::FastStringMap<SNSItem *> itemMap;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;

			void CalcCRC(const UInt8 *buff, UOSInt size, UInt8 *hashVal);
		public:
			SNSRSS(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent, Text::CString channelId);
			virtual ~SNSRSS();

			virtual Bool IsError();
			virtual SNSType GetSNSType();
			virtual Text::String *GetChannelId();
			virtual Text::String *GetName();
			virtual UTF8Char *GetDirName(UTF8Char *dirName);
			virtual UOSInt GetCurrItems(Data::ArrayList<SNSItem*> *itemList);
			virtual UTF8Char *GetItemShortId(UTF8Char *buff, SNSItem *item);
			virtual Int32 GetMinIntevalMS();
			virtual Bool Reload();
		};
	}
}
#endif
