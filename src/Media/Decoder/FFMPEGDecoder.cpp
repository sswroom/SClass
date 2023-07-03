#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/Library.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Media/Decoder/ADecoderBase.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Media/Decoder/M2VDecoder.h"
#include "Media/Decoder/RAVCDecoder.h"
#include "Media/Decoder/RHVCDecoder.h"
#include "Media/Decoder/VDecoderBase.h"
#include "Media/Decoder/VDecoderChain.h"
#include "Media/Decoder/VP09Decoder.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

#include "Text/StringBuilderUTF8.h"
#ifdef _DEBUG
#include "IO/FileStream.h"
#include "IO/StreamWriter.h"
#endif
#define FRAMEBUFFSIZE 64
#define FOURCC(c1, c2, c3, c4) (((UInt32)c1) | (((UInt32)c2) << 8) | (((UInt32)c3) << 16) | (((UInt32)c4) << 24))

#ifndef UINT64_C
#define UINT64_C(c) (c ## ULL)
#endif

#define VERSION_FROM(major, minor, micro) ((LIBAVCODEC_VERSION_MAJOR > major) || ((LIBAVCODEC_VERSION_MAJOR == major) && (LIBAVCODEC_VERSION_MINOR > minor)) || ((LIBAVCODEC_VERSION_MAJOR == major) && (LIBAVCODEC_VERSION_MINOR == minor) && (LIBAVCODEC_VERSION_MICRO == micro)))
#define UTIL_VERSION_FROM(major, minor, micro) ((LIBAVUTIL_VERSION_MAJOR > major) || ((LIBAVUTIL_VERSION_MAJOR == major) && (LIBAVUTIL_VERSION_MINOR > minor)) || ((LIBAVUTIL_VERSION_MAJOR == major) && (LIBAVUTIL_VERSION_MINOR == minor) && (LIBAVUTIL_VERSION_MICRO == micro)))

#if !defined(_MSC_VER) || _MSC_VER >= 1400
extern "C" 
{
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
}

#if !defined(__MINGW32__) && defined(__GNUC__)
#if VERSION_FROM(55, 0, 0) //not sure
#define FFMPEGDecoder_av_frame_alloc av_frame_alloc
#define FFMPEGDecoder_av_frame_free av_frame_free
#else
#define FFMPEGDecoder_av_frame_alloc avcodec_alloc_frame
#define FFMPEGDecoder_av_frame_free avcodec_free_frame
#endif
#if VERSION_FROM(58, 133, 100)
static void FFMPEGDecoder_av_init_packet(AVPacket *pkt)
{
    memset(pkt, 0, sizeof(*pkt));

    pkt->pts             = AV_NOPTS_VALUE;
    pkt->dts             = AV_NOPTS_VALUE;
    pkt->pos             = -1;
}
#else
#define FFMPEGDecoder_av_init_packet av_init_packet
#endif
#if VERSION_FROM(58, 10, 100)
void FFMPEGDecoder_avcodec_register_all()
{
	
}
#else
#define FFMPEGDecoder_avcodec_register_all avcodec_register_all
#endif
#define FFMPEGDecoder_avcodec_send_packet avcodec_send_packet
#define FFMPEGDecoder_avcodec_receive_frame avcodec_receive_frame
#define FFMPEGDecoder_avcodec_find_decoder avcodec_find_decoder
#define FFMPEGDecoder_avcodec_alloc_context3 avcodec_alloc_context3
#if VERSION_FROM(55, 0, 0) //not sure
#define FFMPEGDecoder_avcodec_free_context avcodec_free_context
#else
#define FFMPEGDecoder_avcodec_free_context av_free
#endif
#define FFMPEGDecoder_avcodec_open2 avcodec_open2
#define FFMPEGDecoder_avcodec_close avcodec_close
#define FFMPEGDecoder_av_packet_unref av_packet_unref
#if VERSION_FROM(55, 0, 0) // not sure
static int FFMPEGDecoder_avcodec_decode_audio4(AVCodecContext *avctx, AVFrame *frame, int *got_frame_ptr, const AVPacket *avpkt)
{
	int ret = avcodec_receive_frame(avctx, frame);
	if (ret == 0)
		*got_frame_ptr = true;
	if (ret == AVERROR(EAGAIN))
		ret = 0;
	if (ret == 0)
		ret = avcodec_send_packet(avctx, avpkt);
	if (ret == AVERROR(EAGAIN))
		ret = 0;
	else if (ret < 0)
	{
		ret = 0;
//        Debug(3, "codec/audio: audio decode error: %1 (%2)\n",av_make_error_string(error, sizeof(error), ret),got_frame);
	}
	else
		ret = avpkt->size;
	return ret;
}
#else
#define FFMPEGDecoder_avcodec_decode_audio4 avcodec_decode_audio4
#endif
#define FFMPEGDecoder_avcodec_decode_video2 avcodec_decode_video2
#else
IO::Library *FFMPEGDecoder_lib1 = 0;
IO::Library *FFMPEGDecoder_lib2 = 0;
AVFrame *(__stdcall *FFMPEGDecoder_av_frame_alloc)() = 0;
void (__stdcall *FFMPEGDecoder_av_frame_free)(AVFrame **frame) = 0;

void (__stdcall *FFMPEGDecoder_av_init_packet)(AVPacket *pkt) = 0;
void (__stdcall *FFMPEGDecoder_avcodec_register_all)() = 0;
int (__stdcall *FFMPEGDecoder_avcodec_send_packet)(AVCodecContext *avctx, const AVPacket *avpkt) = 0;
int (__stdcall *FFMPEGDecoder_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame) = 0;
AVCodec *(__stdcall *FFMPEGDecoder_avcodec_find_decoder)(enum AVCodecID id) = 0;
AVCodecContext *(__stdcall *FFMPEGDecoder_avcodec_alloc_context3)(const AVCodec *codec) = 0;
void (__stdcall *FFMPEGDecoder_avcodec_free_context)(AVCodecContext **avctx) = 0;
int (__stdcall *FFMPEGDecoder_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options) = 0;
int (__stdcall *FFMPEGDecoder_avcodec_close)(AVCodecContext *avctx) = 0;
void (__stdcall *FFMPEGDecoder_av_packet_unref)(AVPacket *pkt) = 0;
int (__stdcall *FFMPEGDecoder_avcodec_decode_audio4)(AVCodecContext *avctx, AVFrame *frame, int *got_frame_ptr, const AVPacket *avpkt) = 0;
#endif

typedef struct
{
	UInt32 frameTime;
	Bool isKey;
	Bool discTime;
	Media::FrameType frameType;
	UInt32 frameNum;
	Media::IVideoSource::FrameStruct frameStruct;
} FFMPEGFrameInfo;

struct Media::Decoder::FFMPEGDecoder::ClassData
{
	Bool inited;
	const AVCodec *codec;
	AVCodecContext *ctx;
	AVFrame *frame;
	UInt32 dispWidth;
	UInt32 dispHeight;
	UInt32 storeWidth;
	UInt32 storeHeight;
	Double par;
	UInt32 currFmt;
	UInt32 srcFCC;
	UInt8 *frameBuff;
	UOSInt frameSize;
	Bool seeked;
	UInt32 seekTime;
	AVColorPrimaries colorPrimaries;
	AVColorTransferCharacteristic colorTrc;
	AVColorSpace yuvColor;
	FFMPEGFrameInfo frames[FRAMEBUFFSIZE];
	UOSInt frameIndexS;
	UOSInt frameIndexE;
	UInt32 lastFrameTime;
	Bool isOpenGOP;
#ifdef _DEBUG
	IO::Stream *dbgStm;
	IO::Writer *dbgWriter;
#endif
};

void Media::Decoder::FFMPEGDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	ClassData *data = this->clsData;
    AVPacket avpkt;
	Int32 ret;
