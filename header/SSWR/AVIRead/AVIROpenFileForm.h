#ifndef _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#define _SM_SSWR_AVIREAD_AVIROPENFILEFORM
#include "IO/ParsedObject.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROpenFileForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			Optional<Text::String> fileName;
			IO::ParserType parserType;
			NN<SSWR::AVIRead::AVIRCore> core;
			IO::ParserType t;

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		public:
			AVIROpenFileForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::ParserType t);
			virtual ~AVIROpenFileForm();

			virtual void OnMonitorChanged();

			NN<Text::String> GetFileName() const;
			IO::ParserType GetParserType();
		};
	}
}
#endif
