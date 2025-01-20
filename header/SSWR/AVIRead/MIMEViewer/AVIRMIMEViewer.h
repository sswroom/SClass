#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/MIMEObject.h"
#include "UI/GUIClientControl.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEViewer
			{
			protected:
				NN<SSWR::AVIRead::AVIRCore> core;
				NN<UI::GUIClientControl> ctrl;

			public:
				AVIRMIMEViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUIClientControl> ctrl, NN<Text::MIMEObject> obj);
				virtual ~AVIRMIMEViewer();

				static Optional<AVIRMIMEViewer> CreateViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObject> obj);
			};
		}
	}
}
#endif