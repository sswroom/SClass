#ifndef _SM_SSWR_AVIREAD_AVIROCRFORM
#define _SM_SSWR_AVIREAD_AVIROCRFORM
#include "Media/OCREngine.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUIPictureBoxSimple.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIROCRForm : public UI::GUIForm
		{
		private:
			typedef struct
			{
				Math::RectArea<OSInt> area;
				NotNullPtr<Text::String> result;
				Double confidence;
				Media::StaticImage *resImg;
			} ResultInfo;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::OCREngine ocr;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Data::ArrayList<ResultInfo*> results;
			Media::StaticImage *currImg;

			NotNullPtr<UI::GUIPanel> pnlResult;
			UI::GUIPictureBoxSimple *pbResult;
			UI::GUIListView *lvText;
			UI::GUIHSplitter *hspText;
			UI::GUIPictureBoxDD *pbImg;

			static void __stdcall OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnTextSelChg(void *userObj);
			static void __stdcall OnOCRResult(void *userObj, NotNullPtr<Text::String> txt, Double confidence, Math::RectArea<OSInt> boundary);
			void ClearResults();
		public:
			AVIROCRForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROCRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
