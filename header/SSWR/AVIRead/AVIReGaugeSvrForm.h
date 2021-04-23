#ifndef _SM_SSWR_AVIREAD_AVIREGAUGESVRFORM
#define _SM_SSWR_AVIREAD_AVIREGAUGESVRFORM
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/EGaugeHandler.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/ListBoxLogger.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIGroupBox.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIReGaugeSvrForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Net::WebServer::WebListener *svr;
			IO::LogTool *log;
			Net::WebServer::EGaugeHandler *dirHdlr;
			UI::ListBoxLogger *logger;
			Sync::Mutex *reqMut;
			UTF8Char *reqLast;
			Bool reqUpdated;

			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpControl;
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStart;

			UI::GUITabPage *tpReqText;
			UI::GUITextBox *txtReqText;

			UI::GUIListBox *lbLog;
			UI::GUITextBox *txtLog;
			UI::GUITabPage *tpLog;

		private:
			static void __stdcall OnStartClick(void *userObj);
			static void __stdcall OnLogSel(void *userObj);
			static void __stdcall OnTimerTick(void *userObj);
			static void __stdcall OnEGaugeData(void *userObj, const UInt8 *data, UOSInt dataSize);
		public:
			AVIReGaugeSvrForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIReGaugeSvrForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
