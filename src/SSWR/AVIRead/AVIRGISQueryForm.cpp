#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/Ellipse.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"

UI::EventState __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseLDown(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	me->downPos = scnPos;
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseLUp(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	if (me->downPos == scnPos)
	{
		NN<Map::GetObjectSess> sess;
		UIntOS i;
		UIntOS i2;
		UIntOS j;
		UIntOS k;
		Math::Coord2DDbl mapPt = me->navi->ScnXY2MapXY(scnPos);
		NN<Math::CoordinateSystem> csys = me->navi->GetCoordinateSystem();
		NN<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
		NN<Math::Geometry::Vector2D> vec;
		if (!csys->Equals(lyrCSys))
		{
			mapPt = Math::CoordinateSystem::Convert(csys, lyrCSys, mapPt);
		}
		me->ClearQueryResults();
		if (me->lyr->CanQuery())
		{
			if (me->lyr->QueryInfos(mapPt, me->queryVecList, me->queryValueOfstList, me->queryNameList, me->queryValueList) && me->queryVecList.GetCount() > 0)
			{
				me->cboObj->ClearItems();
				i = 0;
				j = me->queryVecList.GetCount();
				while (i < j)
				{
					vec = me->queryVecList.GetItemNoCheck(i);
					me->cboObj->AddItem(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()), 0);
					i++;
				}
				me->cboObj->SetSelectedIndex(0);
				me->layerNames = false;
				me->SetQueryItem(0);

				if (!csys->Equals(lyrCSys))
				{
					Math::CoordinateSystemConverter converter(lyrCSys, csys);
					i = me->queryVecList.GetCount();
					while (i-- > 0)
					{
						vec = me->queryVecList.GetItemNoCheck(i);
						me->queryVecOriList.Add(vec->Clone());
						vec->Convert(converter);
					}
				}

				me->navi->SetSelectedVectors(me->queryVecList);
				return UI::EventState::ContinueEvent;
			}
		}
		scnPos.x += 5;
		Math::Coord2DDbl mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (!csys->Equals(lyrCSys))
		{
			mapPt2 = Math::CoordinateSystem::Convert(csys, lyrCSys, mapPt2);
		}
		sess = me->lyr->BeginGetObject();
		Data::ArrayListNN<Map::MapDrawLayer::ObjectInfo> objList;
		me->lyr->GetNearObjects(sess, objList, mapPt, mapPt2.x - mapPt.x);
		if (!me->layerNames)
		{
			me->layerNames = true;
			me->ShowLayerNames();
		}
		me->cboObj->ClearItems();
		if (objList.GetCount() == 0)
		{
			i = me->lyr->GetColumnCnt();
			while (i-- > 0)
			{
				me->lvInfo->SetSubItem(i, 1, CSTR(""));
			}
			me->currVec.Delete();
			me->navi->SetSelectedVector(nullptr);
		}
		else
		{
			NN<Map::MapDrawLayer::ObjectInfo> obj;
			NN<Math::Geometry::Vector2D> vec;
			Data::ArrayListInt64 arr;
			Optional<Map::NameArray> nameArr;
			Text::StringBuilderUTF8 sb;
			me->lyr->GetObjectIdsMapXY(arr, nameArr, Math::RectAreaDbl(mapPt, mapPt), true);
			j = 0;
			k = objList.GetCount();
			while (j < k)
			{
				obj = objList.GetItemNoCheck(j);

				if (me->lyr->GetNewVectorById(sess, obj->objId).SetTo(vec))
				{
					if (!csys->Equals(lyrCSys))
					{
						Math::CoordinateSystemConverter converter(lyrCSys, csys);
						me->queryVecOriList.Add(vec->Clone());
						vec->Convert(converter);
					}
					sb.ClearStr();
					if (me->lyr->GetString(sb, nameArr, obj->objId, me->nameCol))
					{
						sb.AppendC(UTF8STRC(" - "));
						sb.Append(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()));
						me->cboObj->AddItem(sb.ToCString(), 0);
					}
					else
					{
						me->cboObj->AddItem(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()), 0);
					}
					me->queryVecList.Add(vec);
					i = 0;
					i2 = me->lyr->GetColumnCnt();
					while (i < i2)
					{
						sb.ClearStr();
						me->lyr->GetString(sb, nameArr, obj->objId, i);
						me->queryValueList.Add(Text::String::New(sb.ToCString()));
						i++;
					}
				}

				j++;
			}
			me->cboObj->SetSelectedIndex(0);
			me->SetQueryItem(0);
			me->lyr->ReleaseNameArr(nameArr);
			me->lyr->FreeObjects(objList);
			me->navi->SetSelectedVectors(me->queryVecList);
		}
		me->lyr->EndGetObject(sess);
	}
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseRDown(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	me->rdownPos = scnPos;
	me->rdown = true;
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseRUp(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<Map::GetObjectSess> sess;
	UIntOS i;
	UIntOS i2;
	UIntOS j;
	UIntOS k;
	Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(me->rdownPos);
	Math::Coord2DDbl pt2 = me->navi->ScnXY2MapXY(scnPos);
	NN<Math::CoordinateSystem> csys = me->navi->GetCoordinateSystem();
	NN<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
	if (!csys->Equals(lyrCSys))
	{
		pt1 = Math::CoordinateSystem::Convert(csys, lyrCSys, pt1);
		pt2 = Math::CoordinateSystem::Convert(csys, lyrCSys, pt2);
	}
	me->ClearQueryResults();
	Data::ArrayListInt64 objIds;
	Optional<Map::NameArray> nameArr;
	sess = me->lyr->BeginGetObject();
	me->lyr->GetObjectIdsMapXY(objIds, nameArr, Math::RectAreaDbl(pt1.Min(pt2), pt1.Max(pt2)), true);
	if (!me->layerNames)
	{
		me->layerNames = true;
		me->ShowLayerNames();
	}
	me->cboObj->ClearItems();
	if (objIds.GetCount() == 0)
	{
		i = me->lyr->GetColumnCnt();
		while (i-- > 0)
		{
			me->lvInfo->SetSubItem(i, 1, CSTR(""));
		}
		me->currVec.Delete();
		me->navi->SetSelectedVector(nullptr);
	}
	else
	{
		Int64 objId;
		NN<Math::Geometry::Vector2D> vec;
		Text::StringBuilderUTF8 sb;
		j = 0;
		k = objIds.GetCount();
		while (j < k)
		{
			objId = objIds.GetItem(j);

			if (me->lyr->GetNewVectorById(sess, objId).SetTo(vec))
			{
				if (!csys->Equals(lyrCSys))
				{
					Math::CoordinateSystemConverter converter(lyrCSys, csys);
					me->queryVecOriList.Add(vec->Clone());
					vec->Convert(converter);
				}
				sb.ClearStr();
				if (me->lyr->GetString(sb, nameArr, objId, me->nameCol))
				{
					sb.AppendC(UTF8STRC(" - "));
					sb.Append(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()));
					me->cboObj->AddItem(sb.ToCString(), 0);
				}
				else
				{
					me->cboObj->AddItem(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()), 0);
				}
				me->queryVecList.Add(vec);
				i = 0;
				i2 = me->lyr->GetColumnCnt();
				while (i < i2)
				{
					sb.ClearStr();
					me->lyr->GetString(sb, nameArr, objId, i);
					me->queryValueList.Add(Text::String::New(sb.ToCString()));
					i++;
				}
			}

			j++;
		}
		me->cboObj->SetSelectedIndex(0);
		me->SetQueryItem(0);
		me->lyr->ReleaseNameArr(nameArr);
		me->navi->SetSelectedVectors(me->queryVecList);
	}
	me->lyr->EndGetObject(sess);
	me->rdown = false;
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseMove(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<Math::Geometry::Vector2D> currVec;
	if (me->currVec.SetTo(currVec))
	{
		NN<Math::CoordinateSystem> csys = me->navi->GetCoordinateSystem();
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		Math::Coord2DDbl nearPos = mapPos;
		currVec->CalBoundarySqrDistance(mapPos, nearPos);
		Double d = csys->CalSurfaceDistance(mapPos, nearPos, Math::Unit::Distance::DU_METER);
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, d);
		me->txtDist->SetText(CSTRP(sbuff, sptr));
		if (currVec->InsideOrTouch(mapPos))
		{
			me->txtInside->SetText(CSTR("Inside"));
		}
		else
		{
			me->txtInside->SetText(CSTR("Outside"));
		}
	}
	if (me->rdown)
	{
		Math::Coord2DDbl pt1 = me->navi->ScnXY2MapXY(me->rdownPos);
		Math::Coord2DDbl pt2 = me->navi->ScnXY2MapXY(scnPos);
		NN<Math::Geometry::LinearRing> lr;
		NEW_CLASSNN(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false));
		UIntOS nPoint;
		UnsafeArray<Math::Coord2DDbl> ptList = lr->GetPointList(nPoint);
		ptList[0] = pt1;
		ptList[1] = Math::Coord2DDbl(pt1.x, pt2.y);
		ptList[2] = pt2;
		ptList[3] = Math::Coord2DDbl(pt2.x, pt1.y);
		ptList[4] = pt1;
		me->navi->SetSelectedVector(lr);
	}
	return UI::EventState::ContinueEvent;
}


