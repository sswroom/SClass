#ifndef _SM_SSWR_AVIREAD_AVIRANPRFORM
#define _SM_SSWR_AVIREAD_AVIRANPRFORM
#include "Data/ArrayListA.h"
#include "Media/ANPR.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUILabel.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUIPictureBoxSimple.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRANPRForm : public UI::GUIForm
		{
		private:
			enum class ActionType
			{
				None,
				Corners,
				Plate
			};

			typedef struct
			{
				Math::RectArea<UOSInt> area;
				NotNullPtr<Text::String> result;
				Double maxTileAngle;
				Double pxArea;
				UOSInt confidence;
				NotNullPtr<Media::StaticImage> plateImg;
			} ResultInfo;
			
		private:
			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::ANPR anpr;
			NotNullPtr<Media::ColorManagerSess> colorSess;
			Data::ArrayList<ResultInfo*> results;
			Media::StaticImage *currImg;
			ActionType selectMode;
			Data::ArrayListA<Math::Coord2DDbl> points;

			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIButton> btnSelPlate;
			NotNullPtr<UI::GUIButton> btnSelCorners;
			NotNullPtr<UI::GUILabel> lblSelStatus;
			NotNullPtr<UI::GUIPanel> pnlPlate;
			NotNullPtr<UI::GUIPictureBoxSimple> pbPlate;
			NotNullPtr<UI::GUIListView> lvPlate;
			NotNullPtr<UI::GUIHSplitter> hspPlate;
			NotNullPtr<UI::GUIPictureBoxDD> pbImg;

			static void __stdcall OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnPlateSelChg(void *userObj);
			static void __stdcall OnSelPlateClicked(void *userObj);
			static void __stdcall OnSelCornersClicked(void *userObj);
			static Bool __stdcall OnImgDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnANPRResult(void *userObj, NotNullPtr<Media::StaticImage> simg, Math::RectArea<UOSInt> area, NotNullPtr<Text::String> result, Double maxTileAngle, Double pxArea, UOSInt confidence, NotNullPtr<Media::StaticImage> plateImg);
			void ClearResults();
		public:
			AVIRANPRForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRANPRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
