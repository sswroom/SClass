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
				NN<Text::String> result;
				Double confidence;
				NN<Media::StaticImage> resImg;
			} ResultInfo;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Media::OCREngine ocr;
			NN<Media::ColorManagerSess> colorSess;
			Data::ArrayListNN<ResultInfo> results;
			Optional<Media::StaticImage> currImg;

			NN<UI::GUIPanel> pnlResult;
			NN<UI::GUIPictureBoxSimple> pbResult;
			NN<UI::GUIListView> lvText;
			NN<UI::GUIHSplitter> hspText;
			NN<UI::GUIPictureBoxDD> pbImg;

			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnTextSelChg(AnyType userObj);
			static void __stdcall OnOCRResult(AnyType userObj, NN<Text::String> txt, Double confidence, Math::RectArea<OSInt> boundary);
			void ClearResults();
		public:
			AVIROCRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIROCRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
