#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Media/PDFDocument.h"
#include "Parser/FileParser/PDFParser.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

struct Parser::FileParser::PDFParser::PDFParseEnv
{
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt currOfst;
	UOSInt dataSize;
	UInt64 dataOfst;
	IO::StreamData *fd;
	UInt64 lineBegin;
};

Bool Parser::FileParser::PDFParser::IsComment(const UTF8Char *buff, UOSInt size)
{
	if (buff[0] == '%')
	{
		if (Text::StrEqualsC(buff, size, UTF8STRC("%%EOF")))
			return false;
		if (Text::StrStartsWithC(buff, size, UTF8STRC("%PDF-")))
			return false;
		return true;
	}
	return false;
}

Bool Parser::FileParser::PDFParser::NextLine(PDFParseEnv *env, Text::StringBuilderUTF8 *sb, Bool skipComment)
{
	UOSInt i = env->currOfst;
	while (i < env->dataSize)
	{
		if (env->buff[i] == '\n')
		{
			if (skipComment && IsComment(&env->buff[env->currOfst], i - env->currOfst))
			{
				env->lineBegin = env->dataOfst + env->currOfst;
				env->currOfst = i + 1;
				return NextLine(env, sb, true);
			}
			sb->AppendC(&env->buff[env->currOfst], i - env->currOfst);
			env->lineBegin = env->dataOfst + env->currOfst;
			env->currOfst = i + 1;
			return true;
		}
		i++;
	}
	if (env->dataSize - env->currOfst > 1024)
	{
		return false;
	}
	env->dataOfst += env->currOfst;
	MemCopyO(env->buff, &env->buff[env->currOfst], env->dataSize - env->currOfst);
	env->dataSize -= env->currOfst;
	env->currOfst = 0;
	i = env->dataSize;
	env->dataSize += env->fd->GetRealData(env->dataOfst + env->dataSize, env->buffSize - env->dataSize, &env->buff[env->dataSize]);
	if (i == env->dataSize)
		return false;
	while (i < env->dataSize)
	{
		if (env->buff[i] == '\n')
		{
			if (skipComment && IsComment(&env->buff[env->currOfst], i - env->currOfst))
			{
				env->lineBegin = env->dataOfst + env->currOfst;
				env->currOfst = i + 1;
				return NextLine(env, sb, true);
			}
			sb->AppendC(&env->buff[env->currOfst], i - env->currOfst);
			env->lineBegin = env->dataOfst + env->currOfst;
			env->currOfst = i + 1;
			return true;
		}
		i++;
	}
	return false;
}

Bool Parser::FileParser::PDFParser::NextLineFixed(PDFParseEnv *env, UOSInt size)
{
	if (env->currOfst + size < env->dataSize)
	{
		if (env->buff[env->currOfst + size] == '\n')
		{
			env->currOfst = env->currOfst + size + 1;
			return true;
		}
		return false;
	}
	else
	{
		env->dataOfst += env->currOfst + size;
		env->dataSize = env->fd->GetRealData(env->dataOfst, env->dataSize, env->buff);
		env->currOfst = 0;
		if (env->buff[0] == '\n')
		{
			env->currOfst++;
			return true;
		}
		return false;
	}
}

Parser::FileParser::PDFParser::PDFParser()
{
}

Parser::FileParser::PDFParser::~PDFParser()
{
}

Int32 Parser::FileParser::PDFParser::GetName()
{
	return *(Int32*)"PDFP";
}

void Parser::FileParser::PDFParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PDFDocument)
	{
		selector->AddFilter(CSTR("*.pdf"), CSTR("Portable Document Format"));
	}
}

IO::ParserType Parser::FileParser::PDFParser::GetParserType()
{
	return IO::ParserType::PDFDocument;
}

