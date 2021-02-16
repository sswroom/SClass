#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Math/Math.h"
#include "Exporter/CIPExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"

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

IO::FileExporter::SupportType Exporter::CIPExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
	Map::DrawLayerType layerType = layer->GetLayerType();
	if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
	{
		return IO::FileExporter::ST_MULTI_FILES;
	}
	else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
	{
		return IO::FileExporter::ST_MULTI_FILES;
	}
	else if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
	{
		return IO::FileExporter::ST_MULTI_FILES;
	}
	return IO::FileExporter::ST_NOT_SUPPORTED;
}

Bool Exporter::CIPExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"CIP file");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.cip");
		return true;
	}
	return false;
}

Bool Exporter::CIPExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	UInt8 buff[256];
	UTF8Char u8buff[256];
	if (param == 0)
		return false;
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (pobj->GetParserType() != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		return false;
	}
	Map::IMapDrawLayer *layer = (Map::IMapDrawLayer *)pobj;
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
	IO::FileStream *cix;
	IO::FileStream *cib;
	IO::FileStream *blk;
	Text::StrConcat(u8buff, fileName);
	IO::Path::ReplaceExt(u8buff, (const UTF8Char*)"cix");
	NEW_CLASS(cix, IO::FileStream(u8buff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	IO::Path::ReplaceExt(u8buff, (const UTF8Char*)"ciu");
	NEW_CLASS(cib, IO::FileStream(u8buff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	IO::Path::ReplaceExt(u8buff, (const UTF8Char*)"blk");
	NEW_CLASS(blk, IO::FileStream(u8buff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));


	Data::ArrayListInt64 *objIds;
	Data::ArrayList<CIPBlock*> *blks;
	Data::ArrayList<CIPStrRecord*> *strs;
	CIPStrRecord *strRec;
	CIPBlock *theBlk;
	void *sess;
	void *nameArr;
	OSInt stmPos = 8;
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
	NEW_CLASS(objIds, Data::ArrayListInt64());
	NEW_CLASS(blks, Data::ArrayList<CIPBlock*>());
	NEW_CLASS(strs, Data::ArrayList<CIPStrRecord*>());
	layer->GetAllObjectIds(objIds, &nameArr);
	sess = layer->BeginGetObject();

	*(Int32*)&buff[0] = (Int32)objIds->GetCount();
	*(Int32*)&buff[4] = iLayerType;
	stm->Write(buff, 8);
	cix->Write(buff, 4);

	i = 0;
	recCnt = objIds->GetCount();
	while (i < recCnt)
	{
		Map::DrawObjectL *dobj;
		dobj = layer->GetObjectByIdD(sess, objIds->GetItem(i));

		*(Int32*)&buff[0] = (Int32)i;
		*(Int32*)&buff[4] = (Int32)stmPos;
		cix->Write(buff, 8);

		if (dobj->nPtOfst == 0)
		{
			*(Int32*)&buff[4] = 1;
			*(Int32*)&buff[8] = 0;
			stm->Write(buff, 12);
			stmPos += 12;
		}
		else
		{
			*(Int32*)&buff[4] = dobj->nPtOfst;
			stm->Write(buff, 8);
			stm->Write((UInt8*)dobj->ptOfstArr, dobj->nPtOfst * 4);
			stmPos += 8 + dobj->nPtOfst * 4;
		}
		Int32 *ptArr = MemAlloc(Int32, dobj->nPoint << 1);
		j = dobj->nPoint << 1;
		while (j-- > 0)
		{
			ptArr[j] = Math::Double2Int32(dobj->pointArr[j] * 200000.0);
		}
		stm->Write((UInt8*)&dobj->nPoint, 4);
		stm->Write((UInt8*)ptArr, 8 * dobj->nPoint);
		stmPos += 8 * dobj->nPoint + 4;

		maxX = minX = ptArr[0];
		maxY = minY = ptArr[1];
		j = dobj->nPoint;
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

		{
			OSInt j;
			OSInt k;
			OSInt l;
			OSInt m;
			OSInt n;
			j = top;
			while (j <= bottom)
			{
				k = left;
				while (k <= right)
				{
					l = 0;
					m = blks->GetCount() - 1;
					while (l <= m)
					{
						n = (l + m) >> 1;
						theBlk = blks->GetItem(n);
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
							strRec = MemAlloc(CIPStrRecord, 1);
							strRec->recId = (Int32)i;
							if (layer->GetString(u8buff, sizeof(u8buff), nameArr, objIds->GetItem(i), p->dispCol))
							{
								strRec->str = Text::StrCopyNew(u8buff);
							}
							else
							{
								strRec->str = 0;
							}
							theBlk->records->Add(strRec);
							strs->Add(strRec);
							l = -1;
							break;
						}
					}
					if (l >= 0)
					{
						theBlk = MemAlloc(CIPBlock, 1);
						theBlk->blockX = (Int32)k;
						theBlk->blockY = (Int32)j;
						NEW_CLASS(theBlk->records, Data::ArrayList<CIPStrRecord*>());

						strRec = MemAlloc(CIPStrRecord, 1);
						strRec->recId = (Int32)i;
						if (layer->GetString(u8buff, sizeof(u8buff), nameArr, objIds->GetItem(i), p->dispCol))
						{
							strRec->str = Text::StrCopyNew(u8buff);
						}
						else
						{
							strRec->str = 0;
						}
						theBlk->records->Add(strRec);
						strs->Add(strRec);

						blks->Insert(l, theBlk);
					}

					k++;
				}
				j++;
			}
		}


		layer->ReleaseObject(sess, dobj);

		i++;
	}

	*(Int32*)&buff[0] = (Int32)blks->GetCount();
	*(Int32*)&buff[4] = p->scale;
	blk->Write(buff, 8);
	cib->Write(buff, 8);
	stmPos = 8;

	i = 0;
	j = blks->GetCount();
	while (i < j)
	{
		theBlk = blks->GetItem(i);
        *(Int32*)&buff[0] = (Int32)theBlk->records->GetCount();
        *(Int32*)&buff[4] = theBlk->blockX;
		*(Int32*)&buff[8] = theBlk->blockY;
		blk->Write(buff, 12);
		k = 0;
		while (k < theBlk->records->GetCount())
		{
			blk->Write((UInt8*)&theBlk->records->GetItem(k)->recId, 4);
			k += 1;
		}
		cib->Write(buff, 16);
		stmPos += 16;
		i += 1;
	}

	i = 0;
	j = blks->GetCount();
	while (i < j)
	{
		theBlk = blks->GetItem(i);
		cib->Seek(IO::SeekableStream::ST_BEGIN, 8 + i * 16);

		*(Int32*)&buff[0] = theBlk->blockX;
		*(Int32*)&buff[4] = theBlk->blockY;
		*(Int32*)&buff[8] = (Int32)theBlk->records->GetCount();
		*(Int32*)&buff[12] = (Int32)stmPos;
		cib->Write(buff, 16);

		cib->Seek(IO::SeekableStream::ST_BEGIN, stmPos);
		k = 0;
		while (k < theBlk->records->GetCount())
		{
			*(Int32*)&buff[0] = theBlk->records->GetItem(k)->recId;
			strRec = theBlk->records->GetItem(k);
			if (strRec->str == 0)
			{
				buff[4] = 0;
				cib->Write(buff, 5);
			}
			else
			{
				l = Text::StrCharCnt(strRec->str);
				if (l > 127)
				{
					l = 127;
				}
				buff[4] = (UInt8)(l << 1);
				cib->Write(buff, 5);
				cib->Write((UInt8*)strRec->str, l << 1);
			}

			stmPos += buff[4] + 5;
			k += 1;
		}
		i += 1;
	}

	layer->EndGetObject(sess);
	layer->ReleaseNameArr(nameArr);
	i = blks->GetCount();
	while (i-- > 0)
	{
		theBlk = blks->GetItem(i);
		DEL_CLASS(theBlk->records);
		MemFree(theBlk);
	}
	i = strs->GetCount();
	while (i-- > 0)
	{
		strRec = strs->GetItem(i);
		if (strRec->str)
		{
			Text::StrDelNew(strRec->str);
		}
		MemFree(strRec);
	}

	DEL_CLASS(blks);
	DEL_CLASS(strs);
	DEL_CLASS(objIds);
	DEL_CLASS(cix);
	DEL_CLASS(cib);
	DEL_CLASS(blk);
	return true;
}

OSInt Exporter::CIPExporter::GetParamCnt()
{
	return 2;
}

void *Exporter::CIPExporter::CreateParam(IO::ParsedObject *pobj)
{
	if (this->IsObjectSupported(pobj) == IO::FileExporter::ST_MULTI_FILES)
	{
		Exporter::CIPExporter::CIPParam *param;
		param = MemAlloc(Exporter::CIPExporter::CIPParam, 1);
		param->layer = (Map::IMapDrawLayer *)pobj;
		param->dispCol = 0;

		if (param->layer->GetLayerType() == Map::DRAW_LAYER_POINT || param->layer->GetLayerType() == Map::DRAW_LAYER_POINT3D)
		{
			param->scale = 5000;
		}
		else
		{
			param->scale = param->layer->CalBlockSize();
		}
		return param;
	}
	return 0;
}

void Exporter::CIPExporter::DeleteParam(void *param)
{
	MemFree(param);
}

Bool Exporter::CIPExporter::GetParamInfo(OSInt index, IO::FileExporter::ParamInfo *info)
{
	if (index == 0)
	{
		info->name = (const UTF8Char*)"Scale";
		info->paramType = IO::FileExporter::PT_INT32;
		info->allowNull = false;
		return true;
	}
	else if (index == 1)
	{
		info->name = (const UTF8Char*)"Display Column";
		info->paramType = IO::FileExporter::PT_SELECTION;
		info->allowNull = false;
		return true;
	}
	return false;
}

Bool Exporter::CIPExporter::SetParamStr(void *param, OSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::CIPExporter::SetParamInt32(void *param, OSInt index, Int32 val)
{
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (index == 0)
	{
		if (val >= 5000 && val <= 5000000)
		{
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

Bool Exporter::CIPExporter::SetParamSel(void *param, OSInt index, Int32 selCol)
{
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (index == 1)
	{
		p->dispCol = selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::CIPExporter::GetParamStr(void *param, OSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::CIPExporter::GetParamInt32(void *param, OSInt index)
{
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (index == 0)
	{
		return p->scale;
	}
	return 0;
}

Int32 Exporter::CIPExporter::GetParamSel(void *param, OSInt index)
{
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (index == 1)
	{
		return p->dispCol;
	}
	return 0;
}

UTF8Char *Exporter::CIPExporter::GetParamSelItems(void *param, OSInt index, OSInt itemIndex, UTF8Char *buff)
{
	Exporter::CIPExporter::CIPParam *p = (Exporter::CIPExporter::CIPParam*)param;
	if (index == 1)
	{
		if (itemIndex >= 0 && (UOSInt)itemIndex < p->layer->GetColumnCnt())
		{
			return p->layer->GetColumnName(buff, itemIndex);
		}
	}
	return 0;
}
