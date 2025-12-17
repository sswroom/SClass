#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Media/MMALVideoCapture.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

#include <stdio.h>
#include <mmal/mmal.h>
#include <mmal/util/mmal_util.h>
#include <mmal/util/mmal_util_params.h>
#include <mmal/util/mmal_default_components.h>

struct Media::MMALVideoCapture::ClassData
{
	MMAL_COMPONENT_T *camera;
	MMAL_PORT_T *port;
	MMAL_POOL_T *buffPool;
	Manage::HiResClock *clk;
	UInt32 frameNum;

	Bool photoMode;
	Bool isRunning;

	Media::VideoSource::FrameCallback cb;
	Media::VideoSource::FrameChangeCallback fcCb;
	AnyType userData;

	Int32 currFourcc;
	OSInt currWidth;
	OSInt currHeight;
	Int32 currRateNumer;
	Int32 currRateDenom;
};

void MMALVideoCapture_FrameCB(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;
	NN<Media::MMALVideoCapture::ClassData> info;
	if (info.Set((Media::MMALVideoCapture::ClassData *) port->userdata))
	{
		if (info->cb)
		{
			UInt32 t = Double2Int32(info->clk->GetTimeDiff() * 1000);
			mmal_buffer_header_mem_lock(buffer);
			info->cb(t, info->frameNum, &buffer->data, buffer->length, Media::VideoSource::FS_I, info->userData, Media::FT_NON_INTERLACE, Media::VideoSource::FF_REALTIME, Media::YCOFST_C_CENTER_LEFT);
			mmal_buffer_header_mem_unlock(buffer);
		}
		mmal_buffer_header_release (buffer);
		info->frameNum++;

		if (port->is_enabled)
		{
			MMAL_STATUS_T status;
			new_buffer = mmal_queue_get(info->buffPool->queue);
			if (new_buffer)
			{
				status = mmal_port_send_buffer(port, new_buffer);
				if (status != MMAL_SUCCESS)
				{
					printf("MMALVideoCapture_FrameCB: cannot send buffer\r\n");
				}
			}
			else
			{
				printf("MMALVideoCapture_FrameCB: cannot get buffer\r\n");
			}
		}
	}
}

Media::MMALVideoCapture::MMALVideoCapture(Bool photoMode)
{
	NN<ClassData> info = MemAllocNN(ClassData);
	MMAL_STATUS_T status;
	this->classData = info;
	NEW_CLASS(info->clk, Manage::HiResClock());
	info->frameNum = 0;
	info->camera = 0;
	info->port = 0;
	info->buffPool = 0;
	info->photoMode = photoMode;
	info->isRunning = false;
	info->cb = 0;
	info->fcCb = 0;
	info->userData = 0;

	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &info->camera);
	if (status != MMAL_SUCCESS)
	{
		printf("MMALVideoCapture: cannot create component\r\n");
		return;
	}
	if (info->camera->output_num < 3)
	{
		printf("MMALVideoCapture: no output found\r\n");
		return;
	}

	if (photoMode)
	{
		info->port = info->camera->output[2];
	}
	else
	{
		info->port = info->camera->output[1];
	}

	MMAL_PARAMETER_CAMERA_CONFIG_T cam_config;
	cam_config.hdr.id = MMAL_PARAMETER_CAMERA_CONFIG;
	cam_config.hdr.size = sizeof(cam_config);
	cam_config.max_stills_w = 2592;
	cam_config.max_stills_h = 1944;
	cam_config.stills_yuv422 = 0;
	cam_config.one_shot_stills = 0;
	cam_config.max_preview_video_w = 1920;
	cam_config.max_preview_video_h = 1080;
	cam_config.num_preview_video_frames = 3;
	cam_config.stills_capture_circular_buffer_height = 0;
	cam_config.fast_preview_resume = 0;
	cam_config.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC;
	mmal_port_parameter_set(info->camera->control, &cam_config.hdr);

	this->SetPreferSize(1920, 1080, *(UInt32*)"I420", 12, 30, 1);
	status = mmal_port_enable(info->port, MMALVideoCapture_FrameCB);
	if (status != MMAL_SUCCESS)
	{
//		printf("MMALVideoCapture: cannot enable port\r\n");
	}

	info->port->buffer_size = info->port->buffer_size_recommended;
	info->port->buffer_num = info->port->buffer_num_recommended;
	info->buffPool = mmal_port_pool_create(info->port, info->port->buffer_num, info->port->buffer_size);
	if (info->buffPool == 0)
	{
		printf("MMALVideoCapture: cannot create pool\r\n");
	}
	info->port->userdata = (struct MMAL_PORT_USERDATA_T*)info;

	status = mmal_component_enable(info->camera);
	if (status != MMAL_SUCCESS)
	{
		printf("MMALVideoCapture: cannot enable camera\r\n");
	}
}

