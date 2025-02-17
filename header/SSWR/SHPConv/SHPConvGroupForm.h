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
			NN<UI::GUILabel> lblGroup;
			NN<UI::GUIListBox> lbGroup;
			NN<UI::GUIButton> btnOk;
			NN<UI::GUIButton> btnCancel;
			NN<Media::MonitorMgr> monMgr;

			static void __stdcall OnOkClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			SHPConvGroupForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui);
			virtual ~SHPConvGroupForm();

			virtual void OnMonitorChanged();

			void AddGroup(Text::CStringNN grpName);
			void SetCurrGroup(UOSInt currGroup);
			UOSInt GetCurrGroup();
		};
	}
}
#endif
