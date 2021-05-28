#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Media/ICCProfile.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Media::ICCProfile::ICCProfile(const UInt8 *iccBuff)
{
	UOSInt leng = ReadMUInt32(iccBuff);
	this->iccBuff = MemAlloc(UInt8, leng);
	MemCopyNO(this->iccBuff, iccBuff, leng);
}

Media::ICCProfile::~ICCProfile()
{
	MemFree(this->iccBuff);
}

Int32 Media::ICCProfile::GetCMMType()
{
	return ReadMInt32(&this->iccBuff[4]);
}

void Media::ICCProfile::GetProfileVer(UInt8 *majorVer, UInt8 *minorVer, UInt8 *bugFixVer)
{
	*majorVer = this->iccBuff[8];
	*minorVer = (UInt8)(this->iccBuff[9] >> 4);
	*bugFixVer = this->iccBuff[9] & 0xf;
}

Int32 Media::ICCProfile::GetProfileClass()
{
	return ReadMInt32(&this->iccBuff[12]);
}

Int32 Media::ICCProfile::GetDataColorspace()
{
	return ReadMInt32(&this->iccBuff[16]);
}

Int32 Media::ICCProfile::GetPCS()
{
	return ReadMInt32(&this->iccBuff[20]);
}

void Media::ICCProfile::GetCreateTime(Data::DateTime *createTime)
{
	ReadDateTimeNumber(&this->iccBuff[24], createTime);
}

Int32 Media::ICCProfile::GetPrimaryPlatform()
{
	return ReadMInt32(&this->iccBuff[40]);
}

Int32 Media::ICCProfile::GetProfileFlag()
{
	return ReadMInt32(&this->iccBuff[44]);
}

Int32 Media::ICCProfile::GetDeviceManufacturer()
{
	return ReadMInt32(&this->iccBuff[48]);
}

Int32 Media::ICCProfile::GetDeviceModel()
{
	return ReadMInt32(&this->iccBuff[52]);
}

Int32 Media::ICCProfile::GetDeviceAttrib()
{
	return ReadMInt32(&this->iccBuff[56]);
}

Int32 Media::ICCProfile::GetDeviceAttrib2()
{
	return ReadMInt32(&this->iccBuff[60]);
}

Int32 Media::ICCProfile::GetRenderingIntent()
{
	return ReadMInt32(&this->iccBuff[64]);
}

void Media::ICCProfile::GetPCSIlluminant(CIEXYZ *xyz)
{
	ReadXYZNumber(&this->iccBuff[68], xyz);
}

Int32 Media::ICCProfile::GetProfileCreator()
{
	return ReadMInt32(&this->iccBuff[80]);
}

Int32 Media::ICCProfile::GetTagCount()
{
	return ReadMInt32(&this->iccBuff[128]);
}

Media::LUT *Media::ICCProfile::CreateRLUT()
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x72545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return 0;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, (const UTF8Char*)"ICC Profile R"));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return 0;
			}
		}
		i++;
	}
	return 0;
}

Media::LUT *Media::ICCProfile::CreateGLUT()
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x67545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return 0;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, (const UTF8Char*)"ICC Profile G"));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return 0;
			}
		}
		i++;
	}
	return 0;
}

Media::LUT *Media::ICCProfile::CreateBLUT()
{
	UInt32 cnt = ReadMUInt32(&this->iccBuff[128]);
	UInt32 i = 0;
	Media::LUT *lut;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		UInt32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x62545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				UInt32 i;
				UInt8 *stab;
				UInt8 *dtab;
				valCnt = ReadMUInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt <= 1)
				{
					return 0;
				}
				NEW_CLASS(lut, Media::LUT(1, valCnt, 1, Media::LUT::DF_UINT16, (const UTF8Char*)"ICC Profile B"));
				stab = &this->iccBuff[tagOfst + 12];
				dtab = lut->GetTablePtr();
				i = 0;
				while (i < valCnt)
				{
					*(UInt16*)&dtab[i * 2] = ReadMUInt16(&stab[i * 2]);
					i++;
				}
				return lut;
			}
			else
			{
				return 0;
			}
		}
		i++;
	}
	return 0;
}

