#ifndef _SM_SSWR_AVIREAD_AVIRLOGMERGEFORM
#define _SM_SSWR_AVIREAD_AVIRLOGMERGEFORM
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRLogMergeForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<UI::GUILabel> lblFile1;
			NotNullPtr<UI::GUITextBox> txtFile1;
			NotNullPtr<UI::GUIButton> btnFile1;
			NotNullPtr<UI::GUILabel> lblFile2;
			NotNullPtr<UI::GUITextBox> txtFile2;
			NotNullPtr<UI::GUIButton> btnFile2;
			NotNullPtr<UI::GUILabel> lblOFile;
			NotNullPtr<UI::GUITextBox> txtOFile;
			NotNullPtr<UI::GUIButton> btnOFile;
			NotNullPtr<UI::GUIButton> btnConvert;

			static void __stdcall OnFile1Clicked(AnyType userObj);
			static void __stdcall OnFile2Clicked(AnyType userObj);
			static void __stdcall OnOFileClicked(AnyType userObj);
			static void __stdcall OnConvertClicked(AnyType userObj);
		public:
			AVIRLogMergeForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogMergeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
