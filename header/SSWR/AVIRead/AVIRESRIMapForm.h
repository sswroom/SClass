#ifndef _SM_SSWR_AVIREAD_AVIRESRIMAPFORM
#define _SM_SSWR_AVIREAD_AVIRESRIMAPFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRESRIMapForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				const Char *name;
				const Char *url;
			} MapServer;
		private:
			static MapServer mapSvrs[];
			SSWR::AVIRead::AVIRCore *core;

			UI::GUIRadioButton *radPredefine;
			UI::GUIRadioButton *radOther;
			UI::GUIComboBox *cboPredefine;
			UI::GUITextBox *txtOther;
			UI::GUIButton *btnOK;
			UI::GUIButton *btnCancel;
			const UTF8Char *url;


			static void __stdcall OKClicked(void *userObj);
			static void __stdcall CancelClicked(void *userObj);
			static void __stdcall OnOtherChanged(void *userObj);
		public:
			AVIRESRIMapForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIRESRIMapForm();

			virtual void OnMonitorChanged();
			const UTF8Char *GetSelectedURL();
		};
	};
};
#endif
