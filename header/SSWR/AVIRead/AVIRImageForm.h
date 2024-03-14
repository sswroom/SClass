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
			NotNullPtr<UI::GUIListBox> lbImages;
			NotNullPtr<UI::GUIHSplitter> hSplitter;
			NotNullPtr<UI::GUIMainMenu> mnuMain;
			NotNullPtr<UI::GUITabControl> tcImage;

			NotNullPtr<UI::GUITabPage> tpImage;
			NotNullPtr<UI::GUIPictureBoxDD> pbImage;
			NotNullPtr<UI::GUITextBox> txtImageStatus;

			NotNullPtr<UI::GUITabPage> tpInfo;
			NotNullPtr<UI::GUIPanel> pnlInfo;
			NotNullPtr<UI::GUIButton> btnInfoICC;
			NotNullPtr<UI::GUITextBox> txtInfo;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			NotNullPtr<Media::ImageList> imgList;
			Bool allowEnlarge;
			Media::RasterImage *currImg;
			UInt32 currImgDelay;

			static void __stdcall ImagesSelChg(AnyType userObj);
			static Bool __stdcall OnImageMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnInfoICCClicked(AnyType userObj);
			void UpdateInfo();
		public:
			AVIRImageForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Media::ImageList> imgList);
			virtual ~AVIRImageForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
