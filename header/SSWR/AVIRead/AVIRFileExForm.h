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
			NotNullPtr<UI::GUILabel> lblSrc;
			NotNullPtr<UI::GUITextBox> txtSrc;
			NotNullPtr<UI::GUIButton> btnSrc;
			NotNullPtr<UI::GUILabel> lblDest;
			NotNullPtr<UI::GUITextBox> txtDest;
			NotNullPtr<UI::GUIButton> btnDest;
			NotNullPtr<UI::GUILabel> lblFileSize;
			NotNullPtr<UI::GUITextBox> txtFileSize;
			NotNullPtr<UI::GUILabel> lblStartOfst;
			NotNullPtr<UI::GUITextBox> txtStartOfst;
			NotNullPtr<UI::GUILabel> lblEndOfst;
			NotNullPtr<UI::GUITextBox> txtEndOfst;
			NotNullPtr<UI::GUIButton> btnStart;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;

			static void __stdcall OnSrcChanged(void *userObj);
			static void __stdcall OnSrcClicked(void *userObj);
			static void __stdcall OnDestClicked(void *userObj);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt fileCnt);
		public:
			AVIRFileExForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRFileExForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
