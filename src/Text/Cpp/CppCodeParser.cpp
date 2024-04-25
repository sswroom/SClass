#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"
#include "Text/Cpp/CppEnv.h"
#include "Text/Cpp/CppCodeParser.h"

UTF8Char *Text::Cpp::CppCodeParser::RemoveSpace(UTF8Char *sptr)
{
	UTF8Char c;
	while ((c = *sptr++) != 0)
	{
		if (c != ' ' && c != '\t')
			break;
		if (c == '/' && *sptr == '*')
		{
		}
	}
	return sptr - 1;
}

void Text::Cpp::CppCodeParser::LogError(Text::Cpp::CppParseStatus *status, const UTF8Char *errMsg, UOSInt msgLen, Data::ArrayListStringNN *errMsgs)
{
	Text::StringBuilderUTF8 sb;
	Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();
	NN<Text::String> fname = fileStatus->fileName;
	UOSInt i = fname->LastIndexOf('\\');
	sb.AppendC(&fname->v[i + 1], fname->leng - i - 1);
	sb.AppendC(UTF8STRC(" ("));
	sb.AppendI32(fileStatus->lineNum);
	sb.AppendC(UTF8STRC("): "));
	sb.AppendC(errMsg, msgLen);
	errMsgs->Add(Text::String::New(sb.ToString(), sb.GetLength()));
}

Bool Text::Cpp::CppCodeParser::ParseSharpIfParam(Text::CString condStr, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, Data::ArrayListStringNN *codePhases, UOSInt cpIndex)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char c;
	const UTF8Char *cond = condStr.v;
	Bool succ = true;
	if (condStr.Equals(UTF8STRC("defined(_WIN32_WCE)")))
	{
		sptr = sbuff;
	}
	sptr = sbuff;
	while (succ)
	{
		c = *cond++;
		if (c == '_')
		{
			*sptr++ = c;
		}
		else if (c >= 'A' && c <= 'Z')
		{
			*sptr++ = c;
		}
		else if (c >= 'a' && c <= 'z')
		{
			*sptr++ = c;
		}
		else if (c >= '0' && c <= '9')
		{
			*sptr++ = c;
		}
		else
		{
			if (sptr != sbuff)
			{
				*sptr = 0;
				codePhases->Insert(cpIndex++, Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				sptr = sbuff;
			}

			if (c == 0)
				break;
			if (c == 0x20 || c == '\t')
			{
			}
			else if (c == '!')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("!=")));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("!")));
				}
			}
			else if (c == '>')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC(">=")));
					cond++;
				}
				else if (*cond == '>')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC(">>")));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC(">")));
				}
			}
			else if (c == '<')
			{
				if (*cond == '=')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("<=")));
					cond++;
				}
				else if (*cond == '<')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("<<")));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("<")));
				}
			}
			else if (c == '=' && *cond == '=')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("==")));
				cond++;
			}
			else if (c == '(')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("(")));
			}
			else if (c == ')')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC(")")));
			}
			else if (c == '&')
			{
				if (*cond == '&')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("&&")));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("&")));
				}
			}
			else if (c == '|')
			{
				if (*cond == '|')
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("||")));
					cond++;
				}
				else
				{
					codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("|")));
				}
			}
			else if (c == '*')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("*")));
			}
			else if (c == '/')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("/")));
			}
			else if (c == '%')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("%")));
			}
			else if (c == '+')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("+")));
			}
			else if (c == '-')
			{
				codePhases->Insert(cpIndex++, Text::String::New(UTF8STRC("-")));
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Unknown symbol '"));
				sb.AppendUTF8Char(c);
				sb.AppendC(UTF8STRC("'"));
				this->LogError(status, sb.ToString(), sb.GetLength(), errMsgs);
				succ = false;
			}
		}
	}
	return succ;
}

