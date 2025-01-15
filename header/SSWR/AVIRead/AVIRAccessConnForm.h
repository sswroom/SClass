#ifndef _SM_SSWR_AVIREAD_AVIRACCESSCONNFORM
#define _SM_SSWR_AVIREAD_AVIRACCESSCONNFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRAccessConnForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblFileName;
			NN<UI::GUITextBox> txtFileName;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Optional<DB::DBConn> conn;

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
		public:
			AVIRAccessConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRAccessConnForm();

			virtual void OnMonitorChanged();

			Optional<DB::DBConn> GetDBConn();
		};
	}
}
#endif