Media::MMALVideoCapture::~MMALVideoCapture()
{
	NN<ClassData> info = this->classData;
	if (info->isRunning)
	{
		this->Stop();
	}

	if (info->port)
	{
		if (info->port->is_enabled)
		{
			mmal_port_disable(info->port);
		}

		if (info->buffPool)
		{
			mmal_port_pool_destroy(info->port, info->buffPool);
		}
	}

	if (info->camera)
	{
		mmal_component_destroy(info->camera);
		info->camera = 0;
	}
	DEL_CLASS(info->clk);
	MemFree(info);
}

UTF8Char *Media::MMALVideoCapture::GetSourceName(UTF8Char *buff)
{
	return Text::StrConcatC(buff, UTF8STRC("Broadcom VC Camera"));
}

Text::CString Media::MMALVideoCapture::GetFilterName()
{
	return CSTR("MMALVideoCapture");
}

Bool Media::MMALVideoCapture::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	NN<ClassData> info = this->classData;
	finfo->dispSize.x = info->currWidth;
	finfo->dispHeight = info->currHeight;
	if (info->currWidth & 31)
	{
		finfo->storeWidth = info->currWidth + 32 - (info->currWidth & 31);
	}
	else
	{
		finfo->storeWidth = info->currWidth;
	}
	if (info->currHeight & 15)
	{
		finfo->storeHeight = info->currHeight + 16 - (info->currHeight & 15);
	}
	else
	{
		finfo->storeHeight = info->currHeight;
	}
	finfo->fourcc = info->currFourcc;
	finfo->storeBPP = 12;
	finfo->pf = Media::PF_UNKNOWN;
	finfo->byteSize = 0;
	finfo->par2 = 1.0;
	finfo->hdpi = 72;
	finfo->vdpi = 72;
	finfo->ftype = Media::FT_NON_INTERLACE;
	finfo->atype = Media::AT_NO_ALPHA;
	finfo->color->SetCommonProfile(Media::ColorProfile::CPT_SRGB);
	finfo->yuvType = Media::ColorProfile::YUVT_BT601;
	finfo->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	frameRateNorm.Set(info->currRateNumer);
	frameRateDenorm.Set(info->currRateDenom);
	maxFrameSize.Set((info->currWidth * info->currHeight * 3) >> 1);
	return true;
}

Bool Media::MMALVideoCapture::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	NN<ClassData> info = this->classData;
	info->cb = cb;
	info->fcCb = fcCb;
	info->userData = userData;
	return true;
}

Bool Media::MMALVideoCapture::Start()
{
	NN<ClassData> info = this->classData;
	if (info->isRunning)
		return true;

	if (info->port == 0)
		return false;

	if (info->buffPool == 0)
		return false;

	info->frameNum = 0;
	info->clk->Start();
	if (mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS )
	{
		return false;
	}

	info->isRunning = true;

	OSInt num = mmal_queue_length(info->buffPool->queue);
	OSInt i = 0;
	while (i < num)
	{
		MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(info->buffPool->queue);

		if (buffer)
		{
			if (mmal_port_send_buffer(info->port, buffer) != MMAL_SUCCESS)
			{
				printf("MMALVideoCapture: error in send buffer\r\n");
			}
		}
		else
		{
			printf("MMALVideoCapture: error in geting buffer\r\n");
		}

		i++;
	}
	return true;
}

void Media::MMALVideoCapture::Stop()
{
	NN<ClassData> info = this->classData;
	if (!info->isRunning)
		return;
	mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 0);
	info->isRunning = false;
}

Bool Media::MMALVideoCapture::IsRunning()
{
	NN<ClassData> info = this->classData;
	return info->isRunning;
}

void Media::MMALVideoCapture::SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom)
{
	MMAL_ES_FORMAT_T *format;
	MMAL_STATUS_T status;
	NN<ClassData> info = this->classData;
	UInt32 inpfcc = fourcc;
	if (fourcc == *(UInt32*)"I420")
	{
		fourcc = MMAL_ENCODING_I420;
	}
	format = info->port->format;
	format->encoding_variant = fourcc;
	format->encoding = fourcc;
	format->es->video.width = width;
	format->es->video.height = height;
	format->es->video.crop.x = 0;
	format->es->video.crop.y = 0;
	format->es->video.crop.width = width;
	format->es->video.crop.height = height;
	format->es->video.frame_rate.num = frameRateNumer;
	format->es->video.frame_rate.den = frameRateDenom;
	mmal_port_disable(info->port);
	status = mmal_port_format_commit(info->port);
	mmal_port_enable(info->port, MMALVideoCapture_FrameCB);

	if (status == MMAL_SUCCESS)
	{
		info->currWidth = width;
		info->currHeight = height;
		info->currRateNumer = frameRateNumer;
		info->currRateDenom = frameRateDenom;
		info->currFourcc = inpfcc;
	}
}