IO::ParsedObject *Parser::FileParser::PDFParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (ReadNInt32(hdr) != ReadNInt32("%PDF") || hdr[4] != '-' || hdr[6] != '.')
	{
		return 0;
	}

	Media::PDFParameter *param;
	Media::PDFDocument *doc = 0;
	PDFParseEnv env;
	env.buff = MemAlloc(UInt8, 1048576);
	env.buffSize = 1048576;
	env.currOfst = 0;
	env.dataOfst = 0;
	env.dataSize = 0;
	env.fd = fd;
	Bool normalEnd = false;
	Bool succ = false;
	Text::StringBuilderUTF8 sb;
	if (!NextLine(&env, &sb, true))
	{
		MemFree(env.buff);
		return 0;
	}
	NEW_CLASS(doc, Media::PDFDocument(fd->GetFullFileName(), sb.ToCString().Substring(5)));
	sb.ClearStr();
	while (NextLine(&env, &sb, true))
	{
		if (sb.EndsWith(UTF8STRC(" 0 obj")))
		{
			UInt32 id;
			sb.RemoveChars(6);
			if (!sb.ToUInt32(&id))
			{
#if defined(VERBOSE)
				printf("PDFParser: Error in parsing object id: %s\r\n", sb.ToString());
#endif
				normalEnd = true;
				break;
			}
#if defined(VERBOSE)
			printf("PDFParser: Object found, id = %d, ofst = %lld\r\n", id, env.lineBegin);
#endif
			sb.ClearStr();
			Bool endObj = false;
			UInt64 dataOfst = 0;
			Media::PDFObject *obj = doc->AddObject(id);
			while (NextLine(&env, &sb, true))
			{
				if (sb.Equals(UTF8STRC("endobj")))
				{
					if (dataOfst != 0)
					{
						obj->SetData(env.fd, dataOfst, (UOSInt)(env.lineBegin - dataOfst));
					}
					endObj = true;
					break;
				}
				else if (sb.StartsWith(UTF8STRC("<<")))
				{
					if (sb.EndsWith(UTF8STRC(">>stream")))
					{
						param = Media::PDFParameter::Parse(Text::CString(&sb.v[2], sb.leng - 10));
						if (param == 0)
						{
#if defined(VERBOSE)
							printf("PDFParser: Error in parsing object stream parameter: %s\r\n", sb.ToString());
#endif
							normalEnd = true;
							break;
						}
						obj->SetParameter(param);
						Text::String *leng = param->GetEntry(CSTR("Length"));
						if (leng == 0)
						{
#if defined(VERBOSE)
							printf("PDFParser: Length not found in stream filters: %s\r\n", sb.ToString());
#endif
							normalEnd = true;
							break;
						}
						UOSInt iLeng;
						if (!leng->ToUOSInt(&iLeng))
						{
#if defined(VERBOSE)
							printf("PDFParser: Stream Length Filter is not valid: %s\r\n", leng->v);
#endif
							normalEnd = true;
							break;
						}
						obj->SetStream(env.fd, env.dataOfst + env.currOfst, iLeng);
						if (!NextLineFixed(&env, iLeng))
						{
#if defined(VERBOSE)
							printf("PDFParser: Stream is not ended by LF\r\n");
#endif
							normalEnd = true;
							break;
						}
						sb.ClearStr();
						if (!NextLine(&env, &sb, true))
						{
							break;
						}
						if (!sb.Equals(UTF8STRC("endstream")))
						{
#if defined(VERBOSE)
							printf("PDFParser: End of stream not found: %s\r\n", sb.ToString());
#endif
							normalEnd = true;
							break;
						}
					}
					else if (sb.EndsWith(UTF8STRC(">>")))
					{
						param = Media::PDFParameter::Parse(Text::CString(&sb.v[2], sb.leng - 4));
						if (param == 0)
						{
#if defined(VERBOSE)
							printf("PDFParser: Error in parsing object parameter: %s\r\n", sb.ToString());
#endif
							normalEnd = true;
							break;
						}
						obj->SetParameter(param);
					}
					else
					{
#if defined(VERBOSE)
						printf("PDFParser: Unknown object content: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
				}
				else
				{
					if (dataOfst == 0)
						dataOfst = env.lineBegin;
				}
				sb.ClearStr();
			}
			if (!endObj)
			{
				break;
			}
		}
		else if (sb.Equals(UTF8STRC("xref")))
		{
#if defined(VERBOSE)
			printf("PDFParser: xref found, ofst = %lld\r\n", env.lineBegin);
#endif
			while (true)
			{
				sb.ClearStr();
				if (!NextLine(&env, &sb, true))
				{
#if defined(VERBOSE)
					printf("PDFParser: xref count not found\r\n");
#endif
					normalEnd = true;
					break;
				}
				if (sb.Equals(UTF8STRC("trailer")))
				{
#if defined(VERBOSE)
					printf("PDFParser: trailer found, ofst = %lld\r\n", env.lineBegin);
#endif
					sb.ClearStr();
					if (!NextLine(&env, &sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: trailer values not found\r\n");
#endif
						normalEnd = true;
						break;
					}
					if (!sb.StartsWith(UTF8STRC("<<")) || !sb.EndsWith(UTF8STRC(">>")))
					{
#if defined(VERBOSE)
						printf("PDFParser: trailer values is not valid format: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
					sb.ClearStr();
					if (!NextLine(&env, &sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref not found\r\n");
#endif
						normalEnd = true;
						break;
					}
					if (!sb.Equals(UTF8STRC("startxref")))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref not present after trailer: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
					sb.ClearStr();
					if (!NextLine(&env, &sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref offset not found\r\n");
#endif
						normalEnd = true;
						break;
					}
					UInt64 ofst;
					if (sb.ToUInt64(&ofst))
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref offset = %lld\r\n", ofst);
#endif
					}
					else
					{
#if defined(VERBOSE)
						printf("PDFParser: startxref offset not valid: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
					sb.ClearStr();
					if (!NextLine(&env, &sb, true))
					{
#if defined(VERBOSE)
						printf("PDFParser: unexpected end of file after startxref\r\n");
#endif
						normalEnd = true;
						break;
					}
					if (sb.Equals(UTF8STRC("%%EOF")))
					{
#if defined(VERBOSE)
						printf("PDFParser: EOF found, ofst = %lld\r\n", env.lineBegin);
#endif
						normalEnd = true;
						succ = true;
						break;
					}
					else
					{
#if defined(VERBOSE)
						printf("PDFParser: Unknown entry after startxref: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
				}
				else
				{
					UOSInt i;
					UOSInt j;
					i = sb.IndexOf(' ');
					if (i == INVALID_INDEX)
					{
#if defined(VERBOSE)
						printf("PDFParser: xref count format is not valid: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
					sb.v[i] = 0;
					if (!Text::StrToUOSInt(&sb.v[i + 1], &j) || !Text::StrToUOSInt(sb.v, &i))
					{
						sb.v[i] = ' ';
#if defined(VERBOSE)
						printf("PDFParser: xref count format is not valid 2: %s\r\n", sb.ToString());
#endif
						normalEnd = true;
						break;
					}
					j += i;
					while (i < j)
					{
						sb.ClearStr();
						if (!NextLine(&env, &sb, true))
						{
#if defined(VERBOSE)
							printf("PDFParser: Error in reading xref value\r\n");
#endif
							normalEnd = true;
							break;
						}
						if ((sb.leng != 18 && sb.leng != 19) || sb.v[10] != ' ' || sb.v[16] != ' ')
						{
#if defined(VERBOSE)
							printf("PDFParser: xref value format invalid: %d, %d, %d, %s\r\n", (UInt32)sb.leng, sb.v[10], sb.v[16], sb.ToString());
#endif
							normalEnd = true;
							break;
						}
						i++;
					}
					if (normalEnd)
						break;
				}
			}
			break;
		}
		else
		{
#if defined(VERBOSE)
			printf("PDFParser: Unsupported line: %s\r\n", sb.ToString());
#endif
			normalEnd = true;
			break;
		}
		sb.ClearStr();
	}
#if defined(VERBOSE)
	if (!normalEnd)
	{
		printf("PDFParser: Error in parsing next line\r\n");
	}
#endif
	MemFree(env.buff);
	if (succ)
		return doc;
	SDEL_CLASS(doc);
	return 0;
}