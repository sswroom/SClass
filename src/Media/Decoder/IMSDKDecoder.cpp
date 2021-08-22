#include "Stdafx.h"
#include "Core/Core.h"
#include "Media/H264Parser.h"
#include "Media/H265Parser.h"
#include "Media/ImageUtil.h"
#include "Media/Decoder/IMSDKDecoder.h"
#include "Media/Decoder/M2VDecoder.h"
#include "Media/Decoder/RAVCDecoder.h"
#include "Media/Decoder/RHVCDecoder.h"
#include "Media/Decoder/VDecoderBase.h"
#include "Media/Decoder/VDecoderChain.h"
#include "Sync/Thread.h"
#include "mfxvideo.h"
#define FOURCC(c1, c2, c3, c4) (((UInt32)c1) | (((UInt32)c2) << 8) | (((UInt32)c3) << 16) | (((UInt32)c4) << 24))

typedef struct
{
	Bool created;
	Bool busy;
	mfxFrameSurface1 surface;
	mfxExtDecodedFrameInfo frameInfo;
	mfxExtBuffer *extBuff[1];
} SurfaceInfo;

class IMSDKDecoder : public Media::Decoder::VDecoderBase
{
private:
	mfxSession session;
	mfxVideoParam par;
	SurfaceInfo *surfaces;
	Media::FrameInfo streamInfo;
	OSInt surfaceCnt;
	Int32 frameTime;
	Bool inited;
	Bool prepared;
	Int32 srcFCC;
	Int32 decFCC;
	Int32 decBPP;
	Int64 decFrameSize;
	Bool endProcessing;
	UInt32 lastFrameTime;
	UInt32 lastFrameNum;
	UInt8 *frameBuff;
	Bool seeked;
	UInt32 seekTime;

	void FreeSurface(SurfaceInfo *surface)
	{
		if (surface->created)
		{
			MemFreeA(surface->surface.Data.Y);
			surface->created = false;
		}
	}

	void AllocSurface(SurfaceInfo *surface)
	{
		if (!surface->created)
		{
			MemCopyNO(&surface->surface.Info, &this->par.mfx.FrameInfo, sizeof(surface->surface.Info));
			surface->surface.reserved[0] = 0;
			surface->surface.reserved[1] = 0;
			surface->surface.reserved[2] = 0;
			surface->surface.reserved[3] = 0;
			MemClear(&surface->surface.Data, sizeof(surface->surface.Data));
			
			switch (this->par.mfx.FrameInfo.FourCC)
			{
			case FOURCC('N', 'V', '1', '2'):
				surface->surface.Data.Y = MemAllocA(UInt8, this->decFrameSize);
				surface->surface.Data.UV = surface->surface.Data.Y + this->par.mfx.FrameInfo.Width * this->par.mfx.FrameInfo.Height;
				surface->surface.Data.Pitch = this->par.mfx.FrameInfo.Width;
				break;
			case FOURCC('P', '0', '1', '0'):
				surface->surface.Data.Y = MemAllocA(UInt8, this->decFrameSize);
				surface->surface.Data.UV = surface->surface.Data.Y + this->par.mfx.FrameInfo.Width * this->par.mfx.FrameInfo.Height * 2;
				surface->surface.Data.Pitch = this->par.mfx.FrameInfo.Width << 1;
				break;
			default:
				surface->surface.Data.Y = MemAllocA(UInt8, this->decFrameSize * 2);
				surface->surface.Data.Pitch = this->par.mfx.FrameInfo.Width * this->par.mfx.FrameInfo.BitDepthLuma >> 3;
				surface->surface.Data.U = 0;
				surface->surface.Data.V = 0;
				break;
			}
			surface->extBuff[0] = &surface->frameInfo.Header;
			surface->frameInfo.Header.BufferId = MFX_EXTBUFF_DECODED_FRAME_INFO;
			surface->frameInfo.Header.BufferSz = sizeof(surface->frameInfo);
			surface->surface.Data.ExtParam = surface->extBuff;
			surface->surface.Data.NumExtParam = 1;
			surface->created = true;
		}
	}

