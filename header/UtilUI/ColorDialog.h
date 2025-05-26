#ifndef _SM_UTILUI_TEXTGOTODIALOG
#define _SM_UTILUI_TEXTGOTODIALOG
#include "Media/ColorManager.h"
#include "Media/ColorProfile.h"
#include "Media/DrawEngine.h"
#include "Media/MonitorMgr.h"
#include "UI/GUIButton.h"
#include "UI/GUIComboBox.h"
#include "UI/GUIForm.h"
#include "UI/GUIPictureBox.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUITextBox.h"
#include "UI/GUILabel.h"

namespace UtilUI
{
	class ColorDialog : public UI::GUIForm, public Media::ColorHandler
	{
	private:
		typedef struct
		{
			NN<ColorDialog> me;
			Int32 status;// 1 = running, 2 = queued task, 3 = processing, 4 = toStop
			UnsafeArray<UInt8> imgPtr;
			UOSInt startIndex;
			UOSInt endIndex;
			UOSInt w;
			UOSInt h;
			NN<Sync::Event> evt;
		} ThreadStat;

		typedef enum
		{
			CT_UNKNOWN,
			CT_RED,
			CT_GREEN,
			CT_BLUE,
			CT_YIQY,
			CT_YIQI,
			CT_YIQQ,
			CT_HSVH,
			CT_HSVS,
			CT_HSVV
		} ColorType;

	public:
		typedef enum
		{
//			CCT_NOCORR,
			CCT_VIDEO,
			CCT_PHOTO
		} ColorCorrType;
	private:
		NN<UI::GUIPictureBox> pbMain;
		NN<UI::GUIPictureBox> pbSub;
		NN<UI::GUIPictureBox> pbColor;
		NN<UI::GUILabel> lblTrans;
		NN<UI::GUITextBox> txtTrans;
		NN<UI::GUILabel> lblPrimaries;
		NN<UI::GUITextBox> txtPrimaries;
		NN<UI::GUIRadioButton> radR;
		NN<UI::GUIRadioButton> radG;
		NN<UI::GUIRadioButton> radB;
		NN<UI::GUIRadioButton> radYIQY;
		NN<UI::GUIRadioButton> radYIQI;
		NN<UI::GUIRadioButton> radYIQQ;
		NN<UI::GUIRadioButton> radHSVH;
		NN<UI::GUIRadioButton> radHSVS;
		NN<UI::GUIRadioButton> radHSVV;

		NN<UI::GUITextBox> txtR;
		NN<UI::GUITextBox> txtG;
		NN<UI::GUITextBox> txtB;
		NN<UI::GUITextBox> txtYIQY;
		NN<UI::GUITextBox> txtYIQI;
		NN<UI::GUITextBox> txtYIQQ;
		NN<UI::GUITextBox> txtHSVH;
		NN<UI::GUITextBox> txtHSVS;
		NN<UI::GUITextBox> txtHSVV;
		NN<UI::GUILabel> lblAlpha;
		NN<UI::GUITextBox> txtAlpha;

		NN<UI::GUIButton> btnOk;
		NN<UI::GUIButton> btnCancel;

		ColorType colorType;
		Math::Vector3 rgbVal;
		Double aVal;
		Bool autoTextUpdate;
		ColorType textUpdating;
		ColorCorrType colorCorr;
		NN<Media::ColorProfile> colorProfile;
		Bool alphaShown;

		Math::Vector3 mainXYZ;
		NN<Media::StaticImage> mainImg;
		NN<Media::StaticImage> subImg;
		NN<Media::ColorManager> colorMgr;
		NN<Media::ColorManagerSess> colorSess;
		Optional<Media::MonitorMgr> monMgr;
		Bool subDowned;
		Bool mainDowned;

		UOSInt genThreadCnt;
		NN<Sync::Event> genEvt;
		UnsafeArray<ThreadStat> genStats;

		static void __stdcall OnOKClicked(AnyType userObj);
		static void __stdcall OnCancelClicked(AnyType userObj);
		static Bool __stdcall OnSubDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnSubMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnSubUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static void __stdcall OnTransChange(AnyType userObj);
		static void __stdcall OnRedChange(AnyType userObj, Bool newState);
		static void __stdcall OnGreenChange(AnyType userObj, Bool newState);
		static void __stdcall OnBlueChange(AnyType userObj, Bool newState);
		static void __stdcall OnYIQYChange(AnyType userObj, Bool newState);
		static void __stdcall OnYIQIChange(AnyType userObj, Bool newState);
		static void __stdcall OnYIQQChange(AnyType userObj, Bool newState);
		static void __stdcall OnHSVHChange(AnyType userObj, Bool newState);
		static void __stdcall OnHSVSChange(AnyType userObj, Bool newState);
		static void __stdcall OnHSVVChange(AnyType userObj, Bool newState);
		static void __stdcall OnRedTChange(AnyType userObj);
		static void __stdcall OnGreenTChange(AnyType userObj);
		static void __stdcall OnBlueTChange(AnyType userObj);
		static void __stdcall OnYIQYTChange(AnyType userObj);
		static void __stdcall OnYIQITChange(AnyType userObj);
		static void __stdcall OnYIQQTChange(AnyType userObj);
		static void __stdcall OnHSVHTChange(AnyType userObj);
		static void __stdcall OnHSVSTChange(AnyType userObj);
		static void __stdcall OnHSVVTChange(AnyType userObj);
		static void __stdcall OnAlphaChange(AnyType userObj);
		static UInt32 __stdcall GenThread(AnyType userObj);

		Math::Vector3 XYZ2RGB(Math::Vector3 xyz);
		static Math::Vector3 YIQ2RGB(Math::Vector3 yiq);
		static Math::Vector3 HSV2RGB(Math::Vector3 hsv);
		Math::Vector3 RGB2XYZ(Math::Vector3 rgb);
		static Math::Vector3 RGB2YIQ(Math::Vector3 rgb);
		static Math::Vector3 RGB2HSV(Math::Vector3 rgb);

		void GenMainImageInner(UnsafeArray<UInt8> imgPtr, UOSInt startIndex, UOSInt endIndex, UOSInt w, UOSInt h);
		void GenMainImage();
		void GenSubImage();
		void StoreColor();
		void LoadColor();
		void UpdateColor();
	public:
		ColorDialog(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::ColorManager> colorMgr, NN<Media::DrawEngine> eng, ColorCorrType colorCorr, NN<const Media::ColorProfile> colorProfile, Optional<Media::MonitorMgr> monMgr);
		virtual ~ColorDialog();

		void SetColor32(UInt32 color);
		UInt32 GetColor32();
		void ShowAlpha();

		virtual void OnMonitorChanged();
		virtual void OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight);

		virtual void YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam);
	};
}
#endif
