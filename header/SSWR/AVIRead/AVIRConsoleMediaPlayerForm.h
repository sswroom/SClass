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
			NotNullPtr<UI::GUILabel> lblPort;
			NotNullPtr<UI::GUITextBox> txtPort;
			NotNullPtr<UI::GUIButton> btnStop;
			NotNullPtr<UI::GUIButton> btnCaptureDev;
			NotNullPtr<UI::GUILabel> lblRotate;
			NotNullPtr<UI::GUIComboBox> cboRotate;
			NotNullPtr<UI::GUICheckBox> chkSurfaceBug;
			NotNullPtr<UI::GUILabel> lblYUVType;
			NotNullPtr<UI::GUIComboBox> cboYUVType;
			NotNullPtr<UI::GUILabel> lblRGBTrans;
			NotNullPtr<UI::GUIComboBox> cboRGBTrans;
			NotNullPtr<UI::GUILabel> lblColorPrimaries;
			NotNullPtr<UI::GUIComboBox> cboColorPrimaries;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ConsoleMediaPlayer *player;
			NotNullPtr<Media::MediaPlayerWebInterface> webIface;
			Net::WebServer::WebListener *listener;
			Bool videoOpening;

			static void __stdcall OnStopClicked(void *userObj);
			static void __stdcall OnCaptureDevClicked(void *userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
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
			AVIRConsoleMediaPlayerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