	SurfaceInfo *GetSurface()
	{
		SurfaceInfo *currSurface = 0;
		Bool found = false;
		OSInt i;
		OSInt j;
		while (!found)
		{
			i = 0;
			j = this->surfaceCnt;
			while (i < j)
			{
				currSurface = &this->surfaces[i];
				if (!currSurface->created)
				{
					AllocSurface(currSurface);
					found = true;
					break;
				}
				else if (!currSurface->busy && !currSurface->surface.Data.Locked)
				{
					found = true;
					break;
				}
				i++;
			}
			if (found)
			{
				break;
			}
			Sync::Thread::Sleep(1);
		}
		return currSurface;
	}

	virtual void ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
	{
		SurfaceInfo *currSurface = 0;
		mfxStatus status;
		mfxBitstream bs;
		currSurface = GetSurface();

		mfxSyncPoint syncp;

		if (flags & Media::IVideoSource::FF_DISCONTTIME)
		{
			this->seeked = true;
			this->seekTime = frameTime;
			MFXVideoDECODE_Reset(this->session, &this->par);
		}

		MemClear(&bs, sizeof(bs));
		bs.Data = imgData[0];
		bs.DataOffset = 0;
		bs.DataLength = (mfxU32)dataSize;
		bs.MaxLength = (mfxU32)dataSize;
		bs.TimeStamp = frameTime * 90LL;
		mfxFrameSurface1 *outSurface;

		status = MFX_WRN_DEVICE_BUSY;
		while (true)
		{
			while (status == MFX_WRN_DEVICE_BUSY)
			{
				status = MFXVideoDECODE_DecodeFrameAsync(this->session, &bs, &currSurface->surface, &outSurface, &syncp);
				if (status == MFX_ERR_MORE_SURFACE)
				{
					currSurface = GetSurface();
					status = MFX_WRN_DEVICE_BUSY;
				}
			}
			if (status == MFX_ERR_MORE_DATA)
			{
				return;
			}
			if (status == MFX_ERR_REALLOC_SURFACE)
			{
				status = MFXVideoDECODE_GetVideoParam(this->session, &this->par);
				FreeSurface(currSurface);
				AllocSurface(currSurface);
				return;
			}

			if (status == MFX_WRN_VIDEO_PARAM_CHANGED)
			{
				status = MFXVideoDECODE_GetVideoParam(this->session, &this->par);
				status = MFX_ERR_NONE;
			}
			if (status == MFX_ERR_NONE)
			{
				status = MFXVideoCORE_SyncOperation(session, syncp, -1);
				if (status >= 0)
				{
					UInt32 outFrameTime = (UInt32)(outSurface->Data.TimeStamp / 90);
					UInt32 outFrameNum = frameNum;
					Bool isKey = false;
					FrameStruct outFrameStruct;
					Media::FrameType outFrameType;
					Media::IVideoSource::FrameFlag outFlags;
					Media::YCOffset outYCOfst;
					if (outSurface->Data.ExtParam && outSurface->Data.ExtParam[0]->BufferId == MFX_EXTBUFF_DECODED_FRAME_INFO)
					{
						switch (((mfxExtDecodedFrameInfo*)outSurface->Data.ExtParam[0])->FrameType)
						{
						default:
						case MFX_FRAMETYPE_I:
						case MFX_FRAMETYPE_xI:
							outFrameStruct = Media::IVideoSource::FS_I;
							isKey = true;
							break;
						case MFX_FRAMETYPE_P:
						case MFX_FRAMETYPE_xP:
							outFrameStruct = Media::IVideoSource::FS_P;
							break;
						case MFX_FRAMETYPE_B:
						case MFX_FRAMETYPE_xB:
							outFrameStruct = Media::IVideoSource::FS_B;
							break;
						}
					}
					else
					{
						outFrameStruct = Media::IVideoSource::FS_I;
					}
					if (this->par.mfx.FrameInfo.PicStruct == MFX_PICSTRUCT_UNKNOWN)
					{
						outFrameType = Media::FT_INTERLACED_TFF;
					}
					else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_PROGRESSIVE)
					{
						outFrameType = Media::FT_NON_INTERLACE;
					}
					else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FRAME_DOUBLING)
					{
						if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FIELD_TFF)
						{
							outFrameType = Media::FT_FIELD_TF;
						}
						else
						{
							outFrameType = Media::FT_FIELD_BF;
						}
					}
					else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FIELD_TFF)
					{
						outFrameType = Media::FT_INTERLACED_TFF;
					}
					else
					{
						outFrameType = Media::FT_INTERLACED_BFF;
					}
					outFlags = Media::IVideoSource::FF_NONE;
					outYCOfst = ycOfst;
					Bool skip = false;
					if (this->seeked)
					{
						if (isKey)
						{
							if ((outFrameTime >= this->seekTime) && (outFrameTime < this->seekTime + 1000))
							{
								this->lastFrameTime = outFrameTime;
								outFlags = (Media::IVideoSource::FrameFlag)(outFlags | Media::IVideoSource::FF_DISCONTTIME);
								this->seeked = false;
							}
							else
							{
								skip = true;
							}
						}
						else
						{
							skip = true;
						}
					}
					else
					{
						if (outFrameTime <= this->lastFrameTime && outFrameTime >= this->lastFrameTime - 200)
						{
							outFrameTime = this->lastFrameTime + this->frameTime;
						}
						this->lastFrameTime = outFrameTime;
					}
					if (!skip)
					{
						if (outSurface->Info.FourCC == FOURCC('P', '0', '1', '0'))
						{
							ImageUtil_CopyShiftW(outSurface->Data.Y, this->frameBuff, outSurface->Info.Width * outSurface->Info.Height * 3, 6);
							this->frameCb(outFrameTime, outFrameNum, &this->frameBuff, this->decFrameSize, outFrameStruct, this->frameCbData, outFrameType, outFlags, outYCOfst);
						}
						else
						{
							this->frameCb(outFrameTime, outFrameNum, &outSurface->Data.Y, this->decFrameSize, outFrameStruct, this->frameCbData, outFrameType, outFlags, outYCOfst);
						}
					}
				}
				status = MFX_WRN_DEVICE_BUSY;
			}
			else
			{
				return;
			}
		}
	}

