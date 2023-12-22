#ifndef _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#define _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#include "Data/FastMap.h"
#include "Net/MQTTStaticClient.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "Sync/Mutex.h"
#include "Text/JSON.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIREWDTU01Form : public UI::GUIForm
		{
		private:
			typedef struct
			{
				UInt8 mac[6];
				UInt64 macInt;
				Int32 rssi;
				Optional<Text::String> name;
				Text::String *remark;
			} DeviceEntry;

		private:
			NotNullPtr<UI::GUIPanel> pnlMQTT;
			NotNullPtr<UI::GUILabel> lblServer;
			NotNullPtr<UI::GUITextBox> txtServer;
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnConnect;
			NotNullPtr<UI::GUIListView> lvDevices;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Net::MQTTStaticClient *cli;
			Sync::Mutex dataMut;
			Bool dataChg;
			Data::FastMap<UInt64, DeviceEntry*> dataMap;

			static void __stdcall OnMQTTMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &buff);
			static void __stdcall OnConnectClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void DataClear();
		public:
			AVIREWDTU01Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREWDTU01Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
