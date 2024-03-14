#ifndef _SM_SSWR_SHPCONV_SHPCONVVALUEFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVVALUEFILTERFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvValueFilterForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUILabel> lblCol;
			NotNullPtr<UI::GUIComboBox> cboCol;
			NotNullPtr<UI::GUILabel> lblType;
			NotNullPtr<UI::GUIRadioButton> radEqual;
			NotNullPtr<UI::GUIRadioButton> radNotEq;
			NotNullPtr<UI::GUILabel> lblValue;
			NotNullPtr<UI::GUITextBox> txtValue;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			DB::DBFFile *dbf;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			SHPConvValueFilterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, DB::DBFFile *dbf);
			virtual ~SHPConvValueFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	}
}
#endif