public:
	IMSDKDecoder(Media::IVideoSource *sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
	{
		mfxStatus status;
		this->inited = false;
		this->prepared = false;
		this->started = false;
		this->frameCb = 0;
		this->frameCbData = 0;
		this->streamInfo.dispWidth = 0;
		this->streamInfo.dispHeight = 0;
		this->decFCC = 0;
		this->decBPP = 0;
		this->frameBuff = 0;
		this->frameTime = 33;
		this->endProcessing = false;
		this->lastFrameTime = -1;
		this->lastFrameNum = 0;
		this->seeked = false;
		this->seekTime = 0;

		Media::FrameInfo info;
		Int32 frameRateNorm;
		Int32 frameRateDenorm;
		UOSInt maxFrameSize;
		this->sourceVideo->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
		this->srcFCC = info.fourcc;

		MemClear(&par, sizeof(par));
		switch (this->srcFCC)
		{
		case FOURCC('V', 'P', '9', '0'):
			par.mfx.CodecId = MFX_CODEC_VP9;
			break;
		case FOURCC('H', '2', '6', '4'):
		case FOURCC('h', '2', '6', '4'):
		case FOURCC('X', '2', '6', '4'):
		case FOURCC('x', '2', '6', '4'):
		case FOURCC('A', 'V', 'C', '1'):
		case FOURCC('a', 'v', 'c', '1'):
		case FOURCC('V', 'S', 'S', 'H'):
			par.mfx.CodecId = MFX_CODEC_AVC;
			break;
		case FOURCC('M', 'P', 'G', '2'):
			par.mfx.CodecId = MFX_CODEC_MPEG2;
			break;
		case FOURCC('H', 'E', 'V', 'C'):
		case FOURCC('X', '2', '6', '5'):
		case FOURCC('x', '2', '6', '5'):
		case FOURCC('H', '2', '6', '5'):
		case FOURCC('h', '2', '6', '5'):
			par.mfx.CodecId = MFX_CODEC_HEVC;
			break;
		case FOURCC('d', 'i', 'v', 'f'):
		case FOURCC('d', 'i', 'v', 'x'):
		case FOURCC('D', 'X', '5', '0'):
		case FOURCC('D', 'I', 'V', 'X'):
		case FOURCC('x', 'v', 'i', 'd'):
		case FOURCC('X', 'V', 'I', 'D'):
		case FOURCC('F', 'M', 'P', '4'):

		case FOURCC('M', 'P', '4', '2'):

		default:
			return;
		}


//		status = MFXInit(MFX_IMPL_HARDWARE_ANY, 0, &this->session);
		status = MFXInit(MFX_IMPL_AUTO_ANY, 0, &this->session);
		if (status != MFX_ERR_NONE)
		{
			return;
		}
		this->inited = true;

		this->frameTime = MulDiv32(frameRateDenorm, 1000, frameRateNorm);
		OSInt frameSize = sourceVideo->GetFrameSize(0);
		if (frameSize > 0)
		{
			mfxBitstream bs;
			UInt8 *firstFrame = MemAlloc(UInt8, frameSize);
			frameSize = sourceVideo->ReadFrame(0, firstFrame);

			MemClear(&bs, sizeof(bs));
			bs.Data = firstFrame;
			bs.DataOffset = 0;
			bs.DataLength = (mfxU32)frameSize;
			bs.MaxLength = (mfxU32)frameSize;

			if (par.mfx.CodecId == MFX_CODEC_AVC)
			{
				Media::H264Parser::H264Flags flags;
				Media::H264Parser::GetFrameInfo(firstFrame, frameSize, &this->streamInfo, &flags);
			}
			else if (par.mfx.CodecId == MFX_CODEC_HEVC);
			{
				Media::H265Parser::GetFrameInfoSPS(firstFrame, frameSize, &this->streamInfo);
			}

			this->par.IOPattern = MFX_IOPATTERN_OUT_SYSTEM_MEMORY;
			this->par.mfx.NumThread = Sync::Thread::GetThreadCnt();

			status = MFXVideoDECODE_DecodeHeader(this->session, &bs, &this->par);
			if (status >= 0)
			{
				status = MFXVideoDECODE_Init(this->session, &this->par);
				if (status >= 0)
				{
					this->decFCC = this->par.mfx.FrameInfo.FourCC;
					switch (this->decFCC)
					{
					case FOURCC('N', 'V', '1', '2'):
						this->decBPP = 12;
						this->decFrameSize = (this->par.mfx.FrameInfo.Width >> 1) * this->par.mfx.FrameInfo.Height * 3;
						break;
					case FOURCC('P', '0', '1', '0'):
						this->decBPP = 24;
						this->decFrameSize = this->par.mfx.FrameInfo.Width * this->par.mfx.FrameInfo.Height * 3;
						this->frameBuff = MemAllocA(UInt8, this->decFrameSize);
						break;
					default:
						this->decBPP = (Int32)(this->par.mfx.FrameInfo.BufferSize * 8 / (this->par.mfx.FrameInfo.Width * this->par.mfx.FrameInfo.Height));
						this->decFrameSize = this->par.mfx.FrameInfo.BufferSize;
						break;
					}

					mfxFrameAllocRequest req;
					status = MFXVideoDECODE_QueryIOSurf(this->session, &this->par, &req);
					if (status >= 0)
					{
						this->surfaceCnt = req.NumFrameSuggested;
					}
					else
					{
						this->surfaceCnt = 8;
					}
					this->surfaces = MemAlloc(SurfaceInfo, this->surfaceCnt);
					MemClear(this->surfaces, sizeof(SurfaceInfo) * this->surfaceCnt);

					this->prepared = true;

				}
			}
			MemFree(firstFrame);
		}
	}

	virtual ~IMSDKDecoder()
	{
		if (this->prepared)
		{
			MFXVideoDECODE_Close(this->session);
			OSInt i = this->surfaceCnt;
			while (i-- > 0)
			{
				if (this->surfaces[i].created)
				{
					FreeSurface(&this->surfaces[i]);
				}
			}
			MemFree(this->surfaces);
			this->prepared = false;
		}
		if (this->inited)
		{
			MFXClose(this->session);
		}
		if (this->frameBuff)
		{
			MemFreeA(this->frameBuff);
			this->frameBuff = 0;
		}
	}

	virtual const UTF8Char *GetFilterName()
	{
		return (const UTF8Char*)"IMSDKDecoder";
	}

	virtual Bool CaptureImage(ImageCallback imgCb, void *userData)
	{
		return false;
	}

	virtual Bool GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize)
	{
		if (this->sourceVideo == 0)
			return false;
		if (this->streamInfo.dispWidth != 0 && this->streamInfo.dispHeight != 0)
		{
			info->Set(&this->streamInfo);
		}
		else
		{
			if (!this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize))
				return false;
		}
		info->storeWidth = this->par.mfx.FrameInfo.Width;
		info->storeHeight = this->par.mfx.FrameInfo.Height;
		info->dispWidth = this->par.mfx.FrameInfo.CropW;
		info->dispHeight = this->par.mfx.FrameInfo.CropH;
		info->fourcc = this->decFCC;
		info->storeBPP = this->decBPP;
		info->pf = Media::PixelFormatGetDef(this->decFCC, this->decBPP);
		info->byteSize = this->decFrameSize;
		*maxFrameSize = this->decFrameSize;
		return true;
	}

	virtual void Stop()
	{
		if (this->sourceVideo == 0)
			return;

		this->started = false;
		this->sourceVideo->Stop();
		while (this->endProcessing)
		{
			Sync::Thread::Sleep(10);
		}
		this->frameCb = 0;
		this->frameCbData = 0;
		this->lastFrameNum = 0;
		this->lastFrameTime = -1;
	}

	virtual OSInt GetFrameCount()
	{
		return this->sourceVideo->GetFrameCount();
	}

	virtual UInt32 GetFrameTime(UOSInt frameIndex)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}

	virtual void EnumFrameInfos(FrameInfoCallback cb, void *userData)
	{
		return this->sourceVideo->EnumFrameInfos(cb, userData);
	}

	virtual void OnFrameChanged(Media::IVideoSource::FrameChange fc)
	{
		if (fc == Media::IVideoSource::FC_PAR)
		{
//			this->frameChg = true;
		}
		else if (fc == Media::IVideoSource::FC_ENDPLAY)
		{
			mfxStatus status;
			SurfaceInfo *currSurface;
			mfxSyncPoint syncp;
			mfxFrameSurface1 *outSurface;
			this->endProcessing = true;
			currSurface = GetSurface();
			status = MFX_WRN_DEVICE_BUSY;
			while (true)
			{
				while (status == MFX_WRN_DEVICE_BUSY)
				{
					status = MFXVideoDECODE_DecodeFrameAsync(this->session, 0, &currSurface->surface, &outSurface, &syncp);
					if (status == MFX_ERR_MORE_SURFACE)
					{
						currSurface = GetSurface();
						status = MFX_WRN_DEVICE_BUSY;
					}
				}
				if (status == MFX_ERR_MORE_DATA)
				{
					break;
				}
				if (status == MFX_ERR_REALLOC_SURFACE)
				{
					status = MFXVideoDECODE_GetVideoParam(this->session, &this->par);
					FreeSurface(currSurface);
					AllocSurface(currSurface);
					break;
				}

				if (status == MFX_WRN_VIDEO_PARAM_CHANGED)
				{
					status = MFXVideoDECODE_GetVideoParam(this->session, &this->par);
					status = MFX_ERR_NONE;
				}
				if (status == MFX_ERR_NONE)
				{
					status = MFXVideoCORE_SyncOperation(session, syncp, -1);
					if (status >= 0)
					{
						UInt32 outFrameTime = (UInt32)(outSurface->Data.TimeStamp / 90);
						UInt32 outFrameNum = this->lastFrameNum++;
						FrameStruct outFrameStruct;
						Media::FrameType outFrameType;
						Media::IVideoSource::FrameFlag outFlags;
						Media::YCOffset outYCOfst;
						if (outSurface->Data.ExtParam && outSurface->Data.ExtParam[0]->BufferId == MFX_EXTBUFF_DECODED_FRAME_INFO)
						{
							switch (((mfxExtDecodedFrameInfo*)outSurface->Data.ExtParam[0])->FrameType)
							{
							default:
							case MFX_FRAMETYPE_I:
							case MFX_FRAMETYPE_xI:
								outFrameStruct = Media::IVideoSource::FS_I;
								break;
							case MFX_FRAMETYPE_P:
							case MFX_FRAMETYPE_xP:
								outFrameStruct = Media::IVideoSource::FS_P;
								break;
							case MFX_FRAMETYPE_B:
							case MFX_FRAMETYPE_xB:
								outFrameStruct = Media::IVideoSource::FS_B;
								break;
							}
						}
						else
						{
							outFrameStruct = Media::IVideoSource::FS_I;
						}
						if (this->par.mfx.FrameInfo.PicStruct == MFX_PICSTRUCT_UNKNOWN)
						{
							outFrameType = Media::FT_INTERLACED_TFF;
						}
						else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_PROGRESSIVE)
						{
							outFrameType = Media::FT_NON_INTERLACE;
						}
						else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FRAME_DOUBLING)
						{
							if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FIELD_TFF)
							{
								outFrameType = Media::FT_FIELD_TF;
							}
							else
							{
								outFrameType = Media::FT_FIELD_BF;
							}
						}
						else if (this->par.mfx.FrameInfo.PicStruct & MFX_PICSTRUCT_FIELD_TFF)
						{
							outFrameType = Media::FT_INTERLACED_TFF;
						}
						else
						{
							outFrameType = Media::FT_INTERLACED_BFF;
						}
						outFlags = Media::IVideoSource::FF_NONE;
						outYCOfst = Media::YCOFST_C_CENTER_LEFT;
						this->frameCb(outFrameTime, outFrameNum, &outSurface->Data.Y, this->decFrameSize, outFrameStruct, this->frameCbData, outFrameType, outFlags, outYCOfst);
					}
					status = MFX_WRN_DEVICE_BUSY;
				}
				else
				{
					break;
				}
			}
			this->endProcessing = false;
		}
	}
	
	Bool IsError()
	{
		return !this->prepared;
	}
};