Bool Text::Cpp::CppCodeParser::EvalSharpIfVal(Data::ArrayListStringNN *codePhases, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, UOSInt cpIndex, Int32 *outVal, OSInt priority)
{
/*
	Priority:
	14	! ~
	13	.* ->*
	12	* / %
	11	+ -
	10	>> <<
	9	>= <= > <
	8	== !=
	7	&
	6	^
	5	|
	4	&&
	3	||
	2	= += -= *= /= %= <<= >>= &= ^= |=
	1	,
*/

	Text::StringBuilderUTF8 debugSB;
	UOSInt i;
	NN<Text::String> phase;
	NN<Text::String> phase1;
	NN<Text::String> phase2;
	debugSB.AppendJoin(codePhases->Iterator(), ' ');
	Int32 val = 0;
	if (!codePhases->RemoveAt(cpIndex).SetTo(phase))
	{
		return false;
	}

	if (phase->Equals(UTF8STRC("defined")))
	{
		phase->Release();
		if (cpIndex + 3 <= codePhases->GetCount() && codePhases->GetItem(cpIndex).SetTo(phase1) && codePhases->GetItem(cpIndex + 2).SetTo(phase2))
		{
			if (phase1->Equals(UTF8STRC("(")) && phase2->Equals(UTF8STRC(")")))
			{
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex + 2));
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				if (codePhases->RemoveAt(cpIndex).SetTo(phase))
				{
					if (status->IsDefined(phase->ToCString()))
					{
						val = 1;
					}
					else
					{
						val = 0;
					}
					phase->Release();
				}
			}
			else
			{
				if (codePhases->RemoveAt(cpIndex).SetTo(phase))
				{
					if (status->IsDefined(phase->ToCString()))
					{
						val = 1;
					}
					else
					{
						val = 0;
					}
					phase->Release();
				}
			}
		}
		else if (cpIndex + 1 == codePhases->GetCount())
		{
			if (codePhases->RemoveAt(cpIndex).SetTo(phase))
			{
				if (status->IsDefined(phase->ToCString()))
				{
					val = 1;
				}
				else
				{
					val = 0;
				}
				phase->Release();
			}
		}
		else
		{
			debugSB.AppendC(UTF8STRC(": invalid defined syntex"));
			this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
			return false;
		}
	}
	else if (phase->Equals(UTF8STRC("__has_feature")))
	{
		phase->Release();
		if (cpIndex + 3 <= codePhases->GetCount() && codePhases->GetItem(cpIndex).SetTo(phase1) && codePhases->GetItem(cpIndex + 2).SetTo(phase2))
		{
			if (phase1->Equals(UTF8STRC("(")) && phase2->Equals(UTF8STRC(")")))
			{
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex + 2));
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				val = 0;
			}
			else
			{
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				val = 0;
			}
		}
		else if (cpIndex + 1 == codePhases->GetCount())
		{
			OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
			val = 0;
		}
		else
		{
			debugSB.AppendC(UTF8STRC(": invalid __has_feature syntex"));
			this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
			return false;
		}
	}
	else if (phase->Equals(UTF8STRC("(")))
	{
		phase->Release();
		if (!EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &val, 0))
		{
			return false;
		}
		Optional<Text::String> optPhase = codePhases->GetItem(cpIndex);
		if (!optPhase.SetTo(phase) || !phase->Equals(UTF8STRC(")")))
		{
			debugSB.AppendC(UTF8STRC(": missing )"));
			if (optPhase.SetTo(phase))
			{
				debugSB.Append(phase);
			}
			this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
			return false;
		}
		OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
	}
	else if (phase->Equals(UTF8STRC("!")))
	{
		phase->Release();
		Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &val, 14);
		if (!succ)
			return false;
		if (val != 0)
			val = 0;
		else
			val = 1;
	}
	else if (phase->v[0] >= '0' && phase->v[0] <= '9')
	{
		if (phase->EndsWith('L'))
		{
			phase->RemoveChars(1);
		}
		if (!phase->ToInt32(val))
		{
			debugSB.AppendC(UTF8STRC(": unknown syntex "));
			debugSB.Append(phase);
			this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
			phase->Release();
			return false;
		}
		phase->Release();
	}
	else if (status->IsDefined(phase->ToCString()))
	{
		if (codePhases->GetCount() <= cpIndex || !codePhases->GetItem(cpIndex).SetTo(phase1) || !phase1->Equals(UTF8STRC("(")))
		{
			Text::StringBuilderUTF8 sb;
			status->GetDefineVal(phase->ToCString(), CSTR_NULL, sb);
			phase->Release();
			this->ParseSharpIfParam(sb.ToCString(), status, errMsgs, codePhases, cpIndex);
			return this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, outVal, priority);
		}
		else
		{
			Int32 lev;
			Text::StringBuilderUTF8 params;
			OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
			lev = 1;
			while (cpIndex < codePhases->GetCount())
			{
				if (codePhases->GetItem(cpIndex).SetTo(phase2))
				{
					if (phase2->Equals(UTF8STRC(")")))
					{
						OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
						if (--lev <= 0)
						{
							break;
						}
						params.AppendC(UTF8STRC(")"));
					}
					else if (phase2->Equals(UTF8STRC("(")))
					{
						lev++;
						params.AppendC(UTF8STRC("("));
						OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
					}
					else
					{
						params.Append(phase2);
						OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
					}
				}
			}
			if (lev != 0)
			{
				debugSB.AppendC(UTF8STRC(": macro not supported "));
				debugSB.Append(phase);
				this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
				phase->Release();
				return false;
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				status->GetDefineVal(phase->ToCString(), params.ToCString(), sb);
				phase->Release();
				this->ParseSharpIfParam(sb.ToCString(), status, errMsgs, codePhases, cpIndex);
				return this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, outVal, priority);
			}
		}
	}
	else
	{
		debugSB.AppendC(UTF8STRC(": unknown syntex "));
		debugSB.Append(phase);
		this->LogError(status, debugSB.ToString(),debugSB.GetLength(), errMsgs);
		phase->Release();
//		return false;
		val = 0;
	}

	while (codePhases->GetCount() > cpIndex)
	{
		if (codePhases->GetItem(cpIndex).SetTo(phase))
		{
			if (phase->Equals(UTF8STRC(")")))
			{
				break;
			}
			else if (phase->Equals(UTF8STRC("*")))
			{
				if (priority > 12)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
				if (!succ)
					break;
				val = val * nextVal;
			}
			else if (phase->Equals(UTF8STRC("/")))
			{
				if (priority > 12)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
				if (!succ)
					break;
				val = val / nextVal;
			}
			else if (phase->Equals(UTF8STRC("%")))
			{
				if (priority > 12)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 12);
				if (!succ)
					break;
				val = val % nextVal;
			}
			else if (phase->Equals(UTF8STRC("+")))
			{
				if (priority > 11)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 11);
				if (!succ)
					break;
				val = val + nextVal;
			}
			else if (phase->Equals(UTF8STRC("-")))
			{
				if (priority > 11)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 11);
				if (!succ)
					break;
				val = val - nextVal;
			}
			else if (phase->Equals(UTF8STRC(">>")))
			{
				if (priority > 10)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 10);
				if (!succ)
					break;
				val = val >> nextVal;
			}
			else if (phase->Equals(UTF8STRC("<<")))
			{
				if (priority > 10)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 10);
				if (!succ)
					break;
				val = val << nextVal;
			}
			else if (phase->Equals(UTF8STRC(">=")))
			{
				if (priority > 9)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
				if (!succ)
					break;
				if (val >= nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC("<=")))
			{
				if (priority > 9)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
				if (!succ)
					break;
				if (val <= nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC(">")))
			{
				if (priority > 9)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
				if (!succ)
					break;
				if (val > nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC("<")))
			{
				if (priority > 9)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 9);
				if (!succ)
					break;
				if (val < nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC("==")))
			{
				if (priority > 8)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 8);
				if (!succ)
					break;
				if (val == nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC("!=")))
			{
				if (priority > 8)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 8);
				if (!succ)
					break;
				if (val != nextVal)
					val = 1;
				else
					val = 0;
			}
			else if (phase->Equals(UTF8STRC("&")))
			{
				if (priority > 7)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				Int32 nextVal;
				Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 7);
				if (!succ)
					break;
				val = val & nextVal;
			}
			else if (phase->Equals(UTF8STRC("&&")))
			{
				if (priority > 4)
				{
					break;
				}
				OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
				if (val == 0)
				{
					i = 0;
					while (cpIndex < codePhases->GetCount())
					{
						if (codePhases->GetItem(i).SetTo(phase))
						{
							if (phase->Equals(UTF8STRC("(")))
							{
								i++;
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
							else if (phase->Equals(UTF8STRC(")")))
							{
								i--;
								if (i < 0)
									break;
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
							else
							{
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
						}
					}
					if (i < 0)
					{
						break;
					}
				}
				else
				{
					Int32 nextVal;
					Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 4);
					if (!succ)
						break;
					if (nextVal != 0)
						val = 1;
					else
						val = 0;
				}
			}
			else if (phase->Equals(UTF8STRC("||")))
			{
				if (priority > 3)
				{
					break;
				}
				if (val != 0)
				{
					i = 0;
					while (cpIndex < codePhases->GetCount())
					{
						if (codePhases->GetItem(i).SetTo(phase))
						{
							if (phase->Equals(UTF8STRC("(")))
							{
								i++;
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
							else if (phase->Equals(UTF8STRC(")")))
							{
								i--;
								if (i < 0)
									break;
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
							else
							{
								OPTSTR_DEL(codePhases->RemoveAt(i));
							}
						}
					}
					if (i < 0)
					{
						break;
					}
				}
				else
				{
					OPTSTR_DEL(codePhases->RemoveAt(cpIndex));
					Int32 nextVal;
					Bool succ = this->EvalSharpIfVal(codePhases, status, errMsgs, cpIndex, &nextVal, 3);
					if (!succ)
						break;
					if (nextVal != 0)
						val = 1;
					else
						val = 0;
				}
			}
			else
			{
				debugSB.AppendC(UTF8STRC(": unknown syntex "));
				debugSB.Append(phase);
				this->LogError(status, debugSB.ToString(), debugSB.GetLength(), errMsgs);
				return false;
			}
		}
	}
	*outVal = val;
	return true;
}

Bool Text::Cpp::CppCodeParser::EvalSharpIf(Text::CString cond, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs, Bool *result)
{
	Bool succ = true;
	UOSInt j;
	Data::ArrayListStringNN codePhase;
	if (!ParseSharpIfParam(cond, status, errMsgs, &codePhase, 0))
	{
		succ = false;
	}

	if (!succ)
	{
		codePhase.FreeAll();
	}
	else
	{
		j = codePhase.GetCount();
		if (j > 0)
		{
			Int32 val;
			if (!EvalSharpIfVal(&codePhase, status, errMsgs, 0, &val, 0))
			{
				succ = false;
			}
			else
			{
				*result = (val != 0);
			}
			if (succ && codePhase.GetCount() > 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(cond);
				sb.AppendC(UTF8STRC(": unknown phases found: "));
				sb.AppendJoin(codePhase.Iterator(), ' ');
			}
			codePhase.FreeAll();
		}
		else
		{
			this->LogError(status, UTF8STRC("#if empty condition"), errMsgs);
			succ = false;
		}
	}
	return succ;
}

Bool Text::Cpp::CppCodeParser::ParseLine(UTF8Char *lineBuff, UTF8Char *lineBuffEnd, Text::Cpp::CppParseStatus *status, Data::ArrayListStringNN *errMsgs)
{
	Bool lineStart;
	Bool nextLine = false;
	Bool parseStatus = true;
	UTF8Char c;
	UTF8Char *wordStart = 0;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	Bool valid;
	UOSInt i;
	UOSInt j;
	Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();

	fileStatus->lineNum++;
	if (fileStatus->lineNum == 290 && fileStatus->fileName->EndsWithICase(UTF8STRC("winspool.h")))
	{
		sptr = lineBuff;
	}

	i = fileStatus->lineBuffSB->GetLength();
	if (i > 0)
	{
		if (fileStatus->lineBuffWS)
		{
			j = (UOSInt)(fileStatus->lineBuffWS - fileStatus->lineBuffSB->ToString());
			fileStatus->lineBuffSB->AppendC(lineBuff, (UOSInt)(lineBuffEnd - lineBuff));
			wordStart = fileStatus->lineBuffSB->v + j;
		}
		else
		{
			fileStatus->lineBuffSB->AppendC(lineBuff, (UOSInt)(lineBuffEnd - lineBuff));
		}
	}
	else
	{
		fileStatus->lineBuffSB->AppendC(lineBuff, (UOSInt)(lineBuffEnd - lineBuff));
	}
	sptr = fileStatus->lineBuffSB->v + i;
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_DEFINE)
	{
		lineStart = false;
	}
	else
	{
		lineStart = true;
	}
	if (*sptr == 0 && lineStart)
		nextLine = true;
	while (!nextLine)
	{
		valid = true;
		i = fileStatus->ifValid->GetCount();
		while (i-- > 0)
		{
			if (fileStatus->ifValid->GetItem(i) != 1)
			{
				valid = false;
				break;
			}
		}
		switch (fileStatus->currMode)
		{
		case Text::Cpp::CppParseStatus::PM_COMMENTPARA:
			while ((c = *sptr++) != 0)
			{
				sptr[-1] = ' ';
				if (c == '*' && *sptr == '/')
				{
					*sptr = ' ';
					UOSInt i = fileStatus->pastModes->GetCount();
					if (i > 0)
					{
						fileStatus->currMode = fileStatus->pastModes->RemoveAt(i - 1);
					}
					else
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					sptr++;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_SHARP:
			lineStart = false;
			while (true)
			{
				c = *sptr++;
				if (wordStart)
				{
					if (c >= 'A' && c <= 'Z')
						continue;
					if (c >= 'a' && c <= 'z')
						continue;

					sptr[-1] = 0;
					if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("include")))
					{
						if (valid)
						{
							sptr[-1] = c;
							if (c == '\"')
							{
								wordStart = sptr;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE;
								break;
							}
							else if (c == '<')
							{
								wordStart = sptr;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEARROW;
								break;
							}
							else if (c == '\t' || c == ' ')
							{
								wordStart = 0;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDE;
								break;
							}
							else if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_INCLUDE);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								sptr++;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								this->LogError(status, UTF8STRC("Missing parameter in #include"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #include"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("pragma")))
					{
						if (valid)
						{
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_PRAGMA);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_PRAGMA;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #pragma"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("ifndef")))
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFNDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #ifndef"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IFNDEF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #ifndef"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("ifdef")))
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #ifdef"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IFDEF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #ifdef"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("if")))
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #if"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t' || c == '(')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_IF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #if"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("elif")))
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ELIF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							wordStart = 0;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t' || c == '(')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_ELIF;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #elif"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("define")))
					{
						fileStatus->modeStatus = 0;
						if (c == '/' && *sptr == '*')
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_DEFINE);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							break;
						}
						else if (c == '/' && *sptr == '/')
						{
							nextLine = true;
							break;
						}
						else if (c == ' ' || c == '\t')
						{
							sptr[-1] = c;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_DEFINE;
							wordStart = 0;
							break;
						}
						else
						{
							this->LogError(status, UTF8STRC("Unknown parameter in #define"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("undef")))
					{
						if (valid)
						{
							fileStatus->modeStatus = 0;
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_UNDEF);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_UNDEF;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #undef"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("else")))
					{
						i = fileStatus->ifValid->GetCount();
						if (i == 0)
						{
							this->LogError(status, UTF8STRC("Corresponding #if not found in #else"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else
						{
							Int32 lastV = fileStatus->ifValid->GetItem(i - 1);
							if (lastV == 0)
							{
								fileStatus->ifValid->SetItem(i - 1, 1);
							}
							else if (lastV == 1)
							{
								fileStatus->ifValid->SetItem(i - 1, 0);
							}
							else
							{
								fileStatus->ifValid->SetItem(i - 1, 2);
							}
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t' || c == 0)
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								wordStart = 0;
								if (c == 0)
								{
									nextLine = true;
								}
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #else"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("endif")))
					{
						i = fileStatus->ifValid->GetCount();
						if (i == 0)
						{
							this->LogError(status, UTF8STRC("Corresponding #if not found in #endif"), errMsgs);
							parseStatus = false;
							nextLine = true;
							break;
						}
						else
						{
							fileStatus->ifValid->RemoveAt(i - 1);
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t' || c == 0)
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								wordStart = 0;
								if (c == 0)
									nextLine = true;
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #endif"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
					}
					else if (Text::StrEqualsC(wordStart, (UOSInt)(sptr - wordStart - 1), UTF8STRC("error")))
					{
						if (valid)
						{
							if (c == '/' && *sptr == '*')
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ERROR);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else if (c == '/' && *sptr == '/')
							{
								nextLine = true;
								break;
							}
							else if (c == ' ' || c == '\t')
							{
								sptr[-1] = c;
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_ERROR;
								wordStart = 0;
								break;
							}
							else
							{
								this->LogError(status, UTF8STRC("Unknown parameter in #error"), errMsgs);
								parseStatus = false;
								nextLine = true;
								break;
							}
						}
						else
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							break;
						}
					}
					else
					{
						this->LogError(status, UTF8STRC("Unknown '#' type"), errMsgs);
						sptr[-1] = c;
						parseStatus = false;
						nextLine = true;
						break;
					}
				}
				else
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
					{
						wordStart = sptr - 1;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARP);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else if (c == '/' && *sptr == '/')
					{
						this->LogError(status, UTF8STRC("Unknown '#' type"), errMsgs);
						parseStatus = false;
						nextLine = true;
						break;
					}
					else
					{
						this->LogError(status, UTF8STRC("Unknown '#' type"), errMsgs);
						parseStatus = false;
						nextLine = true;
						break;
					}
				}
				if (c == 0)
				{
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDE:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
				}
				else if (c == '\"')
				{
					wordStart = sptr;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE;
					break;
				}
				else if (c == '<')
				{
					wordStart = sptr;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_INCLUDEARROW;
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_INCLUDE);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					this->LogError(status, UTF8STRC("Unknown #include parameter"), errMsgs);
					parseStatus = false;
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDEQUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\"')
				{
					sptr[-1] = 0;
					sbuff2[0] = 0;
					sptr2 = this->env->GetIncludeFilePath(sbuff2, CSTRP(wordStart, sptr - 1), status->GetCurrCodeFile().Ptr());
					sptr[-1] = c;

					if (sbuff2[0] == 0)
					{
						parseStatus = false;
						sptr[-1] = 0;
						sptr2 = Text::StrConcatC(Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("Include file ")), wordStart), UTF8STRC(" not found"));
						sptr[-1] = c;
						this->LogError(status, sbuff, (UOSInt)(sptr2 - sbuff), errMsgs);
						nextLine = true;
						break;
					}
					else
					{
						if (ParseFile(sbuff2, (UOSInt)(sptr2 - sbuff2), errMsgs, status))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
							break;
						}
						else
						{
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
				}
				else
				{
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_INCLUDEARROW:
			while ((c = *sptr++) != 0)
			{
				if (c == '>')
				{
					sptr[-1] = 0;
					sptr2 = this->env->GetIncludeFilePath(sbuff2, CSTRP(wordStart, sptr - 1), 0);
					sptr[-1] = c;

					if (sbuff2[0] == 0)
					{
						parseStatus = false;
						sptr2 = Text::StrConcatC(Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("Include file ")), wordStart, (UOSInt)(sptr - wordStart - 1)), UTF8STRC(" not found"));
						this->LogError(status, sbuff, (UOSInt)(sptr2 - sbuff), errMsgs);
					}
					else
					{
						if (ParseFile(sbuff2, (UOSInt)(sptr2 - sbuff2), errMsgs, status))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						}
						else
						{
							parseStatus = false;
							nextLine = true;
							break;
						}
					}
				}
				else
				{
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_PRAGMA:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_PRAGMA);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					wordStart = sptr - 1;
				}
			}
			if (c == 0)
			{
				fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_IFNDEF:
			/////////////////////////////////////////////
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						fileStatus->ifValid->Add((!status->IsDefined(CSTRP(wordStart, sptr - 1)))?1:0);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						break;
					}
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, UTF8STRC("Parameters not found in #ifndef"), errMsgs);
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFNDEF);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				if (wordStart)
				{
					sptr[-1] = 0;
					fileStatus->ifValid->Add((!status->IsDefined(CSTRP(wordStart, sptr - 1)))?1:0);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
					break;
				}
				else
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, UTF8STRC("Parameters not found in #ifndef"), errMsgs);
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_IFDEF:
			/////////////////////////////////////////////
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						fileStatus->ifValid->Add((status->IsDefined(CSTRP(wordStart, sptr - 1)))?1:0);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
						break;
					}
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, UTF8STRC("Parameters not found in #ifdef"), errMsgs);
					break;
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_IFDEF);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				if (wordStart)
				{
					sptr[-1] = 0;
					fileStatus->ifValid->Add((status->IsDefined(CSTRP(wordStart, sptr - 1)))?1:0);
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
					break;
				}
				else
				{
					nextLine = true;
					parseStatus = false;
					this->LogError(status, UTF8STRC("Parameters not found in #ifdef"), errMsgs);
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_ELIF:
		case Text::Cpp::CppParseStatus::PM_IF:
			{
				/////////////////////////////////////////////
				Bool lastNextLine = false;
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						if (wordStart)
						{
							nextLine = true;
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							c = 0;
							break;
						}
						else
						{
							nextLine = true;
							parseStatus = false;
							this->LogError(status, UTF8STRC("Parameters not found in #if"), errMsgs);
							break;
						}
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(fileStatus->currMode);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						sptr[-1] = ' ';
						*sptr = ' ';
						break;
					}
					else if (c == '\\')
					{
						lastNextLine = true;
						sptr[-1] = ' ';
					}
					else
					{
						lastNextLine = false;
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}
					}
				}
				if (c == 0 && !lastNextLine)
				{
					if (wordStart)
					{
						sptr[-1] = 0;
						i = fileStatus->ifValid->GetCount();
						if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_ELIF)
						{
							if (i >= 2 && fileStatus->ifValid->GetItem(i - 2) == 0)
							{
							}
							else
							{
								Int32 lastV = fileStatus->ifValid->GetItem(i - 1);
								if (lastV == 0)
								{
									Bool ifRes;
									if (!EvalSharpIf(CSTRP(wordStart, sptr - 1), status, errMsgs, &ifRes))
									{
										parseStatus = false;
										nextLine = true;
										break;
									}
									else
									{
										fileStatus->ifValid->SetItem(i - 1, ifRes?1:0);
									}
								}
								else
								{
									fileStatus->ifValid->SetItem(i - 1, 2);
								}
							}
						}
						else
						{
							if (i >= 1 && fileStatus->ifValid->GetItem(i - 1) == 0)
							{
								fileStatus->ifValid->Add(0);
							}
							else
							{
								Bool ifRes;
								if (!EvalSharpIf(CSTRP(wordStart, sptr - 1), status, errMsgs, &ifRes))
								{
									parseStatus = false;
									nextLine = true;
									break;
								}
								else
								{
									fileStatus->ifValid->Add(ifRes?1:0);
								}
							}
						}
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						nextLine = true;
						break;
					}
					else
					{
						nextLine = true;
						parseStatus = false;
						this->LogError(status, UTF8STRC("Parameters not found in #if"), errMsgs);
						break;
					}
				}
				else if (lastNextLine)
				{
					nextLine = true;
					break;
				}
			}
			break;
		case Text::Cpp::CppParseStatus::PM_DEFINE:
			{
				Bool lastNextLine = false;
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						c = 0;
						sptr[-1] = 0;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_DEFINE);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						sptr[-1] = ' ';
						*sptr = ' ';
						break;
					}
					else if (c == '\\')
					{
						lastNextLine = true;
						sptr[-1] = ' ';
					}
					else
					{
						lastNextLine = false;
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}