Bool Media::ICCProfile::GetRedTransferParam(Media::CS::TransferParam *param)
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x72545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					Media::LUT *lut = this->CreateRLUT();
					param->Set(lut);
					DEL_CLASS(lut);
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetGreenTransferParam(Media::CS::TransferParam *param)
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x67545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					Media::LUT *lut = this->CreateGLUT();
					param->Set(lut);
					DEL_CLASS(lut);
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetBlueTransferParam(Media::CS::TransferParam *param)
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;
		Int32 valCnt;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x62545243 || tagSign == 0x6B545243)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x63757276)
			{
				valCnt = ReadMInt32(&this->iccBuff[tagOfst + 8]);
				if (valCnt == 0)
				{
					param->Set(Media::CS::TRANT_LINEAR, 1.0);
				}
				else if (valCnt == 1)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadU8Fixed8Number((UInt8*)&this->iccBuff[tagOfst + 12]));
				}
				else
				{
					Media::LUT *lut = this->CreateBLUT();
					param->Set(lut);
					DEL_CLASS(lut);
				}

/*				Double gamma;
				Media::CS::TransferType tranType;
				tranType = FindTransferType(valCnt, (UInt16*)&this->iccBuff[tagOfst + 12], &gamma);
				param->Set(tranType, gamma);*/
				return true;
			}
			else if (tagType == 0x70617261) //parametricCurveType
			{
				Int32 funcType = ReadMInt16(&this->iccBuff[tagOfst + 8]);
				Double params[7];
				if (funcType == 0)
				{
					param->Set(Media::CS::TRANT_GAMMA, ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]));
					return true;
				}
				else if (funcType == 3)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = 0;
					params[6] = 0;
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
				else if (funcType == 4)
				{
					params[0] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 12]);
					params[1] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 16]);
					params[2] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 20]);
					params[3] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 24]);
					params[4] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 28]);
					params[5] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 32]);
					params[6] = ReadS15Fixed16Number(&this->iccBuff[tagOfst + 36]);
					param->Set(Media::CS::TRANT_PARAM1, params, 7);
					return true;
				}
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	return false;
}

Bool Media::ICCProfile::GetColorPrimaries(Media::ColorProfile::ColorPrimaries *color)
{
	Int32 cnt = ReadMInt32(&this->iccBuff[128]);
	Int32 i = 0;
	Bool foundR = false;
	Bool foundG = false;
	Bool foundB = false;
	Bool foundW = false;
	CIEXYZ rxyz;
	CIEXYZ gxyz;
	CIEXYZ bxyz;
	CIEXYZ wxyz;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
//		Int32 tagLeng;
		Int32 tagType;

		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		if (tagSign == 0x7258595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				ReadXYZNumber(&this->iccBuff[tagOfst + 8], &rxyz);
				foundR = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x6758595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				ReadXYZNumber(&this->iccBuff[tagOfst + 8], &gxyz);
				foundG = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x6258595A)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				ReadXYZNumber(&this->iccBuff[tagOfst + 8], &bxyz);
				foundB = true;
			}
			else
			{
				return false;
			}
		}
		else if (tagSign == 0x77747074)
		{
			tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
//			tagLeng = ReadMInt32(&this->iccBuff[140 + 12 * i]);
			tagType = ReadMInt32(&this->iccBuff[tagOfst]);
			if (tagType == 0x58595A20)
			{
				ReadXYZNumber(&this->iccBuff[tagOfst + 8], &wxyz);
				foundW = true;
			}
			else
			{
				return false;
			}
		}
		i++;
	}
	if (foundR && foundG && foundB && foundW)
	{
		Math::Matrix3 mat;
		Math::Matrix3 mat2;
		Math::Matrix3 mat3;
		Math::Vector3 vec1;
		Math::Vector3 vec2;
		Math::Vector3 vec3;

		Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat);
		mat2.Set(&mat);
		mat3.SetIdentity();

		Media::ColorProfile::ColorPrimaries::GetWhitePointXYZ(Media::ColorProfile::WPT_D50, &vec3);
		mat.Multiply(&vec3, &vec1);
		mat.Multiply(&wxyz, &vec2);
		mat.Inverse();
		mat3.vec[0].val[0] = vec2.val[0] / vec1.val[0];
		mat3.vec[1].val[1] = vec2.val[1] / vec1.val[1];
		mat3.vec[2].val[2] = vec2.val[2] / vec1.val[2];
		mat.Multiply(&mat3);
		mat.Multiply(&mat2);

		color->colorType = Media::ColorProfile::CT_CUSTOM;
		mat.Multiply(&rxyz, &vec1);
		Media::ColorProfile::ColorPrimaries::XYZToxyY(&vec1, &vec2);
		color->rx = vec2.val[0];
		color->ry = vec2.val[1];
		mat.Multiply(&gxyz, &vec1);
		Media::ColorProfile::ColorPrimaries::XYZToxyY(&vec1, &vec2);
		color->gx = vec2.val[0];
		color->gy = vec2.val[1];
		mat.Multiply(&bxyz, &vec1);
		Media::ColorProfile::ColorPrimaries::XYZToxyY(&vec1, &vec2);
		color->bx = vec2.val[0];
		color->by = vec2.val[1];
		Media::ColorProfile::ColorPrimaries::XYZToxyY(&wxyz, &vec2);
		color->wx = vec2.val[0];
		color->wy = vec2.val[1];
		return true;
	}
	else
	{
		color->SetColorType(Media::ColorProfile::CT_PUNKNOWN);
		return false;
	}
}

