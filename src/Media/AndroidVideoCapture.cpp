#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Media/AndroidVideoCapture.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

#include <stdio.h>

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24
#include <camera/NdkCameraManager.h>
#include <media/NdkImage.h>
#include <media/NdkImageReader.h>

void AndroidVideoCapture_OnDevDisconn(void *context, ACameraDevice *device)
{
	printf("Camera Disconnected\r\n");
}

void AndroidVideoCapture_OnDevError(void *context, ACameraDevice *device, int error)
{
	printf("Camera Error, code = %d\r\n", error);
}

void AndroidVideoCapture_OnImage(void *context, AImageReader *reader)
{
	AImage *image;
	printf("OnImage\r\n");
	if (AImageReader_acquireNextImage(reader, &image))
	{
		printf("Image received\r\n");
		AImage_delete(image);
	}
}

void AndroidVideoCapture_OnSessActive(void *context, ACameraCaptureSession *session)
{

}

void AndroidVideoCapture_OnSessReady(void *context, ACameraCaptureSession *session)
{
	
}

void AndroidVideoCapture_OnSessClosed(void *context, ACameraCaptureSession *session)
{
	
}

Media::AndroidVideoCapture::AndroidVideoCapture(void *cameraMgr, const Char *cameraId)
{
	this->cameraMgr = cameraMgr;
	this->cameraId = cameraId;
	this->camWidth = 0;
	this->camHeight = 0;
	this->camFourcc = 0;

	this->cb = 0;
	this->fcCb = 0;
	this->userData = 0;
	this->started = false;
}

Media::AndroidVideoCapture::~AndroidVideoCapture()
{
	this->Stop();
}

UTF8Char *Media::AndroidVideoCapture::GetSourceName(UTF8Char *buff)
{
	return Text::StrConcat(buff, (const UTF8Char*)this->cameraId);
}

Text::CString Media::AndroidVideoCapture::GetFilterName()
{
	return CSTR("AndroidVideoCapture");
}

Bool Media::AndroidVideoCapture::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	if (this->camWidth == 0 || this->camHeight == 0)
		return false;

	info->dispWidth = this->camWidth;;
	info->dispHeight = this->camHeight;
	info->storeWidth = info->dispWidth;
	info->storeHeight = info->dispHeight;
	info->fourcc = this->camFourcc;
	info->storeBPP = 12;
	info->pf = Media::PF_UNKNOWN;
	info->byteSize = 0;
	info->par2 = 1;
	info->hdpi = 72;
	info->vdpi = 72;
	info->ftype = Media::FT_NON_INTERLACE;
	info->atype = Media::AT_NO_ALPHA;
	info->color->SetCommonProfile(Media::ColorProfile::CPT_BT709);
	info->yuvType = Media::ColorProfile::YUVT_BT601;
	info->ycOfst = Media::YCOFST_C_CENTER_LEFT;
	*maxFrameSize = (this->camWidth * this->camHeight * info->storeBPP) >> 3;
	*frameRateNorm = 30;
	*frameRateDenorm = 1;
	return true;
}

Bool Media::AndroidVideoCapture::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
{
	this->cb = cb;
	this->fcCb = fcCb;
	this->userData = userData;
	return true;
}

