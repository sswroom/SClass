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
				Text::String *result;
				Double confidence;
				Media::StaticImage *resImg;
			} ResultInfo;
			
		private:
			SSWR::AVIRead::AVIRCore *core;
			Media::OCREngine ocr;
			Media::ColorManagerSess *colorSess;
			Data::ArrayList<ResultInfo*> results;
			Media::StaticImage *currImg;

			UI::GUIPanel *pnlResult;
			UI::GUIPictureBoxSimple *pbResult;
			UI::GUIListView *lvText;
			UI::GUIHSplitter *hspText;
			UI::GUIPictureBoxDD *pbImg;

			static void __stdcall OnFileHandler(void *userObj, Text::String **files, UOSInt nFiles);
			static void __stdcall OnTextSelChg(void *userObj);
			static void __stdcall OnOCRResult(void *userObj, Text::String *txt, Double confidence, Math::RectArea<OSInt> boundary);
			void ClearResults();
		public:
			AVIROCRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core);
			virtual ~AVIROCRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif