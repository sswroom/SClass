#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRUNKNOWNVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRUNKNOWNVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObject.h"
#include "UI/GUILabel.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRUnknownViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUILabel> lblMessage;

				NN<Text::MIMEObject> obj;

			public:
				AVIRUnknownViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObject> obj);
				virtual ~AVIRUnknownViewer();
			};
		}
	}
}
#endif