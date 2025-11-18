#ifndef _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#define _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#include "Data/FastMapNN.h"
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
				UInt64 mac64Int;
				Int32 rssi;
				Optional<Text::String> name;
				Optional<Text::String> remark;
			} DeviceEntry;

		private:
			NN<UI::GUIPanel> pnlMQTT;
			NN<UI::GUILabel> lblServer;
			NN<UI::GUITextBox> txtServer;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnConnect;
			NN<UI::GUIListView> lvDevices;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::MQTTStaticClient> cli;
			Sync::Mutex dataMut;
			Bool dataChg;
			Data::FastMapNN<UInt64, DeviceEntry> dataMap;

			static void __stdcall OnMQTTMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &buff);
			static void __stdcall OnConnectClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			void DataClear();
		public:
			AVIREWDTU01Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIREWDTU01Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