void Media::ICCProfile::ToString(Text::StringBuilderUTF *sb)
{
	UInt8 majorVer;
	UInt8 minorVer;
	UInt8 bugFixVer;
	Data::DateTime dt;
	CIEXYZ xyz;
	Int32 val;

	sb->Append((const UTF8Char*)"Preferred CMM Type = ");
	sb->Append(GetNameCMMType(this->GetCMMType()));

	this->GetProfileVer(&majorVer, &minorVer, &bugFixVer);
	sb->Append((const UTF8Char*)"\r\nProfile version number = ");
	sb->AppendU16(majorVer);
	sb->Append((const UTF8Char*)".");
	sb->AppendU16(minorVer);
	sb->Append((const UTF8Char*)".");
	sb->AppendU16(bugFixVer);

	sb->Append((const UTF8Char*)"\r\nProfile/Device class = ");
	sb->Append(GetNameProfileClass(this->GetProfileClass()));

	sb->Append((const UTF8Char*)"\r\nData colour space = ");
	sb->Append(GetNameDataColorspace(this->GetDataColorspace()));

	sb->Append((const UTF8Char*)"\r\nPCS (profile connection space) = ");
	sb->Append(GetNameDataColorspace(this->GetPCS()));

	this->GetCreateTime(&dt);
	sb->Append((const UTF8Char*)"\r\nCreate Time = ");
	sb->AppendDate(&dt);

	sb->Append((const UTF8Char*)"\r\nPrimary Platform = ");
	sb->Append(GetNamePrimaryPlatform(this->GetPrimaryPlatform()));

	val = this->GetProfileFlag();
	if (val & 1)
	{
		sb->Append((const UTF8Char*)"\r\nEmbedded profile = True");
	}
	else
	{
		sb->Append((const UTF8Char*)"\r\nEmbedded profile = False");
	}
	if (val & 2)
	{
		sb->Append((const UTF8Char*)"\r\nProfile cannot be used independently of the embedded colour data = True");
	}
	else
	{
		sb->Append((const UTF8Char*)"\r\nProfile cannot be used independently of the embedded colour data = False");
	}

	sb->Append((const UTF8Char*)"\r\nDevice manufacturer = ");
	sb->Append(GetNameDeviceManufacturer(this->GetDeviceManufacturer()));

	sb->Append((const UTF8Char*)"\r\nDevice model = ");
	sb->Append(GetNameDeviceModel(this->GetDeviceModel()));

	this->GetPCSIlluminant(&xyz);
	sb->Append((const UTF8Char*)"\r\nPCS illuminant = ");
	GetDispCIEXYZ(sb, &xyz);

	sb->Append((const UTF8Char*)"\r\nProfile creator = ");
	sb->Append(GetNameDeviceManufacturer(this->GetProfileCreator()));

	UInt32 cnt = ReadMUInt32(&this->iccBuff[128]);
	UInt32 i = 0;
	while (i < cnt)
	{
		Int32 tagSign;
		Int32 tagOfst;
		UInt32 tagLeng;
		tagSign = ReadMInt32(&this->iccBuff[132 + 12 * i]);
		tagOfst = ReadMInt32(&this->iccBuff[136 + 12 * i]);
		tagLeng = ReadMUInt32(&this->iccBuff[140 + 12 * i]);

		sb->Append((const UTF8Char*)"\r\n");
		sb->Append(GetNameTag(tagSign));
		sb->Append((const UTF8Char*)" = ");
		GetDispTagType(sb, &this->iccBuff[tagOfst], tagLeng);
		i++;
	}
}

Media::ICCProfile *Media::ICCProfile::Parse(const UInt8 *buff, UOSInt buffSize)
{
	Media::ICCProfile *profile;
	if (ReadMUInt32(buff) != buffSize)
		return 0;
	if (ReadMInt32(&buff[36]) != 0x61637370)
		return 0;
	
	NEW_CLASS(profile, Media::ICCProfile(buff));

	return profile;
}

void Media::ICCProfile::ReadDateTimeNumber(const UInt8 *buff, Data::DateTime *dt)
{
	dt->SetValue(ReadMUInt16(&buff[0]), (UInt8)ReadMUInt16(&buff[2]), (UInt8)ReadMUInt16(&buff[4]), (UInt8)ReadMUInt16(&buff[6]), (UInt8)ReadMUInt16(&buff[8]), (UInt8)ReadMUInt16(&buff[10]), 0);
}

