#ifndef _SM_SSWR_AVIREAD_AVIRLORALOGFORM
#define _SM_SSWR_AVIREAD_AVIRLORALOGFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLoRaLogForm : public UI::GUIForm
		{
		private:
			struct LoRaDevInfo
			{
				UInt8 devEUI[8];
				UInt8 nwkSKey[16];
				UInt8 appSKey[16];
			};

			struct LoRaLogEntry
			{
				Data::Timestamp ts;
				Bool frServer;
				UInt8 gwEUI[8];
				Bool push;
				UInt32 token;
				NN<Text::String> json;
			};
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUILabel> lblDevice;
			NN<UI::GUITextBox> txtDevice;
			NN<UI::GUIPanel> pnlLog;
			NN<UI::GUILabel> lblLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUILabel> lblGateway;
			NN<UI::GUIComboBox> cboGateway;
			NN<UI::GUIListView> lvLog;
			NN<UI::GUITextBox> txtDetail;

			Data::UInt32FastMapNN<LoRaDevInfo> devMap;
			Data::ArrayListNN<LoRaLogEntry> logList;

		private:
			static void __stdcall FreeLoRaLogEntry(NN<LoRaLogEntry> logEntry);
			static void __stdcall OnCSVFile(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnGatewayChanged(AnyType userObj);
			static void __stdcall OnLogSelChg(AnyType userObj);
			Bool LoadCSV(NN<Text::String> fileName);
			Bool LoadLog(NN<Text::String> fileName);
			void ShowLog(Text::CString gateway);
			void ParseJSONText(NN<Text::String> jsonText);
			void PHYPayloadDetail(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> buff, UIntOS buffSize);
			UInt32 MACPayloadDetail(NN<Text::StringBuilderUTF8> sb, Bool downLink, UnsafeArray<const UInt8> buff, UIntOS buffSize, OutParam<UInt32> fCnt);
		public:
			AVIRLoRaLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLoRaLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
