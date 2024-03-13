#ifndef _SM_SSWR_AVIREAD_AVIRJAVACLASSFORM
#define _SM_SSWR_AVIREAD_AVIRJAVACLASSFORM
#include "IO/JavaClass.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "UI/GUIVSplitter.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRJavaClassForm : public UI::GUIForm
		{
		private:
			NotNullPtr<UI::GUITabControl> tcMain;

			NotNullPtr<UI::GUITabPage> tpFileStruct;
			NotNullPtr<UI::GUITextBox> txtFileStruct;

			NotNullPtr<UI::GUITabPage> tpFields;
			NotNullPtr<UI::GUIListBox> lbFields;

			NotNullPtr<UI::GUITabPage> tpMethods;
			NotNullPtr<UI::GUIListBox> lbMethods;
			NotNullPtr<UI::GUIVSplitter> vspMethods;
			NotNullPtr<UI::GUITextBox> txtMethods;

			NotNullPtr<UI::GUITabPage> tpDecompile;
			NotNullPtr<UI::GUITextBox> txtDecompile;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::JavaClass *clsFile;

			static void __stdcall OnMethodsSelChg(void *userObj);
		public:
			AVIRJavaClassForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::JavaClass *clsFile);
			virtual ~AVIRJavaClassForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
