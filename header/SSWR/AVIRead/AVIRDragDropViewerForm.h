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
			NN<SSWR::AVIRead::AVIRCore> core;
			Data::StringUTF8Map<const UTF8Char *> *dropMap;

			NN<UI::GUIListBox> lbType;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITextBox> txtMain;

			static void __stdcall OnTypeSelChg(AnyType userObj);
			void ClearDrops();
		public:
			AVIRDragDropViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRDragDropViewerForm();

			virtual void OnMonitorChanged();

			virtual DragEffect DragEnter(UI::GUIDropData *data);
			virtual void DropData(UI::GUIDropData *data, OSInt x, OSInt y);
		};
	}
}
#endif
