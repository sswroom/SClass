#ifndef _SM_SSWR_SHPCONV_SHPCONVADDFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVADDFILTERFORM
#include "DB/DBFFile.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvAddFilterForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblFilter;
			NN<UI::GUIComboBox> cboFilter;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			NN<DB::DBFFile> dbf;
			NN<Media::DrawEngine> deng;
			NN<Media::MonitorMgr> monMgr;
			Optional<MapFilter> filter;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			SHPConvAddFilterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<DB::DBFFile> dbf, NN<Media::DrawEngine> deng);
			virtual ~SHPConvAddFilterForm();

			virtual void OnMonitorChanged();
			Optional<MapFilter> GetFilter();
		};
	};
};
#endif