Bool Media::AndroidVideoCapture::Start()
{
	if (this->started)
		return true;
	if (this->cb == 0)
		return false;
	if (this->camWidth == 0 || this->camHeight == 0)
		return false;

	Int32 fmt = 0;
	if (this->camFourcc == *(Int32*)"MJPG")
	{
		fmt = AIMAGE_FORMAT_JPEG;
	}
	else if (this->camFourcc == *(Int32*)"I420")
	{
		fmt = AIMAGE_FORMAT_YUV_420_888;
	}
	else
	{
		return false;
	}

	ACameraDevice_stateCallbacks devCb;
	devCb.context = this;
	devCb.onDisconnected = AndroidVideoCapture_OnDevDisconn;
	devCb.onError = AndroidVideoCapture_OnDevError;
	AImageReader_ImageListener listener;
	listener.context = this;
	listener.onImageAvailable = AndroidVideoCapture_OnImage;
	ACameraCaptureSession_stateCallbacks sessionStateCb;
	sessionStateCb.context = this;
	sessionStateCb.onActive = AndroidVideoCapture_OnSessActive;
	sessionStateCb.onReady = AndroidVideoCapture_OnSessReady;
	sessionStateCb.onClosed = AndroidVideoCapture_OnSessClosed;

	ACameraDevice *device;
	AImageReader *reader;
	ANativeWindow *window = 0;
	ACaptureSessionOutput *output = 0;
	ACaptureSessionOutputContainer *outputs = 0;
	ACameraCaptureSession *session = 0;
	ACaptureRequest *request = 0;
	int seqId;

	if (ACAMERA_OK != ACameraManager_openCamera((ACameraManager*)this->cameraMgr, this->cameraId, &devCb, &device))
	{
		printf("Error in opening camera\r\n");
		return false;
	}

	if (AMEDIA_OK != AImageReader_new(this->camWidth, this->camHeight, fmt, 4, &reader))
	{
		ACameraDevice_close(device);
		printf("Error in creating ImageReader\r\n");
		return false;
	}
	AImageReader_setImageListener(reader, &listener);
	AImageReader_getWindow(reader, &window);
	ACaptureSessionOutput_create(window, &output);
	ACaptureSessionOutputContainer_create(&outputs);
	ACaptureSessionOutputContainer_add(outputs, output);
	ACameraDevice_createCaptureSession(device, outputs, &sessionStateCb, &session);
	ACameraDevice_createCaptureRequest(device, TEMPLATE_RECORD, &request);
	ACameraCaptureSession_setRepeatingRequest(session, 0, 1, &request, &seqId);
	this->session = session;
	this->captureRequest = request;
	this->outputs = outputs;
	this->output = output;
	this->reader = reader;
	this->device = device;
	this->started = true;
	return true;
}

void Media::AndroidVideoCapture::Stop()
{
	if (this->started)
	{
		this->started = false;
		ACameraCaptureSession_stopRepeating((ACameraCaptureSession*)this->session);
		ACameraCaptureSession_close((ACameraCaptureSession*)this->session);
		ACaptureRequest_free((ACaptureRequest*)this->captureRequest);
		ACaptureSessionOutputContainer_free((ACaptureSessionOutputContainer*)this->outputs);
		ACaptureSessionOutput_free((ACaptureSessionOutput*)this->output);
		ACameraDevice_close((ACameraDevice*)this->device);
		AImageReader_delete((AImageReader*)this->reader);
	}
}

Bool Media::AndroidVideoCapture::IsRunning()
{
	return this->started;
}

void Media::AndroidVideoCapture::SetPreferSize(UOSInt width, UOSInt height, UInt32 fourcc, UInt32 bpp, UInt32 frameRateNumer, UInt32 frameRateDenom)
{
	this->camWidth = width;
	this->camHeight = height;
	this->camFourcc = fourcc;
}

