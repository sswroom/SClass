#ifndef _SM_SSWR_AVIREAD_AVIRFILEEXFORM
#define _SM_SSWR_AVIREAD_AVIRFILEEXFORM

#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRFileExForm : public UI::GUIForm
		{
		private:
			NN<UI::GUILabel> lblSrc;
			NN<UI::GUITextBox> txtSrc;
			NN<UI::GUIButton> btnSrc;
			NN<UI::GUILabel> lblDest;
			NN<UI::GUITextBox> txtDest;
			NN<UI::GUIButton> btnDest;
			NN<UI::GUILabel> lblFileSize;
			NN<UI::GUITextBox> txtFileSize;
			NN<UI::GUILabel> lblStartOfst;
			NN<UI::GUITextBox> txtStartOfst;
			NN<UI::GUILabel> lblEndOfst;
			NN<UI::GUITextBox> txtEndOfst;
			NN<UI::GUIButton> btnStart;

			NN<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSrcChanged(AnyType userObj);
			static void __stdcall OnSrcClicked(AnyType userObj);
			static void __stdcall OnDestClicked(AnyType userObj);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
		public:
			AVIRFileExForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileExForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
