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
	class ColorDialog : public UI::GUIForm, public Media::IColorHandler
	{
	private:
		typedef struct
		{
			ColorDialog *me;
			Int32 status;// 1 = running, 2 = queued task, 3 = processing, 4 = toStop
			UInt8 *imgPtr;
			UOSInt startIndex;
			UOSInt endIndex;
			UOSInt w;
			UOSInt h;
			Sync::Event *evt;
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
		NotNullPtr<UI::GUIPictureBox> pbMain;
		NotNullPtr<UI::GUIPictureBox> pbSub;
		NotNullPtr<UI::GUIPictureBox> pbColor;
		NotNullPtr<UI::GUILabel> lblTrans;
		NotNullPtr<UI::GUITextBox> txtTrans;
		NotNullPtr<UI::GUILabel> lblPrimaries;
		NotNullPtr<UI::GUITextBox> txtPrimaries;
		NotNullPtr<UI::GUIRadioButton> radR;
		NotNullPtr<UI::GUIRadioButton> radG;
		NotNullPtr<UI::GUIRadioButton> radB;
		NotNullPtr<UI::GUIRadioButton> radYIQY;
		NotNullPtr<UI::GUIRadioButton> radYIQI;
		NotNullPtr<UI::GUIRadioButton> radYIQQ;
		NotNullPtr<UI::GUIRadioButton> radHSVH;
		NotNullPtr<UI::GUIRadioButton> radHSVS;
		NotNullPtr<UI::GUIRadioButton> radHSVV;

		NotNullPtr<UI::GUITextBox> txtR;
		NotNullPtr<UI::GUITextBox> txtG;
		NotNullPtr<UI::GUITextBox> txtB;
		NotNullPtr<UI::GUITextBox> txtYIQY;
		NotNullPtr<UI::GUITextBox> txtYIQI;
		NotNullPtr<UI::GUITextBox> txtYIQQ;
		NotNullPtr<UI::GUITextBox> txtHSVH;
		NotNullPtr<UI::GUITextBox> txtHSVS;
		NotNullPtr<UI::GUITextBox> txtHSVV;
		NotNullPtr<UI::GUILabel> lblAlpha;
		NotNullPtr<UI::GUITextBox> txtAlpha;

		NotNullPtr<UI::GUIButton> btnOk;
		NotNullPtr<UI::GUIButton> btnCancel;

		ColorType colorType;
		Double rVal;
		Double gVal;
		Double bVal;
		Double aVal;
		Bool autoTextUpdate;
		ColorType textUpdating;
		ColorCorrType colorCorr;
		Media::ColorProfile *colorProfile;
		Bool alphaShown;

		Double mainX;
		Double mainY;
		Double mainZ;
		Media::StaticImage *mainImg;
		Media::StaticImage *subImg;
		Media::ColorManager *colorMgr;
		NotNullPtr<Media::ColorManagerSess> colorSess;
		Media::MonitorMgr *monMgr;
		Bool subDowned;
		Bool mainDowned;

		UOSInt genThreadCnt;
		Sync::Event *genEvt;
		ThreadStat *genStats;

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
		static UInt32 __stdcall GenThread(void *userObj);

		void XYZ2RGB(Double x, Double y, Double z, Double *r, Double *g, Double *b);
		static void __inline YIQ2RGB(Double y, Double i, Double q, Double *r, Double *g, Double *b);
		static void __inline HSV2RGB(Double h, Double s, Double v, Double *r, Double *g, Double *b);
		void RGB2XYZ(Double r, Double g, Double b, Double *x, Double *y, Double *z);
		static void __inline RGB2YIQ(Double r, Double g, Double b, Double *y, Double *i, Double *q);
		static void __inline RGB2HSV(Double r, Double g, Double b, Double *h, Double *s, Double *v);

		void GenMainImageInner(UInt8 *imgPtr, UOSInt startIndex, UOSInt endIndex, UOSInt w, UOSInt h);
		void GenMainImage();
		void GenSubImage();
		void StoreColor();
		void LoadColor();
		void UpdateColor();
	public:
		ColorDialog(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, Media::ColorManager *colorMgr, NotNullPtr<Media::DrawEngine> eng, ColorCorrType colorCorr, NotNullPtr<const Media::ColorProfile> colorProfile, Media::MonitorMgr *monMgr);
		virtual ~ColorDialog();

		void SetColor32(UInt32 color);
		UInt32 GetColor32();
		void ShowAlpha();

		virtual void OnMonitorChanged();
		virtual void OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight);

		virtual void YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam);
		virtual void RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam);
	};
}
#endif