UOSInt Media::AndroidVideoCapture::GetSupportedFormats(VideoFormat *fmtArr, UOSInt maxCnt)
{
	ACameraMetadata *metadataObj;
	if (ACAMERA_OK != ACameraManager_getCameraCharacteristics((ACameraManager*)this->cameraMgr, this->cameraId, &metadataObj))
		return 0;
	OSInt ret = 0;
	ACameraMetadata_const_entry entry = { 0 };
	if (ACAMERA_OK == ACameraMetadata_getConstEntry(metadataObj, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &entry))
	{
		OSInt i = 0;
		while (i < entry.count)
		{
			if (entry.data.i32[i + 3] == 0) //Output
			{
				Bool valid = true;
				switch (entry.data.i32[i])
				{
				case AIMAGE_FORMAT_RGBA_8888:
					fmtArr[ret].info.fourcc = 0;
					fmtArr[ret].info.storeBPP = 32;
					fmtArr[ret].info.pf = Media::PF_R8G8B8A8;
					break;
				case AIMAGE_FORMAT_RGBX_8888:
					fmtArr[ret].info.fourcc = 0;
					fmtArr[ret].info.storeBPP = 32;
					fmtArr[ret].info.pf = Media::PF_R8G8B8A8;
					break;
				case AIMAGE_FORMAT_RGB_888:
					fmtArr[ret].info.fourcc = 0;
					fmtArr[ret].info.storeBPP = 24;
					fmtArr[ret].info.pf = Media::PF_R8G8B8;
					break;
				case AIMAGE_FORMAT_RGB_565:
					fmtArr[ret].info.fourcc = 0;
					fmtArr[ret].info.storeBPP = 16;
					fmtArr[ret].info.pf = Media::PF_LE_R5G6B5;
					break;
				case AIMAGE_FORMAT_YUV_420_888:
					fmtArr[ret].info.fourcc = *(Int32*)"I420";
					fmtArr[ret].info.storeBPP = 12;
					fmtArr[ret].info.pf = Media::PF_UNKNOWN;
					break;
				case AIMAGE_FORMAT_JPEG:
					fmtArr[ret].info.fourcc = *(Int32*)"MJPG";
					fmtArr[ret].info.storeBPP = 8;
					fmtArr[ret].info.pf = Media::PF_UNKNOWN;
					break;
				case AIMAGE_FORMAT_PRIVATE:
					valid = false;
					break;
				default:
					valid = false;
					printf("Unsupported format: 0x%x\r\n", entry.data.i32[i]);
					break;
				}
				if (valid)
				{
					fmtArr[ret].info.dispWidth = entry.data.i32[i + 1];
					fmtArr[ret].info.dispHeight = entry.data.i32[i + 2];
					fmtArr[ret].info.storeWidth = fmtArr[ret].info.dispWidth;
					fmtArr[ret].info.storeHeight = fmtArr[ret].info.dispHeight;
					fmtArr[ret].info.byteSize = 0;
					fmtArr[ret].info.par2 = 1;
					fmtArr[ret].info.hdpi = 72;
					fmtArr[ret].info.vdpi = 72;
					fmtArr[ret].info.ftype = Media::FT_NON_INTERLACE;
					fmtArr[ret].info.atype = Media::AT_NO_ALPHA;
					fmtArr[ret].info.color->SetCommonProfile(Media::ColorProfile::CPT_BT709);
					fmtArr[ret].info.yuvType = Media::ColorProfile::YUVT_BT601;
					fmtArr[ret].info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
					fmtArr[ret].frameRateNorm = 30;
					fmtArr[ret].frameRateDenorm = 1;
					ret++;
					if (ret >= maxCnt)
						break;
				}
			}
			i += 4;
		}
	}
	ACameraMetadata_free(metadataObj);
	return ret;
}

void Media::AndroidVideoCapture::GetInfo(Text::StringBuilderUTF8 *sb)
{
	ACameraMetadata *metadataObj;
	if (ACAMERA_OK != ACameraManager_getCameraCharacteristics((ACameraManager*)this->cameraMgr, this->cameraId, &metadataObj))
	{
		printf("Cannot get Camera Characteristics\r\n");
		return;
	}
	ACameraMetadata_const_entry entry = { 0 };
	int32_t nEntries;
	const uint32_t *tags;
	printf("Getting All tags\r\n");
	if (ACAMERA_OK == ACameraMetadata_getAllTags(metadataObj, &nEntries, &tags))
	{
		OSInt i = 0;
		while (i < nEntries)
		{
			if (ACAMERA_OK == ACameraMetadata_getConstEntry(metadataObj, tags[i], &entry))
			{
				OSInt j;
				switch (tags[i])
				{
				case ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS:
					sb->AppendC(UTF8STRC("Available Format:\r\n"));
					j = 0;
					while (j < entry.count)
					{
						sb->AppendC(UTF8STRC("-0x"));
						sb->AppendHex32V(entry.data.i32[j]);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendI32(entry.data.i32[j + 1]);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendI32(entry.data.i32[j + 2]);
						sb->AppendC(UTF8STRC(", "));
						sb->AppendI32(entry.data.i32[j + 3]);
						sb->AppendC(UTF8STRC("\r\n"));
						j += 4;
					}
					break;
				case ACAMERA_INFO_SUPPORTED_HARDWARE_LEVEL:
					sb->AppendC(UTF8STRC("Supported Hardware Level: Count = "));
					sb->AppendU32(entry.count);
					sb->AppendC(UTF8STRC(", "));
					sb->AppendU16(entry.data.u8[0]);
					sb->AppendC(UTF8STRC("\r\n"));
					break;
				default:
					sb->AppendC(UTF8STRC("Unknown Info: 0x"));
					sb->AppendHex32V(tags[i]);
					sb->AppendC(UTF8STRC("\r\n"));
					break;
				}
			}
			i++;
		}
	}
	ACameraMetadata_free(metadataObj);
}