#ifdef _DEBUG
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char fType = '?';
		switch (frameStruct)
		{
		case Media::IVideoSource::FS_I:
			fType = 'I';
			break;
		case Media::IVideoSource::FS_P:
			fType = 'P';
			break;
		case Media::IVideoSource::FS_B:
			fType = 'B';
			break;
		}
		sb.AppendU32(frameNum);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendU32(frameTime);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendChar(fType, 1);
		sb.AppendC(UTF8STRC("\t"));
		sb.AppendOSInt(dataSize);
		data->dbgWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
#endif
	if (frameStruct != Media::IVideoSource::FS_N)
	{
		data->frames[data->frameIndexE].frameTime = frameTime;
		data->frames[data->frameIndexE].isKey = (frameStruct == Media::IVideoSource::FS_I);
		data->frames[data->frameIndexE].discTime = (flags & Media::IVideoSource::FF_DISCONTTIME) != 0;
		data->frames[data->frameIndexE].frameType = frameType;
		data->frames[data->frameIndexE].frameNum = frameNum;
		data->frames[data->frameIndexE].frameStruct = frameStruct;
		data->frameIndexE++;
		if (data->frameIndexE >= FRAMEBUFFSIZE)
		{
			data->frameIndexE = 0;
		}
		if (data->frameIndexE == data->frameIndexS)
		{
			data->frameIndexS++;
			if (data->frameIndexS >= FRAMEBUFFSIZE)
			{
				data->frameIndexS = 0;
			}
		}
	}

	FFMPEGDecoder_av_init_packet(&avpkt);
	avpkt.data = imgData[0];
	avpkt.size = (int)dataSize;
	avpkt.dts = frameTime * 90LL;
	avpkt.pts = frameTime * 90LL;
	if (flags & Media::IVideoSource::FF_DISCONTTIME)
	{
		data->seeked = true;
		data->seekTime = frameTime;
	}

#if VERSION_FROM(57, 41, 0)
	ret = FFMPEGDecoder_avcodec_send_packet(data->ctx, &avpkt);
	if (ret < 0)
	{
		return;
	}
	while ((ret = FFMPEGDecoder_avcodec_receive_frame(data->ctx, data->frame)) == 0)
#else
	int check = 0;
	while ((ret = FFMPEGDecoder_avcodec_decode_video2(data->ctx, data->frame, &check, &avpkt)) == 0)
#endif
	{
		UInt32 outFrameTime = (UInt32)(data->frame->pts / 90);
		UInt32 outFrameNum = frameNum;
		Media::IVideoSource::FrameStruct outFrameStruct;
		Media::FrameType outFrameType;
		Media::IVideoSource::FrameFlag outFlags;
		Media::YCOffset outYCOfst = ycOfst;
		switch (data->frame->pict_type)
		{
		case AV_PICTURE_TYPE_I:
		case AV_PICTURE_TYPE_SI:
		case AV_PICTURE_TYPE_S:
#if VERSION_FROM(55, 0, 0) //not sure
		case AV_PICTURE_TYPE_NONE:
#endif
		default:
			outFrameStruct = Media::IVideoSource::FS_I;
			break;
		case AV_PICTURE_TYPE_P:
		case AV_PICTURE_TYPE_SP:
			outFrameStruct = Media::IVideoSource::FS_P;
			break;
		case AV_PICTURE_TYPE_B:
		case AV_PICTURE_TYPE_BI:
			outFrameStruct = Media::IVideoSource::FS_B;
			break;
		}
		if (data->frame->interlaced_frame)
		{
			if (data->frame->top_field_first)
			{
				outFrameType = Media::FT_INTERLACED_TFF;
			}
			else
			{
				outFrameType = Media::FT_INTERLACED_BFF;
			}
		}
		else
		{
			outFrameType = Media::FT_NON_INTERLACE;
		}
		outFlags = (Media::IVideoSource::FrameFlag)(Media::IVideoSource::FF_BFRAMEPROC | (flags & Media::IVideoSource::FF_REALTIME));
		UInt8 **outFrameBuff = &data->frameBuff;

		UInt8 *frameTempBuff = 0;
		switch (data->frame->format)
		{
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_YUVJ420P:
			outFrameBuff = data->frame->data;
			break;
		case AV_PIX_FMT_YUVJ422P:
			{
				UInt8 *yptr = data->frame->data[0];
				UInt8 *uptr = data->frame->data[1];
				UInt8 *vptr = data->frame->data[2];
				OSInt ybpl = data->frame->linesize[0];
				OSInt ubpl = data->frame->linesize[1];
				OSInt vbpl = data->frame->linesize[2];
				OSInt w = data->frame->width;
				OSInt h = data->frame->height;
				UInt8 *dptr;
				OSInt i;
				frameTempBuff = MemAllocA(UInt8, (UOSInt)(w * h * 2));
				dptr = frameTempBuff;
				w = w >> 1;
				while (h-- > 0)
				{
					i = 0;
					while (i < w)
					{
						dptr[0] = yptr[i * 2];
						dptr[1] = uptr[i];
						dptr[2] = yptr[i * 2 + 1];
						dptr[3] = vptr[i];
						dptr += 4;
						i++;
					}
					yptr += ybpl;
					uptr += ubpl;
					vptr += vbpl;					
				}
				outFrameBuff = &frameTempBuff;
			}
			break;
		case AV_PIX_FMT_YUV420P10LE:
			outFrameBuff = data->frame->data;
			break;
#if VERSION_FROM(57, 27, 2) //not sure
		case AV_PIX_FMT_YUV420P12LE:
			outFrameBuff = data->frame->data;
			break;
#endif
		case AV_PIX_FMT_YUV444P10LE:
			outFrameBuff = data->frame->data;
			break;
		default:
			break;
		}
		Bool skip = false;
		if (data->seeked)
		{
			if (data->frame->key_frame && data->seekTime <= outFrameTime && (data->seekTime + 50) >= outFrameTime)
			{
				outFlags = (Media::IVideoSource::FrameFlag)(outFlags | Media::IVideoSource::FF_DISCONTTIME);
				data->seeked = false;
			}
			else
			{
				skip = true;
			}
		}
		if (data->frame->width != (int)data->dispWidth || data->frame->height != (int)data->dispHeight)
		{
			skip = true;
		}
		if (!skip)
		{
			if (data->frame->key_frame || outFrameStruct == Media::IVideoSource::FS_I)
			{
				while (data->frameIndexS != data->frameIndexE)
				{
					if (data->frames[data->frameIndexS].isKey || data->isOpenGOP)
					{
						if (data->frames[data->frameIndexS].frameTime == outFrameTime)
						{
							skip = true;
						}
						else
						{
							skip = true;
						}
						data->isOpenGOP = false;

						Media::FrameType srcFrameType = data->frames[data->frameIndexS].frameType;
						outFrameNum = data->frames[data->frameIndexS].frameNum;
						
						data->frameIndexS++;
						if (data->frameIndexS == FRAMEBUFFSIZE)
						{
							data->frameIndexS = 0;
						}

						if (data->frameIndexS != data->frameIndexE && srcFrameType == Media::FT_MERGED_TF && data->frames[data->frameIndexS].frameType == Media::FT_MERGED_BF && data->frame->interlaced_frame)
						{
							data->frameIndexS++;
							if (data->frameIndexS == FRAMEBUFFSIZE)
							{
								data->frameIndexS = 0;
							}
						}
						else if (data->frameIndexS != data->frameIndexE && srcFrameType == Media::FT_MERGED_BF && data->frames[data->frameIndexS].frameType == Media::FT_MERGED_TF && data->frame->interlaced_frame)
						{
							data->frameIndexS++;
							if (data->frameIndexS == FRAMEBUFFSIZE)
							{
								data->frameIndexS = 0;
							}
						}
						else
						{
							if (srcFrameType == Media::FT_MERGED_TF)
							{
								srcFrameType = Media::FT_MERGED_TF;
							}
						}
					}
					else
					{
						skip = false;

						data->frameIndexS++;
						if (data->frameIndexS == FRAMEBUFFSIZE)
						{
							data->frameIndexS = 0;
						}
					}

					if (skip)
					{
						break;
					}
				}
			}
			else
			{
				if (data->frameIndexS != data->frameIndexE)
				{
					if (data->frames[data->frameIndexS].isKey && outFrameTime != data->frames[data->frameIndexS].frameTime && outFrameStruct != Media::IVideoSource::FS_B)
					{
						outFrameTime = data->lastFrameTime;
						skip = true;
					}
					else
					{
						if (data->frames[data->frameIndexS].isKey && outFrameStruct == Media::IVideoSource::FS_B)
						{
							data->isOpenGOP = true;
						}

						outFrameTime = data->frames[data->frameIndexS].frameTime;
						outFrameNum = data->frames[data->frameIndexS].frameNum;
						Media::FrameType srcFrameType = data->frames[data->frameIndexS].frameType;

						data->frameIndexS++;
						if (data->frameIndexS == FRAMEBUFFSIZE)
						{
							data->frameIndexS = 0;
						}

						if (data->frameIndexS != data->frameIndexE && srcFrameType == Media::FT_MERGED_TF && data->frames[data->frameIndexS].frameType == Media::FT_MERGED_BF && data->frame->interlaced_frame)
						{
							data->frameIndexS++;
							if (data->frameIndexS == FRAMEBUFFSIZE)
							{
								data->frameIndexS = 0;
							}
						}
						else if (data->frameIndexS != data->frameIndexE && srcFrameType == Media::FT_MERGED_BF && data->frames[data->frameIndexS].frameType == Media::FT_MERGED_TF && data->frame->interlaced_frame)
						{
							data->frameIndexS++;
							if (data->frameIndexS == FRAMEBUFFSIZE)
							{
								data->frameIndexS = 0;
							}
						}
						else
						{
							if (srcFrameType == Media::FT_MERGED_TF)
							{
								srcFrameType = Media::FT_MERGED_TF;
							}
						}
					}
				}
			}
			data->lastFrameTime = outFrameTime;
			this->frameCb(outFrameTime, outFrameNum, outFrameBuff, data->frameSize, outFrameStruct, this->frameCbData, outFrameType, outFlags, outYCOfst);
		}
		if (frameTempBuff)
		{
			MemFreeA(frameTempBuff);
		}
	}

}

