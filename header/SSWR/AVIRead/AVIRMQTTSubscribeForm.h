#ifndef _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBEFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBEFORM
#include "Data/StringMap.h"
#include "Net/MQTTConn.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxSimple.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"
#include "UI/ListBoxLogger.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMQTTSubscribeForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				NotNullPtr<Text::String> topic;
				UTF8Char *currValue;
				UOSInt currValueLen;
				Bool updated;
				UOSInt recvCnt;
				Int64 lastRecvTime;
				Int64 dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MQTTConn *client;
			IO::LogTool log;
			NotNullPtr<UI::ListBoxLogger> logger;
			Sync::Mutex topicMut;
			Data::StringMap<TopicStatus*> topicMap;
			Bool topicListChanged;
			TopicStatus *currTopic;
			Media::DrawImage *dispImg;

			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpStatus;
			NotNullPtr<UI::GUILabel> lblHost;
			NotNullPtr<UI::GUITextBox> txtHost;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUILabel> lblUsername;
			NotNullPtr<UI::GUITextBox> txtUsername;
			NotNullPtr<UI::GUILabel> lblPassword;
			NotNullPtr<UI::GUITextBox> txtPassword;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<UI::GUITabPage> tpSTopic;
			NotNullPtr<UI::GUILabel> lblSTopic;
			NotNullPtr<UI::GUITextBox> txtSTopic;
			NotNullPtr<UI::GUIButton> btnSTopic;
			NotNullPtr<UI::GUIListBox> lbSTopic;

			NotNullPtr<UI::GUITabPage> tpPublish;
			NotNullPtr<UI::GUILabel> lblPublishTopic;
			NotNullPtr<UI::GUITextBox> txtPublishTopic;
			NotNullPtr<UI::GUILabel> lblPublishMessage;
			NotNullPtr<UI::GUITextBox> txtPublishMessage;
			NotNullPtr<UI::GUIButton> btnPublish;

			NotNullPtr<UI::GUITabPage> tpTopic;
			NotNullPtr<UI::GUIListView> lvTopic;
			NotNullPtr<UI::GUIVSplitter> vspTopic;
			NotNullPtr<UI::GUIPictureBoxSimple> pbTopic;

			NotNullPtr<UI::GUITabPage> tpLog;
			NotNullPtr<UI::GUIListBox> lbLog;
			NotNullPtr<UI::GUITextBox> txtLog;

			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnSTopicClicked(void *userObj);
			static void __stdcall OnLogSelChg(void *userObj);
			static void __stdcall OnTopicSelChg(void *userObj);
			static void __stdcall OnPublishClicked(void *userObj);
			static void __stdcall OnPingTimerTick(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnPublishMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &message);

			void UpdateTopicChart();
			void ServerStop();
		public:
			AVIRMQTTSubscribeForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTSubscribeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
