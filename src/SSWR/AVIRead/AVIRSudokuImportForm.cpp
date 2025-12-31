#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/OCREngine.h"
#include "SSWR/AVIRead/AVIRSudokuImportForm.h"
#include "UI/Clipboard.h"
#include "UI/ClipboardUtil.h"
#include "UI/GUIFileDialog.h"

#define VERBOSE
#define GRAYTHRESHOLD 224

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	me->outputBoard->CopyFrom(me->board);
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnClipboardClicked(AnyType userObj)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	UI::Clipboard clipboard(me->GetHandle());
	NN<Media::ImageList> imgList;
	if (UI::ClipboardUtil::LoadImage(clipboard, me->core->GetParserList()).SetTo(imgList))
	{
		me->LoadImage(imgList);
	}
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnBrowseClicked(AnyType userObj)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"SudokuImport", false);
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::ImageList);
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		me->LoadFile(dlg->GetFileName());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	UOSInt i = 0;
	UOSInt j = fileNames.GetSize();
	while (i < j)
	{
		if (me->LoadFile(fileNames[i]))
		{
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnNumberInput(AnyType userObj, UOSInt x, UOSInt y, UInt8 num)
{
	NN<AVIRSudokuImportForm> me = userObj.GetNN<AVIRSudokuImportForm>();
	me->board.SetBoardNum(x, y, num, true);
	me->svMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRSudokuImportForm::OnOCRResult(AnyType userObj, NN<Text::String> txt, Double confidence, Math::RectArea<OSInt> boundary)
{
#ifdef VERBOSE
	printf("OCR Result: %s (Confidence: %lf)\n", txt->ToCString().v.Ptr(), confidence);
#endif
}

Bool SSWR::AVIRead::AVIRSudokuImportForm::LoadFile(NN<Text::String> fileName)
{
	Bool succ = false;
	NN<Media::ImageList> imgList;
	IO::StmData::FileData fd(fileName, false);
	if (Optional<Media::ImageList>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
	{
		succ = this->LoadImage(imgList);
	}
	return succ;
}

Bool SSWR::AVIRead::AVIRSudokuImportForm::LoadImage(NN<Media::ImageList> imgList)
{
	Bool succ = false;
	NN<Media::StaticImage> img;
	imgList->ToStaticImage(0);
	if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
	{
		this->currImage = img;
		imgList->RemoveImage(0, false);
		this->pbImage->SetImage(img, false);
		this->DoOCR(img);
		this->svMain->Redraw();
		succ = true;
	}
	imgList.Delete();
	return succ;
}

void SSWR::AVIRead::AVIRSudokuImportForm::DoOCR(NN<Media::StaticImage> img)
{
	Math::RectArea<UOSInt> rect = CalcBoardRect(img);
	this->board.Clear();
	Media::OCREngine ocr(Media::OCREngine::Language::English);
	ocr.HandleOCRResult(OnOCRResult, this);
	ocr.SetCharWhiteList("123456789");
	ocr.SetParsingImage(img);
	Math::Coord2D<UOSInt> currPos = rect.min;
	Math::RectArea<UOSInt> boxRect = CalcBoxRect(img, Math::Coord2D<UOSInt>(currPos.x + 4, currPos.y + 4));
	Math::Size2D<UOSInt> boxSize = boxRect.GetSize();
	Math::RectArea<UOSInt> textRect;
	UOSInt x;
	UOSInt y = 0;
	UOSInt minConfidence[] = {80, 60, 60, 60, 50, 60, 60, 60, 60};
	while (y < 9)
	{
		boxRect = CalcBoxRect(img, Math::Coord2D<UOSInt>(currPos.x + 4, currPos.y + 10));
		currPos.y = boxRect.min.y + 2;
		x = 0;
		while (x < 9)
		{
			currPos.x += boxSize.x >> 1;
			boxRect = CalcBoxRect(img, currPos);
			if (CalcTextRect(img, boxRect, textRect))
			{
				NN<Text::String> s;
				UOSInt confidence;
				if (ocr.ParseInsideImage(textRect, confidence).SetTo(s))
				{
					if (s->v[0] >= '1' && s->v[0] <= '9')
					{
#ifdef VERBOSE
						printf("Box %d,%d: %c (Confidence: %d)\r\n", (Int32)x, (Int32)y, s->v[0], (Int32)confidence);
#endif
						if (confidence > minConfidence[s->v[0] - '1'])
						{
							this->board.SetBoardNum(x, y, (UInt8)(s->v[0] - '0'), true);
						}
					}
					s->Release();
				}
			}
			x++;
			currPos.x = boxRect.max.x;
		}
		y++;
		currPos.x = rect.min.x;
		currPos.y = boxRect.max.y;
	}
}

Bool SSWR::AVIRead::AVIRSudokuImportForm::CalcTextRect(NN<Media::StaticImage> img, Math::RectArea<UOSInt> boxRect, OutParam<Math::RectArea<UOSInt>> textRect)
{
//	textRect.Set(Math::RectArea<UOSInt>(boxRect.min + 2, boxRect.max - 2));
//	return true;
	Math::RectArea<UOSInt> retRect = boxRect;
	Bool found = false;
	UnsafeArray<UInt8> dataPtr = img->data;
	UOSInt bpl = img->GetDataBpl();
	UOSInt currY = boxRect.min.y + 2;
	UOSInt currX;
	UOSInt ofst;
	while (currY < boxRect.max.y - 2)
	{
		currX = boxRect.min.x + 2;
		ofst = currY * bpl + currX * 4;
		while (currX < boxRect.max.x - 2)
		{
			if (dataPtr[ofst] < GRAYTHRESHOLD)
			{
				if (!found)
				{
					retRect.min = Math::Coord2D<UOSInt>(currX, currY);
					retRect.max = retRect.min + 1;
					found = true;
				}
				else
				{
					if (currX < retRect.min.x)
						retRect.min.x = currX;
					if (currY < retRect.min.y)
						retRect.min.y = currY;
					if (currX + 1 > retRect.max.x)
						retRect.max.x = currX + 1;
					if (currY + 1 > retRect.max.y)
						retRect.max.y = currY + 1;
				}
			}
			currX++;
			ofst += 4;
		}
		currY++;
	}
	if (!found)
	{
		return false;
	}
#if defined(VERBOSE)
	printf("Text Rect: %d, %d - %d, %d\r\n", (UInt32)retRect.min.x, (UInt32)retRect.min.y, (UInt32)retRect.max.x, (UInt32)retRect.max.y);
#endif
	textRect.Set(retRect);
	return true;
}

Math::RectArea<UOSInt> SSWR::AVIRead::AVIRSudokuImportForm::CalcBoxRect(NN<Media::StaticImage> img, Math::Coord2D<UOSInt> pos)
{
	UnsafeArray<UInt8> dataPtr = img->data;
	UOSInt bpl = img->GetDataBpl();
	UOSInt currX = pos.x;
	UOSInt currY = pos.y;
	Math::Size2D<UOSInt> dispSize = img->info.dispSize;
	UOSInt yOfst = currY * bpl;
	Math::RectArea<UOSInt> ret;
	while (currX > 0)
	{
		if (dataPtr[yOfst + currX * 4] >= GRAYTHRESHOLD)
		{
			break;
		}
		currX--;
	}
	while (currX > 0)
	{
		if (dataPtr[yOfst + (currX - 1) * 4] < GRAYTHRESHOLD)
		{
			break;
		}
		currX--;
	}
	ret.min.x = currX;
	currX += 3;
	while (currY > 0)
	{
		if (dataPtr[(currY - 1) * bpl + currX * 4] < GRAYTHRESHOLD)
		{
			break;
		}
		currY--;
	}
	ret.min.y = currY;
	currY += 3;
	currX = pos.x;
	yOfst = currY * bpl;
	while (currX < dispSize.x)
	{
		if (dataPtr[yOfst + currX * 4] < GRAYTHRESHOLD)
		{
			break;
		}
		currX++;
	}
	ret.max.x = currX;
	currX -= 4;
	currY = pos.y;
	while (currY < dispSize.y)
	{
		if (dataPtr[currY * bpl + currX * 4] < GRAYTHRESHOLD)
		{
			break;
		}
		currY++;
	}
	ret.max.y = currY;
#ifdef VERBOSE
	//printf("Box Rect: %d, %d - %d, %d, Pos: %d, %d\r\n", (Int32)ret.min.x, (Int32)ret.min.y, (Int32)ret.max.x, (Int32)ret.max.y, (Int32)pos.x, (Int32)pos.y);
#endif
	return ret;
}

Math::RectArea<UOSInt> SSWR::AVIRead::AVIRSudokuImportForm::CalcBoardRect(NN<Media::StaticImage> img)
{
	img->ToB8G8R8A8();
	Math::RectArea<UOSInt> rect = CalcBoxRect(img, img->info.dispSize / 2);
	if (rect.min.x == 0 || rect.min.y == 0 || rect.max.x == img->info.dispSize.x || rect.max.y == img->info.dispSize.y)
	{
		return Math::RectArea<UOSInt>(Math::Coord2D<UOSInt>(0, 0), img->info.dispSize);
	}
	Math::RectArea<UOSInt> ret = rect;
	Math::Size2D<UOSInt> boxSize = rect.GetSize();
	Math::Coord2D<UOSInt> currPos = img->info.dispSize / 2;
	Math::RectArea<UOSInt> thisRect = rect;
	while (thisRect.min.x > boxSize.x)
	{
		currPos.x = thisRect.min.x - (boxSize.x >> 1);
		currPos.y = thisRect.min.y + 3;
		thisRect = CalcBoxRect(img, currPos);
		if (!SizeSimilar(thisRect.GetSize(), boxSize))
		{
			break;
		}
		ret.min.x = thisRect.min.x;
	}
	while (thisRect.min.y > boxSize.y)
	{
		currPos.x = thisRect.min.x + 3;
		currPos.y = thisRect.min.y - (boxSize.y >> 1);
		thisRect = CalcBoxRect(img, currPos);
		if (!SizeSimilar(thisRect.GetSize(), boxSize))
		{
			break;
		}
		ret.min.y = thisRect.min.y;
	}
	currPos = ret.min + boxSize * 9;
#ifdef VERBOSE
	printf("Box Size: %d, %d, Min: %d, %d\r\n", (Int32)boxSize.x, (Int32)boxSize.y, (Int32)ret.min.x, (Int32)ret.min.y);
#endif
	thisRect = CalcBoxRect(img, currPos);
	if (!SizeSimilar(thisRect.GetSize(), boxSize))
	{
#ifdef VERBOSE
		printf("Board rect not found\r\n");
#endif
		return Math::RectArea<UOSInt>(Math::Coord2D<UOSInt>(0, 0), img->info.dispSize);
	}
#ifdef VERBOSE
	printf("Board rect: %d, %d - %d, %d\r\n", (Int32)ret.min.x, (Int32)ret.min.y, (Int32)thisRect.max.x, (Int32)thisRect.max.y);
#endif
	return Math::RectArea<UOSInt>(ret.min, thisRect.max);
}

Bool SSWR::AVIRead::AVIRSudokuImportForm::SizeSimilar(Math::Size2D<UOSInt> size1, Math::Size2D<UOSInt> size2)
{
	OSInt xMinDiff = (OSInt)(size1.x / 10);
	OSInt yMinDiff = (OSInt)(size1.y / 10);
	OSInt xDiff = (OSInt)size1.x - (OSInt)size2.x;
	OSInt yDiff = (OSInt)size1.y - (OSInt)size2.y;
	return (xDiff >= -xMinDiff && xDiff <= xMinDiff) && (yDiff >= -yMinDiff && yDiff <= yMinDiff);
}

SSWR::AVIRead::AVIRSudokuImportForm::AVIRSudokuImportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Game::Sudoku::SudokuBoard> board) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetText(CSTR("Sudoku Import"));

	this->core = core;
	this->outputBoard = board;
	this->currImage = 0;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 31, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnClipboard = ui->NewButton(this->pnlMain, CSTR("Import from Clipboard"));
	this->btnClipboard->SetRect(4, 4, 150, 23, false);
	this->btnClipboard->HandleButtonClick(OnClipboardClicked, this);
	this->btnBrowse = ui->NewButton(this->pnlMain, CSTR("Browse Image File"));
	this->btnBrowse->SetRect(160, 4, 150, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnCancel = ui->NewButton(this->pnlCtrl, CSTR("Cancel"));
	this->btnCancel->SetRect(4, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(this->pnlCtrl, CSTR("OK"));
	this->btnOK->SetRect(84, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpImage = this->tcMain->AddTabPage(CSTR("Image"));
	this->pbImage = ui->NewPictureBoxDD(this->tpImage, this->colorSess, true, false);
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpBoard = this->tcMain->AddTabPage(CSTR("Board"));
	NEW_CLASSNN(this->svMain, UI::GUISudokuViewer(ui, this->tpBoard, this->core->GetDrawEngine(), this->colorSess, this->board));
	this->svMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->svMain->HandleNumberInput(OnNumberInput, this);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRSudokuImportForm::~AVIRSudokuImportForm()
{
	this->ClearChildren();
	this->core->GetColorManager()->DeleteSess(this->colorSess);
	this->currImage.Delete();
}

void SSWR::AVIRead::AVIRSudokuImportForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess->ChangeMonitor(this->GetHMonitor());
}
