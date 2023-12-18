#ifndef _SM_SSWR_SHPCONV_SHPCONVELEVATIONFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVELEVATIONFILTERFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvElevationFilterForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblCol;
			NotNullPtr<UI::GUIComboBox> cboCol;
			NotNullPtr<UI::GUILabel> lblHeight;
			UI::GUITextBox *txtHeight;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			DB::DBFFile *dbf;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			SHPConvElevationFilterForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, DB::DBFFile *dbf);
			virtual ~SHPConvElevationFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	}
}
#endif