void Media::ICCProfile::ReadXYZNumber(const UInt8 *buff, CIEXYZ *xyz)
{
	xyz->val[0] = ReadS15Fixed16Number(&buff[0]);
	xyz->val[1] = ReadS15Fixed16Number(&buff[4]);
	xyz->val[2] = ReadS15Fixed16Number(&buff[8]);
}

Double Media::ICCProfile::ReadS15Fixed16Number(const UInt8 *buff)
{
	return ReadMInt32(buff) / 65536.0;
}

Double Media::ICCProfile::ReadU16Fixed16Number(const UInt8 *buff)
{
	return ((UInt32)ReadMInt32(buff)) / 65536.0;
}

Double Media::ICCProfile::ReadU8Fixed8Number(const UInt8 *buff)
{
	return ((UInt16)ReadMInt16(buff)) / 256.0;
}

const UTF8Char *Media::ICCProfile::GetNameCMMType(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	case 0x33324254:
		return (const UTF8Char*)"the imaging factory CMM";
	case 0x41434D53:
		return (const UTF8Char*)"Agfa CMM";
	case 0x41444245:
		return (const UTF8Char*)"Adobe CMM";
	case 0x43434D53:
		return (const UTF8Char*)"ColorGear CMM";
	case 0x45464920:
		return (const UTF8Char*)"EFI CMM";
	case 0x46462020:
		return (const UTF8Char*)"Fuji Film CMM";
	case 0x48434d4d:
		return (const UTF8Char*)"Harlequin RIP CMM";
	case 0x48444d20:
		return (const UTF8Char*)"Heidelberg CMM";
	case 0x4b434d53:
		return (const UTF8Char*)"Kodak CMM";
	case 0x4C676f53:
		return (const UTF8Char*)"LogoSync CMM";
	case 0x4d434d44:
		return (const UTF8Char*)"Konica Minolta CMM";
	case 0x52474d53:
		return (const UTF8Char*)"DeviceLink CMM";
	case 0x53494343:
		return (const UTF8Char*)"SampleICC CMM";
	case 0x5349474E:
		return (const UTF8Char*)"Mutoh CMM";
	case 0x5543434D:
		return (const UTF8Char*)"ColorGear CMM Lite";
	case 0x55434D53:
		return (const UTF8Char*)"ColorGear CMM C";
	case 0x57544720:
		return (const UTF8Char*)"Ware to Go CMM";
	case 0x6170706C:
		return (const UTF8Char*)"Apple CMM";
	case 0x6172676C:
		return (const UTF8Char*)"Argyll CMS CMM";
	case 0x7a633030:
		return (const UTF8Char*)"Zoran CMM";
	case 0x6C636d73:
		return (const UTF8Char*)"Little CMS CMM";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameProfileClass(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	case 0x73636E72:
		return (const UTF8Char*)"Input device profile";
	case 0x6D6E7472:
		return (const UTF8Char*)"Display device profile";
	case 0x70727472:
		return (const UTF8Char*)"Output device profile";
	case 0x6C696E6B:
		return (const UTF8Char*)"DeviceLink profile";
	case 0x73706163:
		return (const UTF8Char*)"ColorSpace profile";
	case 0x61627374:
		return (const UTF8Char*)"Abstract profile";
	case 0x6E6D636C:
		return (const UTF8Char*)"NamedColor profile";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameDataColorspace(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	case 0x58595A20:
		return (const UTF8Char*)"nCIEXYZ or PCSXYZ";
	case 0x4C616220:
		return (const UTF8Char*)"CIELAB or PCSLAB";
	case 0x4C757620:
		return (const UTF8Char*)"CIELUV";
	case 0x59436272:
		return (const UTF8Char*)"YCbCr";
	case 0x59787920:
		return (const UTF8Char*)"CIEYxy";
	case 0x52474220:
		return (const UTF8Char*)"RGB";
	case 0x47524159:
		return (const UTF8Char*)"Gray";
	case 0x48535620:
		return (const UTF8Char*)"HSV";
	case 0x484C5320:
		return (const UTF8Char*)"HLS";
	case 0x434D594B:
		return (const UTF8Char*)"CMYK";
	case 0x434D5920:
		return (const UTF8Char*)"CMY";
	case 0x32434C52:
		return (const UTF8Char*)"2 colour";
	case 0x33434C52:
		return (const UTF8Char*)"3 colour";
	case 0x34434C52:
		return (const UTF8Char*)"4 colour";
	case 0x35434C52:
		return (const UTF8Char*)"5 colour";
	case 0x36434C52:
		return (const UTF8Char*)"6 colour";
	case 0x37434C52:
		return (const UTF8Char*)"7 colour";
	case 0x38434C52:
		return (const UTF8Char*)"8 colour";
	case 0x39434C52:
		return (const UTF8Char*)"9 colour";
	case 0x41434C52:
		return (const UTF8Char*)"10 colour";
	case 0x42434C52:
		return (const UTF8Char*)"11 colour";
	case 0x43434C52:
		return (const UTF8Char*)"12 colour";
	case 0x44434C52:
		return (const UTF8Char*)"13 colour";
	case 0x45434C52:
		return (const UTF8Char*)"14 colour";
	case 0x46434C52:
		return (const UTF8Char*)"15 colour";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNamePrimaryPlatform(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	case 0x4150504C:
		return (const UTF8Char*)"Apple Computer, Inc.";
	case 0x4D534654:
		return (const UTF8Char*)"Microsoft Corporation";
	case 0x53474920:
		return (const UTF8Char*)"Silicon Graphics, Inc.";
	case 0x53554E57:
		return (const UTF8Char*)"Sun Microsystems, Inc.";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameDeviceManufacturer(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameDeviceModel(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameTag(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"(not used)";
	case 0x41324230:
		return (const UTF8Char*)"AToB0Tag";
	case 0x41324231:
		return (const UTF8Char*)"AToB1Tag";
	case 0x41324232:
		return (const UTF8Char*)"AToB2Tag";
	case 0x42324130:
		return (const UTF8Char*)"BToA0Tag";
	case 0x42324131:
		return (const UTF8Char*)"BToA1Tag";
	case 0x42324132:
		return (const UTF8Char*)"BToA2Tag";
	case 0x42324430:
		return (const UTF8Char*)"BToD0Tag";
	case 0x42324431:
		return (const UTF8Char*)"BToD1Tag";
	case 0x42324432:
		return (const UTF8Char*)"BToD2Tag";
	case 0x42324433:
		return (const UTF8Char*)"BToD3Tag";
	case 0x44324230:
		return (const UTF8Char*)"DToB0Tag";
	case 0x44324231:
		return (const UTF8Char*)"DToB1Tag";
	case 0x44324232:
		return (const UTF8Char*)"DToB2Tag";
	case 0x44324233:
		return (const UTF8Char*)"DToB3Tag";
	case 0x62545243:
		return (const UTF8Char*)"blueTRCTag";
	case 0x6258595A:
		return (const UTF8Char*)"blueMatrixColumnTag";
	case 0x626B7074:
		return (const UTF8Char*)"mediaBlackPointTag";
	case 0x63616C74:
		return (const UTF8Char*)"calibrationDateTimeTag";
	case 0x63686164:
		return (const UTF8Char*)"chromaticAdaptationTag";
	case 0x63696973:
		return (const UTF8Char*)"colorimetricIntentImageStateTag";
	case 0x636C6F74:
		return (const UTF8Char*)"colorantTableOutTag";
	case 0x6368726D:
		return (const UTF8Char*)"chromaticityTag";
	case 0x636C726F:
		return (const UTF8Char*)"colorantOrderTag";
	case 0x636C7274:
		return (const UTF8Char*)"colorantTableTag";
	case 0x63707274:
		return (const UTF8Char*)"copyrightTag";
	case 0x64657363:
		return (const UTF8Char*)"profileDescriptionTag";
	case 0x646D6464:
		return (const UTF8Char*)"deviceModelDescTag";
	case 0x646D6E64:
		return (const UTF8Char*)"deviceMfgDescTag";
	case 0x67545243:
		return (const UTF8Char*)"greenTRCTag";
	case 0x6758595A:
		return (const UTF8Char*)"greenMatrixColumnTag";
	case 0x67616D74:
		return (const UTF8Char*)"gamutTag";
	case 0x6B545243:
		return (const UTF8Char*)"grayTRCTag";
	case 0x6C756D69:
		return (const UTF8Char*)"luminanceTag";
	case 0x6D656173:
		return (const UTF8Char*)"measurementTag";
	case 0x6E636C32:
		return (const UTF8Char*)"namedColor2Tag";
	case 0x70726530:
		return (const UTF8Char*)"preview0Tag";
	case 0x70726531:
		return (const UTF8Char*)"preview1Tag";
	case 0x70726532:
		return (const UTF8Char*)"preview2Tag";
	case 0x70736571:
		return (const UTF8Char*)"profileSequenceDescTag";
	case 0x70736964:
		return (const UTF8Char*)"profileSequenceIdentifierTag";
	case 0x72545243:
		return (const UTF8Char*)"redTRCTag";
	case 0x7258595A:
		return (const UTF8Char*)"redMatrixColumnTag";
	case 0x72657370:
		return (const UTF8Char*)"outputResponseTag";
	case 0x72696730:
		return (const UTF8Char*)"perceptualRenderingIntentGamutTag";
	case 0x72696732:
		return (const UTF8Char*)"saturationRenderingIntentGamutTag";
	case 0x74617267:
		return (const UTF8Char*)"charTargetTag";
	case 0x74656368:
		return (const UTF8Char*)"technologyTag";
	case 0x76696577:
		return (const UTF8Char*)"viewingConditionsTag";
	case 0x76756564:
		return (const UTF8Char*)"viewingCondDescTag";
	case 0x77747074:
		return (const UTF8Char*)"mediaWhitePointTag";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Media::ICCProfile::GetNameStandardObserver(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"Unknown";
	case 1:
		return (const UTF8Char*)"CIE 1931 standard colorimetric observer";
	case 2:
		return (const UTF8Char*)"CIE 1964 standard colorimetric observer";
	default:
		return (const UTF8Char*)"Not defined";
	}
}

const UTF8Char *Media::ICCProfile::GetNameStandardIlluminent(Int32 val)
{
	switch (val)
	{
	case 0:
		return (const UTF8Char*)"Unknown";
	case 1:
		return (const UTF8Char*)"D50";
	case 2:
		return (const UTF8Char*)"D65";
	case 3:
		return (const UTF8Char*)"D93";
	case 4:
		return (const UTF8Char*)"F2";
	case 5:
		return (const UTF8Char*)"D55";
	case 6:
		return (const UTF8Char*)"A";
	case 7:
		return (const UTF8Char*)"Equi-Power (E)";
	case 8:
		return (const UTF8Char*)"F8";
	default:
		return (const UTF8Char*)"Not defined";
	}
}

void Media::ICCProfile::GetDispCIEXYZ(Text::StringBuilderUTF *sb, CIEXYZ *xyz)
{
	sb->Append((const UTF8Char*)"X = ");
	Text::SBAppendF64(sb, xyz->val[0]);
	sb->Append((const UTF8Char*)", Y = ");
	Text::SBAppendF64(sb, xyz->val[1]);
	sb->Append((const UTF8Char*)", Z = ");
	Text::SBAppendF64(sb, xyz->val[2]);

	Double sum = xyz->val[0] + xyz->val[1] + xyz->val[2];
	if (sum != 0)
	{
		sb->Append((const UTF8Char*)", x = ");
		Text::SBAppendF64(sb, xyz->val[0] / sum);
		sb->Append((const UTF8Char*)", y = ");
		Text::SBAppendF64(sb, xyz->val[1] / sum);
	}
}

void Media::ICCProfile::GetDispTagType(Text::StringBuilderUTF *sb, UInt8 *buff, UInt32 leng)
{
	Int32 typ = ReadMInt32(buff);
	Int32 nCh;
	Int32 val;
	CIEXYZ xyz;
	UTF8Char sbuff[256];
	Media::CS::TransferType tt;
	Double gamma;
	switch(typ)
	{
	case 0:
		sb->Append((const UTF8Char*)"(not used)");
		break;
	case 0x6368726D:
		nCh = ReadMInt16(&buff[8]);
		val = ReadMInt16(&buff[10]);
		sb->AppendI32(val);
		sb->Append((const UTF8Char*)" {");
		val = 0;
		while (val < nCh)
		{
			if (val > 0)
			{
				sb->Append((const UTF8Char*)", ");
			}
			sb->Append((const UTF8Char*)"(");
			Text::SBAppendF64(sb, ReadU16Fixed16Number(&buff[val * 8 + 12]));
			sb->Append((const UTF8Char*)", ");
			Text::SBAppendF64(sb, ReadU16Fixed16Number(&buff[val * 8 + 16]));
			sb->Append((const UTF8Char*)")");
			val++;
		}
		sb->Append((const UTF8Char*)"}");
		break;
	case 0x74657874: //textType
		{
			if (buff[leng - 1])
			{
				sb->AppendC((const UTF8Char*)&buff[8], leng - 8);
			}
			else
			{
				sb->AppendC((const UTF8Char*)&buff[8], leng - 9);
			}
		}
		break;
	case 0x58595A20: //XYZType
		val = 8;
		nCh = 0;
		while ((UInt32)val <= leng - 12)
		{
			if (nCh)
				sb->Append((const UTF8Char*)"  ");
			ReadXYZNumber(&buff[val], &xyz);
			GetDispCIEXYZ(sb, &xyz);
			val += 12;
			nCh++;
		}
		break;
	case 0x76696577: //viewingConditionsTag
		sb->Append((const UTF8Char*)"Illuminant: {");
		ReadXYZNumber(&buff[8], &xyz);
		GetDispCIEXYZ(sb, &xyz);

		sb->Append((const UTF8Char*)"}, Surround: {");
		ReadXYZNumber(&buff[20], &xyz);
		GetDispCIEXYZ(sb, &xyz);
		sb->Append((const UTF8Char*)"}, Illuminant type = ");
		sb->AppendI32(ReadMInt32(&buff[32]));
		break;
	case 0x6D656173: //measurementType
		sb->Append((const UTF8Char*)"Standard observer = ");
		sb->Append(GetNameStandardObserver(ReadMInt32(&buff[8])));
		sb->Append((const UTF8Char*)", Measurement backing: {");
		ReadXYZNumber(&buff[12], &xyz);
		GetDispCIEXYZ(sb, &xyz);
		sb->Append((const UTF8Char*)"}, Measurement geometry = ");
		sb->AppendI32(ReadMInt32(&buff[24]));
		sb->Append((const UTF8Char*)", Measurement flare = ");
		sb->AppendI32(ReadMInt32(&buff[28]));
		sb->Append((const UTF8Char*)", Standard illuminent = ");
		sb->Append(GetNameStandardIlluminent(ReadMInt32(&buff[32])));
		break;
	case 0x64657363: //desc
		{
			val = ReadMInt32(&buff[8]);
			if (buff[12 + val - 1])
			{
				sb->AppendC((const UTF8Char*)&buff[12], (UInt32)val);
			}
			else
			{
				sb->AppendC((const UTF8Char*)&buff[12], (UInt32)val - 1);
			}
		}
		break;
	case 0x73696720: //signatureType
		sb->AppendHex32(ReadMUInt32(&buff[8]));
		break;
	case 0x63757276: //curveType
		val = ReadMInt32(&buff[8]);
		sb->Append((const UTF8Char*)"Curve: ");
		if (val > 1)
		{
			sb->AppendI32(val);
			sb->Append((const UTF8Char*)" entries, ");
			sb->Append((const UTF8Char*)"Closed to ");
		}
		tt = FindTransferType((UInt32)val, (UInt16*)&buff[12], &gamma);
		sb->Append(Media::CS::TransferFunc::GetTransferFuncName(tt));
		if (tt == Media::CS::TRANT_GAMMA)
		{
			sb->Append((const UTF8Char*)", gamma = ");
			Text::SBAppendF64(sb, gamma);
		}
		break;
	case 0x70617261: //parametricCurveType
		sb->Append((const UTF8Char*)"CurveType: ");
		sb->AppendI16(ReadMInt16(&buff[8]));
		{
			Double g;
			Double a;
			Double b;
			Double c;
			Double d;
			Double e;
			Double f;

			switch (ReadMInt16(&buff[8]))
			{
			case 0:
				g = ReadS15Fixed16Number(&buff[12]);
				sb->Append((const UTF8Char*)" Y = X ^ ");
				Text::SBAppendF64(sb, g);
				break;
			case 1:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				break;
			case 2:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				break;
			case 3:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				sb->Append((const UTF8Char*)" if (X >= ");
				Text::SBAppendF64(sb, d);
				sb->Append((const UTF8Char*)") Y = (");
				Text::SBAppendF64(sb, a);
				sb->Append((const UTF8Char*)" * X + ");
				Text::SBAppendF64(sb, b);
				sb->Append((const UTF8Char*)") ^ ");
				Text::SBAppendF64(sb, g);
				sb->Append((const UTF8Char*)" else Y = ");
				Text::SBAppendF64(sb, c);
				sb->Append((const UTF8Char*)" * X");
				break;
			case 4:
				g = ReadS15Fixed16Number(&buff[12]);
				a = ReadS15Fixed16Number(&buff[16]);
				b = ReadS15Fixed16Number(&buff[20]);
				c = ReadS15Fixed16Number(&buff[24]);
				d = ReadS15Fixed16Number(&buff[28]);
				e = ReadS15Fixed16Number(&buff[32]);
				f = ReadS15Fixed16Number(&buff[36]);
				sb->Append((const UTF8Char*)" if (X >= ");
				Text::SBAppendF64(sb, d);
				sb->Append((const UTF8Char*)") Y = (");
				Text::SBAppendF64(sb, a);
				sb->Append((const UTF8Char*)" * X + ");
				Text::SBAppendF64(sb, b);
				sb->Append((const UTF8Char*)") ^ ");
				Text::SBAppendF64(sb, g);
				sb->Append((const UTF8Char*)" + ");
				Text::SBAppendF64(sb, e);
				sb->Append((const UTF8Char*)" else Y = ");
				Text::SBAppendF64(sb, c);
				sb->Append((const UTF8Char*)" * X + ");
				Text::SBAppendF64(sb, f);
				break;
			default:
				break;
			}
		}
		break;
	case 0x6d667431: //lut8Type
		sb->Append((const UTF8Char*)"LUT8");
		break;
	case 0x6d667432: //lut16Type
		sb->Append((const UTF8Char*)"LUT16");
		break;
	case 0x75693332: //uInt32ArrayType
		sb->Append((const UTF8Char*)"uInt32 Array (");
		sb->AppendU32((leng - 8) >> 2);
		sb->Append((const UTF8Char*)")");
		break;
	case 0x75693038: //uInt8ArrayType
		sb->Append((const UTF8Char*)"uInt8 Array (");
		sb->AppendU32((leng - 8));
		sb->Append((const UTF8Char*)")");
		break;
	case 0x73663332: //s15Fixed16ArrayType
		sb->Append((const UTF8Char*)"s15Fixed16 Array (");
		sb->AppendU32((leng - 8) >> 2);
		sb->Append((const UTF8Char*)")");
		break;
	case 0x6D6C7563: //multiLocalizedUnicodeType
		{
			Text::Encoding enc(1201);
			OSInt i;
			OSInt j;
			i = ReadMInt32(&buff[8]);
			j = 16;
			while (i-- > 0)
			{
				enc.UTF8FromBytes(sbuff, &buff[ReadMInt32(&buff[j + 8])], ReadMUInt32(&buff[j + 4]), 0);
				sb->Append(sbuff);
				if (i > 0)
				{
					sb->Append((const UTF8Char*)", ");
				}
				j += 12;
			}
		}
		break;
	case 0x6d6d6f64:
		sb->Append((const UTF8Char*)"Unknown (mmod)");
		break;
	case 0x6D414220:
		sb->Append((const UTF8Char*)"lutAToBType");
		break;
	case 0x6D424120:
		sb->Append((const UTF8Char*)"lutBToAType");
		break;
	default:
		sb->Append((const UTF8Char*)"Unknown");
		break;
	}
}

Media::CS::TransferType Media::ICCProfile::FindTransferType(UOSInt colorCount, UInt16 *curveColors, Double *gamma)
{
	Media::CS::TransferType trans[] = {Media::CS::TRANT_sRGB, Media::CS::TRANT_BT709, Media::CS::TRANT_GAMMA, Media::CS::TRANT_LINEAR, Media::CS::TRANT_SMPTE240};
	UOSInt tranCnt = sizeof(trans) / sizeof(trans[0]);
	if (colorCount == 0)
	{
		*gamma = 1.0;
		return Media::CS::TRANT_LINEAR;
	}
	else if (colorCount == 1)
	{
		*gamma = ReadU8Fixed8Number((UInt8*)curveColors);
		return Media::CS::TRANT_GAMMA;
	}

	Media::CS::TransferFunc **funcs = MemAlloc(Media::CS::TransferFunc*, tranCnt);
	Double *diffSqrSum = MemAlloc(Double, tranCnt);
	UOSInt i = tranCnt;
	while (i-- > 0)
	{
		Media::CS::TransferParam param(trans[i], 2.2);
		funcs[i] = Media::CS::TransferFunc::CreateFunc(&param);
		diffSqrSum[i] = 0;
	}

	Double mulVal = 1.0 / Math::UOSInt2Double(colorCount - 1);
	Double colVal = 1.0 / 65535.0;
	Double v;
	Double tv;

	UOSInt j = 0;
	while (j < colorCount)
	{
		v = ReadMInt16((UInt8*)&curveColors[j]) * colVal;
		i = tranCnt;
		while (i-- > 0)
		{
			tv = funcs[i]->InverseTransfer(Math::UOSInt2Double(j) * mulVal);
			diffSqrSum[i] += (tv - v) * (tv - v);
		}
		j++;
	}
	Double minVal = diffSqrSum[0];
	Media::CS::TransferType minType = trans[0];
	i = tranCnt;
	while (i-- > 1)
	{
		if (diffSqrSum[i] < minVal)
		{
			minType = trans[i];
			minVal = diffSqrSum[i];
		}
	}

	MemFree(funcs);
	MemFree(diffSqrSum);
	*gamma = 2.2;
	return minType;
}

UTF8Char *Media::ICCProfile::GetProfilePath(UTF8Char *sbuff)
{
	UTF8Char *sptr = sbuff;
#if defined(WIN32)
	sptr = IO::Path::GetOSPath(sptr);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"\\System32\\spool\\drivers\\color");
#endif
	return sptr;
}
