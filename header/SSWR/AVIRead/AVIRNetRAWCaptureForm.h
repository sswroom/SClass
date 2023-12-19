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

			NotNullPtr<UI::GUILabel> lblIP;
			NotNullPtr<UI::GUIComboBox> cboIP;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIComboBox> cboType;
			NotNullPtr<UI::GUILabel> lblFormat;
			NotNullPtr<UI::GUIComboBox> cboFormat;
			NotNullPtr<UI::GUILabel> lblFileName;
			NotNullPtr<UI::GUITextBox> txtFileName;
			NotNullPtr<UI::GUIButton> btnAutoGen;
			NotNullPtr<UI::GUIButton> btnBrowse;
			NotNullPtr<UI::GUIButton> btnStart;
			NotNullPtr<UI::GUILabel> lblPacketCnt;
			NotNullPtr<UI::GUITextBox> txtPacketCnt;
			NotNullPtr<UI::GUILabel> lblDataSize;
			NotNullPtr<UI::GUITextBox> txtDataSize;

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