Media::Decoder::FFMPEGDecoder::FFMPEGDecoder(IVideoSource *sourceVideo) : Media::Decoder::VDecoderBase(sourceVideo)
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	this->endProcessing = false;
	this->lastFrameTime = (UInt32)-1;
	data->inited = false;
	data->ctx = 0;
	data->frame = 0;
	data->dispWidth = 0;

	data->dispHeight = 0;
	data->storeWidth = 0;
	data->storeHeight = 0;
	data->par = 1.0;
	data->currFmt = (UInt32)-1;
	data->frameBuff = 0;
	data->seeked = false;
	data->frameIndexE = 0;
	data->frameIndexS = 0;
	data->lastFrameTime = 0;
	data->isOpenGOP = false;
	data->colorPrimaries = AVCOL_PRI_UNSPECIFIED;
	data->colorTrc = AVCOL_TRC_UNSPECIFIED;
	data->yuvColor = AVCOL_SPC_UNSPECIFIED;
#ifdef _DEBUG
	NEW_CLASS(data->dbgStm, IO::FileStream(CSTR("FFMPEGDebug.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(data->dbgWriter, IO::StreamWriter(data->dbgStm, 65001));
#endif

	Media::FrameInfo frameInfo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt maxFrameSize;
	sourceVideo->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);

	AVCodecID codecId;
	data->srcFCC = frameInfo.fourcc;
	switch (frameInfo.fourcc)
	{
#if VERSION_FROM(55, 0, 0) //not sure
	case FOURCC('V', 'P', '9', '0'):
	case FOURCC('v', 'p', '0', '9'):
		codecId = AV_CODEC_ID_VP9;
		break;
#endif
	case FOURCC('H', '2', '6', '4'):
	case FOURCC('h', '2', '6', '4'):
	case FOURCC('X', '2', '6', '4'):
	case FOURCC('x', '2', '6', '4'):
	case FOURCC('A', 'V', 'C', '1'):
	case FOURCC('a', 'v', 'c', '1'):
	case FOURCC('V', 'S', 'S', 'H'):
		codecId = AV_CODEC_ID_H264;
		break;
	case FOURCC('M', 'P', 'G', '2'):
		codecId = AV_CODEC_ID_MPEG2VIDEO;
		break;
#if VERSION_FROM(55, 0, 0) //not sure
	case FOURCC('H', 'E', 'V', 'C'):
	case FOURCC('X', '2', '6', '5'):
	case FOURCC('x', '2', '6', '5'):
	case FOURCC('H', '2', '6', '5'):
	case FOURCC('h', '2', '6', '5'):
		codecId = AV_CODEC_ID_HEVC;
		break;
#endif
	case FOURCC('d', 'i', 'v', 'f'):
	case FOURCC('d', 'i', 'v', 'x'):
	case FOURCC('D', 'X', '5', '0'):
	case FOURCC('D', 'I', 'V', 'X'):
	case FOURCC('x', 'v', 'i', 'd'):
	case FOURCC('X', 'V', 'I', 'D'):
	case FOURCC('F', 'M', 'P', '4'):
		return;
		codecId = AV_CODEC_ID_MPEG4;
		break;
	case FOURCC('M', 'J', 'P', 'G'):
		codecId = AV_CODEC_ID_MJPEG;
		break;
#if VERSION_FROM(55, 0, 0) //not sure
	case FOURCC('a', 'v', '0', '1'):
		codecId = AV_CODEC_ID_AV1;
		break;
#endif
	case FOURCC('M', 'P', '4', '2'):

	default:
		return;
	}
	data->codec = FFMPEGDecoder_avcodec_find_decoder(codecId);
	if (data->codec == 0)
	{
		return;
	}
	data->ctx = FFMPEGDecoder_avcodec_alloc_context3(data->codec);
	if (data->ctx == 0)
	{
		return;
	}

#if VERSION_FROM(55, 0, 0) && !VERSION_FROM(60, 0, 0) //from not sure 
	if ((data->codec->capabilities & AV_CODEC_CAP_TRUNCATED) != 0)
	{
		data->ctx->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames
	}
#endif

	data->ctx->codec_tag = data->srcFCC;
	data->ctx->width = (int)frameInfo.dispSize.x;
	data->ctx->height = (int)frameInfo.dispSize.y;
	data->ctx->thread_count = (int)Sync::ThreadUtil::GetThreadCnt();
	if (data->ctx->thread_count > 16)
	{
		data->ctx->thread_count = 16;
	}
	if (codecId == AV_CODEC_ID_H264)
	{
		UInt32 sz;
		UInt8 *buff = sourceVideo->GetProp(*(Int32*)"AVCH", &sz);
		if (buff)
		{
			data->ctx->extradata_size = (int)sz;
			data->ctx->extradata = buff;
		}
	}	
#if VERSION_FROM(55, 0, 0) //not sure
	else if (codecId == AV_CODEC_ID_HEVC)
	{
		UInt32 sz;
		UInt8 *buff = sourceVideo->GetProp(*(Int32*)"HEVC", &sz);
		if (buff)
		{
			data->ctx->extradata_size = (int)sz;
			data->ctx->extradata = buff;
		}
	}
#endif
	data->frame = FFMPEGDecoder_av_frame_alloc();
	if (data->frame == 0)
	{
		printf("FFMPEG: cannot allocate frame\r\n");
		return;
	}

	UOSInt frameSize;
	if (sourceVideo->IsRealTimeSrc())
	{
		frameSize = frameInfo.dispSize.CalcArea() * 4;
	}
	else
	{
		frameSize = sourceVideo->GetFrameSize(0);
	}
	
	if (frameSize > 0)
	{
	    AVPacket avpkt;
		Int32 ret;
		UInt8 *firstFrame;
		UOSInt frameNum;
		ret = FFMPEGDecoder_avcodec_open2(data->ctx, data->codec, 0);
		if (ret < 0)
		{
			printf("FFMPEG: cannot open codec\r\n");
			return;
		}
	    FFMPEGDecoder_av_init_packet(&avpkt);
		frameNum = 0;

		sourceVideo->ReadFrameBegin();
		while (true)
		{
#if VERSION_FROM(55, 0, 0) //not sure
			firstFrame = MemAlloc(UInt8, frameSize + AV_INPUT_BUFFER_PADDING_SIZE);
#else
			firstFrame = MemAlloc(UInt8, frameSize);
#endif
			frameSize = sourceVideo->ReadFrame(frameNum, firstFrame);

			avpkt.data = firstFrame;
			avpkt.size = (int)frameSize;
#if VERSION_FROM(55, 0, 0) //not sure
			MemClear(firstFrame + frameSize, AV_INPUT_BUFFER_PADDING_SIZE);
#endif

#if VERSION_FROM(57, 41, 0)
			ret = FFMPEGDecoder_avcodec_send_packet(data->ctx, &avpkt);
			MemFree(firstFrame);
			if (ret != 0)
			{
				FFMPEGDecoder_avcodec_close(data->ctx);
				return;
			}
			ret = FFMPEGDecoder_avcodec_receive_frame(data->ctx, data->frame);
			if (ret == 0)
			{
				break;
			}
			if (ret == AVERROR(EAGAIN))
			{
				frameNum++;
				if (sourceVideo->IsRealTimeSrc())
				{
					frameSize = frameInfo.dispSize.CalcArea() * 4;
				}
				else
				{			
					frameSize = sourceVideo->GetFrameSize(frameNum);
				}
				if (frameSize <= 0)
				{
					FFMPEGDecoder_avcodec_close(data->ctx);
					sourceVideo->ReadFrameEnd();
					return;
				}
			}
			else
			{
				FFMPEGDecoder_avcodec_close(data->ctx);
				sourceVideo->ReadFrameEnd();
				return;
			}
#else
			int check = 0;
			ret = FFMPEGDecoder_avcodec_decode_video2(data->ctx, data->frame, &check, &avpkt);
			MemFree(firstFrame);
			if (ret == 0)
			{
				break;
			}
			if (ret == AVERROR(EAGAIN))
			{
				frameNum++;
				frameSize = sourceVideo->GetFrameSize(frameNum);
				if (frameSize <= 0)
				{
					FFMPEGDecoder_avcodec_close(data->ctx);
					sourceVideo->ReadFrameEnd();
					return;
				}
			}
			else
			{
				FFMPEGDecoder_avcodec_close(data->ctx);
				sourceVideo->ReadFrameEnd();
				return;
			}
#endif
		}
		sourceVideo->ReadFrameEnd();

		data->dispWidth = (UInt32)data->ctx->width;
		data->dispHeight = (UInt32)data->ctx->height;
		data->currFmt = data->ctx->pix_fmt;
		if (data->ctx->sample_aspect_ratio.num != 0 && data->ctx->sample_aspect_ratio.den)
		{
			data->par = data->ctx->sample_aspect_ratio.den / (Double)data->ctx->sample_aspect_ratio.num;
		}
		else
		{
			data->par = 1.0;
		}
		if (ret == 0)
		{
			switch (data->currFmt)
			{
#if VERSION_FROM(57, 27, 2) //not sure
			case AV_PIX_FMT_YUV420P12LE:
				data->storeWidth = (UInt32)data->frame->linesize[0] >> 1;
				break;
#endif
			case AV_PIX_FMT_YUV420P10LE:
				data->storeWidth = (UInt32)data->frame->linesize[0] >> 1;
				break;
			case AV_PIX_FMT_YUV444P10LE:
				data->storeWidth = (UInt32)data->frame->linesize[0] >> 1;
				break;
			case AV_PIX_FMT_YUVJ420P:
			case AV_PIX_FMT_YUV420P:
				data->storeWidth = (UInt32)data->frame->linesize[0];
				break;
			case AV_PIX_FMT_YUVJ422P:
			case AV_PIX_FMT_YUV422P:
				data->storeWidth = (UInt32)data->frame->width;
				break;
			default:
				data->storeWidth = (UInt32)data->frame->linesize[0];
				break;
			}
			data->storeHeight = (UInt32)data->dispHeight;
#if VERSION_FROM(55, 0, 0) //not sure
			data->colorPrimaries = data->frame->color_primaries;
			data->colorTrc = data->frame->color_trc;
			data->yuvColor = data->frame->colorspace;
#endif
		}
		FFMPEGDecoder_avcodec_close(data->ctx);

		//av_frame_unref(data->frame);
		if (data->currFmt >= 0 && data->storeWidth != 0 && data->storeHeight != 0)
		{
			ret = FFMPEGDecoder_avcodec_open2(data->ctx, data->codec, 0);
			if (ret >= 0)
			{
				Media::FrameInfo frinfo;
				UInt32 frameRateNorm;
				UInt32 frameRateDenorm;
				UOSInt maxFrameSize;
				this->GetVideoInfo(&frinfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
				data->frameSize = maxFrameSize;
				if (data->frameSize > 0)
				{
					data->frameBuff = MemAllocA(UInt8, data->frameSize);
				}
				data->inited = true;
			}
		}
	}
}

Media::Decoder::FFMPEGDecoder::~FFMPEGDecoder()
{
	ClassData *data = this->clsData;
	if (data->inited)
	{
		FFMPEGDecoder_avcodec_close(data->ctx);
		data->inited = false;
	}
	if (data->frame)
	{
		FFMPEGDecoder_av_frame_free(&data->frame);
		data->frame = 0;
	}
	if (data->ctx)
	{
		if (data->ctx->extradata)
		{
			data->ctx->extradata = 0;
		}
		FFMPEGDecoder_avcodec_free_context(&data->ctx);
		data->ctx = 0;
	}
	if (data->frameBuff)
	{
		MemFreeA(data->frameBuff);
		data->frameBuff = 0;
	}
#ifdef _DEBUG
	DEL_CLASS(data->dbgStm);
	DEL_CLASS(data->dbgWriter);
#endif
	MemFree(data);
}

Bool Media::Decoder::FFMPEGDecoder::CaptureImage(ImageCallback imgCb, void *userData)
{
	return false;
}

Text::CString Media::Decoder::FFMPEGDecoder::GetFilterName()
{
	return CSTR("FFMPEGDecoder");
}

Bool Media::Decoder::FFMPEGDecoder::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	ClassData *data = this->clsData;
	if (this->sourceVideo == 0)
		return false;
	if (!this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize))
		return false;
	Bool fullRange = false;
	info->dispSize = Math::Size2D<UOSInt>(data->dispWidth, data->dispHeight);
	info->storeSize = Math::Size2D<UOSInt>(data->storeWidth, data->storeHeight);
	info->par2 = data->par;
	info->byteSize = 0;
	switch (data->currFmt)
	{
	case AV_PIX_FMT_YUVJ420P:
		fullRange = true;
	case AV_PIX_FMT_YUV420P:
		//info->fourcc = *(Int32*)"YV12";
		info->fourcc = FFMT_YUV420P8;
		info->storeBPP = 12;
		info->pf = Media::PixelFormatGetDef(info->fourcc, info->storeBPP);
		info->byteSize = (info->storeSize.CalcArea() * 3) >> 1;
		break;
	case AV_PIX_FMT_YUVJ422P:
		fullRange = true;
	case AV_PIX_FMT_YUV422P:
		info->fourcc = *(UInt32*)"YUY2";
		info->storeBPP = 16;
		info->pf = Media::PixelFormatGetDef(info->fourcc, info->storeBPP);
		info->byteSize = info->storeSize.CalcArea() * 2;
		break;
	case AV_PIX_FMT_YUV420P10LE:
	case AV_PIX_FMT_YUV420P12LE:
		if (data->currFmt == AV_PIX_FMT_YUV420P10LE)
		{
			info->fourcc = FFMT_YUV420P10LE;
		}
		else if (data->currFmt == AV_PIX_FMT_YUV420P12LE)
		{
			info->fourcc = FFMT_YUV420P12LE;
		}
		info->storeBPP = 24;
		info->pf = Media::PixelFormatGetDef(info->fourcc, info->storeBPP);
		info->byteSize = (info->storeSize.CalcArea() * 6) >> 1;
		break;
	case AV_PIX_FMT_YUV444P10LE:
		info->fourcc = FFMT_YUV444P10LE;
		info->storeBPP = 48;
		info->pf = Media::PixelFormatGetDef(info->fourcc, info->storeBPP);
		info->byteSize = info->storeSize.CalcArea() * 6;
		break;
	default:
		printf("Unsupported decoded format: %d\r\n", data->currFmt);
		return false;
	}
	switch (data->yuvColor)
	{
	case AVCOL_SPC_UNSPECIFIED:
		info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		break;
	case AVCOL_SPC_RESERVED:
	case AVCOL_SPC_RGB:
	default:
		info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		break;
	case AVCOL_SPC_BT709:
		info->yuvType = Media::ColorProfile::YUVT_BT709;
		break;
	case AVCOL_SPC_FCC:
		info->yuvType = Media::ColorProfile::YUVT_FCC;
		break;
	case AVCOL_SPC_BT470BG:
		info->yuvType = Media::ColorProfile::YUVT_BT470BG;
		break;
	case AVCOL_SPC_SMPTE170M:
		info->yuvType = Media::ColorProfile::YUVT_SMPTE170M;
		break;
	case AVCOL_SPC_SMPTE240M:
		info->yuvType = Media::ColorProfile::YUVT_SMPTE240M;
		break;
	case AVCOL_SPC_YCGCO:
		info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		break;
	case AVCOL_SPC_BT2020_NCL:
	case AVCOL_SPC_BT2020_CL:
		info->yuvType = Media::ColorProfile::YUVT_BT2020;
		break;

#if VERSION_FROM(57, 22, 2)
	case AVCOL_SPC_SMPTE2085:
#endif
#if UTIL_VERSION_FROM(55, 73, 0)
	case AVCOL_SPC_CHROMA_DERIVED_CL:
	case AVCOL_SPC_CHROMA_DERIVED_NCL:
	case AVCOL_SPC_ICTCP:
#endif
	case AVCOL_SPC_NB:
		info->yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		break;
	}
	if (fullRange)
	{
		info->yuvType = (Media::ColorProfile::YUVType)(info->yuvType | Media::ColorProfile::YUVT_FLAG_YUV_0_255);
	}
	switch (data->colorTrc)
	{
	case AVCOL_TRC_RESERVED0:
	case AVCOL_TRC_UNSPECIFIED:
	case AVCOL_TRC_RESERVED:
	default:
		info->color->rtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		break;
	case AVCOL_TRC_BT709:
		info->color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
		break;
	case AVCOL_TRC_GAMMA22:
		info->color->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_GAMMA, 2.2);
		break;
	case AVCOL_TRC_GAMMA28:
		info->color->rtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
		info->color->gtransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
		info->color->btransfer.Set(Media::CS::TRANT_GAMMA, 2.8);
		break;
	case AVCOL_TRC_SMPTE170M:
		info->color->rtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		break;
	case AVCOL_TRC_SMPTE240M:
		info->color->rtransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_SMPTE240, 2.2);
		break;
	case AVCOL_TRC_LINEAR:
		info->color->rtransfer.Set(Media::CS::TRANT_LINEAR, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_LINEAR, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_LINEAR, 2.2);
		break;
	case AVCOL_TRC_LOG:
		info->color->rtransfer.Set(Media::CS::TRANT_LOG100, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_LOG100, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_LOG100, 2.2);
		break;
	case AVCOL_TRC_LOG_SQRT:
		info->color->rtransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_LOGSQRT10, 2.2);
		break;
	case AVCOL_TRC_BT1361_ECG:
		info->color->rtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_BT1361, 2.2);
		break;
	case AVCOL_TRC_IEC61966_2_4:
	case AVCOL_TRC_IEC61966_2_1:
		info->color->rtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_sRGB, 2.2);
		break;
	case AVCOL_TRC_BT2020_10:
	case AVCOL_TRC_BT2020_12:
		info->color->rtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_BT709, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_BT709, 2.2);
		break;
