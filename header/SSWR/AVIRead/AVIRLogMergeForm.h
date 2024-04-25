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
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<UI::GUILabel> lblFile1;
			NN<UI::GUITextBox> txtFile1;
			NN<UI::GUIButton> btnFile1;
			NN<UI::GUILabel> lblFile2;
			NN<UI::GUITextBox> txtFile2;
			NN<UI::GUIButton> btnFile2;
			NN<UI::GUILabel> lblOFile;
			NN<UI::GUITextBox> txtOFile;
			NN<UI::GUIButton> btnOFile;
			NN<UI::GUIButton> btnConvert;

			static void __stdcall OnFile1Clicked(AnyType userObj);
			static void __stdcall OnFile2Clicked(AnyType userObj);
			static void __stdcall OnOFileClicked(AnyType userObj);
			static void __stdcall OnConvertClicked(AnyType userObj);
		public:
			AVIRLogMergeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRLogMergeForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