UOSInt Media::MMALVideoCapture::GetSupportedFormats(UnsafeArray<VideoFormat> fmtArr, UOSInt maxCnt)
{
	NN<ClassData> info = this->classData;
	OSInt ret = 0;
	if (info->photoMode)
	{
		if (maxCnt > 0)
		{
			fmtArr[0].info.dispSize.x = 2592;
			fmtArr[0].info.dispSize.y = 1944;
			fmtArr[0].info.storeSize.x = 2592;
			fmtArr[0].info.storeSize.y = 1952;
			fmtArr[0].frameRateNorm = 15;
			fmtArr[0].frameRateDenorm = 1;
			ret = 1;
		}
	}
	else
	{
		if (maxCnt > 0)
		{
			fmtArr[0].info.dispSize.x = 1920;
			fmtArr[0].info.dispSize.y = 1080;
			fmtArr[0].info.storeSize.x = 1920;
			fmtArr[0].info.storeSize.y = 1088;
			fmtArr[0].frameRateNorm = 30;
			fmtArr[0].frameRateDenorm = 1;
			ret++;
		}
		if (maxCnt > 1)
		{
			fmtArr[1].info.dispSize.x = 1296;
			fmtArr[1].info.dispSize.y = 972;
			fmtArr[1].info.storeSize.x = 1312;
			fmtArr[1].info.storeSize.y = 976;
			fmtArr[1].frameRateNorm = 42;
			fmtArr[1].frameRateDenorm = 1;
			ret++;
		}
		if (maxCnt > 2)
		{
			fmtArr[2].info.dispSize.x = 1296;
			fmtArr[2].info.dispSize.y = 730;
			fmtArr[2].info.storeSize.x = 1312;
			fmtArr[2].info.storeSize.y = 736;
			fmtArr[2].frameRateNorm = 49;
			fmtArr[2].frameRateDenorm = 1;
			ret++;
		}
		if (maxCnt > 3)
		{
			fmtArr[3].info.dispSize.x = 1296;
			fmtArr[3].info.dispSize.y = 730;
			fmtArr[3].info.storeSize.x = 1312;
			fmtArr[3].info.storeSize.y = 736;
			fmtArr[3].frameRateNorm = 30;
			fmtArr[3].frameRateDenorm = 1;
			ret++;
		}
		if (maxCnt > 4)
		{
			fmtArr[4].info.dispSize.x = 640;
			fmtArr[4].info.dispSize.y = 480;
			fmtArr[4].info.storeSize.x = 640;
			fmtArr[4].info.storeSize.y = 480;
			fmtArr[4].frameRateNorm = 90;
			fmtArr[4].frameRateDenorm = 1;
			ret++;
		}
		if (maxCnt > 5)
		{
			fmtArr[5].info.dispSize.x = 640;
			fmtArr[5].info.dispSize.y = 480;
			fmtArr[5].info.dispSize.x = 640;
			fmtArr[5].info.dispSize.y = 480;
			fmtArr[5].frameRateNorm = 60;
			fmtArr[5].frameRateDenorm = 1;
			ret++;
		}
	}
	OSInt i = 0;
	while (i < ret)
	{
		fmtArr[i].info.fourcc = *(UInt32*)"I420";
		fmtArr[i].info.storeBPP = 12;
		fmtArr[i].info.pf = Media::PF_UNKNOWN;
		fmtArr[i].info.byteSize = 0;
		fmtArr[i].info.par2 = 1.0;
		fmtArr[i].info.hdpi = 72;
		fmtArr[i].info.vdpi = 72;
		fmtArr[i].info.ftype = Media::FT_NON_INTERLACE;
		fmtArr[i].info.atype = Media::AT_NO_ALPHA;
		fmtArr[i].info.color->SetCommonProfile(Media::ColorProfile::CPT_SRGB);
		fmtArr[i].info.yuvType = Media::ColorProfile::YUVT_BT601;
		fmtArr[i].info.ycOfst = Media::YCOFST_C_CENTER_LEFT;

		i++;
	}
	return ret;
}

void Media::MMALVideoCapture::GetInfo(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("MMAL VideoCapture"));
}

UOSInt Media::MMALVideoCapture::GetDataSeekCount()
{
	return 0;
}

Bool Media::MMALVideoCapture::IsAvailable()
{
	OSInt outputNum;
	MMAL_COMPONENT_T *camera = 0;
	MMAL_STATUS_T status;
	status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);
	if (status != MMAL_SUCCESS)
	{
		return false;
	}
	outputNum = camera->output_num;
	printf("Output count = %d\r\n", camera->output_num);
	mmal_component_destroy(camera);
	return outputNum > 0;
}
