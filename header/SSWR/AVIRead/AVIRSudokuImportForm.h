#ifndef _SM_SSWR_AVIREAD_AVIRSUDOKUIMPORTFORM
#define _SM_SSWR_AVIREAD_AVIRSUDOKUIMPORTFORM
#include "Game/Sudoku/SudokuBoard.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPictureBoxDD.h"
#include "UI/GUISudokuViewer.h"
#include "UI/GUITabControl.h"
#include "UI/GUITabPage.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRSudokuImportForm : public UI::GUIForm
		{
		private:
			NN<SSWR::AVIRead::AVIRCore> core;
			Game::Sudoku::SudokuBoard board;
			NN<Game::Sudoku::SudokuBoard> outputBoard;
			NN<Media::ColorManagerSess> colorSess;
			Optional<Media::StaticImage> currImage;

			NN<UI::GUIPanel> pnlMain;
			NN<UI::GUIButton> btnClipboard;
			NN<UI::GUIButton> btnBrowse;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIButton> btnCancel;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUITabControl> tcMain;

			NN<UI::GUITabPage> tpImage;
			NN<UI::GUIPictureBoxDD> pbImage;

			NN<UI::GUITabPage> tpBoard;
			NN<UI::GUISudokuViewer> svMain;

			static void __stdcall OnCancelClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnClipboardClicked(AnyType userObj);
			static void __stdcall OnBrowseClicked(AnyType userObj);
			static void __stdcall OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames);
			static void __stdcall OnNumberInput(AnyType userObj, UIntOS x, UIntOS y, UInt8 num);
			static void __stdcall OnOCRResult(AnyType userObj, NN<Text::String> txt, Double confidence, Math::RectArea<IntOS> boundary);
			Bool LoadFile(NN<Text::String> fileName);
			Bool LoadImage(NN<Media::ImageList> imgList);
			void DoOCR(NN<Media::StaticImage> img);
			static Bool CalcTextRect(NN<Media::StaticImage> img, Math::RectArea<UIntOS> boxRect, OutParam<Math::RectArea<UIntOS>> textRect);
			static Math::RectArea<UIntOS> CalcBoxRect(NN<Media::StaticImage> img, Math::Coord2D<UIntOS> pos);
			static Math::RectArea<UIntOS> CalcBoardRect(NN<Media::StaticImage> img);
			static Bool SizeSimilar(Math::Size2D<UIntOS> size1, Math::Size2D<UIntOS> size2);
		public:
			AVIRSudokuImportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Game::Sudoku::SudokuBoard> board);
			virtual ~AVIRSudokuImportForm();

			virtual void OnMonitorChanged();
		};
	}
}
#endif
