#ifndef _SM_SSWR_AVIREAD_AVIRLORAJSONFORM
#define _SM_SSWR_AVIREAD_AVIRLORAJSONFORM
#include "Net/LoRaGWUtil.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLoRaJSONForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;

			NN<UI::GUIPanel> pnlDevice;
			NN<UI::GUILabel> lblDevice;
			NN<UI::GUITextBox> txtDevice;
			NN<UI::GUIPanel> pnlJSON;
			NN<UI::GUILabel> lblJSON;
			NN<UI::GUITextBox> txtJSON;
			NN<UI::GUIPanel> pnlJSONCtrl;
			NN<UI::GUIButton> btnJSONParse;
			NN<UI::GUILabel> lblInfo;
			NN<UI::GUITextBox> txtInfo;
			Data::UInt32FastMapNN<Net::LoRaGWUtil::LoRaDevInfo> devMap;

		private:
			static void __stdcall OnJSONParseClick(AnyType userObj);
			static void __stdcall OnCSVFile(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			Bool LoadCSV(NN<Text::String> fileName);
		public:
			AVIRLoRaJSONForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLoRaJSONForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
