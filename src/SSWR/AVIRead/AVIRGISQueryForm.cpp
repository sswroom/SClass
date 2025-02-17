#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Math/CoordinateSystemConverter.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/Ellipse.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIFileDialog.h"

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	me->downPos = scnPos;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	if (me->downPos == scnPos)
	{
		NN<Map::GetObjectSess> sess;
		UOSInt i;
		UOSInt i2;
		UOSInt j;
		UOSInt k;
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
				return false;
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
			me->navi->SetSelectedVector(0);
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
					if (me->lyr->GetString(sb, nameArr, obj->objId, me->lyr->GetNameCol()))
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
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
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
	return false;
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
	UOSInt index = me->cboObj->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		me->SetQueryItem(index);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnInfoDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRGISQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISQueryForm>();
	UOSInt selIndex = me->cboObj->GetSelectedIndex();
	if (selIndex != INVALID_INDEX)
	{
		UOSInt j;
		UOSInt k;
		Optional<Text::String> value = 0;
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
			if (s->StartsWith(CSTR("http://")) || s->StartsWith(CSTR("https://")))
			{
				NN<Net::TCPClientFactory> clif = me->core->GetTCPClientFactory();
				Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
				NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, s->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
				IO::MemoryStream mstm;
				if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
				{
					UInt64 len = cli->ReadToEnd(mstm, 65536);
					if (len > 0)
					{
						Text::StringBuilderUTF8 sb;
						cli->GetContentFileName(sb);
						NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISQueryInfoDownload", true);
						dlg->SetFileName(sb.ToCString());
						if (dlg->ShowDialog(me->GetHandle()))
						{
							IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							if (fs.Write(mstm.GetArray()) != mstm.GetLength())
							{
								me->ui->ShowMsgOK(CSTR("Error in writing to file"), CSTR("GIS Query"), me);
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

void SSWR::AVIRead::AVIRGISQueryForm::ShowLayerNames()
{
	this->lvInfo->ClearItems();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = 0;
	UOSInt j = this->lyr->GetColumnCnt();
	while (i < j)
	{
		sbuff[0] = 0;
		sptr = this->lyr->GetColumnName(sbuff, i).Or(sbuff);
		this->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
}

void SSWR::AVIRead::AVIRGISQueryForm::ClearQueryResults()
{
	NN<Text::String> value;
	this->queryNameList.FreeAll();
	UOSInt i = this->queryValueList.GetCount();
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

void SSWR::AVIRead::AVIRGISQueryForm::SetQueryItem(UOSInt index)
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
	UOSInt i;
	UOSInt j;
	UOSInt k;
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

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapDrawLayer> lyr, NN<IMapNavigator> navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->currVec = 0;
	this->layerNames = true;
	sb.AppendC(UTF8STRC("Query - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->pnlObj = ui->NewPanel(*this);
	this->pnlObj->SetRect(0, 0, 100, 31, false);
	this->pnlObj->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboObj = ui->NewComboBox(this->pnlObj, false);
	this->cboObj->SetRect(4, 4, 200, 23, false);
	this->cboObj->HandleSelectionChange(OnObjSelChg, this);
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

	this->ShowLayerNames();

	UOSInt i = 0;
	UOSInt j = this->writerList.GetCount();
	while (i < j)
	{
		Math::VectorTextWriter *writer = this->writerList.GetItem(i);
		this->cboShapeFmt->AddItem(writer->GetWriterName(), writer);
		i++;
	}
	this->cboShapeFmt->SetSelectedIndex(0);
	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISQueryForm::~AVIRGISQueryForm()
{
	this->ClearQueryResults();
	this->navi->UnhandleMapMouse(this);
	this->currVec.Delete();
}

void SSWR::AVIRead::AVIRGISQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
