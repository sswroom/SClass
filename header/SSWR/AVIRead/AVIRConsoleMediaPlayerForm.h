#ifndef _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#define _SM_SSWR_AVIREAD_AVIRCONSOLEMEDIAPLAYERFORM
#include "Media/ConsoleMediaPlayer.h"
#include "Media/MediaPlayerWebInterface.h"
#include "Net/WebServer/WebListener.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUILabel.h"
#include "UI/GUITextBox.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRConsoleMediaPlayerForm : public UI::GUIForm
		{
		private:
			UI::GUILabel *lblPort;
			UI::GUITextBox *txtPort;
			UI::GUIButton *btnStop;
			UI::GUIButton *btnCaptureDev;
			UI::GUILabel *lblRotate;
			UI::GUIComboBox *cboRotate;
			UI::GUICheckBox *chkSurfaceBug;
			UI::GUILabel *lblYUVType;
			UI::GUIComboBox *cboYUVType;
			UI::GUILabel *lblRGBTrans;
			UI::GUIComboBox *cboRGBTrans;
			UI::GUILabel *lblColorPrimaries;
			UI::GUIComboBox *cboColorPrimaries;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ConsoleMediaPlayer *player;
			Media::MediaPlayerWebInterface *webIface;
			Net::WebServer::WebListener *listener;
			Bool videoOpening;

			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnCaptureDevClicked(void *userObj);
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnRotateChg(void *userObj);
			static void __stdcall OnSurfaceBugChg(void *userObj, Bool newVal);
			static void __stdcall OnYUVTypeChg(void *userObj);
			static void __stdcall OnRGBTransChg(void *userObj);
			static void __stdcall OnColorPrimariesChg(void *userObj);

			void AddYUVType(Media::ColorProfile::YUVType yuvType);
			void AddRGBTrans(Media::CS::TransferType rgbType);
			void AddColorPrimaries(Media::ColorProfile::ColorType colorType);
			void UpdateColorDisp();
			Bool OpenICC(Text::CStringNN iccFile);
		public:
			AVIRConsoleMediaPlayerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
