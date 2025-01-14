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
			NN<UI::GUILabel> lblPort;
			NN<UI::GUITextBox> txtPort;
			NN<UI::GUIButton> btnStop;
			NN<UI::GUIButton> btnCaptureDev;
			NN<UI::GUILabel> lblRotate;
			NN<UI::GUIComboBox> cboRotate;
			NN<UI::GUICheckBox> chkSurfaceBug;
			NN<UI::GUILabel> lblYUVType;
			NN<UI::GUIComboBox> cboYUVType;
			NN<UI::GUILabel> lblRGBTrans;
			NN<UI::GUIComboBox> cboRGBTrans;
			NN<UI::GUILabel> lblColorPrimaries;
			NN<UI::GUIComboBox> cboColorPrimaries;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ConsoleMediaPlayer> player;
			NN<Media::MediaPlayerWebInterface> webIface;
			Optional<Net::WebServer::WebListener> listener;
			Bool videoOpening;

			static void __stdcall OnStopClicked(AnyType userObj);
			static void __stdcall OnCaptureDevClicked(AnyType userObj);
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnRotateChg(AnyType userObj);
			static void __stdcall OnSurfaceBugChg(AnyType userObj, Bool newVal);
			static void __stdcall OnYUVTypeChg(AnyType userObj);
			static void __stdcall OnRGBTransChg(AnyType userObj);
			static void __stdcall OnColorPrimariesChg(AnyType userObj);

			void AddYUVType(Media::ColorProfile::YUVType yuvType);
			void AddRGBTrans(Media::CS::TransferType rgbType);
			void AddColorPrimaries(Media::ColorProfile::ColorType colorType);
			void UpdateColorDisp();
			Bool OpenICC(Text::CStringNN iccFile);
		public:
			AVIRConsoleMediaPlayerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRConsoleMediaPlayerForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
