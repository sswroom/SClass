#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/RGBLUTGen.h"
#include "Media/CS/CSAYUV444_10_RGB32C.h"
#include "Sync/ThreadUtil.h"

extern "C"
{
	void CSAYUV444_10_RGB32C_convert(UInt8 *yPtr, UInt8 *dest, UOSInt width, UOSInt height, OSInt dbpl, UOSInt yBpl, Int64 *yuv2rgb, Int64 *rgbGammaCorr);
}

void Media::CS::CSAYUV444_10_RGB32C::SetupRGB13_LR()
{
	Int32 i;
	Double thisV;
	UInt16 v[4];

	NotNullPtr<Media::ColorProfile> srcProfile;
	if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_VDISPLAY)
	{
		srcProfile = this->colorSess->GetDefVProfile();
	}
	else if (this->srcProfile.GetRTranParam()->GetTranType() == Media::CS::TRANT_PDISPLAY)
	{
		srcProfile = this->colorSess->GetDefPProfile();
	}
	else
	{
		srcProfile = this->srcProfile;
	}

	NotNullPtr<Media::CS::TransferFunc> rtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetRTranParam());
	NotNullPtr<Media::CS::TransferFunc> gtFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetGTranParam());
	NotNullPtr<Media::CS::TransferFunc> btFunc = Media::CS::TransferFunc::CreateFunc(srcProfile->GetBTranParam());
	Math::Matrix3 mat1;
	if (this->destProfile.GetPrimaries()->colorType == Media::ColorProfile::CT_DISPLAY)
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->rgbParam.monProfile.GetPrimaries());
	}
	else
	{
		Media::ColorProfile::GetConvMatrix(mat1, this->srcProfile.GetPrimaries(), this->destProfile.GetPrimaries());
	}

	i = 32768;
	while (i--)
	{
		thisV = rtFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer(i / 32767.0 * 4.0);
		if (thisV > 2.0)
		{
			thisV = 2.0;
		}
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
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[0]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[0]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[0]);
		v[3] = 0;
		this->rgbGammaCorr[i] = *(Int64*)&v[0];

		thisV = gtFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[1]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[1]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[1]);
		v[3] = 0;
		this->rgbGammaCorr[i + 65536] = *(Int64*)&v[0];

		thisV = btFunc->InverseTransfer((i - 65536) / 32767.0 * 4.0);
		if (thisV < -2.0)
		{
			thisV = -2.0;
		}
		v[2] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[0].val[2]);
		v[1] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[1].val[2]);
		v[0] = (UInt16)Math::SDouble2Int16(thisV * 16383.0 * mat1.vec[2].val[2]);
		v[3] = 0;
		this->rgbGammaCorr[i + 131072] = *(Int64*)&v[0];
	}
	btFunc.Delete();
	gtFunc.Delete();
	rtFunc.Delete();

	Media::RGBLUTGen lutGen(this->colorSess);
	if (this->destPF == Media::PF_LE_A2B10G10R10)
	{
		lutGen.GenLARGB_A2B10G10R10((UInt8*)&this->rgbGammaCorr[196608], this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcProfile.rtransfer));
	}
	else
	{
		lutGen.GenLARGB_B8G8R8A8((UInt8*)&this->rgbGammaCorr[196608], this->destProfile, 14, Media::CS::TransferFunc::GetRefLuminance(this->srcProfile.rtransfer));
	}
}

