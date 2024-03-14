#ifndef _SM_SSWR_SHPCONV_SHPCONVCURRFILTERFORM
#define _SM_SSWR_SHPCONV_SHPCONVCURRFILTERFORM
#include "DB/DBFFile.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "SSWR/SHPConv/MapFilter.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvCurrFilterForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUIListBox> lbFilters;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnDelete;
			DB::DBFFile *dbf;
			NotNullPtr<Media::DrawEngine> deng;
			Media::MonitorMgr *monMgr;
			Data::ArrayList<MapFilter*> *filters;

			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnDeleteClicked(AnyType userObj);
			static void __stdcall OnOkClicked(AnyType userObj);
		public:
			SHPConvCurrFilterForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, DB::DBFFile *dbf, Data::ArrayList<MapFilter*> *filters, NotNullPtr<Media::DrawEngine> deng);
			virtual ~SHPConvCurrFilterForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
