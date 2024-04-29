#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Exporter/CIPExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Text/MyString.h"

Exporter::CIPExporter::CIPExporter()
{
}

Exporter::CIPExporter::~CIPExporter()
{
}

Int32 Exporter::CIPExporter::GetName()
{
	return *(Int32*)"CIPE";
}

IO::FileExporter::SupportType Exporter::CIPExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
	{
		return IO::FileExporter::SupportType::MultiFiles;
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::SupportType::MultiFiles;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::SupportType::MultiFiles;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::CIPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("CIP file"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.cip"));
		return true;
	}
	return false;
}

Bool Exporter::CIPExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	UInt8 buff[256];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NN<ParamData> para;
	if (!param.SetTo(para))
		return false;
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		return false;
	}
	NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	Map::DrawLayerType layerType = layer->GetLayerType();
	Int32 iLayerType;
	if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
	{
		iLayerType = 1;
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		iLayerType = 3;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
	{
		iLayerType = 5;
	}
	else
	{
		return false;
	}
	fileName.ConcatTo(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("cix"));
	IO::FileStream cix(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("ciu"));
	IO::FileStream cib(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("blk"));
	IO::FileStream blk(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);


	Data::ArrayListInt64 objIds;
	Data::ArrayListNN<CIPBlock> blks;
	Data::ArrayListNN<CIPStrRecord> strs;
	NN<CIPStrRecord> strRec;
	NN<CIPBlock> theBlk;
	Map::GetObjectSess *sess;
	Map::NameArray *nameArr;
	UOSInt stmPos = 8;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt recCnt;
	Int32 minX;
	Int32 maxX;
	Int32 minY;
	Int32 maxY;
	Int32 left;
	Int32 top;
	Int32 right;
	Int32 bottom;
	layer->GetAllObjectIds(objIds, &nameArr);
	sess = layer->BeginGetObject();

	WriteUInt32(&buff[0], (UInt32)objIds.GetCount());
	WriteInt32(&buff[4], iLayerType);
	stm->Write(buff, 8);
	cix.Write(buff, 4);

	i = 0;
	recCnt = objIds.GetCount();
	while (i < recCnt)
	{
		Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, objIds.GetItem(i));

		WriteUInt32(&buff[0], (UInt32)i);
		WriteUInt32(&buff[4], (UInt32)stmPos);
		cix.Write(buff, 8);

		if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Point)
		{
			WriteUInt32(&buff[4], 1);
			WriteUInt32(&buff[8], 0);
			stm->Write(buff, 12);
			stmPos += 12;

			Math::Coord2DDbl center = vec->GetCenter();
			WriteUInt32(&buff[0], 1);
			WriteInt32(&buff[4], Double2Int32(center.x * 200000.0));
			WriteInt32(&buff[8], Double2Int32(center.y * 200000.0));
			stm->Write(buff, 4);
			stm->Write(&buff[4], 8);
			stmPos += 8 + 4;

			maxX = minX = ReadInt32(&buff[4]);
			maxY = minY = ReadInt32(&buff[8]);

			left = minX / p->scale;
			right = maxX / p->scale;
			top = minY / p->scale;
			bottom = maxY / p->scale;
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polyline)
		{
			Math::Geometry::Polyline *pl = (Math::Geometry::Polyline*)vec;
			UOSInt nPtOfst = pl->GetCount();
			UInt32 *ptOfstArr = MemAlloc(UInt32, nPtOfst + 1);
			WriteUInt32(&buff[4], (UInt32)nPtOfst);
			ptOfstArr[0] = 0;
			UOSInt i = 0;
			NN<Math::Geometry::LineString> ls;
			while (i < nPtOfst)
			{
				i++;
				if (pl->GetItem(i).SetTo(ls))
					ptOfstArr[i] = ptOfstArr[i - 1] + (UInt32)ls->GetPointCount();
				else
					ptOfstArr[i] = ptOfstArr[i - 1];
			}
			stm->Write(buff, 8);
			stm->Write((UInt8*)ptOfstArr, nPtOfst * 4);
			stmPos += 8 + nPtOfst * 4;

			UOSInt nPoint = ptOfstArr[nPtOfst];
			MemFree(ptOfstArr);
			Int32 *ptArr = MemAlloc(Int32, nPoint << 1);
			UOSInt j = 0;
			UOSInt k;
			UOSInt l;
			NN<Math::Geometry::LineString> lineString;
			i = 0;
			while (i < nPtOfst)
			{
				if (pl->GetItem(i).SetTo(lineString))
				{
					k = 0;
					Math::Coord2DDbl *pointArr = lineString->GetPointList(l);
					while (k < l)
					{
						ptArr[j] = Double2Int32(pointArr[k].x * 200000.0);
						ptArr[j + 1] = Double2Int32(pointArr[k].y * 200000.0);
						j += 2;
						k++;
					}
				}
				i++;
			}
			stm->Write((UInt8*)&nPoint, 4);
			stm->Write((UInt8*)ptArr, 8 * nPoint);
			stmPos += 8 * nPoint + 4;

			maxX = minX = ptArr[0];
			maxY = minY = ptArr[1];
			j = nPoint;
			while (j-- > 0)
			{
				if (minX > ptArr[j << 1])
					minX = ptArr[j << 1];
				if (maxX < ptArr[j << 1])
					maxX = ptArr[j << 1];
				if (minY > ptArr[(j << 1) + 1])
					minY = ptArr[(j << 1) + 1];
				if (maxY < ptArr[(j << 1) + 1])
					maxY = ptArr[(j << 1) + 1];
			}

			left = minX / p->scale;
			right = maxX / p->scale;
			top = minY / p->scale;
			bottom = maxY / p->scale;
			MemFree(ptArr);
		}
		else if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Polygon)
		{
			Math::Geometry::Polygon *pg = (Math::Geometry::Polygon*)vec;
			UOSInt nPtOfst = pg->GetCount();
			UInt32 *ptOfstArr = MemAlloc(UInt32, nPtOfst);
			Data::ArrayListA<Math::Coord2DDbl> pointArr;
			NN<Math::Geometry::LinearRing> lr;
			j = 0;
			while (j < nPtOfst)
			{
				ptOfstArr[j] = (UInt32)pointArr.GetCount();
				if (pg->GetItem(j).SetTo(lr))
					lr->GetCoordinates(pointArr);
				j++;
			}
			WriteUInt32(&buff[4], (UInt32)nPtOfst);
			stm->Write(buff, 8);
			stm->Write((UInt8*)ptOfstArr, nPtOfst * 4);
			stmPos += 8 + nPtOfst * 4;

			Int32 *ptArr = MemAlloc(Int32, pointArr.GetCount() << 1);
			j = pointArr.GetCount();
			while (j-- > 0)
			{
				Math::Coord2DDbl pt = pointArr.GetItem(j);
				ptArr[j << 1] = Double2Int32(pt.x * 200000.0);
				ptArr[(j << 1) + 1] = Double2Int32(pt.y * 200000.0);
			}
			UInt32 nPoint = (UInt32)pointArr.GetCount();
			stm->Write((UInt8*)&nPoint, 4);
			stm->Write((UInt8*)ptArr, 8 * nPoint);
			stmPos += 8 * nPoint + 4;

			maxX = minX = ptArr[0];
			maxY = minY = ptArr[1];
			j = nPoint;
			while (j-- > 0)
			{
				if (minX > ptArr[j << 1])
					minX = ptArr[j << 1];
				if (maxX < ptArr[j << 1])
					maxX = ptArr[j << 1];
				if (minY > ptArr[(j << 1) + 1])
					minY = ptArr[(j << 1) + 1];
				if (maxY < ptArr[(j << 1) + 1])
					maxY = ptArr[(j << 1) + 1];
			}

			left = minX / p->scale;
			right = maxX / p->scale;
			top = minY / p->scale;
			bottom = maxY / p->scale;
			MemFree(ptArr);
		}
		else
		{
			printf("CIPExporter: Unsupported vector type\r\n");
			left = 0;
			right = 0;
			top = 0;
			bottom = 0;
		}

		{
			OSInt j;
			OSInt k;
			OSInt l;
			OSInt m;
			OSInt n;
			Text::StringBuilderUTF8 sb;
			j = top;
			while (j <= bottom)
			{
				k = left;
				while (k <= right)
				{
					l = 0;
					m = (OSInt)blks.GetCount() - 1;
					while (l <= m)
					{
						n = (l + m) >> 1;
						theBlk = blks.GetItemNoCheck((UOSInt)n);
						if (theBlk->blockX > k)
						{
							l = n + 1;
						}
						else if (theBlk->blockX < k)
						{
							m = n - 1;
						}
						else if (theBlk->blockY > j)
						{
							l = n + 1;
						}
						else if (theBlk->blockY < j)
						{
							m = n - 1;
						}
						else
						{
							strRec = MemAllocNN(CIPStrRecord);
							strRec->recId = (Int32)i;
							sb.ClearStr();
							if (layer->GetString(sb, nameArr, objIds.GetItem(i), p->dispCol))
							{
								strRec->str = Text::StrCopyNewC(sb.v, sb.leng).Ptr();
							}
							else
							{
								strRec->str = 0;
							}
							theBlk->records.Add(strRec);
							strs.Add(strRec);
							l = -1;

							break;
						}
					}
					if (l >= 0)
					{
						NEW_CLASSNN(theBlk, CIPBlock());
						theBlk->blockX = (Int32)k;
						theBlk->blockY = (Int32)j;

						strRec = MemAllocNN(CIPStrRecord);
						strRec->recId = (Int32)i;
						sb.ClearStr();
						if (layer->GetString(sb, nameArr, objIds.GetItem(i), p->dispCol))
						{
							strRec->str = Text::StrCopyNewC(sb.v, sb.leng).Ptr();
						}
						else
						{
							strRec->str = 0;
						}
						theBlk->records.Add(strRec);
						strs.Add(strRec);

						blks.Insert((UOSInt)l, theBlk);
					}

					k++;
				}
				j++;
			}
		}
		DEL_CLASS(vec);

		i++;
	}

	*(Int32*)&buff[0] = (Int32)blks.GetCount();
	*(Int32*)&buff[4] = p->scale;
	blk.Write(buff, 8);
	cib.Write(buff, 8);
	stmPos = 8;

	i = 0;
	j = blks.GetCount();
	while (i < j)
	{
		theBlk = blks.GetItemNoCheck(i);
        *(Int32*)&buff[0] = (Int32)theBlk->records.GetCount();
        *(Int32*)&buff[4] = theBlk->blockX;
		*(Int32*)&buff[8] = theBlk->blockY;
		blk.Write(buff, 12);
		k = 0;
		while (k < theBlk->records.GetCount())
		{
			blk.Write((UInt8*)&theBlk->records.GetItemNoCheck(k)->recId, 4);
			k += 1;
		}
		cib.Write(buff, 16);
		stmPos += 16;
		i += 1;
	}

	i = 0;
	j = blks.GetCount();
	while (i < j)
	{
		theBlk = blks.GetItemNoCheck(i);
		cib.SeekFromBeginning(8 + i * 16);

		*(Int32*)&buff[0] = theBlk->blockX;
		*(Int32*)&buff[4] = theBlk->blockY;
		*(Int32*)&buff[8] = (Int32)theBlk->records.GetCount();
		*(Int32*)&buff[12] = (Int32)stmPos;
		cib.Write(buff, 16);

		cib.SeekFromBeginning(stmPos);
		k = 0;
		while (k < theBlk->records.GetCount())
		{
			*(Int32*)&buff[0] = theBlk->records.GetItemNoCheck(k)->recId;
			strRec = theBlk->records.GetItemNoCheck(k);
			if (strRec->str == 0)
			{
				buff[4] = 0;
				cib.Write(buff, 5);
			}
			else
			{
				l = Text::StrCharCnt(strRec->str);
				if (l > 127)
				{
					l = 127;
				}
				buff[4] = (UInt8)(l << 1);
				cib.Write(buff, 5);
				cib.Write((UInt8*)strRec->str, l << 1);
			}

			stmPos += (UOSInt)buff[4] + 5;
			k += 1;
		}
		i += 1;
	}

	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);
	blks.DeleteAll();
	i = strs.GetCount();
	while (i-- > 0)
	{
		strRec = strs.GetItemNoCheck(i);
		if (strRec->str)
		{
			Text::StrDelNew(strRec->str);
		}
		MemFreeNN(strRec);
	}
	return true;
}