#if VERSION_FROM(56, 0, 0)
	case AVCOL_TRC_SMPTE2084: //BT2100
		info->color->rtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_BT2100, 2.2);
		break;
#endif
#if VERSION_FROM(56, 0, 0)
	case AVCOL_TRC_ARIB_STD_B67:
		info->color->rtransfer.Set(Media::CS::TRANT_HLG, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_HLG, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_HLG, 2.2);
		break;
	case AVCOL_TRC_SMPTE428:
#endif
	case AVCOL_TRC_NB:
		info->color->rtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		info->color->gtransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		info->color->btransfer.Set(Media::CS::TRANT_VUNKNOWN, 2.2);
		break;
	}
	switch (data->colorPrimaries)
	{
	case AVCOL_PRI_RESERVED0:
	case AVCOL_PRI_UNSPECIFIED:
	case AVCOL_PRI_RESERVED:
	case AVCOL_PRI_NB:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_BT709);
		break;
	case AVCOL_PRI_BT709:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_BT709);
		break;
	case AVCOL_PRI_BT470M:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_BT470M);
		break;
	case AVCOL_PRI_BT470BG:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_BT470BG);
		break;
	case AVCOL_PRI_SMPTE170M:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE170M);
		break;
	case AVCOL_PRI_SMPTE240M:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_SMPTE240M);
		break;
	case AVCOL_PRI_FILM:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_GENERIC_FILM);
		break;
	case AVCOL_PRI_BT2020:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_BT2020);
		break;
