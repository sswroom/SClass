#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/CS/CSYUV444P10LEP_LRGBC.h"
#include "Media/CS/TransferFunc.h"
#include "Sync/Thread.h"

extern "C"
{
	void CSYUV444P10LEP_LRGBC_convert(UInt8 *yPtr, UInt8 *uPtr, UInt8 *vPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

void Media::CS::CSYUV444P10LEP_LRGBC::SetupRGB13_LR()
{
	Int32 i;
	Double thisV;
	UInt16 v[4];
	Media::ColorProfile *srcColor;
	if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcColor = this->colorSess->GetDefPProfile();
	}
	else if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		srcColor = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile->GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcColor = this->colorSess->GetDefPProfile();
	}
	else
	{
		srcColor = this->srcProfile;
	}

	Media::CS::TransferFunc *rtFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetRTranParam());
	Media::CS::TransferFunc *gtFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetGTranParam());
	Media::CS::TransferFunc *btFunc = Media::CS::TransferFunc::CreateFunc(srcColor->GetBTranParam());
	Math::Matrix3 mat1;
	Math::Matrix3 mat2;
	Math::Matrix3 mat3;
	Math::Matrix3 mat4;
	Math::Matrix3 mat5;
	Math::Vector3 vec1;
	Math::Vector3 vec2;
	Math::Vector3 vec3;
	this->srcProfile->GetPrimaries()->GetConvMatrix(&mat1);
	if (this->destProfile->GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		this->rgbParam->monProfile->GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->rgbParam->monProfile->GetPrimaries()->wx, this->rgbParam->monProfile->GetPrimaries()->wy, 1.0);
	}
	else
	{
		this->destProfile->GetPrimaries()->GetConvMatrix(&mat5);
		vec2.Set(this->destProfile->GetPrimaries()->wx, this->destProfile->GetPrimaries()->wy, 1.0);
	}
	mat5.Inverse();

	Media::ColorProfile::ColorPrimaries::GetMatrixBradford(&mat2);
	mat3.Set(&mat2);
	mat4.SetIdentity();
	vec1.Set(this->srcProfile->GetPrimaries()->wx, this->srcProfile->GetPrimaries()->wy, 1.0);
	Media::ColorProfile::ColorPrimaries::xyYToXYZ(&vec2, &vec3);
	Media::ColorProfile::ColorPrimaries::xyYToXYZ(&vec1, &vec2);
	mat2.Multiply(&vec2, &vec1);
	mat2.Multiply(&vec3, &vec2);
	mat2.Inverse();
	mat4.vec[0].val[0] = vec2.val[0] / vec1.val[0];
	mat4.vec[1].val[1] = vec2.val[1] / vec1.val[1];
	mat4.vec[2].val[2] = vec2.val[2] / vec1.val[2];
	mat2.Multiply(&mat4);
	mat2.Multiply(&mat3);
	mat1.MyMultiply(&mat2);

	mat1.MyMultiply(&mat5);

	i = 32768;
	while (i--)
	{
		thisV = rtFunc->InverseTransfer(i / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(i / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(i / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	i = 65536;
	while (i-- > 32768)
	{
		thisV = rtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	DEL_CLASS(btFunc);
	DEL_CLASS(gtFunc);
	DEL_CLASS(rtFunc);
}

void Media::CS::CSYUV444P10LEP_LRGBC::SetupYUV_RGB13()
{
	Double c;

	int i;

	Int32 y;

	Double Kr;
	Double Kb;
	Double Kg;
	Double Kc1;
	Double Kc2;
	Double Kc3;
	Double Kc4;

	Media::ColorProfile::YUVType yuvType;
	Bool fullRange = (this->yuvType & Media::ColorProfile::YUVT_FLAG_YUV_0_255) != 0;
	if ((this->yuvType & Media::ColorProfile::YUVT_MASK) == Media::ColorProfile::YUVT_UNKNOWN)
	{
		yuvType = this->colorSess->GetDefYUVType();
	}
	else
	{
		yuvType = (Media::ColorProfile::YUVType)(this->yuvType & Media::ColorProfile::YUVT_MASK);
	}
	Media::ColorProfile::GetYUVConstants(yuvType, &Kr, &Kb);
	Kg = 1 - Kr - Kb;
	Kc1 = (1 - Kr) / 0.5;
	Kc2 = -(2 * Kr - 2 * Kr * Kr) / Kg;
	Kc3 = -(2 * Kb - 2 * Kb * Kb) / Kg;
	Kc4 = (1 - Kb) / 0.5;

	if (fullRange)
	{
		i = 65536;
		while (i--)
		{
			y = Math::Double2Int32(0x7fff * (Math_Pow(i / 1023.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}
	else
	{
		i = 65536;
		while (i--)
		{
			y = Math::Double2Int32(0x7fff * (Math_Pow((i - 64) / 876.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
			if (y < -0x8000)
				y = 0x8000;
			else if (y > 0x7fff)
				y = 0x7fff;
			else
				y = y & 0xffff;
			yuv2rgb[i] = (((Int64)y) << 32) | (((Int64)y) << 16) | y;
		}
	}

	i = 65536;
	while (i--)
	{
		if (i == 0)
			i = 0;

		if (fullRange)
		{
			if (i > 512)
				c = Math_Pow((i - 512) / 511.0, this->yuvParam.CGamma) * 16383;
			else if (i == 512)
				c = 0;
			else
				c = Math_Pow((-i + 512) / 511.0, this->yuvParam.CGamma) * (-16383);
		}
		else
		{
			if (i > 512)
				c = Math_Pow((i - 512) / 448.0, this->yuvParam.CGamma) * 16383;
			else if (i == 512)
				c = 0;
			else
				c = Math_Pow((-i + 512) / 448.0, this->yuvParam.CGamma) * (-16383);
		}

		Int32 v2r8;
		Int32 u2g8;
		Int32 v2g8;
		Int32 u2b8;
		v2r8 = Math::Double2Int32((Kc1 * c * this->yuvParam.Saturation * this->yuvParam.RMul + this->yuvParam.RAdd) / 4.0);
		u2g8 = Math::Double2Int32((Kc3 * c * this->yuvParam.Saturation * this->yuvParam.GMul + this->yuvParam.GAdd) / 4.0);
		v2g8 = Math::Double2Int32((Kc2 * c * this->yuvParam.Saturation * this->yuvParam.GMul) / 4.0);
		u2b8 = Math::Double2Int32((Kc4 * c * this->yuvParam.Saturation * this->yuvParam.BMul + this->yuvParam.BAdd) / 4.0);
		if (v2r8 < -0x8000)
			v2r8 = 0x8000;
		else if (v2r8 > 0x7fff)
			v2r8 = 0x7fff;
		else
			v2r8 = v2r8 & 0xffff;
		if (u2g8 < -0x8000)
			u2g8 = 0x8000;
		else if (u2g8 > 0x7fff)
			u2g8 = 0x7fff;
		else
			u2g8 = u2g8 & 0xffff;
		if (v2g8 < -0x8000)
			v2g8 = 0x8000;
		else if (v2g8 > 0x7fff)
			v2g8 = 0x7fff;
		else
			v2g8 = v2g8 & 0xffff;
		if (u2b8 < -0x8000)
			u2b8 = 0x8000;
		else if (u2b8 > 0x7fff)
			u2b8 = 0x7fff;
		else
			u2b8 = u2b8 & 0xffff;
		yuv2rgb[i + 65536] = (((Int64)u2g8) << 16) | u2b8;
		yuv2rgb[i + 131072] = (((Int64)v2g8) << 16) | (((Int64)v2r8) << 32);
	}
}

UInt32 Media::CS::CSYUV444P10LEP_LRGBC::WorkerThread(void *obj)
{
	CSYUV444P10LEP_LRGBC *converter = (CSYUV444P10LEP_LRGBC*)obj;
	UOSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];

	ts->status = 1;
	converter->evtMain->Set();
	while (true)
	{
		ts->evt->Wait();
		if (ts->status == 2)
		{
			break;
		}
		else if (ts->status == 3)
		{
			CSYUV444P10LEP_LRGBC_convert(ts->yPtr, ts->uPtr, ts->vPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->yBpl, converter->yuv2rgb, converter->rgbGammaCorr);
			ts->status = 4;
			converter->evtMain->Set();
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain->Set();
	return 0;
}

void Media::CS::CSYUV444P10LEP_LRGBC::WaitForWorker(Int32 jobStatus)
{
	UOSInt i;
	Bool exited;
	while (true)
	{
		exited = true;
		i = this->nThread;
		while (i-- > 0)
		{
			if (this->stats[i].status == jobStatus)
			{
				this->stats[i].evt->Set();
				exited = false;
				break;
			}
		}
		if (exited)
			break;
		this->evtMain->Wait();
	}
}

Media::CS::CSYUV444P10LEP_LRGBC::CSYUV444P10LEP_LRGBC(const Media::ColorProfile *srcProfile, const Media::ColorProfile *destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess) : Media::CS::CSConverter(colorSess)
{
	UOSInt i;
	this->yuvType = yuvType;
	this->rgbGammaCorr = MemAlloc(Int64, 65536 * 3);
	this->yuv2rgb = MemAlloc(Int64, 65536 * 3);
	NEW_CLASS(this->rgbParam, Media::IColorHandler::RGBPARAM2());
	NEW_CLASS(this->srcProfile, Media::ColorProfile(srcProfile));
	NEW_CLASS(this->destProfile, Media::ColorProfile(destProfile));

	this->rgbUpdated = true;
	this->yuvUpdated = true;

	MemCopyNO(&this->yuvParam, colorSess->GetYUVParam(), sizeof(YUVPARAM));
	this->rgbParam->Set(colorSess->GetRGBParam());

	this->nThread = Sync::Thread::GetThreadCnt();
	if (this->nThread > 2)
	{
		this->nThread = 2;
	}
	NEW_CLASS(evtMain, Sync::Event((const UTF8Char*)"Media.CS.CSAYUV444_10_LRGBC.evtMain"));
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		NEW_CLASS(stats[i].evt, Sync::Event((const UTF8Char*)"Media.CS.CSAYUV444_10_LRGBC.stats.evt"));
		stats[i].status = 0;

		currId = i;
		Sync::Thread::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			evtMain->Wait();
		}
	}
}

Media::CS::CSYUV444P10LEP_LRGBC::~CSYUV444P10LEP_LRGBC()
{
	UOSInt i = nThread;
	Bool exited;
	while (i-- > 0)
	{
		if (stats[i].status != 0)
		{
			stats[i].status = 2;
			stats[i].evt->Set();
		}
	}
	while (true)
	{
		exited = true;
		i = nThread;
		while (i-- > 0)
		{
			if (stats[i].status != 0)
			{
				if (stats[i].status == 2)
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else if (stats[i].status > 0)
				{
					stats[i].status = 2;
					stats[i].evt->Set();
					exited = false;
					break;
				}
				else
				{
					stats[i].evt->Set();
					exited = false;
					break;
				}
			}
		}
		if (exited)
			break;

		evtMain->Wait(100);
	}
	i = nThread;
	while (i-- > 0)
	{
		DEL_CLASS(stats[i].evt);
	}
	DEL_CLASS(evtMain);
	MemFree(stats);

	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb);
	DEL_CLASS(this->rgbParam);
	DEL_CLASS(this->srcProfile);
	DEL_CLASS(this->destProfile);
}

void Media::CS::CSYUV444P10LEP_LRGBC::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	
	UInt8 *yPtr = srcPtr[0];
	UInt8 *uPtr = yPtr + (srcStoreWidth * srcStoreHeight << 1);
	UInt8 *vPtr = uPtr + (srcStoreWidth * srcStoreHeight << 1);
	i = this->nThread;
	lastHeight = dispHeight;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, this->nThread);

		stats[i].yPtr = yPtr + (srcStoreWidth * currHeight << 1);
		stats[i].uPtr = uPtr + (srcStoreWidth * currHeight << 1);
		stats[i].vPtr = vPtr + (srcStoreWidth * currHeight << 1);
		stats[i].dest = ((UInt8*)destPtr) + destRGBBpl * (OSInt)currHeight;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;
		stats[i].yBpl = srcStoreWidth << 1;
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	WaitForWorker(3);
}

UOSInt Media::CS::CSYUV444P10LEP_LRGBC::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}

void Media::CS::CSYUV444P10LEP_LRGBC::UpdateTable()
{
	if (this->rgbUpdated)
	{
		this->rgbUpdated = false;
		this->SetupRGB13_LR();
	}
	if (this->yuvUpdated)
	{
		this->yuvUpdated = false;
		this->SetupYUV_RGB13();
	}
}

void Media::CS::CSYUV444P10LEP_LRGBC::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuv)
{
	MemCopyNO(&this->yuvParam, yuv, sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSYUV444P10LEP_LRGBC::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgb)
{
	this->rgbParam->Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSYUV444P10LEP_LRGBC::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
