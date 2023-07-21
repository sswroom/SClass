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
		UI::GUIPictureBox *pbMain;
		UI::GUIPictureBox *pbSub;
		UI::GUIPictureBox *pbColor;
		UI::GUILabel *lblTrans;
		UI::GUITextBox *txtTrans;
		UI::GUILabel *lblPrimaries;
		UI::GUITextBox *txtPrimaries;
		UI::GUIRadioButton *radR;
		UI::GUIRadioButton *radG;
		UI::GUIRadioButton *radB;
		UI::GUIRadioButton *radYIQY;
		UI::GUIRadioButton *radYIQI;
		UI::GUIRadioButton *radYIQQ;
		UI::GUIRadioButton *radHSVH;
		UI::GUIRadioButton *radHSVS;
		UI::GUIRadioButton *radHSVV;

		UI::GUITextBox *txtR;
		UI::GUITextBox *txtG;
		UI::GUITextBox *txtB;
		UI::GUITextBox *txtYIQY;
		UI::GUITextBox *txtYIQI;
		UI::GUITextBox *txtYIQQ;
		UI::GUITextBox *txtHSVH;
		UI::GUITextBox *txtHSVS;
		UI::GUITextBox *txtHSVV;
		UI::GUILabel *lblAlpha;
		UI::GUITextBox *txtAlpha;

		UI::GUIButton *btnOk;
		UI::GUIButton *btnCancel;

		ColorType colorType;
		Double rVal;
		Double gVal;
		Double bVal;
		Double aVal;
		Bool autoTextUpdate;
		ColorType textUpdating;
		ColorCorrType colorCorr;
		Media::ColorProfile *colorProfile;

		Double mainX;
		Double mainY;
		Double mainZ;
		Media::StaticImage *mainImg;
		Media::StaticImage *subImg;
		Media::ColorManager *colorMgr;
		Media::ColorManagerSess *colorSess;
		Media::MonitorMgr *monMgr;
		Bool subDowned;
		Bool mainDowned;

		UOSInt genThreadCnt;
		Sync::Event *genEvt;
		ThreadStat *genStats;

		static void __stdcall OnOKClicked(void *userObj);
		static void __stdcall OnCancelClicked(void *userObj);
		static Bool __stdcall OnSubDown(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnSubMove(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnSubUp(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainDown(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainMove(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static Bool __stdcall OnMainUp(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn);
		static void __stdcall OnTransChange(void *userObj);
		static void __stdcall OnRedChange(void *userObj, Bool newState);
		static void __stdcall OnGreenChange(void *userObj, Bool newState);
		static void __stdcall OnBlueChange(void *userObj, Bool newState);
		static void __stdcall OnYIQYChange(void *userObj, Bool newState);
		static void __stdcall OnYIQIChange(void *userObj, Bool newState);
		static void __stdcall OnYIQQChange(void *userObj, Bool newState);
		static void __stdcall OnHSVHChange(void *userObj, Bool newState);
		static void __stdcall OnHSVSChange(void *userObj, Bool newState);
		static void __stdcall OnHSVVChange(void *userObj, Bool newState);
		static void __stdcall OnRedTChange(void *userObj);
		static void __stdcall OnGreenTChange(void *userObj);
		static void __stdcall OnBlueTChange(void *userObj);
		static void __stdcall OnYIQYTChange(void *userObj);
		static void __stdcall OnYIQITChange(void *userObj);
		static void __stdcall OnYIQQTChange(void *userObj);
		static void __stdcall OnHSVHTChange(void *userObj);
		static void __stdcall OnHSVSTChange(void *userObj);
		static void __stdcall OnHSVVTChange(void *userObj);
		static void __stdcall OnAlphaChange(void *userObj);
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
		ColorDialog(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, Media::ColorManager *colorMgr, NotNullPtr<Media::DrawEngine> eng, ColorCorrType colorCorr, Media::ColorProfile *colorProfile, Media::MonitorMgr *monMgr);
		virtual ~ColorDialog();

		void SetColor32(UInt32 color);
		UInt32 GetColor32();
		void ShowAlpha();

		virtual void OnMonitorChanged();
		virtual void OnDisplaySizeChange(UOSInt dispWidth, UOSInt dispHeight);

		virtual void YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam);
		virtual void RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam);
	};
}
#endif
