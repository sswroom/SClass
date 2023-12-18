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
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Net::SocketFactory> sockf;
			Net::RAWCapture *capture;
			UInt64 currCnt;
			UInt64 currDataSize;

			UI::GUILabel *lblIP;
			UI::GUIComboBox *cboIP;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUILabel *lblFormat;
			UI::GUIComboBox *cboFormat;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			NotNullPtr<UI::GUIButton> btnAutoGen;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIButton> btnStart;
			UI::GUILabel *lblPacketCnt;
			UI::GUITextBox *txtPacketCnt;
			UI::GUILabel *lblDataSize;
			UI::GUITextBox *txtDataSize;

			static void __stdcall OnAutoGenClicked(void *userObj);
			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

		public:
			AVIRNetRAWCaptureForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRNetRAWCaptureForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
