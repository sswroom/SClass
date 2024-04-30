#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEJSONVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEJSONVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEJSONViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITextBox> txtJSON;

				NN<Text::MIMEObj::UnknownMIMEObj> obj;

			public:
				AVIRMIMEJSONViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::UnknownMIMEObj> obj);
				virtual ~AVIRMIMEJSONViewer();
			};
		}
	}
}
#endif