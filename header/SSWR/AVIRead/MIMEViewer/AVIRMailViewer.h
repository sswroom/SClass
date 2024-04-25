#ifndef _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMAILVIEWER
#define _SM_SSWR_AVIREAD_MIMEVIEWER_AVIRMAILVIEWER
#include "SSWR/AVIRead/AVIRCore.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMIMEViewer.h"
#include "Text/MIMEObj/MailMessage.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		namespace MIMEViewer
		{
			class AVIRMailViewer : public AVIRMIMEViewer
			{
			private:
				NN<UI::GUITabControl> tcHeader;
				NN<UI::GUITabPage> tpHeading;
				NN<UI::GUIListView> lvRecp;
				NN<UI::GUIPanel> pnlFrom;
				NN<UI::GUILabel> lblFrom;
				NN<UI::GUITextBox> txtFrom;
				NN<UI::GUIPanel> pnlSubject;
				NN<UI::GUILabel> lblSubject;
				NN<UI::GUITextBox> txtSubject;
				NN<UI::GUIPanel> pnlDate;
				NN<UI::GUILabel> lblDate;
				NN<UI::GUITextBox> txtDate;

				NN<UI::GUITabPage> tpRAW;
				NN<UI::GUIListView> lvRAW;

				NN<UI::GUIPanel> pnlContent;

				Text::MIMEObj::MailMessage *mail;
				SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;

			public:
				AVIRMailViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, Text::MIMEObj::MailMessage *mail);
				virtual ~AVIRMailViewer();
			};
		}
	}
}
#endif