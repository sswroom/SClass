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
		class SNSManager : public Data::ReadingList<Net::SNS::SNSControl*>
		{
		private:
			struct ChannelData
			{
				Net::SNS::SNSControl *ctrl;
				Data::ArrayListStringNN currItems;
				Int64 lastLoadTime;
			};
			
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;
			NotNullPtr<Text::String> dataPath;
			Sync::Mutex mut;
			Data::ArrayList<ChannelData*> channelList;
			Bool threadToStop;
			Bool threadRunning;
			Sync::Event threadEvt;
			NotNullPtr<IO::LogTool> log;

			Net::SNS::SNSControl *CreateControl(Net::SNS::SNSControl::SNSType type, Text::CString channelId);
			ChannelData *ChannelInit(Net::SNS::SNSControl *ctrl);
			void ChannelAddMessage(ChannelData *channel, Net::SNS::SNSControl::SNSItem *item);
			void ChannelStoreCurr(ChannelData *channel);
			void ChannelUpdate(ChannelData *channel);
			void ChannelReload(ChannelData *channel);

			static UInt32 __stdcall ThreadProc(AnyType userObj);
		public:
			SNSManager(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CString userAgent, Text::CString dataPath, NotNullPtr<IO::LogTool> log);
			virtual ~SNSManager();

			Net::SNS::SNSControl *AddChannel(Net::SNS::SNSControl::SNSType type, Text::CString channelId);

			void Use(NotNullPtr<Sync::MutexUsage> mutUsage);
			virtual UOSInt GetCount() const;
			virtual Net::SNS::SNSControl* GetItem(UOSInt index) const;
		};
	}
}
#endif
