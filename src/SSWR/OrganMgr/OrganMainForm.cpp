#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/MemoryReadingStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "Manage/Process.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Math.h"
#include "Media/ImageUtil.h"
#include "Text/URLString.h"
#include "SSWR/OrganMgr/OrganBookForm.h"
#include "SSWR/OrganMgr/OrganDataFileForm.h"
#include "SSWR/OrganMgr/OrganImages.h"
#include "SSWR/OrganMgr/OrganImageDetailForm.h"
#include "SSWR/OrganMgr/OrganImageWebForm.h"
#include "SSWR/OrganMgr/OrganLocationForm.h"
#include "SSWR/OrganMgr/OrganMainForm.h"
#include "SSWR/OrganMgr/OrganParentItem.h"
#include "SSWR/OrganMgr/OrganSearchForm.h"
#include "SSWR/OrganMgr/OrganSelCategoryForm.h"
#include "SSWR/OrganMgr/OrganTripForm.h"
#include "SSWR/OrganMgr/OrganUserForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"
#include "Text/UTF8Reader.h"
#include "UI/Clipboard.h"
#include "UI/GUICore.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"
#include "UtilUI/ColorDialog.h"

#include <stdio.h>
//#include <windows.h>
//#include "Win32/StreamCOM.h"
//#undef MoveFile
//#undef CopyFile

typedef enum
{
	MNU_MANAGE_GROUP = 101,
	MNU_MANAGE_FIND,
	MNU_MANAGE_LIST,
	MNU_MANAGE_TEST,
	MNU_MANAGE_CLEAR_DIR,
	MNU_MANAGE_LOCATION,
	MNU_MANAGE_TRIP,
	MNU_MANAGE_UNK_TRIP,
	MNU_MANAGE_DATABASE,
	MNU_MANAGE_BOOK,
	MNU_MANAGE_USER,
	MNU_MANAGE_DATAFILE,
	MNU_GO_UP,
	MNU_NAV_LEFT,
	MNU_NAV_RIGHT,
	MNU_NAV_UP,
	MNU_NAV_DOWN,
	MNU_EXPORT_LITE
} MenuItems;

