#ifndef _SM_SSWR_AVIREAD_AVIRNETRAWCAPTUREFORM
#define _SM_SSWR_AVIREAD_AVIRNETRAWCAPTUREFORM
#include "Net/RAWCapture.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRNetRAWCaptureForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Net::SocketFactory> sockf;
			Optional<Net::RAWCapture> capture;
			UInt64 currCnt;
			UInt64 currDataSize;

			NN<UI::GUILabel> lblIP;
			NN<UI::GUIComboBox> cboIP;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUILabel> lblFormat;
			NN<UI::GUIComboBox> cboFormat;
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUIButton> btnAutoGen;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUILabel> lblPacketCnt;
			NN<UI::GUITextBox> txtPacketCnt;
			NN<UI::GUILabel> lblDataSize;
			NN<UI::GUITextBox> txtDataSize;

			static void __stdcall OnAutoGenClicked(AnyType userObj);
			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);

		public:
			AVIRNetRAWCaptureForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetRAWCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
