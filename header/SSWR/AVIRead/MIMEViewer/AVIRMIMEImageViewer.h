#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEIMAGEVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMIMEIMAGEVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUIPictureBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMIMEImageViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUIPictureBoxDD> pbContent;

				Text::MIMEObj::UnknownMIMEObj *obj;
				Media::ImageList *imgList;

			public:
				AVIRMIMEImageViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::UnknownMIMEObj *obj);
				virtual ~AVIRMIMEImageViewer();
			};
		}
	}
}
#endif