UOSInt Media::AndroidVideoCapture::GetDataSeekCount()
{
	return 0;
}

Media::AndroidVideoCaptureMgr::AndroidVideoCaptureMgr()
{
	this->cameraMgr = ACameraManager_create();
	if (this->cameraMgr)
	{
		ACameraIdList *cameraIdList;
		if (ACAMERA_OK == ACameraManager_getCameraIdList((ACameraManager*)this->cameraMgr, &cameraIdList))
		{
			this->cameraIdList = cameraIdList;
		}
		else
		{
			this->cameraIdList = 0;
		}
		
	}
}

Media::AndroidVideoCaptureMgr::~AndroidVideoCaptureMgr()
{
	if (this->cameraMgr)
	{
		if (this->cameraIdList)
		{
			ACameraManager_deleteCameraIdList((ACameraIdList*)this->cameraIdList);
		}
		ACameraManager_delete((ACameraManager *)this->cameraMgr);
	}
}

OSInt Media::AndroidVideoCaptureMgr::GetDeviceList(Data::ArrayList<UInt32> *devList)
{
	if (this->cameraMgr && this->cameraIdList)
	{
		ACameraIdList *cameraIdList = (ACameraIdList*)this->cameraIdList;
		UInt32 i = 0;
		while (i < cameraIdList->numCameras)
		{
			devList->Add(i);
			i++;
		}
		return cameraIdList->numCameras;
	}
	return 0;
}

UTF8Char *Media::AndroidVideoCaptureMgr::GetDeviceName(UTF8Char *buff, UOSInt devId)
{
	if (this->cameraMgr && this->cameraIdList)
	{
		ACameraIdList *cameraIdList = (ACameraIdList*)this->cameraIdList;
		if (devId < cameraIdList->numCameras)
		{
			return Text::StrConcat(buff, (const UTF8Char*)cameraIdList->cameraIds[devId]);
		}
		return 0;
	}
	return 0;
}

Media::IVideoCapture *Media::AndroidVideoCaptureMgr::CreateDevice(UOSInt devId)
{
	if (this->cameraMgr && this->cameraIdList)
	{
		ACameraIdList *cameraIdList = (ACameraIdList*)this->cameraIdList;
		if (devId < cameraIdList->numCameras)
		{
			Media::AndroidVideoCapture *capture;
			NEW_CLASS(capture, Media::AndroidVideoCapture(this->cameraMgr, cameraIdList->cameraIds[devId]));
			return capture;
		}
		return 0;
	}
	return 0;
}

#else
Media::AndroidVideoCaptureMgr::AndroidVideoCaptureMgr()
{

}

Media::AndroidVideoCaptureMgr::~AndroidVideoCaptureMgr()
{

}

UOSInt Media::AndroidVideoCaptureMgr::GetDeviceList(Data::ArrayList<UInt32> *devList)
{
	return 0;
}

UTF8Char *Media::AndroidVideoCaptureMgr::GetDeviceName(UTF8Char *buff, UOSInt devId)
{
	return 0;
}

Media::IVideoCapture *Media::AndroidVideoCaptureMgr::CreateDevice(UOSInt devId)
{
	return 0;
}
#endif

