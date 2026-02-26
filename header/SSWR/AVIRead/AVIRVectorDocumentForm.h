#ifndef _SM_SSWR_AVIREAD_AVIRVECTORDOCUMENTFORM
#define _SM_SSWR_AVIREAD_AVIRVECTORDOCUMENTFORM
#include "Media/VectorDocument.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIListView.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVectorDocumentForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::VectorDocument> vdoc;
			NN<Media::ColorManagerSess> colorSess;

			NN<UI::GUIListBox> lbPages;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUITabControl> tcMain;
			NN<UI::GUITabPage> tpPreview;
			NN<UI::GUIPictureBoxDD> pbMain;
			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIListView> lvInfo;

			NN<UI::GUIMainMenu> mnuMain;
			NN<UI::GUIPopupMenu> mnuPage;

			static void __stdcall OnPagesSelChg(AnyType userObj);
			static UI::EventState __stdcall OnPagesRightClickd(AnyType userObj, Math::Coord2D<IntOS> scnPos, MouseButton btn);
		public:
			AVIRVectorDocumentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::VectorDocument> vdoc);
			virtual ~AVIRVectorDocumentForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
