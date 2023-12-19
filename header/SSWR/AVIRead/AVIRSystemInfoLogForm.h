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
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpSystem;
			NotNullPtr<UI::GUILabel> lblOSName;
			NotNullPtr<UI::GUITextBox> txtOSName;
			NotNullPtr<UI::GUILabel> lblOSVer;
			NotNullPtr<UI::GUITextBox> txtOSVer;
			NotNullPtr<UI::GUILabel> lblOSLocale;
			NotNullPtr<UI::GUITextBox> txtOSLocale;
			NotNullPtr<UI::GUILabel> lblArchitecture;
			NotNullPtr<UI::GUITextBox> txtArchitecture;
			NotNullPtr<UI::GUILabel> lblProductType;
			NotNullPtr<UI::GUITextBox> txtProductType;

			NotNullPtr<UI::GUITabPage> tpDevices;
			UI::GUIListView *lvDevices;

			NotNullPtr<UI::GUITabPage> tpDrivers;
			UI::GUIListView *lvDrivers;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::SystemInfoLog *sysInfo;

		public:
			AVIRSystemInfoLogForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::SystemInfoLog *sysInfo);
			virtual ~AVIRSystemInfoLogForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
