#ifndef _SM_SSWR_AVIREAD_AVIRMIMEVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRMIMEVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObject.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMIMEViewerForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Text::MIMEObject> obj;
			Optional<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer> viewer;
			NN<Media::ColorManagerSess> sess;

		public:
			AVIRMIMEViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Text::MIMEObject> obj);
			virtual ~AVIRMIMEViewerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
