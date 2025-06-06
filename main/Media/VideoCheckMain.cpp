#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/StmData/FileData.h"
#include "Media/Decoder/FFMPEGDecoder.h"
#include "Media/MediaFile.h"
#include "Media/VideoChecker.h"
#include "Parser/FullParserList.h"

#include <unistd.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
}

const char *filter_descr = "scale=78:24,transpose=cclock";
/* other way:
   scale=78:24 [scl]; [scl] transpose=cclock // assumes "[in]" and "[out]" to be input output pads respectively
 */
static AVFormatContext *fmt_ctx;
static AVCodecContext *dec_ctx;
static int video_stream_index = -1;
static int64_t last_pts = AV_NOPTS_VALUE;
static int open_input_file(const char *filename)
{
    int ret;
    AVCodec *dec;
    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }
    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }
    /* select the video stream */
    ret = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
        return ret;
    }
	printf("Codec %s (%s) Cap = %x\r\n", dec->name, dec->long_name, dec->capabilities);
    video_stream_index = ret;
    /* create decoding context */
    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx)
        return AVERROR(ENOMEM);
    avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
    av_opt_set_int(dec_ctx, "refcounted_frames", 1, 0);

	printf("Extra size = %d\r\n", dec_ctx->extradata_size);
    /* init the video decoder */
    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
        return ret;
    }
    return 0;
}

int sample_decode(const Char *fileName)
{
    int ret;
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    AVFrame *filt_frame = av_frame_alloc();
    if (!frame || !filt_frame) {
        perror("Could not allocate frame");
        return 1;
    }
    av_register_all();
    if ((ret = open_input_file(fileName)) < 0)
        goto end;
    /* read all packets */
    while (1) {
        if ((ret = av_read_frame(fmt_ctx, &packet)) < 0)
            break;
        if (packet.stream_index == video_stream_index) {
			printf("frame size = %d\r\n", packet.size);
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(packet.data, 32, ' ', Text::LineBreakType::CRLF);
				sb.AppendC(UTF8STRC("\r\n...\r\n"));
				sb.AppendHexBuff(packet.data + packet.size - 16, 32, ' ', Text::LineBreakType::CRLF);
				printf("%s\r\n", sb.ToString());
			}
            ret = avcodec_send_packet(dec_ctx, &packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                break;
            }
            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
                    goto end;
                }
                if (ret >= 0) {
                    frame->pts = frame->best_effort_timestamp;
                    av_frame_unref(frame);
                }
            }
        }
        av_packet_unref(&packet);
    }
end:
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    av_frame_free(&frame);
    av_frame_free(&filt_frame);
    return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Media::Decoder::FFMPEGDecoder::Enable();
	const UTF8Char *fileName = (const UTF8Char*)"test.mp4";
//	return sample_decode((const Char*)fileName);
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		fileName = argv[1];
	}

	IO::ConsoleWriter *console;
	Parser::ParserList *parsers;
	Media::VideoChecker *checker;
	IO::StmData::FileData *fd;
	Media::MediaFile *mediaFile;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(parsers, Parser::FullParserList());
	NEW_CLASS(checker, Media::VideoChecker(false));
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	mediaFile = (Media::MediaFile*)parsers->ParseFileType(fd, IO::ParserType::MediaFile);
	DEL_CLASS(fd);

	if (mediaFile)
	{
		if (checker->IsValid(mediaFile))
		{
			console->WriteLine(CSTR("Video file is valid");
		}
		else
		{
			console->WriteLine(CSTR("Video file is not valid");
		}
		DEL_CLASS(mediaFile);
	}
	else
	{
		console->WriteLine(CSTR("Error in parsing file");
	}
	

	DEL_CLASS(checker);
	DEL_CLASS(parsers);
	DEL_CLASS(console);
	return 0;
}
