#ifndef _SM_SSWR_AVIREAD_AVIRPDFOBJECTFORM
#define _SM_SSWR_AVIREAD_AVIRPDFOBJECTFORM
#include "Media/PDFDocument.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPDFObjectForm : public UI::GUIForm
		{
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::PDFDocument *doc;

			UI::GUIListBox *lbObject;
			UI::GUIHSplitter *hspMain;
			UI::GUITabControl *tcMain;
			
			UI::GUITabPage *tpParameter;
			UI::GUIListView *lvParameter;

			UI::GUIMainMenu *mnuMain;

			static void __stdcall OnObjectSelChg(void *userObj);
		public:
			AVIRPDFObjectForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::PDFDocument *doc);
			virtual ~AVIRPDFObjectForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