void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnShapeFmtChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<Math::Geometry::Vector2D> vec;
	NN<Math::VectorTextWriter> writer;
	if (me->currVec.SetTo(vec) && me->cboShapeFmt->GetSelectedItem().GetOpt<Math::VectorTextWriter>().SetTo(writer))
	{
		Text::StringBuilderUTF8 sb;
		writer->ToText(sb, vec);
		me->txtShape->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnObjSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UIntOS index = me->cboObj->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		me->SetQueryItem(index);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnObjNameSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UIntOS index = me->cboObjName->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		me->nameCol = index;
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnInfoDblClk(AnyType userObj, UIntOS index)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UIntOS selIndex = me->cboObj->GetSelectedIndex();
	if (selIndex != INVALID_INDEX)
	{
		UIntOS j;
		UIntOS k;
		Optional<Text::String> value = nullptr;
		if (me->layerNames)
		{
			j = me->lyr->GetColumnCnt();
			k = selIndex * j;
			if (index < j)
			{
				value = me->queryValueList.GetItem(index + k);
			}
		}
		else
		{
			j = me->queryValueOfstList.GetItem(selIndex);
			k = me->queryValueOfstList.GetItem(selIndex + 1);
			if (k == 0)
				k = me->queryNameList.GetCount();
			if (index < (k - j))
			{
				value = me->queryValueList.GetItem(j + index);
			}
		}
		NN<Text::String> s;
		if (value.SetTo(s))
		{
			me->DownloadURL(s);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnObjDownloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UIntOS i;
	UIntOS j;
	NN<Text::String> value;
	if (me->layerNames)
	{
		i = 0;
		j = me->queryValueList.GetCount();
		while (i < j)
		{
			if (me->queryValueList.GetItem(i).SetTo(value))
			{
				me->DownloadURL(value);
			}
			i++;
		}
	}
	else
	{
		i = 0;
		j = me->queryValueList.GetCount();
		while (i < j)
		{
			if (me->queryValueList.GetItem(i).SetTo(value))
			{
				me->DownloadURL(value);
			}
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnColDownloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS l;
	NN<Text::String> value;
	if (me->layerNames)
	{
		k = me->lyr->GetColumnCnt();
		i = me->lvInfo->GetSelectedIndex();
		if (i == INVALID_INDEX)
			return;
		j = me->queryValueList.GetCount();
		while (i < j)
		{
			if (me->queryValueList.GetItem(i).SetTo(value))
			{
				me->DownloadURL(value);
			}
			i += k;
		}
	}
	else
	{
		l = me->lvInfo->GetSelectedIndex();
		if (l == INVALID_INDEX)
			return;
		k = me->queryValueOfstList.GetCount();
		j = me->queryNameList.GetCount();
		while (k-- > 0)
		{
			i = me->queryValueOfstList.GetItem(k);
			if (j - i > l && me->queryValueList.GetItem(i + l).SetTo(value))
			{
				me->DownloadURL(value);
			}
			j = i;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnOpenTimer(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<Text::String> path;
	while (true)
	{
		{
			Sync::MutexUsage mutUsage(me->openMut);
			if (!me->openList.RemoveAt(0).SetTo(path))
			{
				break;
			}
		}
		if (me->cboAutoSaveAction->GetSelectedIndex() == 2)
		{
			me->navi->AddLayerFromFile(path->ToCString());
		}
		path->Release();
	}

	{
		Sync::MutexUsage mutUsage(me->downMut);
		UIntOS cnt = me->downList.GetCount();
		mutUsage.EndUse();
		if (cnt != me->dispCnt)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Pending download: "));
			sb.AppendUIntOS(cnt);
			me->lblObjMsg->SetText(sb.ToCString());
			me->dispCnt = cnt;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::DownThread(NN<Sync::Thread> thread)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = thread->GetUserObj().GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<Text::String> downPath;
	NN<Text::String> downURL;
	NN<Net::TCPClientFactory> clif = me->core->GetTCPClientFactory();
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	Text::StringBuilderUTF8 sb;
	Sync::MutexUsage downMutUsage(me->downMut);
	while (!thread->IsStopping())
	{
		downMutUsage.BeginUse();
		if (me->downPath.SetTo(downPath))
		{
			while (me->downList.GetItem(0).SetTo(downURL))
			{
				downMutUsage.EndUse();

				NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, downURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
				IO::MemoryStream mstm;
				if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
				{
					sb.ClearStr();
					sb.Append(downPath);
					if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
						sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
					cli->GetContentFileName(sb);
					UInt64 len;
					{
						IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						len = cli->ReadToEnd(fs, 65536);
					}
					if (len == 0)
					{
						IO::Path::DeleteFile(sb.v);
					}
					else
					{
						Sync::MutexUsage openMutUsage(me->openMut);
						me->openList.Add(Text::String::New(sb.ToCString()));
					}
				}
				cli.Delete();

				downMutUsage.BeginUse();
				downURL->Release();
				me->downList.RemoveAt(0);
			}
		}
		downMutUsage.EndUse();
		
		thread->Wait(1000);
	}
	ssl.Delete();
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnFormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	NN<IO::Registry> reg;
	WChar wbuff[512];
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		Text::StringBuilderUTF8 sb;
		me->txtAutoSavePath->GetText(sb);
		Text::StrUTF8_WCharC(wbuff, sb.v, sb.leng, 0);
		reg->SetValue(L"GISQueryDownPath", wbuff);
		IO::Registry::CloseRegistry(reg);
	}
}

void SSWR::AVIRead::AVIRGISQueryForm::ShowLayerNames()
{
	this->lvInfo->ClearItems();
	this->cboObjName->ClearItems();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i = 0;
	UIntOS j = this->lyr->GetColumnCnt();
	while (i < j)
	{
		sbuff[0] = 0;
		sptr = this->lyr->GetColumnName(sbuff, i).Or(sbuff);
		this->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
		this->cboObjName->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
	if (this->nameCol < j)
	{
		this->cboObjName->SetSelectedIndex(this->nameCol);
	}
}

void SSWR::AVIRead::AVIRGISQueryForm::ClearQueryResults()
{
	NN<Text::String> value;
	this->queryNameList.FreeAll();
	UIntOS i = this->queryValueList.GetCount();
	while (i-- > 0)
	{
		value = this->queryValueList.GetItemNoCheck(i);
		value->Release();
	}
	this->queryNameList.Clear();
	this->queryValueList.Clear();
	this->queryValueOfstList.Clear();
	this->queryVecList.Clear();
	this->queryVecOriList.DeleteAll();
}

void SSWR::AVIRead::AVIRGISQueryForm::SetQueryItem(UIntOS index)
{
	NN<Math::VectorTextWriter> writer;
	NN<Math::Geometry::Vector2D> vec;
	if (!this->queryVecList.GetItem(index).SetTo(vec) || !this->cboShapeFmt->GetSelectedItem().GetOpt<Math::VectorTextWriter>().SetTo(writer))
		return;
	this->currVec.Delete();
	this->currVec = vec->Clone();

	this->queryVecOriList.GetItem(index).SetTo(vec);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	Optional<Text::String> name;
	Optional<Text::String> value;
	if (this->layerNames)
	{
		i = 0;
		j = this->lyr->GetColumnCnt();
		k = index * j;
		while (i < j)
		{
			value = this->queryValueList.GetItem(i + k);
			this->lvInfo->SetSubItem(i, 1, Text::String::OrEmpty(value));
			i++;
		}
	}
	else
	{
		this->lvInfo->ClearItems();
		i = this->queryValueOfstList.GetItem(index);
		j = this->queryValueOfstList.GetItem(index + 1);
		if (j == 0)
			j = this->queryNameList.GetCount();
		NN<Text::String> valueStr;
		while (i < j)
		{
			name = this->queryNameList.GetItem(i);
			value = this->queryValueList.GetItem(i);
			k = this->lvInfo->AddItem(Text::String::OrEmpty(name), 0);
			if (value.SetTo(valueStr))
			{
				this->lvInfo->SetSubItem(k, 1, valueStr);
			}
			i++;
		}
	}
	Math::RectAreaDbl bounds = vec->GetBounds();

	Text::StringBuilderUTF8 sb;
	writer->ToText(sb, vec);
	this->txtShape->SetText(sb.ToCString());
	NN<Math::CoordinateSystem> csys = this->lyr->GetCoordinateSystem();
	sb.ClearStr();
	sb.AppendDouble(csys->CalDistance(vec, false, Math::Unit::Distance::DU_METER));
	this->txtShapeHLength->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendDouble(csys->CalDistance(vec, true, Math::Unit::Distance::DU_METER));
	this->txtShapeLength->SetText(sb.ToCString());
	sb.ClearStr();
	if (csys->IsProjected())
	{
		sb.AppendDouble(vec->CalArea());
	}
	else
	{
		sb.AppendDouble(vec->CalArea() / 0.0174532925199433 / 0.0174532925199433);
	}
	this->txtShapeArea->SetText(sb.ToCString());

	sptr = Text::StrDouble(sbuff, bounds.min.x);
	this->txtMinX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.min.y);
	this->txtMinY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.max.x);
	this->txtMaxX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.max.y);
	this->txtMaxY->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRGISQueryForm::DownloadURL(NN<Text::String> url)
{
	if (url->StartsWith(CSTR("http://")) || url->StartsWith(CSTR("https://")))
	{
		if (this->cboAutoSaveAction->GetSelectedIndex() == 0)
		{
			UI::Clipboard::SetString(this->GetHandle(), url->ToCString());
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (this->downList.GetCount() > 0)
			{
				Sync::MutexUsage mutUsage(this->downMut);
				UIntOS cnt = this->downList.Add(url->Clone()) + 1;
				sb.Append(CSTR("Pending download: "));
				sb.AppendUIntOS(cnt);
				this->lblObjMsg->SetText(sb.ToCString());
				this->dispCnt = cnt;
			}
			else
			{
				if (this->txtAutoSavePath->GetText(sb) && sb.leng > 0)
				{
					Sync::MutexUsage mutUsage(this->downMut);
					OPTSTR_DEL(this->downPath);
					this->downPath = Text::String::New(sb.ToCString());
					UIntOS cnt = this->downList.Add(url->Clone()) + 1;
					sb.ClearStr();
					sb.Append(CSTR("Pending download: "));
					sb.AppendUIntOS(cnt);
					this->lblObjMsg->SetText(sb.ToCString());
					this->dispCnt = cnt;
					this->downThread.Start();
				}
				else
				{
					NN<Net::TCPClientFactory> clif = this->core->GetTCPClientFactory();
					Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
					NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
					IO::MemoryStream mstm;
					if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
					{
						UInt64 len = cli->ReadToEnd(mstm, 65536);
						if (len > 0)
						{
							sb.ClearStr();
							cli->GetContentFileName(sb);
							NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISQueryInfoDownload", true);
							dlg->SetFileName(sb.ToCString());
							if (dlg->ShowDialog(this->GetHandle()))
							{
								IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
								if (fs.Write(mstm.GetArray()) != mstm.GetLength())
								{
									this->ui->ShowMsgOK(CSTR("Error in writing to file"), CSTR("GIS Query"), this);
								}
								else if (this->cboAutoSaveAction->GetSelectedIndex() == 2)
								{
									this->navi->AddLayerFromFile(dlg->GetFileName()->ToCString());
								}
							}
							dlg.Delete();
						}
					}
					cli.Delete();
					ssl.Delete();
				}
			}
		}
	}
	else
	{
		UI::Clipboard::SetString(this->GetHandle(), url->ToCString());
	}
}

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapDrawLayer> lyr, NN<AVIRMapNavigator> navi) : UI::GUIForm(parent, 416, 408, ui), downThread(DownThread, this, CSTR("GISQueryDown"))
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->currVec = nullptr;
	this->layerNames = true;
	this->nameCol = lyr->GetNameCol();
	this->dispCnt = 0;
	this->downPath = nullptr;
	this->rdown = false;
	sb.AppendC(UTF8STRC("Query - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);

	this->pnlObj = ui->NewPanel(*this);
	this->pnlObj->SetRect(0, 0, 100, 55, false);
	this->pnlObj->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblObjName = ui->NewLabel(this->pnlObj, CSTR("Name Column"));
	this->lblObjName->SetRect(4, 4, 100, 23, false);
	this->cboObjName = ui->NewComboBox(this->pnlObj, false);
	this->cboObjName->SetRect(104, 4, 200, 23, false);
	this->cboObjName->HandleSelectionChange(OnObjNameSelChg, this);
	this->btnColDownload = ui->NewButton(this->pnlObj, CSTR("Download Column"));
	this->btnColDownload->SetRect(304, 4, 100, 23, false);
	this->btnColDownload->HandleButtonClick(OnColDownloadClicked, this);
	this->cboObj = ui->NewComboBox(this->pnlObj, false);
	this->cboObj->SetRect(4, 28, 200, 23, false);
	this->cboObj->HandleSelectionChange(OnObjSelChg, this);
	this->lblObjMsg = ui->NewLabel(this->pnlObj, CSTR(""));
	this->lblObjMsg->SetRect(204, 28, 200, 23, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lvInfo = ui->NewListView(this->tpInfo, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->HandleDblClk(OnInfoDblClk, this);

	this->tpShape = this->tcMain->AddTabPage(CSTR("Shape"));
	this->pnlShape = ui->NewPanel(this->tpShape);
	this->pnlShape->SetRect(0, 0, 100, 103, false);
	this->pnlShape->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblShapeHLength = ui->NewLabel(this->pnlShape, CSTR("H. Length"));
	this->lblShapeHLength->SetRect(4, 4, 100, 23, false);
	this->txtShapeHLength = ui->NewTextBox(this->pnlShape, CSTR(""));
	this->txtShapeHLength->SetRect(104, 4, 150, 23, false);
	this->lblShapeLength = ui->NewLabel(this->pnlShape, CSTR("3D Length"));
	this->lblShapeLength->SetRect(4, 28, 100, 23, false);
	this->txtShapeLength = ui->NewTextBox(this->pnlShape, CSTR(""));
	this->txtShapeLength->SetRect(104, 28, 150, 23, false);
	this->lblShapeArea = ui->NewLabel(this->pnlShape, CSTR("Area"));
	this->lblShapeArea->SetRect(4, 52, 100, 23, false);
	this->txtShapeArea = ui->NewTextBox(this->pnlShape, CSTR(""));
	this->txtShapeArea->SetRect(104, 52, 150, 23, false);
	this->lblShapeFmt = ui->NewLabel(this->pnlShape, CSTR("Format"));
	this->lblShapeFmt->SetRect(4, 76, 100, 23, false);
	this->cboShapeFmt = ui->NewComboBox(this->pnlShape, false);
	this->cboShapeFmt->SetRect(104, 76, 200, 23, false);
	this->cboShapeFmt->HandleSelectionChange(OnShapeFmtChanged, this);
	this->txtShape = ui->NewTextBox(this->tpShape, CSTR(""), true);
	this->txtShape->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtShape->SetReadOnly(true);

	this->tpBounds = this->tcMain->AddTabPage(CSTR("Bounds"));
	this->lblMinX = ui->NewLabel(this->tpBounds, CSTR("Min X"));
	this->lblMinX->SetRect(4, 4, 100, 23, false);
	this->txtMinX = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMinX->SetRect(104, 4, 150, 23, false);
	this->txtMinX->SetReadOnly(true);
	this->lblMinY = ui->NewLabel(this->tpBounds, CSTR("Min Y"));
	this->lblMinY->SetRect(4, 28, 100, 23, false);
	this->txtMinY = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMinY->SetRect(104, 28, 150, 23, false);
	this->txtMinY->SetReadOnly(true);
	this->lblMaxX = ui->NewLabel(this->tpBounds, CSTR("Max X"));
	this->lblMaxX->SetRect(4, 52, 100, 23, false);
	this->txtMaxX = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMaxX->SetRect(104, 52, 150, 23, false);
	this->txtMaxX->SetReadOnly(true);
	this->lblMaxY = ui->NewLabel(this->tpBounds, CSTR("Max Y"));
	this->lblMaxY->SetRect(4, 76, 100, 23, false);
	this->txtMaxY = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMaxY->SetRect(104, 76, 150, 23, false);
	this->txtMaxY->SetReadOnly(true);

	this->tpDist = this->tcMain->AddTabPage(CSTR("Distance"));
	this->lblDist = ui->NewLabel(this->tpDist, CSTR("Dist to boundaries"));
	this->lblDist->SetRect(4, 4, 100, 23, false);
	this->txtDist = ui->NewTextBox(this->tpDist, CSTR(""));
	this->txtDist->SetReadOnly(true);
	this->txtDist->SetRect(104, 4, 150, 23, false);
	this->lblInside = ui->NewLabel(this->tpDist, CSTR("Inside Vector"));
	this->lblInside->SetRect(4, 28, 100, 23, false);
	this->txtInside = ui->NewTextBox(this->tpDist, CSTR(""));
	this->txtInside->SetReadOnly(true);
	this->txtInside->SetRect(104, 28, 150, 23, false);

	this->tpAutoSave = this->tcMain->AddTabPage(CSTR("AutoSave"));
	this->lblAutoSaveAction = ui->NewLabel(this->tpAutoSave, CSTR("DblClk Action"));
	this->lblAutoSaveAction->SetRect(4, 4, 100, 23, false);
	this->cboAutoSaveAction = ui->NewComboBox(this->tpAutoSave, false);
	this->cboAutoSaveAction->SetRect(104, 4, 150, 23, false);
	this->cboAutoSaveAction->AddItem(CSTR("Copy"), 0);
	this->cboAutoSaveAction->AddItem(CSTR("Save"), 0);
	this->cboAutoSaveAction->AddItem(CSTR("Save and Open"), 0);
	this->cboAutoSaveAction->SetSelectedIndex(2);
	this->lblAutoSavePath = ui->NewLabel(this->tpAutoSave, CSTR("Path"));
	this->lblAutoSavePath->SetRect(4, 28, 100, 23, false);
	this->txtAutoSavePath = ui->NewTextBox(this->tpAutoSave, CSTR(""));
	this->txtAutoSavePath->SetRect(4, 52, 600, 23, false);
	this->btnObjDownload = ui->NewButton(this->tpAutoSave, CSTR("Download All Links"));
	this->btnObjDownload->SetRect(4, 76, 150, 23, false);
	this->btnObjDownload->HandleButtonClick(OnObjDownloadClicked, this);
	this->ShowLayerNames();

	UIntOS i = 0;
	UIntOS j = this->writerList.GetCount();
	while (i < j)
	{
		NN<Math::VectorTextWriter> writer;
		if (this->writerList.GetItem(i).SetTo(writer))
		{
			this->cboShapeFmt->AddItem(writer->GetWriterName(), writer);
		}
		i++;
	}
	this->cboShapeFmt->SetSelectedIndex(0);
	this->navi->HandleMapMouseLDown(OnMouseLDown, this);
	this->navi->HandleMapMouseLUp(OnMouseLUp, this);
	this->navi->HandleMapMouseRDown(OnMouseRDown, this);
	this->navi->HandleMapMouseRUp(OnMouseRUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
	NN<IO::Registry> reg;
	WChar wbuff[512];
	UnsafeArray<WChar> wptr;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		if (reg->GetValueStr(L"GISQueryDownPath", wbuff).SetTo(wptr))
		{
			NN<Text::String> s = Text::String::NewW(wbuff, (UIntOS)(wptr - wbuff));
			this->txtAutoSavePath->SetText(s->ToCString());
			s->Release();
		}
		IO::Registry::CloseRegistry(reg);
	}
	this->AddTimer(1000, OnOpenTimer, this);
	this->HandleFormClosed(OnFormClosed, this);
}

SSWR::AVIRead::AVIRGISQueryForm::~AVIRGISQueryForm()
{
	this->downThread.Stop();
	this->downList.FreeAll();
	this->openList.FreeAll();
	this->ClearQueryResults();
	this->navi->UnhandleMapMouse(this);
	this->currVec.Delete();
	OPTSTR_DEL(this->downPath);
}

void SSWR::AVIRead::AVIRGISQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
