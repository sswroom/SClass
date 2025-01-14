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
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpFileStruct;
			NN<UI::GUITextBox> txtFileStruct;

			NN<UI::GUITabPage> tpFields;
			NN<UI::GUIListBox> lbFields;

			NN<UI::GUITabPage> tpMethods;
			NN<UI::GUIListBox> lbMethods;
			NN<UI::GUIVSplitter> vspMethods;
			NN<UI::GUITextBox> txtMethods;

			NN<UI::GUITabPage> tpDecompile;
			NN<UI::GUITextBox> txtDecompile;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<IO::JavaClass> clsFile;

			static void __stdcall OnMethodsSelChg(AnyType userObj);
		public:
			AVIRJavaClassForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::JavaClass> clsFile);
			virtual ~AVIRJavaClassForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
