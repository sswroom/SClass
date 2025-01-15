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
			NN<UI::GUIListBox> lbImages;
			NN<UI::GUIHSplitter> hSplitter;
			NN<UI::GUIMainMenu> mnuMain;
			NN<UI::GUITabControl> tcImage;

			NN<UI::GUITabPage> tpImage;
			NN<UI::GUIPictureBoxDD> pbImage;
			NN<UI::GUITextBox> txtImageStatus;

			NN<UI::GUITabPage> tpInfo;
			NN<UI::GUIPanel> pnlInfo;
			NN<UI::GUIButton> btnInfoICC;
			NN<UI::GUITextBox> txtInfo;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			NN<Media::ImageList> imgList;
			Bool allowEnlarge;
			Optional<Media::RasterImage> currImg;
			UInt32 currImgDelay;

			static void __stdcall ImagesSelChg(AnyType userObj);
			static Bool __stdcall OnImageMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnInfoICCClicked(AnyType userObj);
			void UpdateInfo();
		public:
			AVIRImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::ImageList> imgList);
			virtual ~AVIRImageForm();

			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
