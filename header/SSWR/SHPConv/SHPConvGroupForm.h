#ifndef _SM_SSWR_SHPCONV_SHPCONVGROUPFORM
#define _SM_SSWR_SHPCONV_SHPCONVGROUPFORM
#include "Media/MonitorMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvGroupForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblGroup;
			UI::GUIListBox *lbGroup;
			UI::GUIButton *btnOk;
			UI::GUIButton *btnCancel;
			Media::MonitorMgr *monMgr;

			static void __stdcall OnOkClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			SHPConvGroupForm(UI::GUIClientControl *parent, UI::GUICore *ui);
			virtual ~SHPConvGroupForm();

			virtual void OnMonitorChanged();

			void AddGroup(const UTF8Char *grpName);
			void SetCurrGroup(OSInt currGroup);
			OSInt GetCurrGroup();
		};
	};
};
#endif
