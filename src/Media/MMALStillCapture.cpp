#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/MMALStillCapture.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

#include <wchar.h>
#include <mmal/mmal.h>
#include <mmal/util/mmal_connection.h>
#include <mmal/util/mmal_default_components.h>
#include <mmal/util/mmal_util.h>
#include <mmal/util/mmal_util_params.h>

typedef struct
{
	MMAL_COMPONENT_T *camera;
	MMAL_PORT_T *port;
	MMAL_POOL_T *buffPool;
	MMAL_COMPONENT_T *encoder;
	MMAL_PORT_T *encoderInputPort;
	MMAL_PORT_T *encoderOutputPort;
	MMAL_CONNECTION_T *connection;
	Bool running;

	Bool imageEnd;
	IO::Stream *stm;
	Sync::Event *readEvt;
	Bool succ;
	Bool start;
	Bool nextStart;
} MMALInfo;

void MMALStillCapture_ControlCB(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	mmal_buffer_header_release(buffer);
}

void MMALStillCapture_BufferCB(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
	MMAL_BUFFER_HEADER_T *new_buffer;
	MMALInfo *info = (MMALInfo *) port->userdata;

	Int32 leng = 0;
	mmal_buffer_header_mem_lock(buffer);
	leng = buffer->length;
	if (info->stm && !info->succ)
	{
		info->stm->Write(buffer->data, buffer->length);
	}
	mmal_buffer_header_mem_unlock(buffer);
	mmal_buffer_header_release (buffer);

	if (leng > 0 && leng < 81920)
	{
		if (info->stm && !info->succ)
		{
			info->succ = true;
			info->stm = 0;
		}
		info->imageEnd = true;
		info->readEvt->Set();
	}

	if (port->is_enabled)
	{
		MMAL_STATUS_T status;
		new_buffer = mmal_queue_get(info->buffPool->queue);
		if (new_buffer)
		{
			status = mmal_port_send_buffer(port, new_buffer);
			if (status != MMAL_SUCCESS)
			{
				wprintf(L"MMALVideoCapture_BufferCB: cannot send buffer\r\n");
			}
		}
		else
		{
			wprintf(L"MMALVideoCapture_FrameCB: cannot get buffer\r\n");
		}
	}
}

Media::MMALStillCapture::MMALStillCapture()
{
	MMALInfo *info = MemAlloc(MMALInfo, 1);
	this->classData = info;
	NEW_CLASS(info->readEvt, Sync::Event(true));
	info->camera = 0;
	info->port = 0;
	info->buffPool = 0;
	info->encoder = 0;
	info->encoderInputPort = 0;
	info->encoderOutputPort = 0;
	info->connection = 0;
	info->stm = 0;
	info->start = false;
	info->nextStart = true;
	info->running = false;
	if (mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &info->camera) != 0)
	{
		wprintf(L"Error in create component\r\n");
		return;
	}
	if (info->camera->output_num < 2)
	{
		wprintf(L"Camera is not enabled\r\n");
		return;
	}
	
	info->port = info->camera->output[2];
}

Media::MMALStillCapture::~MMALStillCapture()
{
	MMALInfo *info =  (MMALInfo*)this->classData;
	this->DeviceEnd();
	if (info->camera)
	{
                mmal_component_destroy(info->camera);
		info->camera = 0;
	}
	DEL_CLASS(info->readEvt);
	MemFree(info);
}

