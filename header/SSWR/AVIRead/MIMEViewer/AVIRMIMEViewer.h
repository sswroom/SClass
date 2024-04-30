#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "Text/IMIMEObj.h"
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
				AVIRMIMEViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUIClientControl> ctrl, NN<Text::IMIMEObj> obj);
				virtual ~AVIRMIMEViewer();

				static Optional<AVIRMIMEViewer> CreateViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::IMIMEObj> obj);
			};
		}
	}
}
#endif