Media::IVideoSource *__stdcall IMSDKDecoder_DecodeVideo(Media::IVideoSource *video)
{
	IMSDKDecoder *decoder;
	Media::FrameInfo frameInfo;
	Media::FrameInfo decFrameInfo;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	UOSInt maxFrameSize;
	video->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == -1)
		return 0;

	if (frameInfo.fourcc == *(Int32*)"ravc")
	{
		Media::Decoder::RAVCDecoder *ravc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(ravc, Media::Decoder::RAVCDecoder(video, false, false));
		NEW_CLASS(decoder, IMSDKDecoder(ravc));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			DEL_CLASS(ravc);
			return 0;
		}
		NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
		decChain->AddDecoder(ravc);
		return decChain;
	}
	else if (frameInfo.fourcc == *(Int32*)"rhvc")
	{
		Media::Decoder::RHVCDecoder *rhvc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(rhvc, Media::Decoder::RHVCDecoder(video, false));
		NEW_CLASS(decoder, IMSDKDecoder(rhvc));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			DEL_CLASS(rhvc);
			return 0;
		}
		NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
		decChain->AddDecoder(rhvc);
		return decChain;
	}
	else if (frameInfo.fourcc == *(Int32*)"m2v1")
	{
		Media::Decoder::M2VDecoder *m2vd;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(m2vd, Media::Decoder::M2VDecoder(video, false));
		NEW_CLASS(decoder, IMSDKDecoder(m2vd));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			DEL_CLASS(m2vd);
			return 0;
		}
		NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
		decChain->AddDecoder(m2vd);
		return decChain;
	}
	else //if (frameInfo.fourcc == *(Int32*)"VP90" || frameInfo.fourcc == *(Int32*)"MPG2")
	{
		NEW_CLASS(decoder, IMSDKDecoder(video));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			return 0;
		}
		return decoder;
	}

	return 0;
}


void Media::Decoder::IMSDKDecoder::Enable()
{
	Core::CoreAddVideoDecFunc(IMSDKDecoder_DecodeVideo);
}

extern "C" OSInt __imp_SetThreadErrorMode(UInt32 dwNewMode, UInt32 *lpOldMode)
{
	return 1;
}