#if VERSION_FROM(56, 0, 0)
	case AVCOL_PRI_SMPTE428:
#endif
		info->color->primaries.SetColorType(Media::ColorProfile::CT_VUNKNOWN);
		break;
#if VERSION_FROM(57, 22, 2)
	case AVCOL_PRI_SMPTE431:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_DCI_P3);
		break;
	case AVCOL_PRI_SMPTE432:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_DCI_P3);
		break;
#endif
#if VERSION_FROM(56, 0, 0)
	case AVCOL_PRI_JEDEC_P22:
#endif
	default:
		info->color->primaries.SetColorType(Media::ColorProfile::CT_VUNKNOWN);
		break;
	}
	*maxFrameSize = info->byteSize;
	return true;
}

void Media::Decoder::FFMPEGDecoder::Stop()
{
	ClassData *data = this->clsData;
	if (this->sourceVideo == 0)
		return;

	this->started = false;
	this->sourceVideo->Stop();
	while (this->endProcessing)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->frameCb = 0;
	this->frameCbData = 0;
//	this->lastFrameNum = 0;
	this->lastFrameTime = (UInt32)-1;
	data->frameIndexS = 0;
	data->frameIndexE = 0;
}

Bool Media::Decoder::FFMPEGDecoder::HasFrameCount()
{
	return this->sourceVideo->HasFrameCount();
}

UOSInt Media::Decoder::FFMPEGDecoder::GetFrameCount()
{
	return this->sourceVideo->GetFrameCount();
}

UInt32 Media::Decoder::FFMPEGDecoder::GetFrameTime(UOSInt frameIndex)
{
	return this->sourceVideo->GetFrameTime(frameIndex);
}

void Media::Decoder::FFMPEGDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	return this->sourceVideo->EnumFrameInfos(cb, userData);
}

void Media::Decoder::FFMPEGDecoder::OnFrameChanged(Media::IVideoSource::FrameChange fc)
{
	if (fc == Media::IVideoSource::FC_PAR)
	{
//		this->frameChg = true;
	}
	else if (fc == Media::IVideoSource::FC_ENDPLAY)
	{
	}
}

Bool Media::Decoder::FFMPEGDecoder::IsError()
{
	ClassData *data = this->clsData;
	return !data->inited || data->frameSize == 0;
}

