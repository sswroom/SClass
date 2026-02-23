#ifndef _SM_SSWR_AVIREAD_AVIRVECTORDOCUMENTFORM
#define _SM_SSWR_AVIREAD_AVIRVECTORDOCUMENTFORM
#include "Media/VectorDocument.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPictureBoxSimple.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRVectorDocumentForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::VectorDocument> vdoc;

			NN<UI::GUIListBox> lbPages;
			NN<UI::GUIHSplitter> hspMain;
			NN<UI::GUIPictureBoxSimple> pbMain;

		public:
			AVIRVectorDocumentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::VectorDocument> vdoc);
			virtual ~AVIRVectorDocumentForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