void __stdcall SSWR::OrganMgr::OrganMainForm::OnGroupEnterClick(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->lastGroupObj != 0 && me->inputMode == IM_GROUP)
	{
		OnObjDblClicked(userObj);
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnGroupAddClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();

	if (me->GroupFormValid())
	{
		OrganGroup *grp = (OrganGroup*)me->lbDir->GetSelectedItem().p;
		Int32 parDir = grp->GetGroupId();

		if (me->inputMode == IM_GROUP || me->inputMode == IM_EMPTY)
		{
			Text::StringBuilderUTF8 sb;
			NN<OrganGroup> newGrp;
			NN<OrganGroupType> grpType;
			NEW_CLASSNN(newGrp, OrganGroup());
			grpType = me->cboGroupType->GetItem((UOSInt)me->cboGroupType->GetSelectedIndex()).GetNN<OrganGroupType>();
			newGrp->SetGroupType(grpType->GetSeq());
			sb.ClearStr();
			me->txtGroupEName->GetText(sb);
			sb.Trim();
			newGrp->SetEName(sb.ToCString());
			sb.ClearStr();
			me->txtGroupCName->GetText(sb);
			sb.Trim();
			newGrp->SetCName(sb.ToCString());
			sb.ClearStr();
			me->txtGroupDesc->GetText(sb);
			sb.Trim();
			newGrp->SetDesc(sb.ToCString());
			sb.ClearStr();
			me->txtGroupKey->GetText(sb);
			newGrp->SetIDKey(sb.ToCString());
			newGrp->SetAdminOnly(me->chkGroupAdmin->IsChecked());
			
			me->env->AddGroup(newGrp, parDir);
            me->lastGroupObj = 0;
            me->UpdateDir();

			UOSInt i;
			UOSInt matchGrpInd = 0;
			Int32 matchGrpId = -1;
			OrganGroupItem *gi;

			i = me->lbObj->GetCount();
			while (i-- > 0)
			{
				gi = (OrganGroupItem*)me->lbObj->GetItem(i).p;
				if (gi->GetItemType() == OrganGroupItem::IT_GROUP)
				{
					grp = (OrganGroup*)gi;
					if (Text::StringTool::Equals(newGrp->GetEName(), grp->GetEName()))
					{
						if (grp->GetGroupId() > matchGrpId)
						{
							matchGrpId = grp->GetGroupId();
							matchGrpInd = i;
						}
					}
				}
			}
			me->lbObj->SetSelectedIndex(matchGrpInd);
			newGrp.Delete();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnGroupRemoveClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	i = me->lbObj->GetSelectedIndex();
    if (i == INVALID_INDEX)
		return;

	if (me->inputMode == IM_GROUP)
	{
		OrganGroup *obj = (OrganGroup*)me->lbObj->GetItem(i).p;
		i = me->env->GetGroupCount(obj->GetGroupId());

		if (i == 0)
		{
			i = me->env->GetSpeciesCount(obj->GetGroupId());

			if (i == 0)
			{
				if (me->ui->ShowMsgYesNo(me->env->GetLang(CSTR("MainFormGroupRemoveQuestMsg")), me->env->GetLang(CSTR("MainFormGroupRemoveQuest")), me))
				{
					me->env->DelGroup(obj->GetGroupId());
					me->lastGroupObj = 0;
					me->UpdateDir();
				}
			}
            else
			{
				sb.Append(me->env->GetLang(CSTR("MainFormGroupRemoveMsg1")));
				sb.AppendUOSInt(i);
				sb.Append(me->env->GetLang(CSTR("MainFormGroupRemoveMsg2")));
				me->ui->ShowMsgOK(sb.ToCString(), me->env->GetLang(CSTR("MainFormGroupRemoveTitle")), me);
			}
		}
        else
		{
			sb.Append(me->env->GetLang(CSTR("MainFormGroupRemoveMsg1")));
			sb.AppendUOSInt(i);
			sb.Append(me->env->GetLang(CSTR("MainFormGroupRemoveMsg2")));
			me->ui->ShowMsgOK(sb.ToCString(), me->env->GetLang(CSTR("MainFormGroupRemoveTitle")), me);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnDirChanged(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	me->lastDirIndex = me->lbDir->GetSelectedIndex();
	me->UpdateDir();
	SDEL_STRING(me->initSelImg);
	me->UpdateImgDir();
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnObjDblClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->lbObj->GetSelectedIndex();
	//System::Int32 i = lbObj->IndexFromPoint(lbObj->PointToClient(this->MousePosition));
	if (i == INVALID_INDEX)
		return;
	if (me->ToSaveGroup())
		return;
	if (me->ToSaveSpecies())
		return;

	NN<OrganGroupItem> gi = me->lbObj->GetItem(i).GetNN<OrganGroupItem>();
	if (gi->GetItemType() == OrganGroupItem::IT_GROUP)
	{
		me->lastSpeciesObj = 0;
		me->lastGroupObj = 0;
		sptr = gi->GetItemName(sbuff);
		me->lbDir->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, gi);
		me->groupList.Add(NN<OrganGroup>::ConvertFrom(gi));
		me->groupItems.RemoveAt(i);
		me->lbDir->SetSelectedIndex(me->lbDir->GetCount() - 1);
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_SPECIES)
	{
		me->tcMain->SetSelectedIndex(2);
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
	{
		if (me->lbDir->GetSelectedIndex() != INVALID_INDEX && me->lbDir->GetSelectedIndex() > 0)
		{
			OrganGroup *groupO = (OrganGroup*)me->lbDir->GetItem(me->lbDir->GetSelectedIndex()).p;
			SDEL_STRING(me->initSelObj);
			me->initSelObj = Text::String::OrEmpty(groupO->GetEName())->Clone().Ptr();
            me->lbDir->SetSelectedIndex(me->lbDir->GetSelectedIndex() - 1);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnObjSelChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	if (me->restoreObj)
		return;
	if (me->lbObj->GetSelectedIndex() == INVALID_INDEX)
	{
		me->lastSpeciesObj = 0;
		me->lastGroupObj = 0;
		me->ClearGroupForm();
		me->ClearSpeciesForm();
		return;
	}

	if (me->ToSaveGroup())
	{
		me->restoreObj = true;
		me->lbObj->SetSelectedIndex(me->lastObjIndex);
		me->restoreObj = false;
		return;
	}
	if (me->ToSaveSpecies())
	{
		me->restoreObj = true;
		me->lbObj->SetSelectedIndex(me->lastObjIndex);
		me->restoreObj = false;
		return;
	}
	me->lastObjIndex = me->lbObj->GetSelectedIndex();

	OrganGroupItem *gi = (OrganGroupItem*)me->lbObj->GetSelectedItem().p;
	me->indexChanged = true;
	me->lastSpeciesObj = 0;
	me->lastGroupObj = 0;
	if (gi->GetItemType() == OrganGroupItem::IT_SPECIES)
	{
		OrganSpecies *spe;
		Text::StringBuilderUTF8 sb;
		spe = (OrganSpecies*)gi;
		sptr = Text::StrInt32(sbuff, spe->GetSpeciesId());
		me->txtSpeciesId->SetText(CSTRP(sbuff, sptr));
		me->txtSpeciesEName->SetText(Text::String::OrEmpty(spe->GetEName())->ToCString());
		me->txtSpeciesCName->SetText(Text::String::OrEmpty(spe->GetCName())->ToCString());
		me->txtSpeciesSName->SetText(Text::String::OrEmpty(spe->GetSName())->ToCString());
		me->txtSpeciesDName->SetText(Text::String::OrEmpty(spe->GetDirName())->ToCString());
		me->txtSpeciesDesc->SetText(Text::String::OrEmpty(spe->GetDesc())->ToCString());
		me->txtSpeciesKey->SetText(Text::String::OrEmpty(spe->GetIDKey())->ToCString());
		s = Text::String::OrEmpty(spe->GetSName());
		if (s->EndsWith(UTF8STRC(" female")))
		{
			sb.ClearStr();
			sb.Append(s);
			sb.RemoveChars(7);
			me->txtSpBook->SetText(sb.ToCString());
		}
		else if (s->EndsWith(UTF8STRC(" male")))
		{
			sb.ClearStr();
			sb.Append(s);
			sb.RemoveChars(5);
			me->txtSpBook->SetText(sb.ToCString());
		}
		else
		{
			me->txtSpBook->SetText(s->ToCString());
		}
		me->lastSpeciesObj = spe;
		me->UpdateSpBook();
		if (me->tcMain->GetSelectedIndex() == 2 || me->tcMain->GetSelectedIndex() == 3)
		{
			SDEL_STRING(me->initSelImg);
			me->UpdateImgDir();
		}
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_GROUP)
	{
		OrganGroup *grp;
		grp = (OrganGroup*)gi;
		sptr = Text::StrInt32(sbuff, grp->GetGroupId());
		me->txtGroupId->SetText(CSTRP(sbuff, sptr));
		me->SelectGroup(me->cboGroupType, grp->GetGroupType());
		me->txtGroupEName->SetText(Text::String::OrEmpty(grp->GetEName())->ToCString());
		me->txtGroupCName->SetText(Text::String::OrEmpty(grp->GetCName())->ToCString());
		me->txtGroupDesc->SetText(Text::String::OrEmpty(grp->GetDesc())->ToCString());
		me->txtGroupKey->SetText(Text::String::OrEmpty(grp->GetIDKey())->ToCString());
		me->chkGroupAdmin->SetChecked(grp->GetAdminOnly());
		me->lastGroupObj = grp;
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
	{
		me->ClearGroupForm();
		me->ClearSpeciesForm();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImgSelChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();

//	StopAudio();
	me->dispImageToCrop = false;
	me->dispImageDown = false;
	me->dispImageDownPos = {0, 0};
	if (me->lbImage->GetSelectedIndex() == INVALID_INDEX)
	{
		me->pbImg->SetImage(0, false);
		me->dispImage.Delete();
		me->dispImageUF = 0;
		me->dispImageWF = 0;
	}
	else
	{
		me->pbImg->SetImage(0, false);
		me->dispImage.Delete();
		if (me->lastBmp)
		{
			DEL_CLASS(me->lastBmp);
			me->lastBmp = 0;
		}
		me->dispImageUF = 0;
		me->dispImageWF = 0;

		if (me->inputMode == IM_SPECIES)
		{
			NN<OrganImageItem> img = me->lbImage->GetSelectedItem().GetNN<OrganImageItem>();
			me->dispImage = me->env->ParseImage(img, me->dispImageUF, me->dispImageWF);
			NN<Media::ImageList> dispImage;
			if (me->dispImage.SetTo(dispImage))
			{
				me->pbImg->SetImage(dispImage->GetImage(0, 0), false);
			}
		}
		else if (me->inputMode == IM_GROUP)
		{
			OrganGroup *o = (OrganGroup*)me->lbImage->GetSelectedItem().p;
			if (o == 0 || o->GetPhotoSpecies() == -1)
				return;
			NN<OrganSpecies> sp;
			if (me->env->GetSpecies(o->GetPhotoSpecies()).SetTo(sp))
			{
				me->dispImage = me->env->ParseSpImage(sp);
				me->dispImageUF = 0;
				me->dispImageWF = 0;
				NN<Media::ImageList> dispImage;
				if (me->dispImage.SetTo(dispImage))
				{
					me->pbImg->SetImage(dispImage->GetImage(0, 0), false);
				}
				sp.Delete();
			}
		}
	}
}

Bool __stdcall SSWR::OrganMgr::OrganMainForm::OnImgRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	NN<OrganSpecies> species;
	if (me->inputMode == IM_SPECIES && species.Set(me->lastSpeciesObj))
	{
		UOSInt index = me->lbImage->GetSelectedIndex();
		if (index == INVALID_INDEX)
			return false;
		Bool showDef = false;
		if (me->lbObj->GetSelectedIndex() == INVALID_INDEX)
		{
			showDef = true;
		}
		else if (((OrganGroupItem*)me->lbObj->GetSelectedItem().p)->GetItemType() == OrganGroupItem::IT_PARENT)
		{
			showDef = true;
		}
		if (showDef)
		{
			NN<OrganImageItem> imgItem = me->lbImage->GetItem(index).GetNN<OrganImageItem>();
			if (imgItem->GetIsCoverPhoto())
				return false;
			NN<OrganGroup> go = me->lbDir->GetSelectedItem().GetNN<OrganGroup>();
			me->env->SetGroupDefSp(go, imgItem);
			SDEL_STRING(me->initSelImg);
			me->initSelImg = imgItem->GetDispName()->Clone().Ptr();
			me->UpdateImgDir();
		}
		else
		{
			NN<OrganImageItem> imgItem = me->lbImage->GetItem(index).GetNN<OrganImageItem>();
			if (imgItem->GetIsCoverPhoto())
				return false;
			me->env->SetSpeciesImg(species, imgItem);
			SDEL_STRING(me->initSelImg);
			me->initSelImg = imgItem->GetDispName()->Clone().Ptr();
			me->UpdateImgDir();
		}
	}
	else if (me->inputMode == IM_GROUP)
	{
/*		System::Int32 index = this->lbImage->IndexFromPoint(e->X, e->Y);
		if (index == -1)
			return;
		Group *o = static_cast<Group*>(lbImage->Items->Item[index]);
		if (o->isDefault || o->photoSpecies == -1)
			return;
		Group *go = static_cast<Group*>(this->lbDir->SelectedItem);
		
		if (go->photoSpecies != -1)
		{
			System::Int32 i = 0;
			while (i < this->lbImage->Items->Count)
			{
				Group *sp = static_cast<Group*>(lbImage->Items->Item[i]);
				if (sp->isDefault)
				{
					sp->isDefault = false;
					lbImage->Items->Item[i] = sp;
					break;
				}
				i += 1;
			}
		}
		System::Int32 oldId = go->photoSpecies;
		go->photoGroup = o->id;
		go->photoSpecies = o->photoSpecies;
		if (oldId == -1)
		{
			db->ExecuteNonQuery(System::String::Concat(S"update groups set photo_group=", db->DBInt32(go->photoGroup), S", photo_species=", System::String::Concat(db->DBInt32(go->photoSpecies), S" where id=", db->DBInt32(go->id))));
		}
		else
		{
			db->ExecuteNonQuery(System::String::Concat(S"update groups set photo_group=", db->DBInt32(go->photoGroup), S", photo_species=", System::String::Concat(db->DBInt32(go->photoSpecies), S" where id=", db->DBInt32(go->id))));
			db->ExecuteNonQuery(System::String::Concat(S"update groups set photo_species=", db->DBInt32(go->photoSpecies), S" where photo_species=", System::String::Concat(db->DBInt32(oldId), S" and group_type < ", db->DBInt32(go->group_type))));
		}
		o->isDefault = true;
		lbImage->Items->Item[index] = o;*/
	}
	return false;
}

Bool __stdcall SSWR::OrganMgr::OrganMainForm::OnImgMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if ((me->dispImageUF.IsNull() && me->dispImageWF.IsNull()) || !me->dispImageToCrop)
	{
		return false;
	}

	me->dispImageDown = true;
	me->dispImageDownPos = scnPos;
	me->dispImageCurrPos = scnPos;
	return true;
}

Bool __stdcall SSWR::OrganMgr::OrganMainForm::OnImgMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->dispImageToCrop && me->dispImageDown)
	{
		Math::Coord2D<OSInt> rect[2];
		Math::Coord2DDbl pt1;
		Math::Coord2DDbl pt2;
		NN<UserFileInfo> userFile;
		NN<WebFileInfo> wfile;
		if (me->CalcCropRect(rect))
		{
			pt1 = me->pbImg->Scn2ImagePos(rect[0]);
			pt2 = me->pbImg->Scn2ImagePos(rect[1]);
			NN<Media::RasterImage> img;
			NN<Media::ImageList> dispImage;
			if (me->dispImage.SetTo(dispImage) && dispImage->GetImage(0, 0).SetTo(img))
			{
				if (me->dispImageUF.SetTo(userFile))
				{
					me->env->UpdateUserFileCrop(userFile, pt1.x, pt1.y, UOSInt2Double(img->info.dispSize.x) - pt2.x, UOSInt2Double(img->info.dispSize.y) - pt2.y);
				}
				else if (me->dispImageWF.SetTo(wfile))
				{
					me->env->UpdateWebFileCrop(wfile, pt1.x, pt1.y, UOSInt2Double(img->info.dispSize.x) - pt2.x, UOSInt2Double(img->info.dispSize.y) - pt2.y);
				}
			}
		}

		me->dispImageToCrop = false;
		me->dispImageDown = false;
		me->pbImg->UpdateBufferImage();
		return true;
	}
	return false;
}

Bool __stdcall SSWR::OrganMgr::OrganMainForm::OnImgMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->dispImageDown)
	{
		me->dispImageCurrPos = scnPos;
		me->pbImg->UpdateBufferImage();
	}
	return false;
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImgDraw(AnyType userObj, UnsafeArray<UInt8> imgPtr, UOSInt w, UOSInt h, UOSInt bpl)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	Math::Coord2DDbl pos1;
	Math::Coord2DDbl pos2;
	NN<Media::ImageList> dispImage;
	if (me->dispImage.SetTo(dispImage))
	{
		NN<UserFileInfo> userFile;
		NN<WebFileInfo> wfile;
		if (me->dispImageUF.SetTo(userFile))
		{
			NN<Media::RasterImage> img;
			if (userFile->cropLeft != 0 || userFile->cropTop != 0 || userFile->cropRight != 0 || userFile->cropBottom)
			{
				if (dispImage->GetImage(0, 0).SetTo(img))
				{
					pos1 = me->pbImg->Image2ScnPos(Math::Coord2DDbl(userFile->cropLeft, userFile->cropTop));
					pos2 = me->pbImg->Image2ScnPos(Math::Coord2DDbl(UOSInt2Double(img->info.dispSize.x) - userFile->cropRight, UOSInt2Double(img->info.dispSize.y) - userFile->cropBottom));
					if (pos2.x < 0 || pos1.x >= UOSInt2Double(w) || pos2.y < 0 || pos1.y >= UOSInt2Double(h))
					{

					}
					else if (pos1.x < 0 || pos2.x >= UOSInt2Double(w) || pos1.y < 0 || pos2.y >= UOSInt2Double(h))
					{
						Media::ImageUtil::DrawHLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos1.y), Double2OSInt(pos1.x), Double2OSInt(pos2.x), 0xff4040ff);
						Media::ImageUtil::DrawHLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos2.y), Double2OSInt(pos1.x), Double2OSInt(pos2.x), 0xff4040ff);
						Media::ImageUtil::DrawVLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos1.x), Double2OSInt(pos1.y), Double2OSInt(pos2.y), 0xff4040ff);
						Media::ImageUtil::DrawVLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos2.x), Double2OSInt(pos1.y), Double2OSInt(pos2.y), 0xff4040ff);
					}
					else
					{
						ImageUtil_DrawRectNA32(imgPtr.Ptr() + bpl * (UInt32)Double2Int32(pos1.y) + (UInt32)Double2Int32(pos1.x) * 4, (UInt32)Double2Int32(pos2.x - pos1.x), (UInt32)Double2Int32(pos2.y - pos1.y), bpl, 0xff4040ff);
					}
				}
			}
		}
		else if (me->dispImageWF.SetTo(wfile))
		{
			NN<Media::RasterImage> img;
			if (wfile->cropLeft != 0 || wfile->cropTop != 0 || wfile->cropRight != 0 || wfile->cropBottom)
			{
				if (dispImage->GetImage(0, 0).SetTo(img))
				{
					pos1 = me->pbImg->Image2ScnPos(Math::Coord2DDbl(wfile->cropLeft, wfile->cropTop));
					pos2 = me->pbImg->Image2ScnPos(Math::Coord2DDbl(UOSInt2Double(img->info.dispSize.x) - wfile->cropRight, UOSInt2Double(img->info.dispSize.y) - wfile->cropBottom));
					if (pos2.x < 0 || pos1.x >= UOSInt2Double(w) || pos2.y < 0 || pos1.y >= UOSInt2Double(h))
					{

					}
					else if (pos1.x < 0 || pos2.x >= UOSInt2Double(w) || pos1.y < 0 || pos2.y >= UOSInt2Double(h))
					{
						Media::ImageUtil::DrawHLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos1.y), Double2OSInt(pos1.x), Double2OSInt(pos2.x), 0xff4040ff);
						Media::ImageUtil::DrawHLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos2.y), Double2OSInt(pos1.x), Double2OSInt(pos2.x), 0xff4040ff);
						Media::ImageUtil::DrawVLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos1.x), Double2OSInt(pos1.y), Double2OSInt(pos2.y), 0xff4040ff);
						Media::ImageUtil::DrawVLineNA32(imgPtr, w, h, bpl, Double2OSInt(pos2.x), Double2OSInt(pos1.y), Double2OSInt(pos2.y), 0xff4040ff);
					}
					else
					{
						ImageUtil_DrawRectNA32(imgPtr.Ptr() + bpl * (UInt32)Double2Int32(pos1.y) + (UInt32)Double2Int32(pos1.x) * 4, (UInt32)Double2Int32(pos2.x - pos1.x), (UInt32)Double2Int32(pos2.y - pos1.y), bpl, 0xff4040ff);
					}
				}
			}
		}
	}
	if (me->dispImageDown)
	{
		Math::Coord2D<OSInt> rect[2];
		if (me->CalcCropRect(rect))
		{
			ImageUtil_DrawRectNA32(imgPtr.Ptr() + (OSInt)bpl * rect[0].y + rect[0].x * 4, (UOSInt)(rect[1].x - rect[0].x), (UOSInt)(rect[1].y - rect[0].y), bpl, 0xffff0000);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImgDblClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		UOSInt index = me->lbImage->GetSelectedIndex();
		if (index == INVALID_INDEX)
			return;
		Bool showDef = false;
		if (me->lbObj->GetSelectedIndex() == INVALID_INDEX)
		{
			showDef = true;
		}
		else if (((OrganGroupItem*)me->lbObj->GetSelectedItem().p)->GetItemType() == OrganGroupItem::IT_PARENT)
		{
			showDef = true;
		}
		if (showDef)
		{
		}
		else
		{
			NN<OrganImageItem> imgItem = me->lbImage->GetItem(index).GetNN<OrganImageItem>();
			NN<UserFileInfo> userFile;
			NN<WebFileInfo> wfile;
			if (imgItem->GetUserFile().SetTo(userFile))
			{
				OrganImageDetailForm frm(0, me->GetUI(), me->env, userFile);
				if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
				{
					me->env->UpdateUserFileDesc(userFile, frm.GetDescript().Or(U8STR("")));
				}
			}
			else if (imgItem->GetSrcURL() && imgItem->GetWebFile().SetTo(wfile))
			{
				OrganImageWebForm frm(0, me->GetUI(), me->env, imgItem, wfile);
				if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
				{
					Text::String *sURL = frm.GetSrcURL();
					Text::String *location = frm.GetLocation();
					if (!Text::StringTool::Equals(wfile->srcUrl.Ptr(), sURL) || !Text::StringTool::Equals(wfile->location.Ptr(), location))
					{
						NN<OrganGroupItem> item = me->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
						me->env->UpdateSpeciesWebFile(NN<OrganSpecies>::ConvertFrom(item), wfile, sURL, location);
						SDEL_STRING(me->initSelImg);
						me->initSelImg = imgItem->GetDispName()->Clone().Ptr();
						me->UpdateImgDir();
					}
				}
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImgDirClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	if (me->inputMode == IM_SPECIES)
	{
		UOSInt i;
		NN<OrganSpecies> o;
		NN<OrganGroupItem> gi;
		i = me->lbObj->GetSelectedIndex();
		if (i == INVALID_INDEX)
		{
			return;
		}
		else
		{
            gi = me->lbObj->GetItem(i).GetNN<OrganGroupItem>();
			if (gi->GetItemType() == OrganGroupItem::ItemType::IT_PARENT)
				return;
		}

		o = NN<OrganSpecies>::ConvertFrom(gi);
		sptr = me->env->GetSpeciesDir(o, sbuff);
		Manage::Process::OpenPath(CSTRP(sbuff, sptr));
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImageRotateClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		UOSInt index = me->lbImage->GetSelectedIndex();
		if (index == INVALID_INDEX)
			return;
		OrganImageItem *imgItem = (OrganImageItem*)me->lbImage->GetItem(index).p;
		NN<UserFileInfo> userFile;
		if (imgItem->GetFileType() == OrganImageItem::FileType::UserFile && imgItem->GetUserFile().SetTo(userFile))
		{
			me->env->UpdateUserFileRot(userFile, (userFile->rotType + 1) & 3);
			me->OnImgSelChg(me);
			me->lbImage->Focus();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImageCropClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->dispImageUF != 0 || me->dispImageWF != 0)
	{
		me->dispImageToCrop = !me->dispImageToCrop;
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImageSaveClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		UOSInt index = me->lbImage->GetSelectedIndex();
		if (index == INVALID_INDEX)
			return;
		OrganImageItem *imgItem = (OrganImageItem*)me->lbImage->GetItem(index).p;
		NN<UserFileInfo> userFile;
		if (imgItem->GetFileType() == OrganImageItem::FileType::UserFile && imgItem->GetUserFile().SetTo(userFile))
		{
			Text::StringBuilderUTF8 sb;
			if (me->env->GetUserFilePath(userFile, sb))
			{
				NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"OrganMgrSave", true);
				dlg->AddFilter(CSTR("*.jpg"), CSTR("JPEG File"));
				dlg->SetFileName(userFile->oriFileName->ToCString());
				if (dlg->ShowDialog(me->GetHandle()))
				{
					IO::FileUtil::CopyFile(sb.ToCString(), dlg->GetFileName()->ToCString(), IO::FileUtil::FileExistAction::Fail, 0, 0);
				}
				dlg.Delete();
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImageSaveAllClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
		if (dlg->ShowDialog(me->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			NN<OrganImageItem> imgItem;
			NN<UserFileInfo> userFile;
			UOSInt i;
			UOSInt j;
			i = 0;
			j = me->imgItems.GetCount();
			while (i < j)
			{
				imgItem = me->imgItems.GetItemNoCheck(i);
				if (imgItem->GetFileType() == OrganImageItem::FileType::UserFile && imgItem->GetUserFile().SetTo(userFile))
				{
					sb.ClearStr();
					if (me->env->GetUserFilePath(userFile, sb))
					{
						sb2.ClearStr();
						sb2.Append(dlg->GetFolder());
						if (!sb2.EndsWith(IO::Path::PATH_SEPERATOR))
						{
							sb2.AppendChar(IO::Path::PATH_SEPERATOR, 1);
						}
						sb2.AppendC(userFile->oriFileName->v, userFile->oriFileName->leng);
						IO::FileUtil::CopyFile(sb.ToCString(), sb2.ToCString(), IO::FileUtil::FileExistAction::Fail, 0, 0);
					}
				}
				i++;
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImageClipboardClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		UOSInt i = me->lbObj->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			NN<OrganGroupItem> gi = me->lbObj->GetItem(i).GetNN<OrganGroupItem>();
			if (gi->GetItemType() != OrganGroupItem::IT_PARENT)
			{
				UI::Clipboard clipboard(me->hwnd);
				UTF8Char sbuff[512];
				UnsafeArray<UTF8Char> sptr;
				UOSInt i;
				UOSInt j;
				UInt32 fmt;
				Data::ArrayList<UInt32> formats;
				UInt32 filePathFmt = (UInt32)-1;
				UInt32 urlFmt = (UInt32)-1;
				clipboard.GetDataFormats(&formats);
				i = 0;
				j = formats.GetCount();
				while (i < j)
				{
					fmt = formats.GetItem(i);
					sptr = UI::Clipboard::GetFormatName(fmt, sbuff, 256);
					if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("application/x-moz-file-promise-url")))
					{
						urlFmt = fmt;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HDROP")))
					{
						filePathFmt = fmt;
					}
					else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("URIs")))
					{
						filePathFmt = fmt;
					}
					i++;
				}

				if (urlFmt != (UInt32)-1 && filePathFmt != (UInt32)-1)
				{
					Bool succ;
					Text::StringBuilderUTF8 urlSb;
					Text::StringBuilderUTF8 fileNameSb;
					clipboard.GetDataText(urlFmt, urlSb);
					clipboard.GetDataText(filePathFmt, fileNameSb);

					if (IO::Path::GetFileSize(fileNameSb.ToString()) > 0)
					{
						{
							IO::FileStream fs(fileNameSb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							NN<Text::String> url = Text::String::New(urlSb.ToString(), urlSb.GetLength());
							succ = (me->env->AddSpeciesWebFile(NN<OrganSpecies>::ConvertFrom(gi), url, url, &fs, sbuff) == OrganEnv::FS_SUCCESS);
							url->Release();
						}

						if (succ)
						{
							SDEL_STRING(me->initSelImg);
							me->initSelImg = Text::String::NewNotNullSlow(sbuff).Ptr();
							me->UpdateImgDir();
						}
					}
				}
				else if (filePathFmt != (UInt32)-1)
				{
					Text::StringBuilderUTF8 sb;

					if (clipboard.GetDataText(filePathFmt, sb))
					{
						Bool chg = false;
						Bool firstPhoto = me->lbImage->GetCount() == 0;
						SDEL_STRING(me->initSelImg);
						Text::PString sarr[2];
						sarr[1] = sb;
						printf("HDROP: %s\r\n", sb.ToPtr());
						j = 2;
						while (j == 2)
						{
							j = Text::StrSplitLineP(sarr, 2, sarr[1]);
							OrganEnv::FileStatus fs;
							if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("file://")))
							{
								sptr = Text::URLString::GetURLFilePath(sbuff, sarr[0].v, sarr[0].leng).Or(sbuff);
								sarr[0].v = sbuff;
								sarr[0].leng = (UOSInt)(sptr - sbuff);
							}
							fs = me->env->AddSpeciesFile(NN<OrganSpecies>::ConvertFrom(gi), sarr[0].ToCString(), firstPhoto, false, 0);
							if (fs == OrganEnv::FS_SUCCESS)
							{
								if (!chg)
								{
									UOSInt tmp = Text::StrLastIndexOfChar(sarr[0].v, IO::Path::PATH_SEPERATOR);
									me->initSelImg = Text::String::New(&sarr[0].v[tmp + 1], sarr[0].leng - tmp - 1).Ptr();
								}
								chg = true;
								firstPhoto = false;
							}
							else if (fs == OrganEnv::FS_NOTSUPPORT)
							{
							}
							else
							{
								UOSInt i;
								NN<Text::String> s;
								Text::StringBuilderUTF8 sb;
								i = Text::StrLastIndexOfChar(sarr[0].v, IO::Path::PATH_SEPERATOR);
								s = Text::String::NewNotNull(L"不能複製檔案: ");
								sb.Append(s);
								s->Release();
								sb.AppendC(&sarr[0].v[i + 1], sarr[0].leng - i - 1);
								s = Text::String::NewNotNull(L", 要繼續?");
								sb.Append(s);
								s->Release();
								s = Text::String::NewNotNull(L"錯誤");
								if (!me->ui->ShowMsgYesNo(sb.ToCString(), s->ToCString(), me))
								{
									s->Release();
									break;
								}
								s->Release();
							}
						}

						if (chg)
						{
							me->UpdateImgDir();
						}
					}
				}
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpAddClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UOSInt i;
	if (me->SpeciesFormValid())
	{
		if (me->inputMode == IM_SPECIES || me->inputMode == IM_EMPTY)
		{
			Text::StringBuilderUTF8 sb;
			Text::StringBuilderUTF8 sb2;
			Text::StringBuilderUTF8 sb3;
			OrganGroup *grp = (OrganGroup*)me->lbDir->GetSelectedItem().p;
			NN<OrganGroupItem> gi;
			Int32 id = grp->GetGroupId();
			sb.ClearStr();
			me->txtSpeciesSName->GetText(sb);
			if (me->env->IsSpeciesExist(sb.ToString()))
			{
				me->ui->ShowMsgOK(CSTR("Species already exist"), CSTR("Add Species"), me);
				return;
			}
			sb2.AppendC(UTF8STRC("Species already exist in book:\r\n"));
			if (me->env->IsBookSpeciesExist(sb.ToString(), sb2))
			{
				sb2.AppendC(UTF8STRC("\r\n, continue?"));
				if (!me->ui->ShowMsgYesNo(sb2.ToCString(), CSTR("Add Species"), me))
				{
					return;
				}
			}

			NN<OrganSpecies> sp;
			NEW_CLASSNN(sp, OrganSpecies());
			sb3.ClearStr();
			me->txtSpeciesEName->GetText(sb3);
			sp->SetEName(sb3.ToCString());
			sb3.ClearStr();
			me->txtSpeciesCName->GetText(sb3);
			sp->SetCName(sb3.ToCString());
			sb3.ClearStr();
			me->txtSpeciesSName->GetText(sb3);
			sp->SetSName(sb3.ToCString());
			sp->SetGroupId(id);
			sb3.ClearStr();
			me->txtSpeciesDesc->GetText(sb3);
			sp->SetDesc(sb3.ToCString());
			sb3.ClearStr();
			me->txtSpeciesDName->GetText(sb3);
			sp->SetDirName(sb3.ToCString());
			sb3.ClearStr();
			me->txtSpeciesKey->GetText(sb3);
			sp->SetIDKey(sb3.ToCString());
			if (me->env->AddSpecies(sp))
			{
				me->lastSpeciesObj = 0;
				me->UpdateDir();

				UOSInt matchSpInd = 0;
				OSInt matchSpId = -1;
				NN<OrganSpecies> sp2;
				i = me->groupItems.GetCount();
				while (i-- > 0)
				{
					gi = me->groupItems.GetItemNoCheck(i);
					if (gi->GetItemType() == OrganGroupItem::IT_SPECIES)
					{
						sp2 = NN<OrganSpecies>::ConvertFrom(gi);
						if (Text::StringTool::Equals(sp->GetSName(), sp2->GetSName()))
						{
							if (sp2->GetSpeciesId() > matchSpId)
							{
								matchSpId = sp2->GetSpeciesId();
								matchSpInd = i;
							}
						}
					}
				}
				me->lbObj->SetSelectedIndex(matchSpInd);
				me->txtSpeciesCName->Focus();
			}
			sp.Delete();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpRemoveClicked(AnyType userObj)
{
	UOSInt i;
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES)
	{
		i = me->lbObj->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			if (me->ui->ShowMsgYesNo(CSTR("Are you sure to remove the species?"), CSTR("Remove Species"), me))
			{
				NN<OrganSpecies> o = me->lbObj->GetItem(i).GetNN<OrganSpecies>();
				if (me->env->DelSpecies(o))
				{
					me->UpdateDir();
				}
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpPasteSNameClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->inputMode == IM_SPECIES || me->inputMode == IM_EMPTY)
	{
		Text::StringBuilderUTF8 sb;
		if (UI::Clipboard::GetString(me->GetHandle(), sb))
		{
			UnsafeArrayOpt<UTF8Char> sciPtr = 0;
			UnsafeArrayOpt<UTF8Char> chiPtr = 0;
			UnsafeArray<UTF8Char> nnsciPtr;
			UnsafeArray<UTF8Char> nnchiPtr;
			UnsafeArray<UTF8Char> sptr;
			UTF8Char c;
			Bool found = false;
			sb.Trim();
			sptr = sb.v;
			c = *sptr;
			if (c >= 128)
			{
				chiPtr = sb.v;
			}
			else if (c >= 'A' && c <= 'Z')
			{
				sciPtr = sb.v;
			}
			else
			{
				return;
			}
			while (true)
			{
				c = *++sptr;
				if (c == ' ')
				{
					while (*++sptr == ' ');
					c = *sptr;
					if (c >= 'A' && c <= 'Z' && sciPtr == 0)
					{
						sciPtr = sptr;
						c = *++sptr;
					}
					else if (c == '(' || (c >= 'A' && c <= 'Z'))
					{
						if (!found || !sciPtr.SetTo(nnsciPtr))
							return;
						me->txtSpeciesDesc->SetText(CSTRP(nnsciPtr, sb.GetEndPtr()));
						sptr[-1] = 0;
						sptr = Text::StrRTrim(nnsciPtr);
						me->txtSpeciesSName->SetText(CSTRP(nnsciPtr, sptr));
						if (chiPtr.SetTo(nnchiPtr))
						{
							nnsciPtr[-1] = 0;
							sptr = Text::StrRTrim(nnchiPtr);
							me->txtSpeciesCName->SetText(CSTRP(nnchiPtr, sptr));
						}
						return;
					}
					else
					{
						found = true;
					}
				}
				if (c >= 128 && sciPtr == 0)
				{
				}
				else if (c != '-' && (c < 'a' || c > 'z'))
				{
					return;
				}
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpBookYearChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	Text::StringBuilderUTF8 sb;
	Int32 spBkYr;
	NN<OrganBook> book;
	sb.ClearStr();
	me->txtSpBookYear->GetText(sb);
	spBkYr = Text::StrToInt32(sb.ToString());
	Data::ArrayListNN<OrganBook> items;
	if (spBkYr == 0)
	{
		me->env->GetBooksAll(items);
	}
	else
	{
		me->env->GetBooksOfYear(items, spBkYr);
	}

	me->cboSpBook->ClearItems();
	UOSInt i = 0;
	UOSInt j = items.GetCount();
	while (i < j)
	{
		book = items.GetItemNoCheck(i);
		sb.ClearStr();
		book->GetString(sb);
		me->cboSpBook->AddItem(sb.ToCString(), book);

		i++;
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpBookAddClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	Text::StringBuilderUTF8 sb;

	if (me->lastSpeciesObj == 0)
		return ;
	if (me->inputMode != IM_SPECIES)
		return ;
	UOSInt i = me->cboSpBook->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("Please select a book to add"), CSTR("Add Book"), me);
		return;
	}
	NN<OrganBook> bk = me->cboSpBook->GetItem(i).GetNN<OrganBook>();
	sb.ClearStr();
	me->txtSpBook->GetText(sb);
	if (sb.ToString()[0] == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter display name to add"), CSTR("Add Book"), me);
		return;
	}

	if (me->env->IsSpeciesBookExist(me->lastSpeciesObj->GetSpeciesId(), bk->GetBookId()))
	{
		if (!me->ui->ShowMsgYesNo(CSTR("The book is already exist. Are you sure to add this book?"), CSTR("Question"), me))
		{
			return;
		}
	}

	if (me->env->NewSpeciesBook(me->lastSpeciesObj->GetSpeciesId(), bk->GetBookId(), sb.ToString()))
	{
		UOSInt i;
		i = me->lvSpBook->AddItem(sb.ToCString(), 0);
		sb.ClearStr();
		bk->GetString(sb);
		me->lvSpBook->SetSubItem(i, 1, sb.ToCString());
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpBookSelChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	NN<Text::String> s;
	if (me->lvSpBook->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtSpBook->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpeciesSNameChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->newDirName)
	{
		Text::StringBuilderUTF8 sb;;
		me->txtSpeciesSName->GetText(sb);
		sb.ToLower();
		sb.ReplaceStr(UTF8STRC(" "), UTF8STRC("_"));
		sb.ReplaceStr(UTF8STRC("."), UTF8STRC(""));
		me->txtSpeciesDName->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnSpeciesColorClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->lastSpeciesObj == 0)
		return;
	NN<OrganSpecies> sp;
	if (me->inputMode == IM_SPECIES && sp.Set(me->lastSpeciesObj))
	{
		Media::ColorProfile profile(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->GetUI(), me->env->GetColorMgr(), me->env->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, profile, me->env->GetMonitorMgr());
		dlg.SetColor32(me->lastSpeciesObj->GetMapColor());
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->env->SetSpeciesMapColor(sp, dlg.GetColor32());
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnTabSelChg(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->tcMain->GetSelectedIndex() == 2 || me->tcMain->GetSelectedIndex() == 3)
	{
		if (me->indexChanged)
		{
			me->indexChanged = false;
			SDEL_STRING(me->initSelImg);
			me->UpdateImgDir();
		}
	}
	else
	{
		me->pbImg->SetImage(0, false);
		me->dispImage.Delete();
		SDEL_CLASS(me->lastBmp);
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImagePickClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[512];
	Data::ArrayListUInt32 sels;
	UOSInt i;
	UOSInt j;
	NN<OrganImages> imgs;
	me->lbImage->GetSelectedIndices(&sels);
	if (sels.GetCount() <= 0)
		return;
	if (me->inputMode == IM_SPECIES)
	{
		Bool showDef = false;
		NN<OrganGroupItem> gi;
		me->ClearPicks();
        gi = me->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
		if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
		{
			showDef = true;
		}
		if (showDef)
		{
		}
		else
		{
			me->env->GetSpeciesDir(NN<OrganSpecies>::ConvertFrom(gi), sbuff);
			i = 0;
			j = sels.GetCount();
			while (i < j)
			{
				NEW_CLASSNN(imgs, OrganImages(me->imgItems.GetItemNoCheck(sels.GetItem(i)), sbuff));
				me->pickObjs.Add(imgs);
				i++;
			}
		}

		me->UpdatePicks();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImagePickAddClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[512];
	Data::ArrayListUInt32 sels;
	NN<OrganGroupItem> gi;
	UOSInt i;
	UOSInt j;
	NN<OrganImages> imgs;
	me->lbImage->GetSelectedIndices(&sels);
	if (sels.GetCount() <= 0)
		return;
	if (me->pickObjs.GetCount() > 0)
	{
		gi = me->pickObjs.GetItemNoCheck(0);
		if (gi->GetItemType() != SSWR::OrganMgr::OrganGroupItem::IT_IMAGE)
		{
			return;
		}
	}
	if (me->inputMode == IM_SPECIES)
	{
		Bool showDef = false;
        gi = me->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
		if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
		{
			showDef = true;
		}
		if (showDef)
		{
		}
		else
		{
			me->env->GetSpeciesDir(NN<OrganSpecies>::ConvertFrom(gi), sbuff);
			i = 0;
			j = sels.GetCount();
			while (i < j)
			{
				NEW_CLASSNN(imgs, OrganImages(me->imgItems.GetItemNoCheck(sels.GetItem(i)), sbuff));
				me->pickObjs.Add(imgs);
				i++;
			}
		}

		me->UpdatePicks();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnImagePickAllClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;
	NN<OrganGroupItem> gi;
	NN<OrganImages> imgs;
	if (me->inputMode == IM_SPECIES)
	{
		Bool showDef = false;
		me->ClearPicks();
        gi = me->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
		if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
		{
			showDef = true;
		}
		if (showDef)
		{
		}
		else
		{
			me->env->GetSpeciesDir(NN<OrganSpecies>::ConvertFrom(gi), sbuff);
			i = 0;
			j = me->imgItems.GetCount();
			while (i < j)
			{
				NEW_CLASSNN(imgs, OrganImages(me->imgItems.GetItemNoCheck(i), sbuff));
				me->pickObjs.Add(imgs);
				i++;
			}
		}

		me->UpdatePicks();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnObjPickClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	NN<OrganGroupItem> gi;
	Data::ArrayListUInt32 sels;
	Data::ArrayListNN<OrganGroupItem> newList;
	me->lbObj->GetSelectedIndices(&sels);
	UOSInt i;
	UOSInt j;
	i = 0;
	j = sels.GetCount();
	while (i < j)
	{
		gi = me->groupItems.GetItemNoCheck(sels.GetItem(i));
		if (gi->GetItemType() != OrganGroupItem::IT_PARENT)
		{
			newList.Add(gi->Clone());
		}
		i++;
	}
	if (newList.GetCount() > 0)
	{
		me->ClearPicks();
		me->pickObjs.AddAll(newList);
		me->UpdatePicks();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnObjPlaceClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	NN<OrganGroupItem> gi;
	if (me->pickObjs.GetCount() == 0)
		return;

	gi = me->pickObjs.GetItemNoCheck(0);
	if (gi->GetItemType() == OrganGroupItem::IT_IMAGE)
	{
		Data::ArrayListUInt32 sels;
		Data::ArrayListNN<OrganImages> imgList;
		me->lbObj->GetSelectedIndices(&sels);
		if (sels.GetCount() != 1 && me->inputMode != IM_GROUP)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorNonSp")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
			return;
		}
		gi = me->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
		if (gi->GetItemType() != OrganGroupItem::IT_SPECIES)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorGroupNonSp")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
			return;
		}
		SDEL_STRING(me->initSelImg);
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->pickObjs.GetCount();
		if (j > 0)
		{
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			sbuff[0] = 0;
			sptr = NN<OrganImages>::ConvertFrom(me->pickObjs.GetItemNoCheck(0))->GetItemName(sbuff);
			me->initSelImg = Text::String::NewP(sbuff, sptr).Ptr();
		}
		while (i < j)
		{
			imgList.Add(NN<OrganImages>::ConvertFrom(me->pickObjs.GetItemNoCheck(i)));
			i++;
		}
		me->env->MoveImages(imgList, NN<OrganSpecies>::ConvertFrom(gi), me);

		me->ClearPicks();
		me->UpdatePicks();

		if (me->tcMain->GetSelectedIndex() == 2)
			me->UpdateImgDir();
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_SPECIES)
	{
		if (me->inputMode != IM_SPECIES && me->inputMode != IM_EMPTY)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorNotSpAvail")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
			return;
		}

		NN<OrganGroup> grp = me->lbDir->GetItem(me->lbDir->GetSelectedIndex()).GetNN<OrganGroup>();
		Data::ArrayListNN<OrganSpecies> spList;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->pickObjs.GetCount();
		while (i < j)
		{
			spList.Add(NN<OrganSpecies>::ConvertFrom(me->pickObjs.GetItemNoCheck(i)));
			i++;
		}
		if (me->env->MoveSpecies(spList, grp))
		{
			me->ClearPicks();
			me->UpdatePicks();
			me->UpdateDir();
		}
		else
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
		}
	}
	else if (gi->GetItemType() == OrganGroupItem::IT_GROUP)
	{
		if (me->inputMode != IM_GROUP && me->inputMode != IM_EMPTY)
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorNotGrpAvail")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
			return;
		}

		NN<OrganGroup> grp = me->lbDir->GetItem(me->lbDir->GetSelectedIndex()).GetNN<OrganGroup>();
		Data::ArrayListNN<OrganGroup> grpList;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = me->pickObjs.GetCount();
		while (i < j)
		{
			grpList.Add(NN<OrganGroup>::ConvertFrom(me->pickObjs.GetItemNoCheck(i)));
			i++;
		}
		if (me->env->MoveGroups(grpList, grp))
		{
			me->ClearPicks();
			me->UpdatePicks();
			me->UpdateDir();
		}
		else
		{
			me->ui->ShowMsgOK(me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me->env->GetLang(CSTR("MainFormObjPlaceErrorTitle")), me);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnObjCombineClicked(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->pickObjs.GetCount() == 0)
		return;
	if (me->lastSpeciesObj == 0 || me->pickObjs.GetCount() != 1)
	{
		me->ui->ShowMsgOK(CSTR("You can only combine single species"), CSTR("Error"), me);
		return;
	}
	NN<OrganSpecies> lastSp;
	if (me->inputMode == IM_SPECIES && lastSp.Set(me->lastSpeciesObj))
	{
		if (NN<OrganGroupItem>::ConvertFrom(me->pickObjs.GetItemNoCheck(0))->GetItemType() == OrganGroupItem::IT_SPECIES)
		{
			NN<OrganSpecies> species = NN<OrganSpecies>::ConvertFrom(me->pickObjs.GetItemNoCheck(0));
			if (me->env->CombineSpecies(lastSp, species))
			{
				me->ClearPicks();
				me->UpdatePicks();
				me->UpdateDir();
			}
			else
			{
				me->ui->ShowMsgOK(CSTR("Error in combining books"), CSTR("Error"), me);
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("You can only combine single species"), CSTR("Error"), me);
		}
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("You can only combine species"), CSTR("Error"), me);
		return;
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnTimerTick(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	if (me->mapUpdated)
	{
		me->mapUpdated = false;
		me->mcMap->UpdateMap();
		me->mcMap->Redraw();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnMapScaleScroll(AnyType userObj, UOSInt newVal)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	me->mcMap->SetMapScale(Double2Int32(me->mapTile->GetLevelScale(newVal)));
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnTileUpdated(AnyType userObj)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	me->mapUpdated = true;
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnMapMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	Bool updated = false;
	Math::Coord2D<OSInt> dispPos;
	NN<Media::DrawImage> img;
	if (me->mapCurrFile)
	{
		dispPos = me->mcMap->MapXY2ScnXY(Math::Coord2DDbl(me->mapCurrFile->lon, me->mapCurrFile->lat));
		if (scnPos.x >= dispPos.x - 3 && scnPos.x <= dispPos.x + 3 && scnPos.y >= dispPos.y - 3 && scnPos.y <= dispPos.y + 3)
		{
			return;
		}
		if (me->mapCurrImage.SetTo(img))
		{
			me->env->GetDrawEngine()->DeleteImage(img);
		}
		me->mapCurrImage = 0;
		me->mapCurrFile = 0;
		updated = true;
	}

	UOSInt i;
	UOSInt j;
	NN<UserFileInfo> ufile;
	i = 0;
	j = me->mapUFiles.GetCount();
	while (i < j)
	{
		ufile = me->mapUFiles.GetItemNoCheck(i);
		if (ufile->lat != 0 || ufile->lon != 0)
		{
			if (me->mcMap->InMapMapXY(Math::Coord2DDbl(ufile->lon, ufile->lat)))
			{
				dispPos = me->mcMap->MapXY2ScnXY(Math::Coord2DDbl(ufile->lon, ufile->lat));
				if (scnPos.x >= dispPos.x - 3 && scnPos.x <= dispPos.x + 3 && scnPos.y >= dispPos.y - 3 && scnPos.y <= dispPos.y + 3)
				{
					Text::StringBuilderUTF8 sb;
					NN<Media::ImageList> imgList;
					me->env->GetUserFilePath(ufile, sb);
					IO::StmData::FileData fd(sb.ToCString(), false);
					if (Optional<Media::ImageList>::ConvertFrom(me->env->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
					{
						imgList->ToStaticImage(0);
						NN<Media::StaticImage> img;
						if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
						{
							Media::StaticImage *nimg;
							img->To32bpp();
							me->mapResizer->SetTargetSize(Math::Size2D<UOSInt>(320, 320));
							me->mapResizer->SetResizeAspectRatio(Media::ImageResizer::RAR_SQUAREPIXEL);
							nimg = me->mapResizer->ProcessToNew(img);
							imgList.Delete();
							me->mapCurrFile = ufile.Ptr();
							me->mapCurrImage = me->env->GetDrawEngine()->ConvImageOrNull(nimg);
							SDEL_CLASS(nimg);
						}
						me->mcMap->Redraw();
					}
					else if (updated)
					{
						me->mcMap->Redraw();
					}
					return;
				}
			}
		}
		i++;
	}
	if (updated)
	{
		me->mcMap->Redraw();
	}
}

void __stdcall SSWR::OrganMgr::OrganMainForm::OnMapDraw(AnyType userObj, NN<Media::DrawImage> dimg, OSInt xOfst, OSInt yOfst)
{
	NN<OrganMainForm> me = userObj.GetNN<OrganMainForm>();
	NN<Media::DrawImage> img;
	if (me->mapCurrImage.SetTo(img))
	{
		Math::Size2D<UOSInt> scnSize = me->mcMap->GetSizeP();
		dimg->DrawImagePt(img, Math::Coord2DDbl(OSInt2Double(xOfst), OSInt2Double(yOfst + (OSInt)(scnSize.y - img->GetHeight()))));
		//BitBlt((HDC)hdc, 0, scnH - me->mapCurrImage->info.dispSize.y, me->mapCurrImage->info.dispSize.x, me->mapCurrImage->info.dispSize.y, (HDC)me->mapCurrImage->GetHDC(), 0, 0, SRCCOPY);
	}
}

OSInt __stdcall SSWR::OrganMgr::OrganMainForm::GroupCompare(NN<OrganGroupItem> group1, NN<OrganGroupItem> group2)
{
	if (group1->GetItemType() == SSWR::OrganMgr::OrganGroupItem::IT_PARENT)
	{
		return -1;
	}
	else if (group2->GetItemType() == SSWR::OrganMgr::OrganGroupItem::IT_PARENT)
	{
		return -1;
	}
	else
	{
		UTF8Char sbuff1[256];
		UTF8Char sbuff2[256];
		group1->GetEngName(sbuff1);
		group2->GetEngName(sbuff2);
		return Text::StrCompare(sbuff1, sbuff2);
	}
}

Bool SSWR::OrganMgr::OrganMainForm::CalcCropRect(Math::Coord2D<OSInt> *rect)
{
	OSInt drawWidth;
	OSInt drawHeight;
	if (this->dispImageDownPos.x < this->dispImageCurrPos.x)
	{
		drawWidth = this->dispImageCurrPos.x - this->dispImageDownPos.x;
	}
	else if (this->dispImageDownPos.x > this->dispImageCurrPos.x)
	{
		drawWidth = this->dispImageDownPos.x - this->dispImageCurrPos.x;
	}
	else
	{
		return false;
	}
	if (this->dispImageDownPos.y < this->dispImageCurrPos.y)
	{
		drawHeight = this->dispImageCurrPos.y - this->dispImageDownPos.y;
	}
	else if (this->dispImageDownPos.y > this->dispImageCurrPos.y)
	{
		drawHeight = this->dispImageDownPos.y - this->dispImageCurrPos.y;
	}
	else
	{
		return false;
	}

	NN<Media::RasterImage> img;
	NN<Media::ImageList> dispImage;
	if (!this->dispImage.SetTo(dispImage) || !dispImage->GetImage(0, 0).SetTo(img))
	{
		return false;
	}
	if ((OSInt)img->info.dispSize.x * drawHeight > (OSInt)img->info.dispSize.y * drawWidth)
	{
		drawHeight = MulDivOS(drawWidth, (OSInt)img->info.dispSize.y, (OSInt)img->info.dispSize.x);
	}
	else
	{
		drawWidth = MulDivOS(drawHeight, (OSInt)img->info.dispSize.x, (OSInt)img->info.dispSize.y);
	}

	if (this->dispImageDownPos.x < this->dispImageCurrPos.x)
	{
		rect[0].x = this->dispImageDownPos.x;
		rect[1].x = this->dispImageDownPos.x + drawWidth;
	}
	else
	{
		rect[0].x = this->dispImageDownPos.x - drawWidth;
		rect[1].x = this->dispImageDownPos.x;
	}
	if (this->dispImageDownPos.y < this->dispImageCurrPos.y)
	{
		rect[0].y = this->dispImageDownPos.y;
		rect[1].y = this->dispImageDownPos.y + drawHeight;
	}
	else
	{
		rect[0].y = this->dispImageDownPos.y - drawHeight;
		rect[1].y = this->dispImageDownPos.y;
	}
	return true;
}
void SSWR::OrganMgr::OrganMainForm::UpdateDir()
{
	NN<OrganGroup> grp;
	NN<OrganGroupItem> item;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (ToSaveGroup())
	{
        this->lbDir->SetSelectedIndex(this->lastDirIndex);
        this->lbObj->SetSelectedIndex(this->lastObjIndex);
        return;
	}

    if (ToSaveSpecies())
	{
        this->lbDir->SetSelectedIndex(this->lastDirIndex);
        this->lbObj->SetSelectedIndex(this->lastObjIndex);
        return;
	}

    ClearSpeciesForm();
    ClearGroupForm();
    if (lbDir->GetSelectedIndex() == INVALID_INDEX)
	{
		this->groupItems.DeleteAll();
        this->lbObj->ClearItems();
		this->inputMode = IM_EMPTY;
        this->tpGroup->SetEnabled(false);
        this->tpSpecies->SetEnabled(false);
	}
    else
	{
		UOSInt i = (UOSInt)this->lbDir->GetSelectedIndex();
		UOSInt j = this->lbDir->GetCount();
		UOSInt k;
		while (--j > i)
		{
            this->lbDir->RemoveItem(j);
			grp = this->groupList.GetItemNoCheck(j);
			this->groupList.RemoveAt(j);
			SDEL_STRING(this->initSelObj);
			this->initSelObj = Text::String::OrEmpty(grp->GetEName())->Clone().Ptr();
			grp.Delete();
		}
        this->lastSpeciesObj = 0;
        this->lastGroupObj = 0;

		this->groupItems.DeleteAll();
		NEW_CLASSNN(item, OrganParentItem());
		this->groupItems.Add(item);
		this->lbObj->ClearItems();
		grp = this->lbDir->GetItem(this->lbDir->GetSelectedIndex()).GetNN<OrganGroup>();
		i = this->env->GetGroupItems(this->groupItems, grp);
		if (i == 0)
		{
			this->inputMode = IM_EMPTY;
            this->tpGroup->SetEnabled(true);
            this->tpSpecies->SetEnabled(true);

			i = 0;
			j = this->groupItems.GetCount();
			while (i < j)
			{
				item = this->groupItems.GetItemNoCheck(i);
				sptr = item->GetItemName(sbuff);
				k = this->lbObj->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, item);
				i++;
			}
		}
		else
		{
			item = this->groupItems.GetItemNoCheck(1);
			if (item->GetItemType() == OrganGroupItem::IT_GROUP)
			{
				this->inputMode = IM_GROUP;
				this->tpGroup->SetEnabled(true);
				this->tpSpecies->SetEnabled(false);
			}
			else
			{
				this->inputMode = IM_SPECIES;
				this->tpGroup->SetEnabled(false);
				this->tpSpecies->SetEnabled(true);
			}

			Data::Sort::ArtificialQuickSortFunc<NN<OrganGroupItem>>::Sort(this->groupItems, GroupCompare);
			i = 0;
			j = this->groupItems.GetCount();
			while (i < j)
			{
				item = this->groupItems.GetItemNoCheck(i);
				sptr = item->GetItemName(sbuff);
				k = this->lbObj->AddItem({sbuff, (UOSInt)(sptr - sbuff)}, item);
				if (i > 0 && this->initSelObj)
				{
					if (this->inputMode == IM_SPECIES)
					{
						if (Text::StringTool::Equals(NN<OrganSpecies>::ConvertFrom(item)->GetSName(), this->initSelObj))
						{
							this->lbObj->SetSelectedIndex(k);
						}
					}
					else if (this->inputMode == IM_GROUP)
					{
						if (Text::StringTool::Equals(NN<OrganGroup>::ConvertFrom(item)->GetEName(), this->initSelObj))
						{
							this->lbObj->SetSelectedIndex(k);
						}
					}
				}
				i++;
			}
		}
	}
}

void SSWR::OrganMgr::OrganMainForm::UpdateImgDir()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArrayOpt<const UTF8Char> csptr;
	UnsafeArray<const UTF8Char> nncsptr;
	Data::Timestamp ts;
	NN<Text::String> s;
	this->pbImg->SetImage(0, false);
	if (this->lastBmp)
	{
		DEL_CLASS(this->lastBmp);
		this->lastBmp = 0;
	}
	this->dispImage.Delete();

	NN<OrganImageItem> imgItem;
	UOSInt i;
	UOSInt j;
	this->ClearImgLayers();
	this->imgItems.DeleteAll();
	this->lbImage->ClearItems();
	if (this->inputMode == IM_SPECIES)
	{
		Bool showDef = false;
		NN<OrganSpecies> o;
		NN<OrganGroup> grp;
		NN<OrganGroupItem> gi;

		if (this->lbObj->GetSelectedIndex() != INVALID_INDEX)
		{
            gi = this->lbObj->GetSelectedItem().GetNN<OrganGroupItem>();
			if (gi->GetItemType() == OrganGroupItem::IT_PARENT)
				showDef = true;
		}
		else
		{
			showDef = true;
		}
		NN<OrganSpImgLayer> lyr;
		if (showDef)
		{
			grp = this->groupList.GetItemNoCheck(this->groupList.GetCount() - 1);
			this->env->GetGroupImages(this->imgItems, grp);

			Data::ArrayListNN<UserFileInfo> ufileList;
			Data::ArrayList<UInt32> ufileColor;
			this->env->GetGroupAllUserFile(ufileList, ufileColor, grp);
			i = ufileList.GetCount();
			while (i-- > 0)
			{
				lyr = this->GetImgLayer(ufileColor.GetItem(i));
				lyr->AddItem(ufileList.GetItemNoCheck(i));
			}
			
			this->mapUFiles.Clear();
			this->mapUFiles.AddAll(ufileList);
			this->mcMap->UpdateMap();
			this->mcMap->Redraw();
		}
        else
		{
			o = NN<OrganSpecies>::ConvertFrom(gi);
			this->env->GetSpeciesImages(this->imgItems, o);
			lyr = this->GetImgLayer(o->GetMapColor());
			lyr->AddItems(this->imgItems);
			this->mcMap->UpdateMap();
			this->mcMap->Redraw();

			NN<UserFileInfo> ufile;
			this->mapUFiles.Clear();
			i = 0;
			j = this->imgItems.GetCount();
			while (i < j)
			{
				imgItem = this->imgItems.GetItemNoCheck(i);
				if (imgItem->GetUserFile().SetTo(ufile))
				{
					this->mapUFiles.Add(ufile);
				}
				i++;
			}
		}

		UOSInt initSel = 0;
		j = this->imgItems.GetCount();
		if (showDef)
		{
			i = 0;
			while (i < j)
			{
				imgItem = this->imgItems.GetItemNoCheck(i);
				this->lbImage->AddItem(imgItem->GetDispName()->ToCString(), imgItem);
				if (this->initSelImg && Text::StringTool::Equals(imgItem->GetDispName(), this->initSelImg))
				{
					initSel = i;
				}
				i++;
			}
		}
		else
		{
			i = 0;
			while (i < j)
			{
				imgItem = this->imgItems.GetItemNoCheck(i);
				if (imgItem->GetIsCoverPhoto())
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("*"));
					if (this->initSelImg == 0)
					{
						initSel = i;
					}
				}
				else
				{
					sptr = sbuff;
				}
				sptr = imgItem->GetDispName()->ConcatTo(sptr);
				ts = imgItem->GetPhotoDate();
				if (ts != 0)
				{
					Data::Timestamp dt = ts.ToLocalTime();
					sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
					sptr = dt.ToString(sptr, "yyyy-MM-dd HH:mm:sszz");
					
					NN<UserFileInfo> userFile;
					Optional<UserFileInfo> optuserFile = 0;
					csptr = 0;
					if (imgItem->GetUserFile().SetTo(userFile))
					{
						csptr = OPTSTR_CSTR(userFile->location).v;
						optuserFile = userFile;
					}
					if (csptr.SetTo(nncsptr))
					{
						sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
						sptr = Text::StrConcat(sptr, nncsptr);
					}
					else
					{
						NN<Text::String> s;
						if (!this->env->GetLocName(imgItem->GetUserId(), dt, this, this->ui).SetTo(s))
						{
						}
						else
						{
							sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
							sptr = s->ConcatTo(sptr);
							if (optuserFile.SetTo(userFile))
							{
								this->env->UpdateUserFileLoc(userFile, s->v);
							}
						}
					}
					sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
				}
				else
				{
					Text::String *sURL = imgItem->GetSrcURL();
					if (sURL)
					{
						sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
						sptr = sURL->ConcatTo(sptr);
						sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
					}
				}
				NN<UserFileInfo> userFile;
				if (imgItem->GetUserFile().SetTo(userFile))
				{
					if (userFile->descript.SetTo(s))
					{
						sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
						sptr = s->ConcatTo(sptr);
					}
				}
				NN<WebFileInfo> wfile;
				if (imgItem->GetWebFile().SetTo(wfile))
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
					sptr = wfile->location->ConcatTo(sptr);
				}

				this->lbImage->AddItem(CSTRP(sbuff, sptr), imgItem);

				if (this->initSelImg && Text::StringTool::Equals(imgItem->GetDispName(), this->initSelImg))
				{
					initSel = i;
				}

				i++;
			}
		}
		if (j > 0)
		{
			this->lbImage->SetSelectedIndex(initSel);
		}
	}
	else if (this->inputMode == IM_GROUP)
	{
		NN<OrganGroupItem> gi;
		NN<OrganGroup> o;
		UOSInt i = 0;
		UOSInt j = this->lbObj->GetCount();
		while (i < j)
		{
			gi = this->lbObj->GetItem(i).GetNN<OrganGroupItem>();
			if (gi->GetItemType() != OrganGroupItem::IT_PARENT)
			{
				o = NN<OrganGroup>::ConvertFrom(gi);
				sptr = o->GetItemName(sbuff);
				this->lbImage->AddItem(CSTRP(sbuff, sptr), o);
			}
			i += 1;
		}

		if (this->groupList.GetItem(this->groupList.GetCount() - 1).SetTo(o))
		{
			Data::ArrayListNN<UserFileInfo> ufileList;
			Data::ArrayList<UInt32> ufileColor;
			NN<OrganSpImgLayer> lyr;
			this->env->GetGroupAllUserFile(ufileList, ufileColor, o);
			i = ufileList.GetCount();
			while (i-- > 0)
			{
				lyr = this->GetImgLayer(ufileColor.GetItem(i));
				lyr->AddItem(ufileList.GetItemNoCheck(i));
			}

			this->mapUFiles.Clear();
			this->mapUFiles.AddAll(ufileList);
			this->mcMap->UpdateMap();
			this->mcMap->Redraw();
		}
	}
}

void SSWR::OrganMgr::OrganMainForm::UpdateSpBook()
{
	Data::ArrayListNN<SpeciesBook> spBooks;
	NN<SpeciesBook> spBook;
	Text::StringBuilderUTF8 sb;

	this->env->GetSpeciesBooks(spBooks, this->lastSpeciesObj->GetSpeciesId());
	UOSInt i = 0;
	UOSInt j = spBooks.GetCount();
	this->lvSpBook->ClearItems();
	while (i < j)
	{
		spBook = spBooks.GetItemNoCheck(i);
		this->lvSpBook->AddItem(spBook->dispName, 0);
		sb.ClearStr();
		spBook->book->GetString(sb);
		this->lvSpBook->SetSubItem(i, 1, sb.ToCString());

		i++;
	}
	this->env->ReleaseSpeciesBooks(spBooks);
}

void SSWR::OrganMgr::OrganMainForm::UpdateSpBookList()
{
	Text::StringBuilderUTF8 sb;
	NN<OrganBook> book;
	Data::ArrayListNN<OrganBook> bookList;
	this->env->GetBooksAll(bookList);
	this->cboSpBook->ClearItems();
	UOSInt i = 0;
	UOSInt j = bookList.GetCount();
	while (i < j)
	{
		book = bookList.GetItemNoCheck(i);
		sb.ClearStr();
		book->GetString(sb);
		this->cboSpBook->AddItem(sb.ToCString(), book);

		i++;
	}
}

void SSWR::OrganMgr::OrganMainForm::UpdatePicks()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->env->GetLang(CSTR("MainFormPick1")));
	sb.AppendUOSInt(this->pickObjs.GetCount());
	sb.Append(this->env->GetLang(CSTR("MainFormPick2")));
	this->lblPickMsg->SetText(sb.ToCString());
}

void SSWR::OrganMgr::OrganMainForm::ClearPicks()
{
	this->pickObjs.DeleteAll();
}

Bool SSWR::OrganMgr::OrganMainForm::ToSaveGroup()
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	NN<OrganGroup> lastGroupObj;
	if (!lastGroupObj.Set(this->lastGroupObj))
		return false;

	if (this->GroupFormValid())
	{
		if (this->inputMode == IM_GROUP)
		{
//			Int32 id = this->lastGroupObj->GetGroupId();

			NN<OrganGroupType> grpType;
			i = this->cboGroupType->GetSelectedIndex();
			grpType = this->cboGroupType->GetItem(i).GetNN<OrganGroupType>();
			this->lastGroupObj->SetGroupType(grpType->GetSeq());
			sb.ClearStr();
			this->txtGroupEName->GetText(sb);
			this->lastGroupObj->SetEName(sb.ToCString());
			sb.ClearStr();
			this->txtGroupCName->GetText(sb);
			this->lastGroupObj->SetCName(sb.ToCString());
			sb.ClearStr();
			this->txtGroupDesc->GetText(sb);
			this->lastGroupObj->SetDesc(sb.ToCString());
			sb.ClearStr();
			this->txtGroupKey->GetText(sb);
			this->lastGroupObj->SetIDKey(sb.ToCString());
			this->lastGroupObj->SetAdminOnly(this->chkGroupAdmin->IsChecked());
			if (!this->env->SaveGroup(lastGroupObj))
			{
				this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSaveGroupError")), this->env->GetLang(CSTR("MainFormSaveGroupTitle")), this);
				return true;
			}
		}
	}
	else
	{
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganMainForm::ToSaveSpecies()
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt i;
	NN<OrganSpecies> lastSpeciesObj;
	if (!lastSpeciesObj.Set(this->lastSpeciesObj))
		return false;

	if (this->SpeciesFormValid())
	{
		if (this->inputMode == IM_SPECIES)
		{
//			Int32 id = this->lastSpeciesObj->GetSpeciesId();
			
			sptr = this->txtSpeciesDName->GetText(sbuff).Or(sbuff);
			if (!Text::String::OrEmpty(this->lastSpeciesObj->GetDirName())->Equals(sbuff, (UOSInt)(sptr - sbuff)))
			{
				sptr2 = this->env->GetSpeciesDir(lastSpeciesObj, sbuff2);
				sptr = Text::StrConcatC(sbuff, sbuff2, (UOSInt)(sptr2 - sbuff2));
				i = Text::StrLastIndexOfCharC(sbuff2, (UOSInt)(sptr2 - sbuff2), IO::Path::PATH_SEPERATOR);
				sptr2 = this->txtSpeciesDName->GetText(&sbuff2[i + 1]).Or(sptr2);
				
				if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::Directory)
				{
					if (IO::FileUtil::MoveFile(CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2), IO::FileUtil::FileExistAction::Fail, 0, 0))
					{
						this->lastSpeciesObj->SetDirName(CSTRP(&sbuff2[i + 1], sptr2));
					}
					else
					{
						this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSaveSpExist")), this->env->GetLang(CSTR("MainFormTitleSp")), this);
						return true;
					}
				}
				else
				{
					this->lastSpeciesObj->SetDirName(CSTRP(&sbuff2[i + 1], sptr2));
				}
			}
			Text::StringBuilderUTF8 sb;
			sb.ClearStr();
			this->txtSpeciesEName->GetText(sb);
			this->lastSpeciesObj->SetEName(sb.ToCString());
			sb.ClearStr();
			this->txtSpeciesCName->GetText(sb);
			this->lastSpeciesObj->SetCName(sb.ToCString());
			sb.ClearStr();
			this->txtSpeciesSName->GetText(sb);
			this->lastSpeciesObj->SetSName(sb.ToCString());
			sb.ClearStr();
			this->txtSpeciesDesc->GetText(sb);
			this->lastSpeciesObj->SetDesc(sb.ToCString());
			sb.ClearStr();
			this->txtSpeciesKey->GetText(sb);
			this->lastSpeciesObj->SetIDKey(sb.ToCString());
			if (!this->env->SaveSpecies(lastSpeciesObj))
			{
				this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSaveSpError")), this->env->GetLang(CSTR("MainFormTitleSp")), this);
				return true;
			}
		}
	}
	else
	{
		return true;
	}
	return false;
}

Bool SSWR::OrganMgr::OrganMainForm::GroupFormValid()
{
	UTF8Char sbuff[512];
	if (this->lbDir->GetSelectedIndex() == INVALID_INDEX)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormGroupErrorLoc")), this->env->GetLang(CSTR("MainFormTitleGroup")), this);
		return false;
	}

    if (this->cboGroupType->GetSelectedIndex() == INVALID_INDEX)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormGroupErrorGrpType")), this->env->GetLang(CSTR("MainFormTitleGroup")), this);
		return false;
	}

	if (this->txtGroupCName->GetText(sbuff) == sbuff)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormGroupErrorCName")), this->env->GetLang(CSTR("MainFormTitleGroup")), this);
        return false;
	}

	if (this->txtGroupEName->GetText(sbuff) == sbuff)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormGroupErrorEName")), this->env->GetLang(CSTR("MainFormTitleGroup")), this);
        return false;
	}
    return true;
}

Bool SSWR::OrganMgr::OrganMainForm::SpeciesFormValid()
{
	UTF8Char sbuff[512];
	if (this->txtSpeciesCName->GetText(sbuff) == sbuff)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSpeciesErrorCName")), this->env->GetLang(CSTR("MainFormTitleSp")), this);
		return false;
	}

	if (this->txtSpeciesSName->GetText(sbuff) == sbuff)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSpeciesErrorSName")), this->env->GetLang(CSTR("MainFormTitleSp")), this);
		return false;
	}

	if (this->txtSpeciesDName->GetText(sbuff) == sbuff)
	{
		this->ui->ShowMsgOK(this->env->GetLang(CSTR("MainFormSpeciesErrorDName")), this->env->GetLang(CSTR("MainFormTitleSp")), this);
        return false;
	}
	return true;
}

void SSWR::OrganMgr::OrganMainForm::ClearSpeciesForm()
{
    this->txtSpeciesCName->SetText(CSTR(""));
    this->txtSpeciesDesc->SetText(CSTR(""));
    this->txtSpeciesEName->SetText(CSTR(""));
    this->txtSpeciesId->SetText(CSTR(""));
    this->txtSpeciesSName->SetText(CSTR(""));
    this->txtSpeciesDName->SetText(CSTR(""));
    this->txtSpeciesKey->SetText(CSTR(""));
	this->lvSpBook->ClearItems();
    this->newDirName = true;
}

void SSWR::OrganMgr::OrganMainForm::ClearGroupForm()
{
    this->txtGroupCName->SetText(CSTR(""));
    this->txtGroupDesc->SetText(CSTR(""));
    this->txtGroupEName->SetText(CSTR(""));
    this->txtGroupId->SetText(CSTR(""));
    this->txtGroupKey->SetText(CSTR(""));
	this->chkGroupAdmin->SetChecked(false);
    if (this->lbObj->GetCount() == 0)
	{
		this->cboGroupType->SetSelectedIndex((UOSInt)-1);
	}
}

void SSWR::OrganMgr::OrganMainForm::FillGroupCboBox()
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NN<Data::ArrayListNN<OrganGroupType>> grpTypes = this->env->GetGroupTypes();
	NN<OrganGroupType> grpType;
	UOSInt i = 0;
	UOSInt j = grpTypes->GetCount();
	this->cboGroupType->ClearItems();
	while (i < j)
	{
		grpType = grpTypes->GetItemNoCheck(i);
		sptr = grpType->ToString(sbuff);
		this->cboGroupType->AddItem(CSTRP(sbuff, sptr), grpType);
		i++;
	}
}

void SSWR::OrganMgr::OrganMainForm::InitCategory()
{
	this->lbDir->SetSelectedIndex(0);
	this->FillGroupCboBox();
	this->UpdateDir();
}

void SSWR::OrganMgr::OrganMainForm::SelectGroup(NN<UI::GUIComboBox> cbo, Int32 groupType)
{
	NN<Data::ArrayListNN<OrganGroupType>> grpTypes = this->env->GetGroupTypes();
	UOSInt i = grpTypes->GetCount();
	while (i-- > 0)
	{
		NN<OrganGroupType> grpType = grpTypes->GetItemNoCheck(i);
		if (grpType->GetSeq() == groupType)
		{
            cbo->SetSelectedIndex(i);
            break;
		}
	}
}

void SSWR::OrganMgr::OrganMainForm::GoToDir(NN<OrganGroup> grp, Int32 parentId)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<OrganGroup> group;
	UOSInt j;
	UOSInt i = this->groupList.GetCount();
	while (i-- > 1)
	{
		this->lbDir->RemoveItem(i);
		group = this->groupList.GetItemNoCheck(i);
		this->groupList.RemoveAt(i);
		group.Delete();
	}
	this->groupList.Add(grp);
	while (this->env->GetGroup(parentId, parentId).SetTo(grp))
	{
		this->groupList.Insert(1, grp);
	}
	i = 1;
	j = this->groupList.GetCount();
	while (i < j)
	{
		grp = this->groupList.GetItemNoCheck(i);
		sptr = grp->GetItemName(sbuff);
		this->lbDir->AddItem(CSTRP(sbuff, sptr), grp);
		i++;
	}
	this->lbDir->SetSelectedIndex(j - 1);
}

void SSWR::OrganMgr::OrganMainForm::ClearImgLayers()
{
	UOSInt i;
	NN<OrganSpImgLayer> lyr;
	i = this->mapImgLyrs.GetCount();
	while (i-- > 0)
	{
		lyr = this->mapImgLyrs.GetItemNoCheck(i);
		lyr->ClearItems();
	}
}

NN<SSWR::OrganMgr::OrganSpImgLayer> SSWR::OrganMgr::OrganMainForm::GetImgLayer(UInt32 mapColor)
{
	NN<OrganSpImgLayer> lyr;
	if (this->mapImgLyrs.Get(mapColor).SetTo(lyr))
	{
		return lyr;
	}
	NN<Media::StaticImage> stimg;
	NN<Media::ImageList> imgList;
	Map::MapEnv::LayerItem sett;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt imgInd;
	UOSInt lyrInd;
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(lyr, OrganSpImgLayer());
	NEW_CLASSNN(stimg, Media::StaticImage(Math::Size2D<UOSInt>(7, 7), 0, 32, Media::PF_B8G8R8A8, 0, srcColor, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	lyr->SetCoordinateSystem(this->mapEnv->GetCoordinateSystem()->Clone());
	stimg->FillColor(mapColor);
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("PointImage")));
	imgList->AddImage(stimg, 0);
	sptr = Text::StrHexVal32(Text::StrConcatC(sbuff, UTF8STRC("Image")), mapColor);
	imgInd = this->mapEnv->AddImage(CSTRP(sbuff, sptr), imgList);
	lyrInd = this->mapEnv->AddLayer(0, lyr, true);
	this->mapEnv->GetLayerProp(sett, 0, lyrInd);
	sett.fontStyle = this->imgFontStyle;
	sett.labelCol = 0;
	sett.flags |= 3;
	sett.imgIndex = imgInd;
	this->mapEnv->SetLayerProp(sett, 0, lyrInd);
	this->mapImgLyrs.Put(mapColor, lyr);
	return lyr;
}

SSWR::OrganMgr::OrganMainForm::OrganMainForm(NN<UI::GUICore> ui, Optional<UI::GUIClientControl> parent, NN<OrganEnv> env) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(UTF8STRC("Arial"), 10.5, false);
	this->colorMgr = env->GetColorMgr();
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());
	this->env = env;
	this->mapUpdated = false;

	this->SetText(this->env->GetLang(CSTR("MainFormTitle")));

    this->lastSpeciesObj = 0;
    this->lastGroupObj = 0;
	this->lastDirIndex = (UOSInt)-1;
	this->lastObjIndex = (UOSInt)-1;
	this->inputMode = IM_EMPTY;
	this->dispImage = 0;
	this->dispImageUF = 0;
	this->dispImageWF = 0;
	this->dispImageToCrop = false;
	this->dispImageDown = false;

	this->lastBmp = 0;
	this->initSelObj = 0;
	this->initSelImg = 0;
	this->restoreObj = false;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile color2(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->mapResizer, Media::Resizer::LanczosResizer8_C8(4, 3, color, color2, this->colorSess.Ptr(), Media::AT_NO_ALPHA));
	this->mapCurrFile = 0;
	this->mapCurrImage = 0;


	this->unkCnt = 0;

	this->pnlLeft = ui->NewPanel(*this);
	this->pnlLeft->SetArea(0, 0, 100, 100, false);
	this->pnlLeft->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblLeft = ui->NewLabel(this->pnlLeft, this->env->GetLang(CSTR("MainFormLeft")));
	this->lblLeft->SetArea(0, 0, 88, 20, false);
	this->lblLeft->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbDir = ui->NewListBox(this->pnlLeft, false);
	this->lbDir->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbDir->HandleSelectionChange(OnDirChanged, this);
	this->hsbLeft = ui->NewHSplitter(*this, 3, false);

	this->pnlMid = ui->NewPanel(*this);
	this->pnlMid->SetArea(0, 0, 238, 50, false);
	this->pnlMid->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblMid = ui->NewLabel(this->pnlMid, this->env->GetLang(CSTR("MainFormMid")));
	this->lblMid->SetArea(0,0, 138, 20, false);
	this->lblMid->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlMidBottom = ui->NewPanel(this->pnlMid);
	this->pnlMidBottom->SetArea(0, 0, 138, 80, false);
	this->pnlMidBottom->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblPickMsg = ui->NewLabel(this->pnlMidBottom, CSTR(""));
	this->lblPickMsg->SetArea(0, 0, 138, 23, false);
	this->lblPickMsg->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnObjPick = ui->NewButton(this->pnlMidBottom, this->env->GetLang(CSTR("MainFormMidPick")));
	this->btnObjPick->SetArea(0, 30, 64, 53, false);
	this->btnObjPick->HandleButtonClick(OnObjPickClicked, this);
	this->btnObjPlace = ui->NewButton(this->pnlMidBottom, this->env->GetLang(CSTR("MainFormMidPlace")));
	this->btnObjPlace->SetArea(72, 30, 136, 53, false);
	this->btnObjPlace->HandleButtonClick(OnObjPlaceClicked, this);
	this->btnObjCombine = ui->NewButton(this->pnlMidBottom, this->env->GetLang(CSTR("MainFormMidCombine")));
	this->btnObjCombine->SetArea(72, 56, 136, 79, false);
	this->btnObjCombine->HandleButtonClick(OnObjCombineClicked, this);
	this->lbObj = ui->NewListBox(this->pnlMid, false);
	this->lbObj->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbObj->HandleDoubleClicked(OnObjDblClicked, this);
	this->lbObj->HandleSelectionChange(OnObjSelChg, this);
	this->hsbMid = ui->NewHSplitter(*this, 3, false);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tcMain->HandleSelChanged(OnTabSelChg, this);

	NN<UI::GUILabel> lbl;
	this->tpGroup = this->tcMain->AddTabPage(this->env->GetLang(CSTR("MainFormTabGroup")));
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupId")));
	lbl->SetRect(8, 4, 56, 29, false);
	this->txtGroupId = ui->NewTextBox(this->tpGroup, CSTR(""));
	this->txtGroupId->SetRect(64, 4, 80, 23, false);
	this->txtGroupId->SetReadOnly(true);
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupType")));
	lbl->SetRect(8, 28, 56, 23, false);
	this->cboGroupType = ui->NewComboBox(this->tpGroup, false);
	this->cboGroupType->SetRect(64, 28, 144, 23, false);
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupEName")));
	lbl->SetRect(8, 52, 56, 23, false);
	this->txtGroupEName = ui->NewTextBox(this->tpGroup, CSTR(""));
	this->txtGroupEName->SetRect(64, 52, 144, 23, false);
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupCName")));
	lbl->SetRect(8, 76, 56, 23, false);
	this->txtGroupCName = ui->NewTextBox(this->tpGroup, CSTR(""));
	this->txtGroupCName->SetRect(64, 76, 144, 23, false);
	this->chkGroupAdmin = ui->NewCheckBox(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupAdmin")), false);
	this->chkGroupAdmin->SetRect(64, 100, 144, 23, false);
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupDesc")));
	lbl->SetRect(8, 124, 56, 23, false);
	this->txtGroupDesc = ui->NewTextBox(this->tpGroup, CSTR(""), true);
	this->txtGroupDesc->SetRect(8, 148, 200, 83, false);
	lbl = ui->NewLabel(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupKey")));
	lbl->SetRect(8, 232, 80, 23, false);
	this->txtGroupKey = ui->NewTextBox(this->tpGroup, CSTR(""));
	this->txtGroupKey->SetRect(8, 254, 280, 23, false);
	this->btnGroupRemove = ui->NewButton(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupRemove")));
	this->btnGroupRemove->SetRect(64, 124, 75, 23, false);
	this->btnGroupRemove->HandleButtonClick(OnGroupRemoveClicked, this);
	this->btnGroupAdd = ui->NewButton(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupAdd")));
	this->btnGroupAdd->SetRect(144, 124, 75, 23, false);
	this->btnGroupAdd->HandleButtonClick(OnGroupAddClicked, this);
	this->btnGroupEnter = ui->NewButton(this->tpGroup, this->env->GetLang(CSTR("MainFormTabGroupEnter")));
	this->btnGroupEnter->SetRect(144, 4, 75, 23, false);
	this->btnGroupEnter->HandleButtonClick(OnGroupEnterClick, this);


	this->tpSpecies = this->tcMain->AddTabPage(this->env->GetLang(CSTR("MainFormTabSpecies")));
	this->pnlSpecies = ui->NewPanel(this->tpSpecies);
	this->pnlSpecies->SetRect(0, 0, 480, 288, false);
	this->pnlSpecies->SetDockType(UI::GUIControl::DOCK_TOP);
	lbl = ui->NewLabel(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesId")));
	lbl->SetRect(8, 4, 56, 23, false);
	this->txtSpeciesId = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesId->SetRect(64, 4, 64, 23, false);
	this->txtSpeciesId->SetReadOnly(true);
	lbl = ui->NewLabel(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesCName")));
	lbl->SetRect(8, 28, 56, 23, false);
	this->txtSpeciesCName = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesCName->SetRect(64, 28, 236, 23, false);
	lbl = ui->NewLabel(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesSName")));
	lbl->SetRect(8, 52, 56, 23, false);
	this->txtSpeciesSName = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesSName->SetRect(64, 52, 236, 23, false);
	this->txtSpeciesSName->HandleTextChanged(OnSpeciesSNameChg, this);
	lbl = ui->NewLabel(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesEName")));
	lbl->SetRect(8, 76, 56, 23, false);
	this->txtSpeciesEName = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesEName->SetRect(64, 76, 236, 23, false);
	lbl = ui->NewLabel(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesDName")));
	lbl->SetRect(8, 100, 56, 23, false);
	this->txtSpeciesDName = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesDName->SetReadOnly(true);
	this->txtSpeciesDName->SetRect(64, 100, 236, 23, false);
	lbl = ui->NewLabel(this->tpSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesDesc")));
	lbl->SetRect(8, 124, 56, 23, false);
	this->txtSpeciesDesc = ui->NewTextBox(this->pnlSpecies, CSTR(""), true);
	this->txtSpeciesDesc->SetRect(8, 148, 500, 91, false);
	lbl = ui->NewLabel(this->pnlSpecies, CSTR(""));
	lbl->SetRect(8, 240, 80, 23, false);
	this->txtSpeciesKey = ui->NewTextBox(this->pnlSpecies, CSTR(""));
	this->txtSpeciesKey->SetRect(8, 264, 480, 23, false);
	this->btnSpeciesRemove = ui->NewButton(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesRemove")));
	this->btnSpeciesRemove->SetRect(64, 124, 75, 23, false);
	this->btnSpeciesRemove->HandleButtonClick(OnSpRemoveClicked, this);
	this->btnSpeciesAdd = ui->NewButton(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesAdd")));
	this->btnSpeciesAdd->SetRect(144, 124, 75, 23, false);
	this->btnSpeciesAdd->HandleButtonClick(OnSpAddClicked, this);
	this->btnSpeciesPasteSName = ui->NewButton(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesPasteSName")));
	this->btnSpeciesPasteSName->SetRect(252, 124, 100, 23, false);
	this->btnSpeciesPasteSName->HandleButtonClick(OnSpPasteSNameClicked, this);
	this->btnSpeciesColor = ui->NewButton(this->pnlSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesColor")));
	this->btnSpeciesColor->SetRect(360, 124, 75, 23, false);
	this->btnSpeciesColor->HandleButtonClick(OnSpeciesColorClicked, this);
	this->grpSpBook = ui->NewGroupBox(this->tpSpecies, this->env->GetLang(CSTR("MainFormTabSpeciesBook")));
	this->grpSpBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlSpBook = ui->NewPanel(this->grpSpBook);
	this->pnlSpBook->SetRect(0, 0, 474, 49, false);
	this->pnlSpBook->SetDockType(UI::GUIControl::DOCK_TOP);
	lbl = ui->NewLabel(this->pnlSpBook, this->env->GetLang(CSTR("MainFormTabSpeciesBookDisp")));
	lbl->SetRect(64, 0, 100, 23, false);
	lbl = ui->NewLabel(this->pnlSpBook, this->env->GetLang(CSTR("MainFormTabSpeciesBookYear")));
	lbl->SetRect(184, 0, 64, 23, false);
	lbl = ui->NewLabel(this->pnlSpBook, this->env->GetLang(CSTR("MainFormTabSpeciesBookName")));
	lbl->SetRect(248, 0, 100, 23, false);
	this->btnSpBookAdd = ui->NewButton(this->pnlSpBook, this->env->GetLang(CSTR("MainFormTabSpeciesBookAdd")));
	this->btnSpBookAdd->SetRect(0, 24, 64, 23, false);
	this->btnSpBookAdd->HandleButtonClick(OnSpBookAddClicked, this);
	this->txtSpBook = ui->NewTextBox(this->pnlSpBook, CSTR(""));
	this->txtSpBook->SetRect(64, 24, 100, 23, false);
	this->txtSpBookYear = ui->NewTextBox(this->pnlSpBook, CSTR(""));
	this->txtSpBookYear->SetRect(184, 24, 64, 23, false);
	this->txtSpBookYear->HandleTextChanged(OnSpBookYearChg, this);
	this->cboSpBook = ui->NewComboBox(this->pnlSpBook, false);
	this->cboSpBook->SetRect(248, 24, 700, 23, false);
	this->pnlSpBookCtrl = ui->NewPanel(this->grpSpBook);
	this->pnlSpBookCtrl->SetRect(0, 0, 474, 24, false);
	this->pnlSpBookCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSpBookDel = ui->NewButton(this->pnlSpBookCtrl, this->env->GetLang(CSTR("MainFormTabSpeciesBookDel")));
	this->btnSpBookDel->SetRect(8, 0, 75, 23, false);
	this->lvSpBook = ui->NewListView(this->grpSpBook, UI::ListViewStyle::Table, 2);
	this->lvSpBook->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSpBook->SetFullRowSelect(true);
	this->lvSpBook->AddColumn(this->env->GetLang(CSTR("MainFormTabSpeciesBookColDisp")), 200);
	this->lvSpBook->AddColumn(this->env->GetLang(CSTR("MainFormTabSpeciesBookColName")), 800);
	this->lvSpBook->HandleSelChg(OnSpBookSelChg, this);
	
	this->tpImage = this->tcMain->AddTabPage(this->env->GetLang(CSTR("MainFormTabImage")));
	this->pnlImage = ui->NewPanel(this->tpImage);
	this->pnlImage->SetArea(0, 0, 480, 22, false);
	this->pnlImage->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnImageSaveAll = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageSaveAll")));
	this->btnImageSaveAll->SetRect(8, 0, 75, 21, false);
	this->btnImageSaveAll->HandleButtonClick(OnImageSaveAllClicked, this);
	this->btnImageSave = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageSave")));
	this->btnImageSave->SetRect(88, 0, 75, 21, false);
	this->btnImageSave->HandleButtonClick(OnImageSaveClicked, this);
	this->btnImagePick = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImagePick")));
	this->btnImagePick->SetRect(168, 0, 75, 21, false);
	this->btnImagePick->HandleButtonClick(OnImagePickClicked, this);
	this->btnImagePickAdd = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImagePickAdd")));
	this->btnImagePickAdd->SetRect(248, 0, 75, 21, false);
	this->btnImagePickAdd->HandleButtonClick(OnImagePickAddClicked, this);
	this->btnImagePickAll = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImagePickAll")));
	this->btnImagePickAll->SetRect(328, 0, 75, 21, false);
	this->btnImagePickAll->HandleButtonClick(OnImagePickAllClicked, this);
	this->btnImageDir = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageDir")));
	this->btnImageDir->SetRect(408, 0, 75, 21, false);
	this->btnImageDir->HandleButtonClick(OnImgDirClicked, this);
	this->btnImageRotate = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageRotate")));
	this->btnImageRotate->SetRect(488, 0, 75, 21, false);
	this->btnImageRotate->HandleButtonClick(OnImageRotateClicked, this);
	this->btnImageCrop = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageCrop")));
	this->btnImageCrop->SetRect(568, 0, 75, 21, false);
	this->btnImageCrop->HandleButtonClick(OnImageCropClicked, this);
	this->btnImageClipboard = ui->NewButton(this->pnlImage, this->env->GetLang(CSTR("MainFormTabImageClipboard")));
	this->btnImageClipboard->SetRect(648, 0, 75, 21, false);
	this->btnImageClipboard->HandleButtonClick(OnImageClipboardClicked, this);
	this->lbImage = ui->NewListBox(this->tpImage, false);
	this->lbImage->SetArea(0, 0, 480, 76, false);
	this->lbImage->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbImage->HandleSelectionChange(OnImgSelChg, this);
	this->lbImage->HandleRightClicked(OnImgRClicked, this);
	this->lbImage->HandleDoubleClicked(OnImgDblClicked, this);
	this->vsbImg = ui->NewVSplitter(this->tpImage, 2, true);
	this->pbImg = ui->NewPictureBoxDD(this->tpImage, this->colorSess, true, false);
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbImg->HandleMouseDown(OnImgMouseDown, this);
	this->pbImg->HandleMouseUp(OnImgMouseUp, this);
	this->pbImg->HandleMouseMove(OnImgMouseMove, this);
	this->pbImg->HandleDraw(OnImgDraw, this);
	this->pbImg->HandleDropEvents(this);
//	RegisterDragDrop((HWND)this->pbImg->GetHandle(), this);

	this->tpMap = this->tcMain->AddTabPage(this->env->GetLang(CSTR("MainFormTabMap")));
	this->pnlMapCtrl = ui->NewPanel(this->tpMap);
	this->pnlMapCtrl->SetRect(0, 0, 100, 24, false);
	this->pnlMapCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlMapStatus = ui->NewPanel(this->tpMap);
	this->pnlMapStatus->SetRect(0, 0, 100, 24, false);
	this->pnlMapStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->tbMapScale = ui->NewTrackBar(this->pnlMapCtrl, 0, 18, 16);
	this->tbMapScale->SetRect(0, 0, 100, 23, false);
	this->tbMapScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->tbMapScale->HandleScrolled(OnMapScaleScroll, this);
	this->txtMapScale = ui->NewTextBox(this->pnlMapStatus, CSTR(""));
	this->txtMapScale->SetRect(0, 0, 100, 23, false);
	this->txtMapScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->txtMapScale->SetReadOnly(true);
	this->txtMapPos = ui->NewTextBox(this->pnlMapStatus, CSTR(""));
	this->txtMapPos->SetRect(0, 0, 150, 23, false);
	this->txtMapPos->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->txtMapPos->SetReadOnly(true);

	NN<Map::OSM::OSMTileMap> tileMap;
	NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.opencyclemap.org/cycle/"), this->env->GetCacheDir()->ToCString(), 0, 18, this->env->GetTCPClientFactory(), this->env->GetSSLEngine()));
	tileMap->AddAlternateURL(CSTR("http://b.tile.opencyclemap.org/cycle/"));
	tileMap->AddAlternateURL(CSTR("http://c.tile.opencyclemap.org/cycle/"));
	this->mapTile = tileMap;
	NEW_CLASSNN(this->mapTileLyr, Map::TileMapLayer(tileMap, this->env->GetParserList()));
	this->mapTileLyr->AddUpdatedHandler(OnTileUpdated, this);
	NEW_CLASSNN(this->mapEnv, Map::MapEnv(CSTR("File"), 0, this->mapTileLyr->GetCoordinateSystem()->Clone()));
	this->mapEnv->AddLayer(0, this->mapTileLyr, true);
	this->imgFontStyle = this->mapEnv->AddFontStyle(CSTR("Temp"), this->env->GetMapFont(), 12, false, 0xff000000, 2, 0x80ffffff);

	Media::ColorProfile dispColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->mapRenderer, Map::DrawMapRenderer(this->env->GetDrawEngine(), this->mapEnv, dispColor, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	this->mapView = this->mapEnv->CreateMapView(Math::Size2DDbl(1024, 768));
	NEW_CLASSNN(this->mcMap, UI::GUIMapControl(ui, this->tpMap, this->env->GetDrawEngine(), 0xff000000, this->mapRenderer, this->mapView, this->colorSess));
	this->mcMap->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mcMap->HandleMouseMove(OnMapMouseMove, this);
	this->mcMap->HandleCustomDraw(OnMapDraw, this);

	NN<UI::GUIMenu> mnu;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(this->env->GetLang(CSTR("MainFormMenuManage")));
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageGroup")), MNU_MANAGE_GROUP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageFind")), MNU_MANAGE_FIND, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_F);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageList")), MNU_MANAGE_LIST, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageTest")), MNU_MANAGE_TEST, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_T);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageClearDir")), MNU_MANAGE_CLEAR_DIR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageLocation")), MNU_MANAGE_LOCATION, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageTrip")), MNU_MANAGE_TRIP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageUnkTrip")), MNU_MANAGE_UNK_TRIP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageDatabase")), MNU_MANAGE_DATABASE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageBook")), MNU_MANAGE_BOOK, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageUser")), MNU_MANAGE_USER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuManageDataFile")), MNU_MANAGE_DATAFILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(this->env->GetLang(CSTR("MainFormMenuNav")));
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuNavGoUp")), MNU_GO_UP, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_BACKSPACE);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuNavLeft")), MNU_NAV_LEFT, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_LEFT);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuNavRight")), MNU_NAV_RIGHT, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_RIGHT);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuNavUp")), MNU_NAV_UP, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_UP);
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuNavDown")), MNU_NAV_DOWN, UI::GUIMenu::KM_ALT, UI::GUIControl::GK_DOWN);
	mnu = this->mnuMain->AddSubMenu(this->env->GetLang(CSTR("MainFormMenuExport")));
	mnu->AddItem(this->env->GetLang(CSTR("MainFormMenuExportLite")), MNU_EXPORT_LITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	NEW_CLASSNN(this->rootGroup, OrganGroup());
	this->rootGroup->SetGroupId(0);
	this->rootGroup->SetCName(CSTR("<ROOT>"));
	this->rootGroup->SetEName(CSTR("<ROOT>"));
	this->rootGroup->SetGroupType(0);
	this->rootGroup->SetDesc(CSTR("Base Root"));
	this->rootGroup->SetPhotoSpecies(-1);
	this->rootGroup->SetPhotoGroup(-1);
	this->rootGroup->SetIDKey(CSTR(""));
	sptr = this->rootGroup->GetItemName(sbuff);
	this->lbDir->AddItem(CSTRP(sbuff, sptr), this->rootGroup);
	this->groupList.Add(this->rootGroup);

	UpdateSpBookList();
	InitCategory();
	this->AddTimer(500, OnTimerTick, this);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganMainForm::~OrganMainForm()
{
	NN<Media::DrawImage> dimg;
	this->ClearChildren();

	SDEL_CLASS(this->lastBmp);
	this->dispImage.Delete();
	this->groupList.DeleteAll();
	this->groupItems.DeleteAll();
	this->imgItems.DeleteAll();
	this->ClearPicks();

	DEL_CLASS(this->mapResizer);
	if (this->mapCurrImage.SetTo(dimg))
	{
		this->env->GetDrawEngine()->DeleteImage(dimg);
		this->mapCurrImage = 0;
	}
	this->mapRenderer.Delete();
	this->mapEnv.Delete();
	SDEL_STRING(this->initSelObj);
	SDEL_STRING(this->initSelImg);
	this->colorMgr->DeleteSess(this->colorSess);
}

void SSWR::OrganMgr::OrganMainForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_MANAGE_GROUP:
		{
/*			frmMainGrpType frm;
			frm.grps = this->grpTypes;
			frm.db = this->db;
			frm.cate = this->cate;
			frm.ShowDialog(this);
			FillGroupCboBox();*/
		}
		break;
	case MNU_MANAGE_FIND:
		{
			OrganSearchForm frm(0, this->ui, this->env);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SDEL_STRING(this->initSelObj);
				this->initSelObj = frm.GetFoundStr()->Clone().Ptr();
				this->GoToDir(frm.GetFoundGroup(), frm.GetParentId());
			}
		}
		break;
	case MNU_MANAGE_LIST:
		{
			NN<OrganGroupItem> item;
			OrganGroup *selObj = (OrganGroup*)this->lbDir->GetSelectedItem().p;
			NN<OrganGroup> g;
			UOSInt i;
			UOSInt j;
			UTF8Char sbuff[256];
			UnsafeArray<UTF8Char> sptr;
			if (selObj->GetGroupId() <= 0)
				return;

			NEW_CLASSNN(g, OrganGroup());
			g->SetGroupId(-1);
			sptr = Text::StrWChar_UTF8(sbuff, L"所有品種");
			g->SetCName(CSTRP(sbuff, sptr));
			g->SetEName(CSTR("All species"));
			g->SetGroupType(0);
			g->SetDesc(CSTR(""));
			g->SetPhotoGroup(-1);
			g->SetPhotoSpecies(-1);
			g->SetIDKey(CSTR(""));
			this->groupList.Add(g);
			sptr = g->GetItemName(sbuff);
			this->lbDir->SetSelectedIndex(this->lbDir->AddItem(CSTRP(sbuff, sptr), g));

			Data::ArrayListNN<OrganSpecies> spList;
			this->env->GetGroupAllSpecies(spList, selObj);
			i = 0;
			j = spList.GetCount();
			while (i < j)
			{
				item = spList.GetItemNoCheck(i);
				this->groupItems.Add(item);
				sptr = item->GetItemName(sbuff);
				this->lbObj->AddItem(CSTRP(sbuff, sptr), item);
				i++;
			}

			if (j > 0)
			{
				this->inputMode = IM_SPECIES;
				this->tpGroup->SetEnabled(false);
				this->tpSpecies->SetEnabled(true);
				this->tpImage->SetEnabled(true);
				this->tcMain->SetSelectedIndex(1);
			}
		}
		break;
	case MNU_MANAGE_TEST:
		{
/*			GroupItem *gi = (GroupItem*)this->lbObj->GetSelectedItem();
			if (gi && gi->GetItemType() == OrganGroupItem::IT_SPECIES)
			{
				Species *spe;
				spe = (Species*)gi;
				this->env->UpgradeFileStruct(spe);
			}*/

			this->env->Test();
		}
		break;
	case MNU_MANAGE_CLEAR_DIR:
		{
/*			System::IO::DirectoryInfo *di = new System::IO::DirectoryInfo(System::String::Concat(cfgImgDirBase, S"\\", cate->srcDir));
			System::IO::DirectoryInfo *dis[] = di->GetDirectories();
			System::IO::DirectoryInfo *subdi;
			System::Int32 i = 0;
			System::Int32 j = 0;
			System::Int32 k = 0;
			System::Int32 l = dis->Length;
			while (k < l)
			{
				subdi = dis[k];
				if (subdi->GetFiles()->Length == 0)
				{
					try
					{
						subdi->Delete();
						i += 1;
					}
					catch (System::Exception*)
					{
						j += 1;
					}
				}
				k += 1;
			}
			System::Windows::Forms::MessageBox::Show(System::String::Concat(S"Total ", k.ToString(), S" dir, ", System::String::Concat(i.ToString(), S" deleted, ", j.ToString(), S" errors")));*/
		}
		break;
	case MNU_MANAGE_LOCATION:
		{
			SSWR::OrganMgr::OrganLocationForm frm(0, this->ui, this->env, OrganLocationForm::SM_NONE, 0);
			frm.ShowDialog(this);
		}
		break;
	case MNU_MANAGE_TRIP:
		{
			SSWR::OrganMgr::OrganTripForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_MANAGE_UNK_TRIP:
		{
/*			System::IO::DirectoryInfo *di = new System::IO::DirectoryInfo(System::String::Concat(cfgImgDirBase, S"\\", cate->srcDir));
			System::IO::DirectoryInfo *dis[] = di->GetDirectories();
			System::IO::DirectoryInfo *subdi;
			System::IO::FileInfo *fis[];
			System::IO::FileInfo *fi;
			
			System::Collections::ArrayList *missDates = new System::Collections::ArrayList();
			
			System::Int32 i;
			System::Int32 j;
			System::Int32 k;

			System::Boolean found;
			ResetPercentage(S"Checking directory", System::Drawing::Color::Gainsboro);
			System::Int32 cnt = 0;
			System::Int32 cnt2;

			while (cnt < dis->Length)
			{
				subdi = dis[cnt];
				fis = subdi->GetFiles();
				cnt2 = 0;
				while (cnt2 < fis->Length)
				{
					fi = fis[cnt2++];
					if (fi->FullName->ToUpper()->EndsWith(S".TIF"))
					{
						Corelib::Img::ImgInfoTool *ii = new Corelib::Img::ImgInfoTool(fi->FullName);
						System::DateTime fDate = ii->DateLoc;

						if (trips->GetTrip(fDate) == 0)
						{
							DateCnt *dc;
							found = false;
							i = 0;
							j = missDates->Count - 1;
							while (i <= j)
							{
								k = (i + j) >> 1;
								dc = static_cast<DateCnt*>(missDates->Item[k]);
								if (dc->d > fDate.Date)
								{
									j = k - 1;
								}
								else if (dc->d < fDate.Date)
								{
									i = k + 1;
								}
								else
								{
									found = true;
									if (dc->minTime > fDate)
									{
										dc->minTime = fDate;
									}
									if (dc->maxTime < fDate)
									{
										dc->maxTime = fDate;
									}
									dc->cnt += 1;
									break;
								}
							}
							if (!found)
								missDates->Insert(i, new DateCnt(fDate));
						}
					}
				}
				this->ChangePercentage(cnt, dis->Length);
				cnt += 1;
			}
			this->ClosePercentage();
			if (missDates->Count > 0)
			{
				frmDateList frm(trips, missDates);
				frm.ShowDialog(this);
			}*/
		}
		break;
	case MNU_MANAGE_DATABASE:
		{
			SSWR::OrganMgr::OrganSelCategoryForm frm(0, this->ui, this->env);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->InitCategory();
			}
		}
		break;
	case MNU_MANAGE_BOOK:
		{
			SSWR::OrganMgr::OrganBookForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
			if (frm.IsChanged())
			{
				this->UpdateSpBookList();
			}
		}
		break;
	case MNU_MANAGE_USER:
		{
			SSWR::OrganMgr::OrganUserForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_MANAGE_DATAFILE:
		{
			SSWR::OrganMgr::OrganDataFileForm frm(0, this->ui, this->env);
			frm.ShowDialog(this);
		}
		break;
	case MNU_GO_UP:
		{
			UOSInt i;
			if (ToSaveGroup())
				return;
			if (ToSaveSpecies())
				return;

			i = this->lbDir->GetSelectedIndex();
			if (i != INVALID_INDEX && i > 0)
			{
				OrganGroup *groupO = (OrganGroup*)this->lbDir->GetItem(i).p;
				SDEL_STRING(this->initSelObj);
				this->initSelObj = Text::String::OrEmpty(groupO->GetEName())->Clone().Ptr();
				this->lbDir->SetSelectedIndex(i - 1);
			}
		}
		break;
	case MNU_NAV_RIGHT:
		{
			UOSInt i = this->tcMain->GetSelectedIndex();
			switch (i)
			{
			case 0:
				this->tcMain->SetSelectedIndex(1);
				this->txtSpeciesCName->Focus();
				break;
			case 1:
				this->tcMain->SetSelectedIndex(2);
				this->lbImage->Focus();
				break;
			case 2:
				this->tcMain->SetSelectedIndex(3);
				this->mcMap->Focus();
				break;
			}
		}
		break;
	case MNU_NAV_LEFT:
		{
			UOSInt i = this->tcMain->GetSelectedIndex();
			switch (i)
			{
			case 1:
				this->tcMain->SetSelectedIndex(0);
				this->txtGroupEName->Focus();
				break;
			case 2:
				this->tcMain->SetSelectedIndex(1);
				this->txtSpeciesCName->Focus();
				break;
			case 3:
				this->tcMain->SetSelectedIndex(2);
				this->lbImage->Focus();
				break;
			}
		}
		break;
	case MNU_NAV_UP:
		{
			UOSInt i = this->lbObj->GetSelectedIndex();
			if (i != INVALID_INDEX && i > 0)
			{
				this->lbObj->SetSelectedIndex(i - 1);
			}
			i = this->tcMain->GetSelectedIndex();
			if (i == 0)
			{
				this->txtGroupEName->Focus();
			}
			else if (i == 1)
			{
				this->txtSpeciesCName->Focus();
			}
		}
		break;
	case MNU_NAV_DOWN:
		{
			UOSInt i = this->lbObj->GetSelectedIndex();
			if (i < this->lbObj->GetCount() - 1)
			{
				this->lbObj->SetSelectedIndex(i + 1);
			}
			i = this->tcMain->GetSelectedIndex();
			if (i == 0)
			{
				this->txtGroupEName->Focus();
			}
			else if (i == 1)
			{
				this->txtSpeciesCName->Focus();
			}
		}
		break;
	case MNU_EXPORT_LITE:
		{
			NN<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();
			if (dlg->ShowDialog(this->GetHandle()))
			{
				this->env->ExportLite(dlg->GetFolder()->v);
			}
			dlg.Delete();
		}
		break;
	}
}

