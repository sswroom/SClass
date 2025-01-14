#ifndef _SM_SSWR_AVIREAD_AVIRSYSTEMINFOLOGFORM
#define _SM_SSWR_AVIREAD_AVIRSYSTEMINFOLOGFORM
#include "IO/SystemInfoLog.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSystemInfoLogForm : public UI::GUIForm
		{
		private:
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpSystem;
			NN<UI::GUILabel> lblOSName;
			NN<UI::GUITextBox> txtOSName;
			NN<UI::GUILabel> lblOSVer;
			NN<UI::GUITextBox> txtOSVer;
			NN<UI::GUILabel> lblOSLocale;
			NN<UI::GUITextBox> txtOSLocale;
			NN<UI::GUILabel> lblArchitecture;
			NN<UI::GUITextBox> txtArchitecture;
			NN<UI::GUILabel> lblProductType;
			NN<UI::GUITextBox> txtProductType;

			NN<UI::GUITabPage> tpDevices;
			NN<UI::GUIListView> lvDevices;

			NN<UI::GUITabPage> tpDrivers;
			NN<UI::GUIListView> lvDrivers;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::SystemInfoLog> sysInfo;

		public:
			AVIRSystemInfoLogForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::SystemInfoLog> sysInfo);
			virtual ~AVIRSystemInfoLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
