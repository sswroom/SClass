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
			SSWR::AVIRead::AVIRCore *core;
			Net::SocketFactory *sockf;
			Net::RAWCapture *capture;
			Int64 currCnt;
			Int64 currDataSize;

			UI::GUILabel *lblIP;
			UI::GUIComboBox *cboIP;
			UI::GUILabel *lblType;
			UI::GUIComboBox *cboType;
			UI::GUILabel *lblFormat;
			UI::GUIComboBox *cboFormat;
			UI::GUILabel *lblFileName;
			UI::GUITextBox *txtFileName;
			UI::GUIButton *btnAutoGen;
			UI::GUIButton *btnBrowse;
			UI::GUIButton *btnStart;
			UI::GUILabel *lblPacketCnt;
			UI::GUITextBox *txtPacketCnt;
			UI::GUILabel *lblDataSize;
			UI::GUITextBox *txtDataSize;

			static void __stdcall OnAutoGenClicked(void *userObj);
			static void __stdcall OnBrowseClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);

		public:
			AVIRNetRAWCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRNetRAWCaptureForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
