#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ClockSpeechCh.h"
#include "Media/MediaFile.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"

void Media::ClockSpeechCh::AppendWAV(NN<Media::AudioConcatSource> source, NN<Parser::FileParser::WAVParser> parser, Text::CStringNN fileName)
{
	IO::ParsedObject *pobj;
	{
		IO::StmData::FileData fd(fileName, false);
		pobj = parser->ParseFile(fd, 0, IO::ParserType::MediaFile);
	}
	if (pobj == 0)
		return;

	if (pobj->GetParserType() == IO::ParserType::MediaFile)
	{
		Media::MediaFile *file = (Media::MediaFile *)pobj;
		NN<Media::IMediaSource> msrc;
		Int32 syncTime;
		UOSInt i = 0;
		while (file->GetStream(i++, syncTime).SetTo(msrc))
		{
			if (msrc->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
			{
				NN<Media::IAudioSource> asrc = NN<Media::IAudioSource>::ConvertFrom(msrc);
				if (source->AppendAudio(asrc))
				{
					file->KeepStream(i - 1, true);
					break;
				}
			}
		}
		DEL_CLASS(pobj);
	}
	else
	{
		DEL_CLASS(pobj);
	}
}

Media::IAudioSource *Media::ClockSpeechCh::GetSpeech(NN<Data::DateTime> time)
{
	Int32 hour = time->GetHour();
	Int32 minute = time->GetMinute();
	UTF8Char sbuff[256];
	UTF8Char *sptr;

	Parser::FileParser::WAVParser parser;
	NN<Media::AudioConcatSource> source;

	NEW_CLASSNN(source, Media::AudioConcatSource());
	
	if (hour >= 10)
	{
		if (hour >= 20)
		{
			AppendWAV(source, parser, CSTR("Num2.wav"));
		}
		AppendWAV(source, parser, CSTR("Num10.wav"));
		hour = hour % 10;
		if (hour)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Num"));
			sptr = Text::StrInt32(sptr, hour);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));
			AppendWAV(source, parser, CSTRP(sbuff, sptr));
		}
	}
	else if (hour == 2)
	{
		AppendWAV(source, parser, CSTR("Num2_.wav"));
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Num"));
		sptr = Text::StrInt32(sptr, hour);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));
		AppendWAV(source, parser, CSTRP(sbuff, sptr));
	}
	AppendWAV(source, parser, CSTR("Hour.wav"));
	if (minute)
	{
		if (minute >= 10)
		{
			if (minute >= 20)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Num"));
				sptr = Text::StrInt32(sptr, minute / 10);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));
				AppendWAV(source, parser, CSTRP(sbuff, sptr));
			}
			AppendWAV(source, parser, CSTR("Num10.wav"));
			minute = minute % 10;
			if (minute)
			{
				sptr = Text::StrConcatC(sbuff, UTF8STRC("Num"));
				sptr = Text::StrInt32(sptr, minute);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));
				AppendWAV(source, parser, CSTRP(sbuff, sptr));
			}
		}
		else
		{
			AppendWAV(source, parser, CSTR("Num0.wav"));
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Num"));
			sptr = Text::StrInt32(sptr, minute);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".wav"));
			AppendWAV(source, parser, CSTRP(sbuff, sptr));
		}
		AppendWAV(source, parser, CSTR("Minute.wav"));
	}
	else
	{
		AppendWAV(source, parser, CSTR("FullHour.wav"));
	}
	return source.Ptr();
}