UOSInt Exporter::CIPExporter::GetParamCnt()
{
	return 2;
}

Optional<IO::FileExporter::ParamData> Exporter::CIPExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	if (this->IsObjectSupported(pobj) == IO::FileExporter::SupportType::MultiFiles)
	{
		Exporter::CIPExporter::CIPParam *param;
		param = MemAlloc(Exporter::CIPExporter::CIPParam, 1);
		param->layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
		param->dispCol = 0;

		if (param->layer->GetLayerType() == Map::DRAW_LAYER_POINT || param->layer->GetLayerType() == Map::DRAW_LAYER_POINT3D)
		{
			param->scale = 5000;
		}
		else
		{
			param->scale = param->layer->CalBlockSize();
		}
		return (ParamData*)param;
	}
	return 0;
}

void Exporter::CIPExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		MemFree(para.Ptr());
	}
}

Bool Exporter::CIPExporter::GetParamInfo(UOSInt index, NN<IO::FileExporter::ParamInfo> info)
{
	if (index == 0)
	{
		info->name = CSTR("Scale");
		info->paramType = IO::FileExporter::ParamType::INT32;
		info->allowNull = false;
		return true;
	}
	else if (index == 1)
	{
		info->name = CSTR("Display Column");
		info->paramType = IO::FileExporter::ParamType::SELECTION;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::CIPExporter::SetParamStr(Optional<ParamData> param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::CIPExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		if (val >= 5000 && val <= 5000000)
		{
			Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
			p->scale = val;
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

Bool Exporter::CIPExporter::SetParamSel(Optional<ParamData> param, UOSInt index, UOSInt selCol)
{
	NN<ParamData> para;
	if (index == 1 && param.SetTo(para))
	{
		Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
		p->dispCol = (UInt32)selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::CIPExporter::GetParamStr(Optional<ParamData> param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::CIPExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
		return p->scale;
	}
	return 0;
}

Int32 Exporter::CIPExporter::GetParamSel(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 1 && param.SetTo(para))
	{
		Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
		return (Int32)p->dispCol;
	}
	return 0;
}

UTF8Char *Exporter::CIPExporter::GetParamSelItems(Optional<ParamData> param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
{
	NN<ParamData> para;
	if (index == 1 && param.SetTo(para))
	{
		Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)para.Ptr();
		if (itemIndex >= 0 && (UOSInt)itemIndex < p->layer->GetColumnCnt())
		{
			return p->layer->GetColumnName(buff, itemIndex);
		}
	}
	return 0;
}