void Media::CS::CSAYUV444_10_RGB32C::SetupYUV_RGB13()
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
			y = Double2Int32(0x7fff * (Math_Pow(i / 1023.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
			y = Double2Int32(0x7fff * (Math_Pow((i - 64) / 876.0, this->yuvParam.YGamma) * this->yuvParam.Contrast + this->yuvParam.Brightness - 1) / 4.0);
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
		v2r8 = Double2Int32((Kc1 * c * this->yuvParam.Saturation * this->yuvParam.RMul + this->yuvParam.RAdd) / 4.0);
		u2g8 = Double2Int32((Kc3 * c * this->yuvParam.Saturation * this->yuvParam.GMul + this->yuvParam.GAdd) / 4.0);
		v2g8 = Double2Int32((Kc2 * c * this->yuvParam.Saturation * this->yuvParam.GMul) / 4.0);
		u2b8 = Double2Int32((Kc4 * c * this->yuvParam.Saturation * this->yuvParam.BMul + this->yuvParam.BAdd) / 4.0);
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

UInt32 Media::CS::CSAYUV444_10_RGB32C::WorkerThread(AnyType obj)
{
	NotNullPtr<CSAYUV444_10_RGB32C> converter = obj.GetNN<CSAYUV444_10_RGB32C>();
	UOSInt threadId = converter->currId;
	THREADSTAT *ts = &converter->stats[threadId];
	{
		Sync::Event evt;
		ts->evt = &evt;
		ts->status = 1;
		converter->evtMain.Set();
		while (true)
		{
			ts->evt->Wait();
			if (ts->status == 2)
			{
				break;
			}
			else if (ts->status == 3)
			{
				CSAYUV444_10_RGB32C_convert(ts->yPtr, ts->dest, ts->width, ts->height, ts->dbpl, ts->yBpl, converter->yuv2rgb, converter->rgbGammaCorr);
				ts->status = 4;
				converter->evtMain.Set();
			}
		}
	}
	converter->stats[threadId].status = 0;
	converter->evtMain.Set();
	return 0;
}

void Media::CS::CSAYUV444_10_RGB32C::WaitForWorker(Int32 jobStatus)
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
		this->evtMain.Wait();
	}
}

Media::CS::CSAYUV444_10_RGB32C::CSAYUV444_10_RGB32C(NotNullPtr<const Media::ColorProfile> srcProfile, NotNullPtr<const Media::ColorProfile> destProfile, Media::ColorProfile::YUVType yuvType, Media::ColorManagerSess *colorSess, Media::PixelFormat destPF) : Media::CS::CSConverter(colorSess), srcProfile(srcProfile), destProfile(destProfile)
{
	UOSInt i;
	this->yuvType = yuvType;
	this->destPF = destPF;
	this->rgbGammaCorr = MemAlloc(Int64, 65536 * 3 + 65536 * 2);
	this->yuv2rgb = MemAlloc(Int64, 65536 * 3);

	this->rgbUpdated = true;
	this->yuvUpdated = true;

	MemCopyNO(&this->yuvParam, colorSess->GetYUVParam().Ptr(), sizeof(YUVPARAM));
	this->rgbParam.Set(colorSess->GetRGBParam());

	this->nThread = Sync::ThreadUtil::GetThreadCnt();
	if (this->nThread > 2)
	{
		this->nThread = 2;
	}
	stats = MemAlloc(THREADSTAT, nThread);
	i = nThread;
	while(i-- > 0)
	{
		stats[i].status = 0;

		currId = i;
		Sync::ThreadUtil::Create(WorkerThread, this, 65536);
		while (stats[i].status == 0)
		{
			this->evtMain.Wait();
		}
	}
}

Media::CS::CSAYUV444_10_RGB32C::~CSAYUV444_10_RGB32C()
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

		this->evtMain.Wait(100);
	}
	MemFree(stats);

	MemFree(this->rgbGammaCorr);
	MemFree(this->yuv2rgb);
}

void Media::CS::CSAYUV444_10_RGB32C::ConvertV2(UInt8 *const*srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	this->UpdateTable();
	UOSInt i = this->nThread;
	UOSInt lastHeight = dispHeight;
	UOSInt currHeight;
	

	i = this->nThread;
	lastHeight = dispHeight;
	while (i-- > 0)
	{
		currHeight = MulDivUOS(i, dispHeight, this->nThread);

		stats[i].yPtr = srcPtr[0] + (srcStoreWidth * currHeight << 3);
		stats[i].yBpl = srcStoreWidth << 3;
		stats[i].dest = ((UInt8*)destPtr) + destRGBBpl * (OSInt)currHeight;
		stats[i].width = dispWidth;
		stats[i].height = lastHeight - currHeight;
		stats[i].dbpl = destRGBBpl;
		stats[i].status = 3;
		stats[i].evt->Set();
		lastHeight = currHeight;
	}
	WaitForWorker(3);
}

UOSInt Media::CS::CSAYUV444_10_RGB32C::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}

void Media::CS::CSAYUV444_10_RGB32C::UpdateTable()
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

void Media::CS::CSAYUV444_10_RGB32C::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuv)
{
	MemCopyNO(&this->yuvParam, yuv.Ptr(), sizeof(YUVPARAM));
	this->yuvUpdated = true;
}

void Media::CS::CSAYUV444_10_RGB32C::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgb)
{
	this->rgbParam.Set(rgb);
	this->rgbUpdated = true;
}

UOSInt Media::CS::CSAYUV444_10_RGB32C::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height << 3;
}