Media::IVideoSource *__stdcall FFMPEGDecoder_DecodeVideo(Media::IVideoSource *sourceVideo)
{
	Media::Decoder::FFMPEGDecoder *decoder;
	Media::FrameInfo frameInfo;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UOSInt maxFrameSize;
	sourceVideo->GetVideoInfo(&frameInfo, &frameRateNorm, &frameRateDenorm, &maxFrameSize);
	if (frameInfo.fourcc == 0 || frameInfo.fourcc == 0xFFFFFFFF)
		return 0;

	if (frameInfo.fourcc == *(UInt32*)"ravc")
	{
		Media::Decoder::RAVCDecoder *ravc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(ravc, Media::Decoder::RAVCDecoder(sourceVideo, false, true));
		NEW_CLASS(decoder, Media::Decoder::FFMPEGDecoder(ravc));
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
	else if (frameInfo.fourcc == *(UInt32*)"rhvc")
	{
		Media::Decoder::RHVCDecoder *rhvc;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(rhvc, Media::Decoder::RHVCDecoder(sourceVideo, false));
		NEW_CLASS(decoder, Media::Decoder::FFMPEGDecoder(rhvc));
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
	else if (frameInfo.fourcc == *(UInt32*)"m2v1")
	{
		Media::Decoder::M2VDecoder *m2vd;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(m2vd, Media::Decoder::M2VDecoder(sourceVideo, false));
		NEW_CLASS(decoder, Media::Decoder::FFMPEGDecoder(m2vd));
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
	else if (frameInfo.fourcc == *(UInt32*)"vp09")
	{
		Media::Decoder::VP09Decoder *vp09;
		Media::Decoder::VDecoderChain *decChain;

		NEW_CLASS(vp09, Media::Decoder::VP09Decoder(sourceVideo, false));
		NEW_CLASS(decoder, Media::Decoder::FFMPEGDecoder(vp09));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			DEL_CLASS(vp09);
			return 0;
		}
		NEW_CLASS(decChain, Media::Decoder::VDecoderChain(decoder));
		decChain->AddDecoder(vp09);
		return decChain;
	}
	else
	{
		NEW_CLASS(decoder, Media::Decoder::FFMPEGDecoder(sourceVideo));
		if (decoder->IsError())
		{
			DEL_CLASS(decoder);
			return 0;
		}
	}
	return decoder;
}

class FFMPEGADecoder : public Media::Decoder::ADecoderBase
{
private:
	Bool inited;
	const AVCodec *codec;
	AVCodecContext *ctx;
	AVFrame *frame;
	Media::AudioFormat *decFmt;
	UInt8 *frameBuff;
	UOSInt frameMaxSize;
	UOSInt frameBuffSize;
	UInt8 *readBuff;
	UOSInt readBlockSize;
	Bool seeked;

	Sync::Event *readEvt;

	OSInt GetChannelCnt()
	{
#if defined(FF_API_OLD_CHANNEL_LAYOUT) && FF_API_OLD_CHANNEL_LAYOUT
		return this->frame->ch_layout.nb_channels;
#else
		return this->frame->channels;
#endif
	}

public:
	FFMPEGADecoder(IAudioSource *sourceAudio)
	{
		Media::AudioFormat fmt;
		Int32 ret;
		this->sourceAudio = sourceAudio;
		this->inited = false;
		this->codec = 0;
		this->ctx = 0;
		this->frame = 0;
		this->decFmt = 0;
		this->frameBuff = 0;
		this->readBuff = 0;
		this->readBlockSize = 0;
		this->frameMaxSize = 65536;
		this->frameBuffSize = 0;
		this->seeked = true;
		this->readEvt = 0;
		sourceAudio->GetFormat(&fmt);

		AVCodecID codecId;
		switch (fmt.formatId)
		{
		case 0x566f:
			codecId = AV_CODEC_ID_VORBIS;
			break;
		case 0x1610:
		case 0xff:
			codecId = AV_CODEC_ID_AAC;
			break;
		default:
			return;
		}
		this->codec = FFMPEGDecoder_avcodec_find_decoder(codecId);
		if (this->codec == 0)
			return;
		this->ctx = FFMPEGDecoder_avcodec_alloc_context3(this->codec);
		if (this->ctx == 0)
			return;

#if !VERSION_FROM(60, 0, 0)
		if (this->codec->capabilities & AV_CODEC_CAP_TRUNCATED)
			this->ctx->flags |= AV_CODEC_FLAG_TRUNCATED; // we do not send complete frames
#endif
		if (fmt.bitpersample == 16)
		{
			this->ctx->request_sample_fmt = AV_SAMPLE_FMT_S16;
		}
		else
		{
			return;
		}
		switch (fmt.nChannels)
		{
		case 1:
#if defined(FF_API_OLD_CHANNEL_LAYOUT) && FF_API_OLD_CHANNEL_LAYOUT
			////////////////////////////////
//			this->ctx->request_channel_layout = AV_CH_LAYOUT_MONO;
//			this->ctx->channel_layout = AV_CH_LAYOUT_MONO;
			this->ctx->ch_layout.order = AV_CHANNEL_ORDER_UNSPEC;
			this->ctx->ch_layout.nb_channels = 1;
#else
			this->ctx->request_channel_layout = AV_CH_LAYOUT_MONO;
			this->ctx->channel_layout = AV_CH_LAYOUT_MONO;
#endif
			break;
		case 2:
#if defined(FF_API_OLD_CHANNEL_LAYOUT) && FF_API_OLD_CHANNEL_LAYOUT
			////////////////////////////////
//			this->ctx->request_channel_layout = AV_CH_LAYOUT_STEREO;
//			this->ctx->channel_layout = AV_CH_LAYOUT_STEREO;
			this->ctx->ch_layout.order = AV_CHANNEL_ORDER_UNSPEC;
			this->ctx->ch_layout.nb_channels = 2;
#else
			this->ctx->request_channel_layout = AV_CH_LAYOUT_STEREO;
			this->ctx->channel_layout = AV_CH_LAYOUT_STEREO;
#endif
			break;
		default:
			return;
		}
		this->frame = FFMPEGDecoder_av_frame_alloc();
		this->ctx->bit_rate = fmt.bitRate;
		this->ctx->sample_rate = (int)fmt.frequency;
#if defined(FF_API_OLD_CHANNEL_LAYOUT) && FF_API_OLD_CHANNEL_LAYOUT
		this->ctx->ch_layout.nb_channels = fmt.nChannels;
#else
		this->ctx->channels = fmt.nChannels;
#endif
		this->ctx->flags2 = AV_CODEC_FLAG2_CHUNKS;
		if (fmt.formatId != 1 && fmt.extraSize > 0)
		{
			this->ctx->extradata_size = (int)fmt.extraSize;
			this->ctx->extradata = fmt.extra;
		}
		ret = FFMPEGDecoder_avcodec_open2(this->ctx, this->codec, 0);
		if (ret < 0)
		{
			return;
		}
		this->readBlockSize = sourceAudio->GetMinBlockSize();
		if (this->readBlockSize == 0)
		{
			this->readBlockSize = 1024;
		}
		else if (this->readBlockSize < 1024)
		{
			this->readBlockSize = (1024 / this->readBlockSize) * this->readBlockSize;
		}
		this->readBuff = MemAlloc(UInt8, this->readBlockSize);
		this->frameMaxSize = 65536;
		this->frameBuff = MemAlloc(UInt8, this->frameMaxSize);
		this->frameBuffSize = 0;
		NEW_CLASS(this->decFmt, Media::AudioFormat());
		this->decFmt->FromAudioFormat(&fmt);
		switch (this->ctx->sample_fmt)
		{
		case AV_SAMPLE_FMT_U8:
		case AV_SAMPLE_FMT_U8P:
			this->decFmt->formatId = 1;
			this->decFmt->bitpersample = 8;
			break;
		case AV_SAMPLE_FMT_S16P:
		case AV_SAMPLE_FMT_S16:
			this->decFmt->formatId = 1;
			this->decFmt->bitpersample = 16;
			break;
		case AV_SAMPLE_FMT_S32:
		case AV_SAMPLE_FMT_S32P:
			this->decFmt->formatId = 1;
			this->decFmt->bitpersample = 32;
			break;
#if UTIL_VERSION_FROM(55, 29, 0)
		case AV_SAMPLE_FMT_S64:
		case AV_SAMPLE_FMT_S64P:
			this->decFmt->formatId = 1;
			this->decFmt->bitpersample = 64;
			break;
#endif
		case AV_SAMPLE_FMT_FLT:
		case AV_SAMPLE_FMT_FLTP:
			this->decFmt->formatId = 3;
			this->decFmt->bitpersample = 32;
			break;
		case AV_SAMPLE_FMT_DBL:
		case AV_SAMPLE_FMT_DBLP:
			this->decFmt->formatId = 3;
			this->decFmt->bitpersample = 64;
			break;
		case AV_SAMPLE_FMT_NONE:
		case AV_SAMPLE_FMT_NB:
		default:
			break;
		}
		this->decFmt->align = (UInt32)this->decFmt->nChannels * this->decFmt->bitpersample >> 3;
		this->decFmt->bitRate = this->decFmt->frequency * this->decFmt->align << 3;
		this->inited = true;
	}

	virtual ~FFMPEGADecoder()
	{
		if (this->inited)
		{
			FFMPEGDecoder_avcodec_close(this->ctx);
			this->inited = false;
		}
		if (this->ctx)
		{
			if (this->ctx->extradata)
			{
				this->ctx->extradata = 0;
			}
			this->ctx->extradata_size = 0;
			FFMPEGDecoder_avcodec_free_context(&this->ctx);
			this->ctx = 0;
		}
		if (this->frameBuff)
		{
			MemFree(this->frameBuff);
			this->frameBuff = 0;
		}
		if (this->readBuff)
		{
			MemFree(this->readBuff);
			this->readBuff = 0;
		}
		if (this->frame)
		{
			FFMPEGDecoder_av_frame_free(&this->frame);
			this->frame = 0;
		}
		if (this->decFmt)
		{
			DEL_CLASS(this->decFmt);
			this->decFmt = 0;
		}
	}

	virtual void GetFormat(Media::AudioFormat *format)
	{
		if (this->decFmt)
		{
			format->FromAudioFormat(this->decFmt);
		}
		else
		{
			format->formatId = 0;
		}
	}

	virtual UInt32 SeekToTime(UInt32 time)
	{
		if (this->sourceAudio)
		{
			this->seeked = true;
			this->frameBuffSize = 0;
			return this->sourceAudio->SeekToTime(time);
		}
		return 0;
	}

	virtual Bool Start(Sync::Event *evt, UOSInt blkSize)
	{
		if (this->sourceAudio)
		{
			this->seeked = true;
			this->frameBuffSize = 0;
			this->sourceAudio->Start(0, blkSize);
			this->readEvt = evt;

			if (this->readEvt)
				this->readEvt->Set();
			return true;
		}
		return false;
	}

	virtual void Stop()
	{
		if (this->sourceAudio)
		{
			this->sourceAudio->Stop();
		}
		this->readEvt = 0;
	}

	virtual UOSInt ReadBlock(UInt8 *buff, UOSInt blkSize)
	{
		UOSInt outSize = 0;
		UOSInt i;
	    AVPacket avpkt;
		Int32 ret;
		if (this->decFmt == 0 || !this->inited)
		{
			return 0;
		}
		else if (this->decFmt->align == 0)
		{
			if (this->decFmt->frequency == 0)
				return 0;
			else
			{
				this->decFmt->align = 1;
			}
		}
		i = blkSize % this->decFmt->align;
		if (i)
		{
			blkSize -= i;
		}

		FFMPEGDecoder_av_init_packet(&avpkt);
		while (blkSize > 0)
		{
			if (this->frameBuffSize)
			{
				if (this->frameBuffSize < blkSize)
				{
					MemCopyNO(buff, this->frameBuff, this->frameBuffSize);
					buff += this->frameBuffSize;
					outSize += this->frameBuffSize;
					blkSize -= this->frameBuffSize;
					this->frameBuffSize = 0;
				}
				else
				{
					MemCopyNO(buff, this->frameBuff, blkSize);
					outSize += blkSize;
					if (this->frameBuffSize > blkSize)
					{
						MemCopyO(this->frameBuff, &this->frameBuff[blkSize], this->frameBuffSize - blkSize);
						this->frameBuffSize -= blkSize;
						if (this->readEvt)
							this->readEvt->Set();
						return outSize;
					}
					else
					{
						this->frameBuffSize = 0;
						if (this->readEvt)
							this->readEvt->Set();
						return outSize;
					}
				}
			}

			UOSInt srcSize = this->sourceAudio->ReadBlock(this->readBuff, this->readBlockSize);
			if (srcSize == 0)
			{
				if (this->readEvt)
					this->readEvt->Set();
				return outSize;
			}
			avpkt.data = this->readBuff;
			avpkt.size = (int)srcSize;
			avpkt.dts = AV_NOPTS_VALUE;
			avpkt.pts = AV_NOPTS_VALUE;
			avpkt.pos = -1;
			if (this->seeked)
			{
				avpkt.pos = 0;
				this->seeked = false;
			}

			while (avpkt.size > 0)
			{
				int gotFrame = 0;
				ret = FFMPEGDecoder_avcodec_decode_audio4(this->ctx, this->frame, &gotFrame, &avpkt);
				if (ret < 0)
				{
					if (this->readEvt)
						this->readEvt->Set();
					return outSize;
				}
				else
				{
					avpkt.data += ret;
					avpkt.size -= ret;

					if (gotFrame)
					{
						UInt8 *dataPtr = this->frame->data[0];
						UOSInt dataSize = (UOSInt)this->frame->nb_samples * this->decFmt->align;
						if (this->frameBuffSize + dataSize > this->frameMaxSize)
						{
							while (this->frameBuffSize + dataSize > this->frameMaxSize)
							{
								this->frameMaxSize = this->frameMaxSize << 1;
							}
							UInt8 *tmpBuff = MemAlloc(UInt8, this->frameMaxSize);
							if (this->frameBuffSize)
							{
								MemCopyNO(tmpBuff, this->frameBuff, this->frameBuffSize);
							}
							MemFree(this->frameBuff);
							this->frameBuff = tmpBuff;
						}
						if (this->GetChannelCnt() == 1)
						{
							MemCopyNO(&this->frameBuff[this->frameBuffSize], dataPtr, dataSize);
						}
						else if (this->frame->format == AV_SAMPLE_FMT_U8P)
						{
							UInt8 *arr[8];
							UInt8 *outBuff = &this->frameBuff[this->frameBuffSize];
							OSInt ch = this->GetChannelCnt();
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
						else if (this->frame->format == AV_SAMPLE_FMT_S16P)
						{
							Int16 *arr[8];
							Int16 *outBuff = (Int16*)&this->frameBuff[this->frameBuffSize];
							OSInt ch = this->GetChannelCnt();
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = (Int16*)this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
						else if (this->frame->format == AV_SAMPLE_FMT_S32P)
						{
							Int32 *arr[8];
							Int32 *outBuff = (Int32*)&this->frameBuff[this->frameBuffSize];
							OSInt ch = this->GetChannelCnt();
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = (Int32*)this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
						else if (this->frame->format == AV_SAMPLE_FMT_FLTP)
						{
							Single *arr[8];
							Single *outBuff = (Single*)&this->frameBuff[this->frameBuffSize];
							OSInt ch = this->GetChannelCnt();
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = (Single*)this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
						else if (this->frame->format == AV_SAMPLE_FMT_DBLP)
						{
							Double *arr[8];
							Double *outBuff = (Double*)&this->frameBuff[this->frameBuffSize];
							OSInt ch = this->GetChannelCnt();
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = (Double*)this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
#ifdef AV_SAMPLE_FMT_S64P
						else if (this->frame->format == AV_SAMPLE_FMT_S64P)
						{
							Int64 *arr[8];
							Int64 *outBuff = (Int64*)&this->frameBuff[this->frameBuffSize];
							OSInt ch = this->frame->channels;
							OSInt ind = 0;
							OSInt sampleLeft = this->frame->nb_samples;
							while (ind < ch)
							{
								arr[ind] = (Int64*)this->frame->data[i];
								ind++;
							}
							while (sampleLeft-- > 0)
							{
								ind = 0;
								while (ind < ch)
								{
									*outBuff++ = arr[ind][0];
									arr[ind]++;
									ind++;
								}
							}
						}
#endif
						else
						{
							MemCopyNO(&this->frameBuff[this->frameBuffSize], dataPtr, dataSize);
						}
						this->frameBuffSize += dataSize;

						if (blkSize >= this->frameBuffSize)
						{
							MemCopyNO(buff, this->frameBuff, this->frameBuffSize);
							buff += this->frameBuffSize;
							outSize += this->frameBuffSize;
							blkSize -= this->frameBuffSize;
							this->frameBuffSize = 0;
						}
						else if (blkSize > 0)
						{
							MemCopyNO(buff, this->frameBuff, blkSize);
							buff += blkSize;
							outSize += blkSize;
							MemCopyO(this->frameBuff, &this->frameBuff[blkSize], this->frameBuffSize - blkSize);
							this->frameBuffSize -= blkSize;
							blkSize = 0;
						}
					}
				}
			}

/*			ret = FFMPEGDecoder_avcodec_send_packet(this->ctx, &avpkt);
			if (ret < 0)
			{
//				FFMPEGDecoder_av_packet_unref(&avpkt);
				if (this->readEvt)
					this->readEvt->Set();
				return outSize;
			}
			while ((ret = FFMPEGDecoder_avcodec_receive_frame(this->ctx, this->frame)) == 0)
			{
				UInt8 *dataPtr = this->frame->data[0];
				OSInt dataSize = this->frame->nb_samples * this->decFmt->align;
				if (blkSize >= dataSize)
				{
					MemCopyNO(buff, this->frame->data[0], dataSize);
					buff += dataSize;
					outSize += dataSize;
					blkSize -= dataSize;
				}
				else
				{
					if (blkSize > 0)
					{
						MemCopyNO(buff, dataPtr, blkSize);
						buff += blkSize;
						outSize += blkSize;
						dataPtr += blkSize;
						dataSize -= blkSize;
						blkSize = 0;
					}

					if (this->frameBuffSize + dataSize > this->frameMaxSize)
					{
						while (this->frameBuffSize + dataSize > this->frameMaxSize)
						{
							this->frameMaxSize = this->frameMaxSize << 1;
						}
						UInt8 *tmpBuff = MemAlloc(UInt8, this->frameMaxSize);
						if (this->frameBuffSize)
						{
							MemCopyNO(tmpBuff, this->frameBuff, this->frameBuffSize);
						}
						MemFree(this->frameBuff);
						this->frameBuff = tmpBuff;
					}
					MemCopyNO(&this->frameBuff[this->frameBuffSize], dataPtr, dataSize);
					this->frameBuffSize += dataSize;
				}
			}*/
		}
		if (this->readEvt)
			this->readEvt->Set();
		//FFMPEGDecoder_av_packet_unref(&avpkt);
		return outSize;
	}

	virtual UOSInt GetMinBlockSize()
	{
		return this->decFmt->align;
	}

	Bool IsError()
	{
		return !this->inited;
	}
};



Media::IAudioSource *__stdcall FFMPEGDecoder_DecodeAudio(Media::IAudioSource *sourceAudio)
{
	FFMPEGADecoder *decoder;
	NEW_CLASS(decoder, FFMPEGADecoder(sourceAudio));
	if (!decoder->IsError())
		return decoder;
	return 0;
}

void __stdcall FFMPEGDecoder_OnExit()
{
#if !defined(__GNUC__) || defined(__MINGW32__)
	DEL_CLASS(FFMPEGDecoder_lib1);
	DEL_CLASS(FFMPEGDecoder_lib2);
#endif
}

void Media::Decoder::FFMPEGDecoder::Enable()
{
#if !defined(__GNUC__) || defined(__MINGW32__)
	NEW_CLASS(FFMPEGDecoder_lib1, IO::Library((const UTF8Char*)"avcodec-57.dll"));
	NEW_CLASS(FFMPEGDecoder_lib2, IO::Library((const UTF8Char*)"avutil-55.dll"));
	Core::CoreAddOnExitFunc(FFMPEGDecoder_OnExit);

	FFMPEGDecoder_av_frame_alloc = (AVFrame *(__stdcall *)())FFMPEGDecoder_lib2->GetFunc("av_frame_alloc");
	FFMPEGDecoder_av_frame_free = (void (__stdcall *)(AVFrame **frame))FFMPEGDecoder_lib2->GetFunc("av_frame_free");

	FFMPEGDecoder_av_init_packet = (void (__stdcall *)(AVPacket *pkt))FFMPEGDecoder_lib1->GetFunc("av_init_packet");
	FFMPEGDecoder_av_packet_unref = (void (__stdcall *)(AVPacket *))FFMPEGDecoder_lib1->GetFunc("av_packet_unref");
	FFMPEGDecoder_avcodec_register_all = (void (__stdcall*)())FFMPEGDecoder_lib1->GetFunc("avcodec_register_all");
	FFMPEGDecoder_avcodec_send_packet = (int (__stdcall *)(AVCodecContext *, const AVPacket *))FFMPEGDecoder_lib1->GetFunc("avcodec_send_packet");
	FFMPEGDecoder_avcodec_receive_frame = (int (__stdcall *)(AVCodecContext *, AVFrame *))FFMPEGDecoder_lib1->GetFunc("avcodec_receive_frame");
	FFMPEGDecoder_avcodec_find_decoder = (AVCodec *(__stdcall *)(enum AVCodecID id))FFMPEGDecoder_lib1->GetFunc("avcodec_find_decoder");
	FFMPEGDecoder_avcodec_alloc_context3 = (AVCodecContext *(__stdcall *)(const AVCodec *))FFMPEGDecoder_lib1->GetFunc("avcodec_alloc_context3");
	FFMPEGDecoder_avcodec_free_context = (void (__stdcall *)(AVCodecContext **))FFMPEGDecoder_lib1->GetFunc("avcodec_free_context");
	FFMPEGDecoder_avcodec_open2 = (int (__stdcall *)(AVCodecContext *, const AVCodec *, AVDictionary **))FFMPEGDecoder_lib1->GetFunc("avcodec_open2");
	FFMPEGDecoder_avcodec_close = (int (__stdcall *)(AVCodecContext *))FFMPEGDecoder_lib1->GetFunc("avcodec_close");
	FFMPEGDecoder_avcodec_decode_audio4 = (int (__stdcall *)(AVCodecContext *, AVFrame *, int *, const AVPacket *))FFMPEGDecoder_lib1->GetFunc("avcodec_decode_audio4");

	if (FFMPEGDecoder_av_frame_alloc == 0)
		return;
	if (FFMPEGDecoder_av_frame_free == 0)
		return;
	if (FFMPEGDecoder_av_init_packet == 0)
		return;
	if (FFMPEGDecoder_av_packet_unref == 0)
		return;
	if (FFMPEGDecoder_avcodec_register_all == 0)
		return;
	if (FFMPEGDecoder_avcodec_send_packet == 0)
		return;
	if (FFMPEGDecoder_avcodec_receive_frame == 0)
		return;
	if (FFMPEGDecoder_avcodec_find_decoder == 0)
		return;
	if (FFMPEGDecoder_avcodec_alloc_context3 == 0)
		return;
	if (FFMPEGDecoder_avcodec_free_context == 0)
		return;
	if (FFMPEGDecoder_avcodec_open2 == 0)
		return;
	if (FFMPEGDecoder_avcodec_close == 0)
		return;
	if (FFMPEGDecoder_avcodec_decode_audio4 == 0)
		return;
#endif
	FFMPEGDecoder_avcodec_register_all();
	Core::CoreAddVideoDecFunc(FFMPEGDecoder_DecodeVideo);
	Core::CoreAddAudioDecFunc(FFMPEGDecoder_DecodeAudio);
}
#else
void Media::Decoder::FFMPEGDecoder::Enable()
{
}
#endif
