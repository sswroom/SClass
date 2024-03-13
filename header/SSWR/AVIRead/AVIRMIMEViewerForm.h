#ifndef _SM_SSWR_AVIREAD_AVIRMIMEVIEWERFORM
#define _SM_SSWR_AVIREAD_AVIRMIMEVIEWERFORM
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/IMIMEObj.h"
#include "UI/GUIForm.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRMIMEViewerForm : public UI::GUIForm
		{
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Text::IMIMEObj *obj;
			SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;
			NotNullPtr<Media::ColorManagerSess> sess;

		public:
			AVIRMIMEViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Text::IMIMEObj *obj);
			virtual ~AVIRMIMEViewerForm();

			virtual void OnMonitorChanged();
		};
	};
};
#endif
