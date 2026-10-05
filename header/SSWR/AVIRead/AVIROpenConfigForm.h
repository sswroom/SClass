#ifndef _SM_SSWR_AVIREAD_AVIROPENCONFIGFORM
#define _SM_SSWR_AVIREAD_AVIROPENCONFIGFORM
#include "IO/ConfigFile.h"
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
		class AVIROpenConfigForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblName;
			NN<UI::GUITextBox> txtName;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUILabel> lblType;
			NN<UI::GUIComboBox> cboType;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;
			Optional<IO::ConfigFile> cfg;
			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			Bool SetFile(Text::CStringNN fileName);
		public:
			AVIROpenConfigForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROpenConfigForm();

			virtual void OnMonitorChanged();

			Optional<IO::ConfigFile> GetConfigFile() const;
		};
	}
}
#endif
