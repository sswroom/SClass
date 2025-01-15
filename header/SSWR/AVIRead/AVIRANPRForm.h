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
				NN<Text::String> result;
				Double maxTileAngle;
				Double pxArea;
				UOSInt confidence;
				NN<Media::StaticImage> plateImg;
			} ResultInfo;
			
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Media::ANPR anpr;
			NN<Media::ColorManagerSess> colorSess;
			Data::ArrayListNN<ResultInfo> results;
			Optional<Media::StaticImage> currImg;
			ActionType selectMode;
			Data::ArrayListA<Math::Coord2DDbl> points;

			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnSelPlate;
			NN<UI::GUIButton> btnSelCorners;
			NN<UI::GUILabel> lblSelStatus;
			NN<UI::GUIPanel> pnlPlate;
			NN<UI::GUIPictureBoxSimple> pbPlate;
			NN<UI::GUIListView> lvPlate;
			NN<UI::GUIHSplitter> hspPlate;
			NN<UI::GUIPictureBoxDD> pbImg;

			static void __stdcall OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnPlateSelChg(AnyType userObj);
			static void __stdcall OnSelPlateClicked(AnyType userObj);
			static void __stdcall OnSelCornersClicked(AnyType userObj);
			static Bool __stdcall OnImgDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn);
			static void __stdcall OnANPRResult(AnyType userObj, NN<Media::StaticImage> simg, Math::RectArea<UOSInt> area, NN<Text::String> result, Double maxTileAngle, Double pxArea, UOSInt confidence, NN<Media::StaticImage> plateImg);
			void ClearResults();
		public:
			AVIRANPRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core);
			virtual ~AVIRANPRForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
