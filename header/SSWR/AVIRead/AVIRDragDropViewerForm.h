#ifndef _SM_SSWR_AVIREAD_AVIRDRAGDROPVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRDRAGDROPVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRDragDropViewerForm : public UI::GUIForm, public UI::GUIDropHandler
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Data::StringUTF8Map<const UTF8Char *> *dropMap;

			UI::GUIListBox *lbType;
			NotNullPtr<UI::GUIHSplitter> hspMain;
			NotNullPtr<UI::GUITextBox> txtMain;

			static void __stdcall OnTypeSelChg(void *userObj);
			void ClearDrops();
		public:
			AVIRDragDropViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDragDropViewerForm();

			virtual void OnMonitorChanged();

			virtual DragEffect DragEnter(UI::GUIDropData *data);
			virtual void DropData(UI::GUIDropData *data, OSInt x, OSInt y);
		};
	}
}
#endif
