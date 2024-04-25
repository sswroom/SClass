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
			NN<UI::GUILabel> lblCol;
			NN<UI::GUIComboBox> cboCol;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIRadioButton> radEqual;
			NN<UI::GUIRadioButton> radNotEq;
			NN<UI::GUILabel> lblValue;
			NN<UI::GUITextBox> txtValue;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			DB::DBFFile *dbf;
			Media::MonitorMgr *monMgr;
			MapFilter *filter;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			SHPConvValueFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, DB::DBFFile *dbf);
			virtual ~SHPConvValueFilterForm();

			virtual void OnMonitorChanged();
			MapFilter *GetFilter();
		};
	}
}
#endif
