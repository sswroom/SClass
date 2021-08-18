#ifndef _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTEXPLORERFORM
#include "Data/StringUTF8Map.h"
#include "Net/MQTTClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMQTTExplorerForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const UTF8Char *topic;
				UTF8Char *currValue;
				Bool updated;
				UOSInt recvCnt;
				Int64 lastRecvTime;
				Int64 dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::MQTTClient *client;
			IO::LogTool *log;
			Sync::Mutex *topicMut;
			Data::StringUTF8Map<TopicStatus*> *topicMap;
			Bool topicListChanged;
			TopicStatus *currTopic;
			Media::DrawImage *dispImg;

			UI::GUIPanel *pnlConnect;
			UI::GUILabel *lblHost;
			UI::GUITextBox *txtHost;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUILabel *lblUsername;
			UI::GUITextBox *txtUsername;
			UI::GUILabel *lblPassword;
			UI::GUITextBox *txtPassword;
			UI::GUICheckBox *chkSSL;
			UI::GUILabel *lblStatus;
			UI::GUIButton *btnStart;
			UI::GUIListView *lvTopic;
			UI::GUIVSplitter *vspTopic;
			UI::GUIPictureBoxSimple *pbTopic;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTopicSelChg(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, const UTF8Char *topic, const UInt8 *message, UOSInt msgSize);

			void UpdateTopicChart();
			void ServerStop();
		public:
			AVIRMQTTExplorerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRMQTTExplorerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
