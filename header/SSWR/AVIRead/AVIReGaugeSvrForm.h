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
			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<Net::WebServer::WebListener> svr;
			Optional<IO::LogTool> log;
			Optional<Net::WebServer::EGaugeHandler> dirHdlr;
			Optional<UI::ListBoxLogger> logger;
			Sync::Mutex reqMut;
			Optional<Text::String> reqLast;
			Bool reqUpdated;

			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpControl;
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStart;

			NN<UI::GUITabPage> tpReqText;
			NN<UI::GUITextBox> txtReqText;

			NN<UI::GUIListBox> lbLog;
			NN<UI::GUITextBox> txtLog;
			NN<UI::GUITabPage> tpLog;

		private:
			static void __stdcall OnStartClick(AnyType userObj);
			static void __stdcall OnLogSel(AnyType userObj);
			static void __stdcall OnTimerTick(AnyType userObj);
			static void __stdcall OnEGaugeData(AnyType userObj, UnsafeArray<const UInt8> data, UIntOS dataSize);
		public:
			AVIReGaugeSvrForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIReGaugeSvrForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