/*						if (fileStatus->modeStatus == 0)
						{
							if (wordStart == 0)
							{
								wordStart = sptr - 1;
							}
							else if (c == '(')
							{
								Bool found = false;
								UTF8Char *tmpPtr = wordStart;
								sptr[-1] = 0;
								while (true)
								{
									c = *tmpPtr++;
									if (c == ' ' || c == '\t')
									{
										found = true;
										break;
									}
									else if (c == 0)
									{
										break;
									}
								}
								if (found)
								{
									sptr[-1] = '(';
								}
								else
								{
									if (!valid)
									{
										sptr[-1] = c;
										fileStatus->modeStatus = 1;
									}
									else if (status->AddDef(wordStart, 0, fileStatus->lineNum))
									{
										sptr[-1] = c;
										fileStatus->modeStatus = 1;
									}
									else
									{
										parseStatus = false;
										this->LogError(status, L"Define already exist", errMsgs);
										nextLine = true;
										break;
									}
								}
							}
						}*/
					}
				}
				if (c == 0)
				{
					if (lastNextLine)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_DEFINE;
					}
					else if (fileStatus->modeStatus == 0 && wordStart)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						if (!valid)
						{
							fileStatus->modeStatus = 1;
						}
						else
						{
							UTF8Char *paramPtr = 0;
							UTF8Char *paramPtrEnd = 0;
							UTF8Char *tmpPtr = wordStart;
							UTF8Char *wordEnd = 0;
							Int32 mode = 0;
							/*
							  Name( Param ) Cont
							0011113355555577
							*/
							while (true)
							{
								c = *tmpPtr++;
								if (c == 0)
								{
									if (paramPtr == 0)
									{
										if (status->AddDef(CSTRP(wordStart, tmpPtr - 1), CSTR_NULL, CSTR_NULL, fileStatus->lineNum))
										{
											fileStatus->modeStatus = 1;
										}
										else
										{
											parseStatus = false;
											this->LogError(status, UTF8STRC("Define already exist"), errMsgs);
											nextLine = true;
											break;
										}
									}
									else
									{
										if (status->AddDef(CSTRP(wordStart, wordEnd), CSTRP(paramPtr, tmpPtr - 1), CSTR_NULL, fileStatus->lineNum))
										{
											fileStatus->modeStatus = 1;
										}
										else
										{
											parseStatus = false;
											this->LogError(status, UTF8STRC("Define already exist"), errMsgs);
											nextLine = true;
											break;
										}
									}
									break;
								}
								else if (c == ' ' || c == '\t')
								{
									if (mode == 1)
									{
										mode = 7;
										tmpPtr[-1] = 0;
										wordEnd = tmpPtr - 1;
									}
								}
								else if (c == '('  && mode == 1)
								{
									mode = 3;
									tmpPtr[-1] = 0;
									wordEnd = tmpPtr - 1;
								}
								else if (c == ')')
								{
									if (mode == 5)
									{
										OSInt tmpI = -1;
										while (true)
										{
											tmpI--;
											c = tmpPtr[tmpI];
											if (c == ' ' || c == '\t')
											{
											}
											else
											{
												tmpPtr[tmpI + 1] = 0;
												paramPtrEnd = &tmpPtr[tmpI + 1];
												break;
											}
										}
										mode = 7;
									}
									else if (mode == 3)
									{
										mode = 7;
 									}
									else
									{
										parseStatus = false;
										this->LogError(status, UTF8STRC("Syntax error in define"), errMsgs);
										nextLine = true;
										break;
									}
								}
								else
								{
									if (mode == 0)
									{
										mode = 1;
									}
									else if (mode == 3)
									{
										paramPtr = &tmpPtr[-1];
										mode = 5;
									}
									else if (mode == 2 || mode == 7)
									{
										if (status->AddDef(CSTRP(wordStart, wordEnd), CSTRP(paramPtr, paramPtrEnd), Text::CString::FromPtr(&tmpPtr[-1]), fileStatus->lineNum))
										{
											fileStatus->modeStatus = 1;
										}
										else
										{
											parseStatus = false;
											this->LogError(status, UTF8STRC("Define already exist"), errMsgs);
											nextLine = true;
											break;
										}
										break;
									}
								}
							}
						}
					}
					else if (!valid)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					else if (fileStatus->modeStatus == 0)
					{
						parseStatus = false;
						this->LogError(status, UTF8STRC("No parameters in #define"), errMsgs);
					}
					else
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					}
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_UNDEF:
			if (valid)
			{
				wordStart = 0;
				/////////////////////////////////////////////
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
						if (wordStart)
						{
							sptr[-1] = 0;
							if (status->Undefine(CSTRP(wordStart, sptr - 1)))
							{
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								break;
							}
							else
							{
/*								this->LogError(status, L"#undef error, define not found", errMsgs);
								nextLine = true;
								parseStatus = false;*/
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARPEND;
								break;
							}
							break;
						}
					}
					else if (c == '/' && *sptr == '/')
					{
						sptr[-1] = 0;

						if (status->Undefine(CSTRP(wordStart, sptr - 1)))
						{
							nextLine = true;
							break;
						}
						else
						{
/*							this->LogError(status, L"#undef error, define not found", errMsgs);
							nextLine = true;
							parseStatus = false;*/
							nextLine = true;
							break;
						}
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						if (wordStart)
						{
							sptr[-1] = 0;

							if (status->Undefine(CSTRP(wordStart, sptr - 1)))
							{
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							else
							{
								/*this->LogError(status, L"#undef error, define not found", errMsgs);
								nextLine = true;
								parseStatus = false;*/
								fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
								fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
								break;
							}
							sptr[-1] = c;
							break;
						}
						else
						{
							fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_UNDEF);
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
							break;
						}
					}
					else
					{
						if (wordStart == 0)
						{
							wordStart = sptr - 1;
						}
					}
				}
				if (c == 0)
				{
					if (wordStart)
					{
						if (status->Undefine(CSTRP(wordStart, sptr - 1)))
						{
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							nextLine = true;
						}
						else
						{
							/*this->LogError(status, CSTR("#undef error, define not found"), errMsgs);
							nextLine = true;
							parseStatus = false;*/
							fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
							nextLine = true;
						}
					}
					else
					{
						this->LogError(status, UTF8STRC("#undef parameter error"), errMsgs);
						nextLine = true;
						parseStatus = false;
					}
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_ERROR:
			if (valid)
			{
				/////////////////////////////////////////////
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_ERROR);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else
					{
						wordStart = sptr - 1;
					}
				}
				if (c == 0)
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_SHARPEND:
			if (valid)
			{
				while ((c = *sptr++) != 0)
				{
					if (c == ' ' || c == '\t')
					{
					}
					else if (c == '/' && *sptr == '/')
					{
						nextLine = true;
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
						break;
					}
					else if (c == '/' && *sptr == '*')
					{
						fileStatus->pastModes->Add(Text::Cpp::CppParseStatus::PM_SHARPEND);
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
						break;
					}
					else
					{
						nextLine = true;
						parseStatus = false;
						break;
					}
				}
				if (c == 0)
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					nextLine = true;
				}
				break;
			}
		case Text::Cpp::CppParseStatus::PM_NORMAL:
			while ((c = *sptr++) != 0)
			{
				if (c == ' ' || c == '\t')
				{
					if (wordStart)
					{
						wordStart = 0;
					}
				}
				else if (c == '/' && *sptr == '*')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_COMMENTPARA;
					sptr++;
					break;
				}
				else if (c == '/' && *sptr == '/')
				{
					nextLine = true;
					break;
				}
				else if (c == '"')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_QUOTE;
					break;
				}
				else if (c == '\'')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SQUOTE;
					break;
				}
				else if (c == '#')
				{
					if (lineStart)
					{
						fileStatus->currMode = Text::Cpp::CppParseStatus::PM_SHARP;
					}
					else
					{
						this->LogError(status, UTF8STRC("'#' is not at beginning of line"), errMsgs);
						nextLine = true;
					}
					break;
				}
				else
				{
					if (wordStart == 0)
					{
						wordStart = sptr - 1;
					}
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_QUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\\')
				{
					sptr++;
				}
				else if (c == '"')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		case Text::Cpp::CppParseStatus::PM_SQUOTE:
			while ((c = *sptr++) != 0)
			{
				if (c == '\\')
				{
					sptr++;
				}
				else if (c == '\'')
				{
					fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
					break;
				}
			}
			if (c == 0)
			{
				nextLine = true;
			}
			break;
		default:
			this->LogError(status, UTF8STRC("Unknown status"), errMsgs);
			parseStatus = false;
			nextLine = true;
			break;
		}
	}
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_SHARPEND)
	{
		fileStatus->currMode = Text::Cpp::CppParseStatus::PM_NORMAL;
	}
	else if (parseStatus && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_NORMAL && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_COMMENTPARA && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_DEFINE && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_IF && fileStatus->currMode != Text::Cpp::CppParseStatus::PM_ELIF)
	{
		this->LogError(status, UTF8STRC("Status error in line end"), errMsgs);
		parseStatus = false;
	}
	if (fileStatus->currMode == Text::Cpp::CppParseStatus::PM_NORMAL)
	{
		fileStatus->lineBuffSB->ClearStr();
	}
	else
	{
		fileStatus->lineBuffWS = wordStart;
	}
	if (!parseStatus)
	{
		return false;
	}
	return parseStatus;
}

