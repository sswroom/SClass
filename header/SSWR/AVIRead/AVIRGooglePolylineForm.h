#ifndef _SM_SSWR_AVIREAD_AVIRGOOGLEPOLYLINEFORM
#define _SM_SSWR_AVIREAD_AVIRGOOGLEPOLYLINEFORM
#include "Math/Geometry/LineString.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRGooglePolylineForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			UI::GUILabel *lblPolylineText;
			UI::GUITextBox *txtPolylineText;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			Math::Geometry::LineString *polyline;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRGooglePolylineForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGooglePolylineForm();

			virtual void OnMonitorChanged();

			Math::Geometry::LineString *GetPolyline();
		};
	}
}
#endif
