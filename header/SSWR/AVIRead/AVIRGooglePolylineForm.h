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

			NotNullPtr<UI::GUILabel> lblPolylineText;
			NotNullPtr<UI::GUITextBox> txtPolylineText;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;
			Math::Geometry::LineString *polyline;

			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRGooglePolylineForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRGooglePolylineForm();

			virtual void OnMonitorChanged();

			Math::Geometry::LineString *GetPolyline();
		};
	}
}
#endif