void SSWR::OrganMgr::OrganMainForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

UI::GUIDropHandler::DragEffect SSWR::OrganMgr::OrganMainForm::DragEnter(NN<UI::GUIDropData> data)
{
	UI::GUIDropHandler::DragEffect retEff = UI::GUIDropHandler::DE_NONE;
	if (this->inputMode == IM_SPECIES)
	{
		UOSInt i = this->lbObj->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			OrganGroupItem *gi = (OrganGroupItem*)this->lbObj->GetItem((UOSInt)i).p;
			if (gi->GetItemType() != OrganGroupItem::IT_PARENT)
			{
				UnsafeArray<const UTF8Char> name;
				UOSInt j = data->GetCount();
				UOSInt fmtSURL = INVALID_INDEX;
				UOSInt fmtIURL = INVALID_INDEX;
				UOSInt fmtFile = INVALID_INDEX;
				UOSInt fmtHDROP = INVALID_INDEX;
				while (j-- > 0)
				{
					name = data->GetName(j).Or(U8STR(""));
					UOSInt nameLen = Text::StrCharCnt(name);
					printf("Drag Enter: %s\r\n", name.Ptr());
					if (Text::StrEqualsC(name, nameLen, UTF8STRC("HTML Format")))
					{
						fmtSURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("application/x-moz-file-promise-url")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/url-list")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url-data")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url-desc")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("UniformResourceLocatorW")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url")))
					{
						fmtIURL = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("FileContents")))
					{
						fmtFile = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("HDROP")))
					{
						fmtHDROP = j;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/uri-list")))
					{
						fmtHDROP = j;
					}
				}

				if (fmtSURL != (UOSInt)-1 && fmtIURL != (UOSInt)-1 && fmtFile != (UOSInt)-1)
				{
					retEff = UI::GUIDropHandler::DE_LINK;
				}
				else if (fmtHDROP != (UOSInt)-1)
				{
					retEff = UI::GUIDropHandler::DE_COPY;
				}
			}
		}
	}
	return retEff;
}