Text::Cpp::CppCodeParser::CppCodeParser(Text::Cpp::CppEnv *env)
{
	this->env = env;
}

Text::Cpp::CppCodeParser::~CppCodeParser()
{
}

Bool Text::Cpp::CppCodeParser::ParseFile(const UTF8Char *fileName, UOSInt fileNameLen, Data::ArrayListStringNN *errMsgs, Text::Cpp::CppParseStatus *status)
{
	UTF8Char *lineBuff;
	UTF8Char *sptr;
	UOSInt i;
	Bool succ;

	lineBuff = MemAlloc(UTF8Char, 65536);
	{
		IO::FileStream fs({fileName, fileNameLen}, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
		if (fs.IsError())
		{
			Text::Cpp::CppParseStatus::FileParseStatus *fileStatus = status->GetFileStatus();
			if (fileStatus)
			{
				i = fileStatus->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
				sptr = Text::StrConcatC(lineBuff, &fileStatus->fileName->v[i + 1], fileStatus->fileName->leng - i - 1);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" ("));
				sptr = Text::StrOSInt(sptr, fileStatus->lineNum);
				sptr = Text::StrConcatC(sptr, UTF8STRC("): "));
			}
			else
			{
				sptr = lineBuff;
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("Cannot open \""));
			sptr = Text::StrConcatC(sptr, fileName, fileNameLen);
			sptr = Text::StrConcatC(sptr, UTF8STRC("\""));
			errMsgs->Add(Text::String::New(lineBuff, (UOSInt)(sptr - lineBuff)));
			MemFree(lineBuff);
			return false;
		}

		status->BeginParseFile({fileName, fileNameLen});
		succ = true;

		IO::StreamReader reader(fs, 0);
		while ((sptr = reader.ReadLine(lineBuff, 65535)) != 0)
		{
			if (!ParseLine(lineBuff, sptr, status, errMsgs))
			{
				succ = false;
				break;
			}
		}
	}
	if (!status->EndParseFile(fileName, fileNameLen))
	{
		succ = false;
		sptr = Text::StrConcatC(Text::StrConcatC(lineBuff, fileName, fileNameLen), UTF8STRC(" File End error"));
		errMsgs->Add(Text::String::New(lineBuff, (UOSInt)(sptr - lineBuff)));
	}

	MemFree(lineBuff);
	return succ;
}

void Text::Cpp::CppCodeParser::FreeErrMsgs(Data::ArrayListStringNN *errMsgs)
{
	UOSInt i = errMsgs->GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(errMsgs->RemoveAt(i));
	}
}
