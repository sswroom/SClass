#ifndef _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBEFORM
#define _SM_SSWR_AVIREAD_AVIRMQTTSUBSCRIBEFORM
#include "Data/StringMapNN.hpp"
#include "Net/MQTTConn.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
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
				NN<Text::String> topic;
				UnsafeArray<UTF8Char> currValue;
				UOSInt currValueLen;
				Bool updated;
				UOSInt recvCnt;
				Int64 lastRecvTime;
				Int64 dateList[256];
				Double valueList[256];
			} TopicStatus;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::MQTTConn> client;
			IO::LogTool log;
			NN<UI::ListBoxLogger> logger;
			Sync::Mutex topicMut;
			Data::StringMapNN<TopicStatus> topicMap;
			Bool topicListChanged;
			Optional<TopicStatus> currTopic;
			Optional<Media::DrawImage> dispImg;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpStatus;
			NN<UI::GUILabel> lblHost;
			NN<UI::GUITextBox> txtHost;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUILabel> lblUsername;
			NN<UI::GUITextBox> txtUsername;
			NN<UI::GUILabel> lblPassword;
			NN<UI::GUITextBox> txtPassword;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUITabPage> tpSTopic;
			NN<UI::GUILabel> lblSTopic;
			NN<UI::GUITextBox> txtSTopic;
			NN<UI::GUIButton> btnSTopic;
			NN<UI::GUIListBox> lbSTopic;

			NN<UI::GUITabPage> tpPublish;
			NN<UI::GUILabel> lblPublishTopic;
			NN<UI::GUITextBox> txtPublishTopic;
			NN<UI::GUILabel> lblPublishMessage;
			NN<UI::GUITextBox> txtPublishMessage;
			NN<UI::GUICheckBox> chkPublishDUP;
			NN<UI::GUICheckBox> chkPublishRetain;
			NN<UI::GUIButton> btnPublish;

			NN<UI::GUITabPage> tpTopic;
			NN<UI::GUIListView> lvTopic;
			NN<UI::GUIVSplitter> vspTopic;
			NN<UI::GUIPictureBoxSimple> pbTopic;

			NN<UI::GUITabPage> tpLog;
			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;

			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnSTopicClicked(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			static void __stdcall OnTopicSelChg(AnyType userObj);
			static void __stdcall OnPublishClicked(AnyType userObj);
			static void __stdcall OnPingTimerTick(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnPublishMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &message);

			void UpdateTopicChart();
			void ServerStop();
		public:
			AVIRMQTTSubscribeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRMQTTSubscribeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
