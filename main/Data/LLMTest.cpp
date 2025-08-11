#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "Net/HTTPUtil.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Text/CString.h"
#include "Text/RegEx.h"
#include "Text/StringTool.h"
#include "Text/TextWriteUtil.h"

Int32 TestPage2_7()
{
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	Text::CStringNN url = CSTR("https://raw.githubusercontent.com/rasbt/LLMs-from-scratch/main/ch02/01_main-chapter-code/the-verdict.txt");
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	NN<IO::Stream> f;
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	NN<Text::String> s2;
	if (Net::HTTPUtil::DownloadAndOpen(clif, ssl, url, filePath, 1048576).SetTo(f))
	{
		rawText = f->ReadAsString();
		f.Delete();
	}
	if (rawText.SetTo(s))
	{
		printf("Total number of character: %d\r\n", (UInt32)s->leng);
		s2 = Text::String::NewSubstr(s->ToCString(), 0, 100);
		printf("%s\r\n", s2->v.Ptr());
		s2->Release();
		s->Release();
	}

	ssl.Delete();
	return 0;
}

Int32 TestPage2_9_1()
{
	IO::ConsoleWriter console;
	Text::CStringNN text = CSTR("Hello, world. This, is a test.");
	Data::ArrayListStringNN result;
	Text::RegEx("(\\s)").Split(text, result);
	Text::TextWriteUtil::WriteArray(console, result);
	result.FreeAll();
	return 0;
}

Int32 TestPage2_9_2()
{
	IO::ConsoleWriter console;
	Text::CStringNN text = CSTR("Hello, world. This, is a test.");
	Data::ArrayListStringNN result;
	Text::RegEx("([,.]|\\s)").Split(text, result);
	Text::TextWriteUtil::WriteArray(console, result);
	Text::StringTool::RemoveEmptyOrWS(result);
	console.WriteLine();
	Text::TextWriteUtil::WriteArray(console, result);
	result.FreeAll();
	return 0;
}

Int32 TestPage2_10()
{
	IO::ConsoleWriter console;
	Text::CStringNN text = CSTR("Hello, world. This-- a test?");
	Data::ArrayListStringNN result;
	Text::RegEx("([,.:;?_!\"()\\']|--|\\s)").Split(text, result);
	Text::StringTool::RemoveEmptyOrWS(result);
	Text::TextWriteUtil::WriteArray(console, result);
	result.FreeAll();
	return 0;
}

Int32 TestPage2_11()
{
	IO::ConsoleWriter console;
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	NN<Text::String> s2;
	{
		IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			rawText = fs.ReadAsString();
		}
	}
	if (rawText.SetTo(s))
	{
		Data::ArrayListStringNN result;
		Text::RegEx("([,.:;?_!\"()\\']|--|\\s)").Split(s->ToCString(), result);
		Text::StringTool::RemoveEmptyOrWS(result);
		printf("%d\r\n", (UInt32)result.GetCount());
		UOSInt i = result.GetCount();
		while (i-- > 30)
		{
			if (result.RemoveAt(i).SetTo(s2)) s2->Release();
		}
		Text::TextWriteUtil::WriteArray(console, result);
		s->Release();
		result.FreeAll();
	}
	return 0;
}

Int32 TestPage2_12()
{
	IO::ConsoleWriter console;
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	{
		IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			rawText = fs.ReadAsString();
		}
	}
	if (rawText.SetTo(s))
	{
		Data::ArrayListStringNN result;
		Text::RegEx("([,.:;?_!\"()\\']|--|\\s)").Split(s->ToCString(), result);
		Text::StringTool::RemoveEmptyOrWS(result);
		s->Release();
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(result, result);
		result.RemoveDuplicates();
		printf("%d\r\n", (UInt32)result.GetCount());
		UOSInt i = 0;
		UOSInt j = result.GetCount();
		while (i < j)
		{
			printf("%s, %d\r\n", result.GetItemNoCheck(i)->v.Ptr(), (UInt32)i);
			if (i >= 50)
				break;
			i++;
		}
		result.FreeAll();
	}
	return 0;
}

class SimpleTokenizerV1
{
private:
	Data::ArrayListStringNN intToStr;
	Data::FastStringMap<UOSInt> strToInt;
	Text::RegEx regEx;
public:
	SimpleTokenizerV1(NN<Data::ArrayListStringNN> vocab) : regEx("([,.:;?_!\"()\\']|--|\\s)")
	{
		this->intToStr.AddAll(vocab);
		UOSInt i = 0;
		UOSInt j = this->intToStr.GetCount();
		NN<Text::String> s;
		while (i < j)
		{
			s = this->intToStr.GetItemNoCheck(i);
			this->strToInt.PutNN(s, i);
			i++;
		}
	}

	~SimpleTokenizerV1()
	{
		this->intToStr.FreeAll();
	}

	void Encode(Text::CStringNN text, NN<Data::ArrayList<UOSInt>> ids) const
	{
		Data::ArrayListStringNN result;
		this->regEx.Split(text, result);
		Text::StringTool::RemoveEmptyOrWS(result);
		UOSInt i = 0;
		UOSInt j = result.GetCount();
		while (i < j)
		{
			ids->Add(this->strToInt.GetNN(result.GetItemNoCheck(i)));
			i++;
		}
		result.FreeAll();
	}

