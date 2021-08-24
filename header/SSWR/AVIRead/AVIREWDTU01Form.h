#ifndef _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#define _SM_SSWR_AVIREAD_AVIREWDTU01FORM
#include "Data/UInt64Map.h"
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
				const UTF8Char *name;
				const UTF8Char *remark;
			} DeviceEntry;

		private:
			UI::GUIPanel *pnlMQTT;
			UI::GUILabel *lblServer;
			UI::GUITextBox *txtServer;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnConnect;
			UI::GUIListView *lvDevices;

			SSWR::AVIRead::AVIRCore *core;
			Net::MQTTStaticClient *cli;
			Sync::Mutex *dataMut;
			Bool dataChg;
			Data::UInt64Map<DeviceEntry*> *dataMap;

			static void __stdcall OnMQTTMessage(void *userObj, const UTF8Char *topic, const UInt8 *buff, UOSInt buffSize);
			static void __stdcall OnConnectClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			void DataClear();
		public:
			AVIREWDTU01Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIREWDTU01Form();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
