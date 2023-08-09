#ifndef _SM_SSWR_AVIREAD_AVIRIMAGEFORM
#define _SM_SSWR_AVIREAD_AVIRIMAGEFORM
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"
#include "SSWR/AVIRead/AVIRCore.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRImageForm : public UI::GUIForm
		{
		private:
			UI::GUIListBox *lbImages;
			UI::GUIHSplitter *hSplitter;
			UI::GUIMainMenu *mnuMain;
			UI::GUITabControl *tcImage;

			UI::GUITabPage *tpImage;
			UI::GUIPictureBoxDD *pbImage;
			UI::GUITextBox *txtImageStatus;

			UI::GUITabPage *tpInfo;
			UI::GUIPanel *pnlInfo;
			UI::GUIButton *btnInfoICC;
			UI::GUITextBox *txtInfo;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ColorManagerSess *colorSess;
			Media::ImageList *imgList;
			Bool allowEnlarge;
			Media::Image *currImg;
			UInt32 currImgDelay;

			static void __stdcall ImagesSelChg(void *userObj);
			static Bool __stdcall OnImageMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnInfoICCClicked(void *userObj);
			void UpdateInfo();
		public:
			AVIRImageForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::ImageList *imgList);
			virtual ~AVIRImageForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