Bool Media::MMALStillCapture::DeviceBegin()
{
	MMALInfo *info =  (MMALInfo*)this->classData;
	if (info->camera == 0)
	{
		return false;
	}
	if (mmal_port_enable(info->camera->control, MMALStillCapture_ControlCB))
	{
		wprintf(L"Cannot enable control\r\n|");
		return false;
	}
	MMAL_PARAMETER_CAMERA_CONFIG_T cam_config;
	cam_config.hdr.id = MMAL_PARAMETER_CAMERA_CONFIG;
	cam_config.hdr.size = sizeof(cam_config);
	cam_config.max_stills_w = 2592;
	cam_config.max_stills_h = 1944;
	cam_config.stills_yuv422 = 0;
	cam_config.one_shot_stills = 1;
	cam_config.max_preview_video_w = 2592;
	cam_config.max_preview_video_h = 1944;
	cam_config.num_preview_video_frames = 3;
	cam_config.stills_capture_circular_buffer_height = 0;
	cam_config.fast_preview_resume = 0;
	cam_config.use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC;
	mmal_port_parameter_set(info->camera->control, &cam_config.hdr);

	MMAL_ES_FORMAT_T *format;
	format = info->port->format;
	format->encoding = MMAL_ENCODING_OPAQUE;
	format->es->video.width = 2592;
	format->es->video.height = 1944;
	format->es->video.crop.x = 0;
	format->es->video.crop.y = 0;
	format->es->video.crop.width = 2592;
	format->es->video.crop.height = 1944;
	format->es->video.frame_rate.num = 3;
	format->es->video.frame_rate.den = 1;
	if (info->port->buffer_size < info->port->buffer_size_min)
		info->port->buffer_size = info->port->buffer_size_min;
	info->port->buffer_num = info->port->buffer_num_recommended;
	if (mmal_port_format_commit(info->port))
	{
		wprintf(L"Cannot commit format\r\n|");
	}
	
	if (mmal_component_enable(info->camera))
	{
		wprintf(L"Cannot enable control\r\n|");
		return false;
	}

//	info->buffPool = mmal_port_pool_create(info->port, info->port->buffer_num, info->port->buffer_size );

	if (mmal_component_create ( MMAL_COMPONENT_DEFAULT_IMAGE_ENCODER, &info->encoder))
	{
		wprintf(L"Cannot create encoding\r\n|");
		return false;
	}
	if (info->encoder->input_num <= 0 || info->encoder->output_num <= 0)
	{
		wprintf(L"Encoder is not valid\r\n|");
		return false;
	}

	info->encoderInputPort = info->encoder->input[0];
	info->encoderOutputPort = info->encoder->output[0];

	mmal_format_copy(info->encoderOutputPort->format, info->encoderInputPort->format );
	info->encoderOutputPort->format->encoding = MMAL_ENCODING_JPEG;
	info->encoderOutputPort->buffer_size = info->encoderOutputPort->buffer_size_recommended;
	if (info->encoderOutputPort->buffer_size < info->encoderOutputPort->buffer_size_min)
		info->encoderOutputPort->buffer_size = info->encoderOutputPort->buffer_size_min;
	info->encoderOutputPort->buffer_num = info->encoderOutputPort->buffer_num_recommended;
	if (info->encoderOutputPort->buffer_num < info->encoderOutputPort->buffer_num_min)
		info->encoderOutputPort->buffer_num = info->encoderOutputPort->buffer_num_min;

	if (mmal_port_format_commit(info->encoderOutputPort ) )
	{
		wprintf(L"Cannot set encoder format\r\n|");
		return false;
	}
	if (mmal_component_enable(info->encoder))
	{
		wprintf(L"Cannot set encoder format\r\n|");
		return false;
	}
	info->buffPool = mmal_port_pool_create(info->encoderOutputPort, info->encoderOutputPort->buffer_num, info->encoderOutputPort->buffer_size);
	if (info->buffPool == 0)
	{
		wprintf(L"Cannot create pool\r\n|");
		return false;
	}
	
	info->encoderOutputPort->userdata = (struct MMAL_PORT_USERDATA_T *)info;

	MMAL_STATUS_T status =  mmal_connection_create(&info->connection, info->port, info->encoderInputPort, MMAL_CONNECTION_FLAG_TUNNELLING | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);
	if (status == MMAL_SUCCESS)
	{
		status =  mmal_connection_enable(info->connection);
		if (status != MMAL_SUCCESS)
		{
			mmal_connection_destroy(info->connection);
			info->connection = 0;
			wprintf(L"Cannot enable connection\r\n|");
			return false;
		}
	}
	else
	{
		wprintf(L"Cannot create connection\r\n|");
		return false;
	}

	return true;
}

void Media::MMALStillCapture::DeviceEnd()
{
	MMALInfo *info =  (MMALInfo*)this->classData;
	if (info->connection)
	{
		mmal_connection_disable(info->connection);
		mmal_connection_destroy(info->connection);
		info->connection = 0;
	}
	if (info->encoder)
	{
		mmal_component_disable(info->encoder);
	}
	if (info->camera)
	{
		mmal_port_disable(info->camera->control);
		mmal_component_disable(info->camera);
	}
	if (info->connection)
	{
		mmal_connection_destroy(info->connection);
		info->connection = 0;
	}
	if (info->buffPool)
	{
		mmal_port_pool_destroy(info->encoderOutputPort, info->buffPool);
		info->buffPool = 0;
	}
	if (info->encoder)
	{
		mmal_component_destroy(info->encoder);
		info->encoderInputPort = 0;
		info->encoderOutputPort = 0;
		info->encoder = 0;
	}
}

Bool Media::MMALStillCapture::CapturePhoto(Media::IPhotoCapture::PhotoFormat *fmt, IO::Stream *stm)
{
	MMALInfo *info =  (MMALInfo*)this->classData;
	if (info->connection == 0)
	{
		return false;
	}

	if (mmal_port_enable(info->encoderOutputPort, MMALStillCapture_BufferCB) != MMAL_SUCCESS) 
	{
		wprintf(L"Cannot enable encoder port\r\n");
		return false;
	}
	OSInt num = mmal_queue_length(info->buffPool->queue);
	OSInt i = 0;
	while (i < num)
	{
		MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get (info->buffPool->queue);
		if (buffer == 0)
		{
			wprintf(L"Cannot get buffer from pool\r\n");
		}
		else
		{
			if (mmal_port_send_buffer(info->encoderOutputPort, buffer) != MMAL_SUCCESS)
			{
				wprintf(L"Cannot send buffer to port\r\n");
			}
		}
		i++;
	}
	Manage::HiResClock clk;
	info->imageEnd = false;
 	if (mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
	{
		wprintf(L"Cannot start capture\r\n");
		mmal_port_disable(info->encoderOutputPort);
		return false;
	}
	
	clk.Start();
	while (!info->imageEnd && clk.GetTimeDiff() < 2.0)
	{
		info->readEvt->Wait(1000);
	}
	info->imageEnd = false;
 	if (mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
	{
		wprintf(L"Cannot start capture\r\n");
		mmal_port_disable(info->encoderOutputPort);
		return false;
	}
	clk.Start();
	while (!info->imageEnd && clk.GetTimeDiff() < 2.0)
	{
		info->readEvt->Wait(1000);
	}
	info->succ = false;
	info->stm = stm;
	info->imageEnd = false;
 	if (mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 1) != MMAL_SUCCESS)
	{
		wprintf(L"Cannot start capture\r\n");
		mmal_port_disable(info->encoderOutputPort);
		return false;
	}
	*fmt = PF_JPG;
	clk.Start();
	while (!info->imageEnd && clk.GetTimeDiff() < 2.0)
	{
		info->readEvt->Wait(1000);
	}

	mmal_port_parameter_set_boolean(info->port, MMAL_PARAMETER_CAPTURE, 0);
	mmal_port_disable(info->encoderOutputPort);
	return info->succ;
}

