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
				UI::GUITabControl *tcHeader;
				UI::GUITabPage *tpHeading;
				UI::GUIListView *lvRecp;
				UI::GUIPanel *pnlFrom;
				UI::GUILabel *lblFrom;
				UI::GUITextBox *txtFrom;
				UI::GUIPanel *pnlSubject;
				UI::GUILabel *lblSubject;
				UI::GUITextBox *txtSubject;
				UI::GUIPanel *pnlDate;
				UI::GUILabel *lblDate;
				UI::GUITextBox *txtDate;

				UI::GUITabPage *tpRAW;
				UI::GUIListView *lvRAW;

				UI::GUIPanel *pnlContent;

				Text::MIMEObj::MailMessage *mail;
				SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer *viewer;

			public:
				AVIRMailViewer(NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::MailMessage *mail);
				virtual ~AVIRMailViewer();
			};
		}
	}
}
#endif