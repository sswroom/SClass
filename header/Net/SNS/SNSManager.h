#ifndef _SM_NET_SNS_SNSMANAGER
#define _SM_NET_SNS_SNSMANAGER
#include "AnyType.h"
#include "Data/ArrayListStringNN.h"
#include "Data/List.h"
#include "Data/Usable.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SNS/SNSControl.h"
#include "Sync/MutexUsage.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"

namespace Net
{
	namespace SNS
	{
		class SNSManager : public Data::ReadingListNN<Net::SNS::SNSControl>
		{
		private:
			struct ChannelData
			{
				NN<Net::SNS::SNSControl> ctrl;
				Data::ArrayListStringNN currItems;
				Int64 lastLoadTime;
			};
			
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;
			NN<Text::String> dataPath;
			Sync::Mutex mut;
			Data::ArrayListNN<ChannelData> channelList;
			Bool threadToStop;
			Bool threadRunning;
			Sync::Event threadEvt;
			NN<IO::LogTool> log;

			Optional<Net::SNS::SNSControl> CreateControl(Net::SNS::SNSControl::SNSType type, Text::CStringNN channelId);
			NN<ChannelData> ChannelInit(NN<Net::SNS::SNSControl> ctrl);
			void ChannelAddMessage(NN<ChannelData> channel, NN<Net::SNS::SNSControl::SNSItem> item);
			void ChannelStoreCurr(NN<ChannelData> channel);
			void ChannelUpdate(NN<ChannelData> channel);
			void ChannelReload(NN<ChannelData> channel);

			static UInt32 __stdcall ThreadProc(AnyType userObj);
		public:
			SNSManager(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CString userAgent, Text::CString dataPath, NN<IO::LogTool> log);
			virtual ~SNSManager();

			Optional<Net::SNS::SNSControl> AddChannel(Net::SNS::SNSControl::SNSType type, Text::CStringNN channelId);

			void Use(NN<Sync::MutexUsage> mutUsage);
			virtual UOSInt GetCount() const;
			virtual NN<Net::SNS::SNSControl> GetItemNoCheck(UOSInt index) const;
			virtual Optional<Net::SNS::SNSControl> GetItem(UOSInt index) const;
		};
	}
}
#endif
