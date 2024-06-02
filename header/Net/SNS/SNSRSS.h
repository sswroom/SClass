#ifndef _SM_NET_SNS_SNSRSS
#define _SM_NET_SNS_SNSRSS
#include "Crypto/Hash/CRC32R.h"
#include "Data/FastStringMapNN.h"
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
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;
			NN<Text::String> channelId;
			NN<Text::String> chName;
			Text::String *chDesc;
			Data::FastStringMapNN<SNSItem> itemMap;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;
			Data::Duration timeout;
			NN<IO::LogTool> log;			

			void CalcCRC(UnsafeArray<const UInt8> buff, UOSInt size, UnsafeArray<UInt8> hashVal);
		public:
			SNSRSS(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent, Text::CStringNN channelId, NN<IO::LogTool> log);
			virtual ~SNSRSS();

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
