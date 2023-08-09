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
			UI::GUITabControl *tcMain;

			UI::GUITabPage *tpFileStruct;
			UI::GUITextBox *txtFileStruct;

			UI::GUITabPage *tpFields;
			UI::GUIListBox *lbFields;

			UI::GUITabPage *tpMethods;
			UI::GUIListBox *lbMethods;
			UI::GUIVSplitter *vspMethods;
			UI::GUITextBox *txtMethods;

			UI::GUITabPage *tpDecompile;
			UI::GUITextBox *txtDecompile;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			IO::JavaClass *clsFile;

			static void __stdcall OnMethodsSelChg(void *userObj);
		public:
			AVIRJavaClassForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::JavaClass *clsFile);
			virtual ~AVIRJavaClassForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