void SSWR::OrganMgr::OrganMainForm::DropData(NN<UI::GUIDropData> data, OSInt x, OSInt y)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (this->inputMode == IM_SPECIES)
	{
		UOSInt i = this->lbObj->GetSelectedIndex();
		if (i != INVALID_INDEX)
		{
			NN<OrganGroupItem> gi = this->lbObj->GetItem(i).GetNN<OrganGroupItem>();
			if (gi->GetItemType() != OrganGroupItem::IT_PARENT)
			{
				UnsafeArrayOpt<const UTF8Char> fmtSURL = 0;
				UnsafeArrayOpt<const UTF8Char> fmtIURL = 0;
				UnsafeArrayOpt<const UTF8Char> fmtFile = 0;
				UnsafeArrayOpt<const UTF8Char> fmtHDrop = 0;
				UnsafeArray<const UTF8Char> name;
				UnsafeArray<const UTF8Char> nns;
				UOSInt j = data->GetCount();
				while (j-- > 0)
				{
					name = data->GetName(j).Or(U8STR(""));
					printf("Drag Drop: %s\r\n", name.Ptr());
					UOSInt nameLen = Text::StrCharCnt(name);
					if (Text::StrEqualsC(name, nameLen, UTF8STRC("HTML Format")))
					{
						fmtSURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("application/x-moz-file-promise-url")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/url-list")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url-data")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url-desc")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("UniformResourceLocatorW")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/x-moz-url")))
					{
						fmtIURL = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("FileContents")))
					{
						fmtFile = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("HDROP")))
					{
						fmtHDrop = name;
					}
					else if (Text::StrEqualsC(name, nameLen, UTF8STRC("text/uri-list")))
					{
						fmtHDrop = name;
					}
				}

				Text::StringBuilderUTF8 sb;
				Text::StringBuilderUTF8 sURL;
				Text::StringBuilderUTF8 iURL;
				Bool hasSURL = false;
				Bool hasIURL = false;

				if (fmtSURL.SetTo(nns))
				{
					if (data->GetDataText(nns, sb))
					{
						IO::MemoryReadingStream mstm(sb.v, sb.GetLength());
						Text::UTF8Reader reader(mstm);
						while (reader.ReadLine(sbuff, 511).SetTo(sptr))
						{
							if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SourceURL:")))
							{
								sURL.AppendC(&sbuff[10], (UOSInt)(sptr - &sbuff[10]));
								hasSURL = true;
								break;
							}
						}
						if (!hasSURL)
						{
							sURL.AppendC(UTF8STRC("about:blank"));
							hasSURL = true;
						}
					}
				}

				if (fmtIURL.SetTo(nns))
				{
					hasIURL = data->GetDataText(nns, iURL);
				}

				if (hasSURL && hasIURL)
				{
					if (fmtFile.SetTo(nns))
					{
						IO::Stream *stm = data->GetDataStream(nns);
						if (stm)
						{
							NN<Text::String> ssurl = Text::String::New(sURL.ToString(), sURL.GetLength());
							NN<Text::String> siurl = Text::String::New(iURL.ToString(), iURL.GetLength());
							Bool succ = (this->env->AddSpeciesWebFile(NN<OrganSpecies>::ConvertFrom(gi), ssurl, siurl, stm, sbuff) == OrganEnv::FS_SUCCESS);
							ssurl->Release();
							siurl->Release();
							DEL_CLASS(stm);

							if (succ)
							{
								SDEL_STRING(this->initSelImg);
								this->initSelImg = Text::String::NewNotNullSlow(sbuff).Ptr();
								this->UpdateImgDir();
							}
						}
						return;
					}
					else
					{
						
					}
				}
				if (fmtHDrop.SetTo(nns))
				{
					Bool firstPhoto = this->lbImage->GetCount() == 0;
					Bool chg = false;
					SDEL_STRING(this->initSelImg);

					sb.ClearStr();
					if (data->GetDataText(nns, sb))
					{
						Text::PString sarr[2];
						sarr[1] = sb;
						printf("HDROP: %s\r\n", sb.ToPtr());
						j = 2;
						while (j == 2)
						{
							j = Text::StrSplitLineP(sarr, 2, sarr[1]);
							OrganEnv::FileStatus fs;
							if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("file://")))
							{
								sptr = Text::URLString::GetURLFilePath(sbuff, sarr[0].v, sarr[0].leng).Or(sbuff);
								sarr[0].v = sbuff;
								sarr[0].leng = (UOSInt)(sptr - sbuff);
							}
							fs = this->env->AddSpeciesFile(NN<OrganSpecies>::ConvertFrom(gi), sarr[0].ToCString(), firstPhoto, false, 0);
							if (fs == OrganEnv::FS_SUCCESS)
							{
								if (!chg)
								{
									UOSInt tmp = Text::StrLastIndexOfChar(sarr[0].v, IO::Path::PATH_SEPERATOR);
									this->initSelImg = Text::String::New(&sarr[0].v[tmp + 1], sarr[0].leng - tmp - 1).Ptr();
								}
								chg = true;
								firstPhoto = false;
							}
							else if (fs == OrganEnv::FS_NOTSUPPORT)
							{
							}
							else
							{
								UOSInt i;
								NN<Text::String> s;
								Text::StringBuilderUTF8 sb;
								i = Text::StrLastIndexOfChar(sarr[0].v, IO::Path::PATH_SEPERATOR);
								s = Text::String::NewNotNull(L"不能複製檔案: ");
								sb.Append(s);
								s->Release();
								sb.AppendC(&sarr[0].v[i + 1], sarr[0].leng - i - 1);
								s = Text::String::NewNotNull(L", 要繼續?");
								sb.Append(s);
								s->Release();
								s = Text::String::NewNotNull(L"錯誤");
								if (!this->ui->ShowMsgYesNo(sb.ToCString(), s->ToCString(), this))
								{
									s->Release();
									break;
								}
								s->Release();
							}
						}

						if (chg)
						{
							this->UpdateImgDir();
						}
					}
					return;
				}
			}
		}
	}
}
