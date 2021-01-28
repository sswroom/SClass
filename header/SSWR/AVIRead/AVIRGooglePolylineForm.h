#ifndef _SM_SSWR_AVIREAD_AVIRGOOGLEPOLYLINEFORM
#define _SM_SSWR_AVIREAD_AVIRGOOGLEPOLYLINEFORM
#include "Math/Polyline.h"
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
			SSWR::AVIRead::AVIRCore *core;

			UI::GUILabel *lblPolylineText;
			UI::GUITextBox *txtPolylineText;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			Math::Polyline *polyline;

			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
		public:
			AVIRGooglePolylineForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRGooglePolylineForm();

			virtual void OnMonitorChanged();

			Math::Polyline *GetPolyline();
		};
	};
};
#endif
