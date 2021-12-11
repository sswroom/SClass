#ifndef _SM_NET_SNS_SNSMANAGER
#define _SM_NET_SNS_SNSMANAGER
#include "Data/ArrayListString.h"
#include "Data/List.h"
#include "Data/Usable.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SNS/SNSControl.h"
#include "Sync/MutexUsage.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace SNS
	{
		class SNSManager : public Data::ReadingList<Net::SNS::SNSControl*>
		{
		private:
			typedef struct
			{
				Net::SNS::SNSControl *ctrl;
				Data::ArrayListString *currItems;
				Int64 lastLoadTime;
			} ChannelData;
			
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Text::EncodingFactory *encFact;
			const UTF8Char *userAgent;
			const UTF8Char *dataPath;
			Sync::Mutex *mut;
			Data::ArrayList<ChannelData*> *channelList;
			Bool threadToStop;
			Bool threadRunning;
			Sync::Event *threadEvt;

			Net::SNS::SNSControl *CreateControl(Net::SNS::SNSControl::SNSType type, const UTF8Char *channelId);
			ChannelData *ChannelInit(Net::SNS::SNSControl *ctrl);
			void ChannelAddMessage(ChannelData *channel, Net::SNS::SNSControl::SNSItem *item);
			void ChannelStoreCurr(ChannelData *channel);
			void ChannelUpdate(ChannelData *channel);
			void ChannelReload(ChannelData *channel);

			static UInt32 __stdcall ThreadProc(void *userObj);
		public:
			SNSManager(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *dataPath);
			virtual ~SNSManager();

			Net::SNS::SNSControl *AddChannel(Net::SNS::SNSControl::SNSType type, const UTF8Char *channelId);

			void Use(Sync::MutexUsage *mutUsage);
			virtual UOSInt GetCount();
			virtual Net::SNS::SNSControl* GetItem(UOSInt index);
		};
	}
}
#endif
