#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "IO/FileStream.h"
#include "IO/YAMLParser.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

Optional<IO::ConfigFile> IO::YAMLParser::ParseStream(NN<IO::Stream> stm)
{
	NN<IO::ConfigFile> cfg;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbVal;
	Data::ArrayListStringNN tags;
	Data::ArrayListNative<UIntOS> indents;
	Text::UTF8Reader reader(stm);
	UIntOS lastIndent = INVALID_INDEX;
	UIntOS i;
	UIntOS j;
	NEW_CLASSNN(cfg, IO::ConfigFile(stm->GetSourceNameObj()));
	while (reader.ReadLine(sb, 4096))
	{
		i = 0;
		while (true)
		{
			if (sb.v[i] == ' ')
			{
				i++;
			}
			else
			{
				break;
			}
		}
		if (sb.v[i] != 0 && sb.v[i] != '#')
		{
			j = sb.IndexOf(':', i);
			if (j != INVALID_INDEX)
			{
				if (indents.GetCount() == 0 || i > lastIndent)
				{
					indents.Add(i);
					tags.Add(Text::String::New(&sb.v[i], j - i));
					lastIndent = i;
				}
				else
				{
					while (true)
					{
						lastIndent = indents.GetItem(indents.GetCount() - 1);
						if (i > lastIndent)
						{
							indents.Add(i);
							tags.Add(Text::String::New(&sb.v[i], j - i));
							lastIndent = i;
							break;
						}
						else
						{
							indents.RemoveAt(indents.GetCount() - 1);
							tags.GetItemNoCheck(tags.GetCount() - 1)->Release();
							tags.RemoveAt(tags.GetCount() - 1);
							if (indents.GetCount() == 0 || lastIndent == i)
							{
								indents.Add(i);
								tags.Add(Text::String::New(&sb.v[i], j - i));
								lastIndent = i;
								break;
							}
						}
					}
				}
				j++;
				while (sb.v[j] == ' ')
				{
					j++;
				}
				if (sb.v[j] != 0)
				{
					i = j;
					UIntOS lastWS = 0;
					sbVal.ClearStr();
					while (true)
					{
						if (sb.v[i] == 0)
						{
							break;
						}
						if (sb.v[i] == '#')
						{
							break;
						}
						if (sb.v[i] == '\\')
						{
							i++;
							if (sb.v[i] == 0)
							{
								break;
							}
							lastWS = 0;
							if (sb.v[i] == '0')
							{
								sbVal.AppendUTF8Char('\0');
							}
							else if (sb.v[i] == 'a')
							{
								sbVal.AppendUTF8Char('\a');
							}
							else if (sb.v[i] == 'b')
							{
								sbVal.AppendUTF8Char('\b');
							}
							else if (sb.v[i] == 't')
							{
								sbVal.AppendUTF8Char('\t');
							}
							else if (sb.v[i] == 'n')
							{
								sbVal.AppendUTF8Char('\n');
							}
							else if (sb.v[i] == 'v')
							{
								sbVal.AppendUTF8Char('\v');
							}
							else if (sb.v[i] == 'f')
							{
								sbVal.AppendUTF8Char('\f');
							}
							else if (sb.v[i] == 'r')
							{
								sbVal.AppendUTF8Char('\r');
							}
							else if (sb.v[i] == 'e')
							{
								sbVal.AppendUTF8Char('\e');
							}
							else if (sb.v[i] == '\"')
							{
								sbVal.AppendUTF8Char('\"');
							}
							else if (sb.v[i] == '/')
							{
								sbVal.AppendUTF8Char('/');
							}
							else if (sb.v[i] == '\\')
							{
								sbVal.AppendUTF8Char('\\');
							}
							else if (sb.v[i] == 'N')
							{
								sbVal.AppendChar(0x85, 1);
							}
							else if (sb.v[i] == '_')
							{
								sbVal.AppendChar(0xA0, 1);
							}
							else if (sb.v[i] == 'L')
							{
								sbVal.AppendChar(0x2028, 1);
							}
							else if (sb.v[i] == 'P')
							{
								sbVal.AppendChar(0x2029, 1);
							}
							else if (sb.v[i] == 'x')
							{
								i++;
								if (sb.v[i] != 0)
								{
									UInt8 c = 0;
									if (sb.v[i] >= '0' && sb.v[i] <= '9')
									{
										c = (UInt8)(sb.v[i] - '0');
									}
									else if (sb.v[i] >= 'A' && sb.v[i] <= 'F')
									{
										c = (UInt8)(sb.v[i] - 'A' + 10);
									}
									else if (sb.v[i] >= 'a' && sb.v[i] <= 'f')
									{
										c = (UInt8)(sb.v[i] - 'a' + 10);
									}
									i++;
									if (sb.v[i] != 0)
									{
										UInt8 c2 = 0;
										if (sb.v[i] >= '0' && sb.v[i] <= '9')
										{
											c2 = (UInt8)(sb.v[i] - '0');
										}
										else if (sb.v[i] >= 'A' && sb.v[i] <= 'F')
										{
											c2 = (UInt8)(sb.v[i] - 'A' + 10);
										}
										else if (sb.v[i] >= 'a' && sb.v[i] <= 'f')
										{
											c2 = (UInt8)(sb.v[i] - 'a' + 10);
										}
										c = (UInt8)((c << 4) | c2);
										sbVal.AppendChar(c, 1);
									}
								}
							}
							else if (sb.v[i] == 'u')
							{
								i++;
								if (sb.v[i] != 0)
								{
									UInt16 c = 0;
									for (Int32 k = 0; k < 4; k++)
									{
										if (sb.v[i] == 0)
										{
											break;
										}
										if (sb.v[i] >= '0' && sb.v[i] <= '9')
										{
											c = (UInt16)((c << 4) | (sb.v[i] - '0'));
										}
										else if (sb.v[i] >= 'A' && sb.v[i] <= 'F')
										{
											c = (UInt16)((c << 4) | (sb.v[i] - 'A' + 10));
										}
										else if (sb.v[i] >= 'a' && sb.v[i] <= 'f')
										{
											c = (UInt16)((c << 4) | (sb.v[i] - 'a' + 10));
										}
										i++;
									}
									sbVal.AppendChar(c, 1);
								}
							}
							else if (sb.v[i] == 'U')
							{
								i++;
								if (sb.v[i] != 0)
								{
									UTF32Char c = 0;
									for (Int32 k = 0; k < 8; k++)
									{
										if (sb.v[i] == 0)
										{
											break;
										}
										if (sb.v[i] >= '0' && sb.v[i] <= '9')
										{
											c = (UTF32Char)((c << 4) | (sb.v[i] - '0'));
										}
										else if (sb.v[i] >= 'A' && sb.v[i] <= 'F')
										{
											c = (UTF32Char)((c << 4) | (sb.v[i] - 'A' + 10));
										}
										else if (sb.v[i] >= 'a' && sb.v[i] <= 'f')
										{
											c = (UTF32Char)((c << 4) | (sb.v[i] - 'a' + 10));
										}
										i++;
									}
									sbVal.AppendChar(c, 1);
								}
							}
							else
							{
								sbVal.AppendUTF8Char(sb.v[i]);
							}
						}
						else if (sb.v[i] == ' ')
						{
							sbVal.AppendUTF8Char(sb.v[i]);
							lastWS++;
						}
						else
						{
							sbVal.AppendUTF8Char(sb.v[i]);
							lastWS = 0;
						}
						i++;
					}
					if (lastWS)
					{
						sbVal.RemoveChars(lastWS);
					}
					sb.ClearStr();
					sb.Append(tags.GetItemNoCheck(0));
					i = 1;
					j = tags.GetCount();
					while (i < j)
					{
						sb.AppendUTF8Char('.');
						sb.Append(tags.GetItemNoCheck(i));
						i++;
					}
					cfg->SetValue(CSTR(""), sb.ToCString(), sbVal.ToCString());
				}
			}
		}
		sb.ClearStr();
	}
	tags.FreeAll();
	return cfg;
}

Optional<IO::ConfigFile> IO::YAMLParser::ParseFile(Text::CStringNN fileName)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return nullptr;
	}
	return ParseStream(fs);
}