	void Decode(NN<Data::ArrayList<UOSInt>> ids, NN<Text::StringBuilderUTF8> text) const
	{
		Text::CStringNN pun = CSTR(",.?\"()\\'");
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = ids->GetCount();
		while (i < j)
		{
			if (this->intToStr.GetItem(ids->GetItem(i)).SetTo(s))
			{
				if (text->leng == 0)
				{
					text->Append(s);
				}
				else if (pun.IndexOf(s->v[0]) != INVALID_INDEX)
				{
					text->Append(s);
				}
				else
				{
					text->AppendUTF8Char(' ')->Append(s);
				}
			}
			i++;
		}
	}
};

Int32 TestPage2_16()
{
	IO::ConsoleWriter console;
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	{
		IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			rawText = fs.ReadAsString();
		}
	}
	if (rawText.SetTo(s))
	{
		Data::ArrayListStringNN result;
		Text::RegEx("([,.:;?_!\"()\\']|--|\\s)").Split(s->ToCString(), result);
		Text::StringTool::RemoveEmptyOrWS(result);
		s->Release();
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(result, result);
		result.RemoveDuplicates();
		SimpleTokenizerV1 tokenizer(result);

		Text::CStringNN text = CSTR("\"It's the last he painted, you know,\" Mrs. Gisburn said with pardonable pride.");
		Data::ArrayList<UOSInt> ids;
		tokenizer.Encode(text, ids);
		Text::TextWriteUtil::WriteArrayUOS(console, ids);
		console.WriteLine();
		Text::StringBuilderUTF8 sb;
		tokenizer.Decode(ids, sb);
		console.WriteLine(sb.ToCString());
	}
	return 0;
}

class SimpleTokenizerV2
{
private:
	Data::ArrayListStringNN intToStr;
	Data::FastStringMap<UOSInt> strToInt;
	Text::RegEx regEx;
public:
	SimpleTokenizerV2(NN<Data::ArrayListStringNN> vocab) : regEx("([,.:;?_!\"()\\']|--|\\s)")
	{
		this->intToStr.AddAll(vocab);
		UOSInt i = 0;
		UOSInt j = this->intToStr.GetCount();
		NN<Text::String> s;
		while (i < j)
		{
			s = this->intToStr.GetItemNoCheck(i);
			this->strToInt.PutNN(s, i);
			i++;
		}
	}

	~SimpleTokenizerV2()
	{
		this->intToStr.FreeAll();
	}

	void Encode(Text::CStringNN text, NN<Data::ArrayList<UOSInt>> ids) const
	{
		Data::ArrayListStringNN result;
		this->regEx.Split(text, result);
		Text::StringTool::RemoveEmptyOrWS(result);
		IO::ConsoleWriter console;
		Text::TextWriteUtil::WriteArray(console, result);
		UOSInt i = 0;
		UOSInt j = result.GetCount();
		OSInt k;
		while (i < j)
		{
			k = this->strToInt.IndexOf(result.GetItemNoCheck(i));
			if (k >= 0)
				ids->Add(this->strToInt.GetItem((UOSInt)k));
			else
				ids->Add(this->strToInt.GetCount() - 1);
			i++;
		}
		result.FreeAll();
	}

	void Decode(NN<Data::ArrayList<UOSInt>> ids, NN<Text::StringBuilderUTF8> text) const
	{
		Text::CStringNN pun = CSTR(",.?\"()\\'");
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = ids->GetCount();
		while (i < j)
		{
			if (this->intToStr.GetItem(ids->GetItem(i)).SetTo(s))
			{
				if (text->leng == 0)
				{
					text->Append(s);
				}
				else if (s->leng < 4 && pun.IndexOf(s->v[0]) != INVALID_INDEX)
				{
					text->Append(s);
				}
				else
				{
					text->AppendUTF8Char(' ')->Append(s);
				}
			}
			i++;
		}
	}
};


Int32 TestPage2_19()
{
	IO::ConsoleWriter console;
	Text::CStringNN filePath = CSTR("the-verdict.txt");
	Optional<Text::String> rawText = 0;
	NN<Text::String> s;
	{
		IO::FileStream fs(filePath, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			rawText = fs.ReadAsString();
		}
	}
	if (rawText.SetTo(s))
	{
		Data::ArrayListStringNN result;
		Text::RegEx("([,.:;?_!\"()\\']|--|\\s)").Split(s->ToCString(), result);
		Text::StringTool::RemoveEmptyOrWS(result);
		s->Release();
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(result, result);
		result.RemoveDuplicates();
		result.Add(Text::String::New(CSTR("<|endoftext|>")));
		result.Add(Text::String::New(CSTR("<|unk|>")));
		SimpleTokenizerV2 tokenizer(result);

		Text::CStringNN text = CSTR("Hello, do you like tea? <|endoftext|> In the sunlit terraces of the palace.");
		Data::ArrayList<UOSInt> ids;
		tokenizer.Encode(text, ids);
		Text::TextWriteUtil::WriteArrayUOS(console, ids);
		console.WriteLine();
		Text::StringBuilderUTF8 sb;
		tokenizer.Decode(ids, sb);
		console.WriteLine(sb.ToCString());
	}
	return 0;
}
Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt page = 20191;
	switch (page)
	{
	case 20070:
		return TestPage2_7();
	case 20091:
		return TestPage2_9_1();
	case 20092:
		return TestPage2_9_2();
	case 20101:
		return TestPage2_10();
	case 20111:
		return TestPage2_11();
	case 20121:
		return TestPage2_12();
	case 20161:
		return TestPage2_16();
	case 20191:
		return TestPage2_19();
	default:
		return 0;
	}
}
