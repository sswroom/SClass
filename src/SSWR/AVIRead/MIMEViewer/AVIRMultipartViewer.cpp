#include "Stdafx.h"
#include "Crypto/Cert/X509PKCS7.h"
#include "Crypto/Hash/HashCreator.h"
#include "IO/MemoryStream.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/SSLEngineFactory.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMultipartViewer.h"
#include "Text/MIMEObj/UnknownMIMEObj.h"
#include "UI/GUITabPage.h"
#include "UI/GUITextBox.h"

#include <stdio.h>

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::AVIRMultipartViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::MultipartMIMEObj> obj) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, obj)
{
	UInt8 hashBuff[64];
	UOSInt i;
	UOSInt j;
	NN<Text::MIMEObject> subObj;
	NN<Text::MIMEObj::MIMEMessage> part;
	this->obj = obj;

	if (obj->GetContentType().StartsWith(UTF8STRC("multipart/signed")))
	{
		this->pnlSMIME = ui->NewPanel(ctrl);
		this->pnlSMIME->SetRect(0, 0, 100, 31, false);
		this->pnlSMIME->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		this->lblSignState = ui->NewLabel(this->pnlSMIME, CSTR("Signature"));
		this->lblSignState->SetRect(4, 4, 100, 23, false);
		this->txtSignState = ui->NewTextBox(this->pnlSMIME, CSTR(""));
		this->txtSignState->SetRect(104, 4, 200, 23, false);
		this->txtSignState->SetReadOnly(true);

		j = this->obj->GetPartCount();
		if (j == 2 && this->obj->GetPart(0).SetTo(part))
		{
			IO::MemoryStream mstm;
			part->WriteStream(mstm);
			
			if (this->obj->GetPartContent(1).SetTo(subObj) && subObj->GetContentType().StartsWith(UTF8STRC("application/pkcs7-signature")))
			{
				UOSInt dataSize;
				UnsafeArray<const UInt8> data = NN<Text::MIMEObj::UnknownMIMEObj>::ConvertFrom(subObj)->GetRAWData(dataSize);
				NN<Crypto::Cert::X509File> x509;
				if (Parser::FileParser::X509Parser::ParseBuff(Data::ByteArrayR(data, dataSize), subObj->GetSourceNameObj()).SetTo(x509))
				{
					if (x509->GetFileType() == Crypto::Cert::X509File::FileType::PKCS7)
					{
						NN<Crypto::Cert::X509PKCS7> pkcs7 = NN<Crypto::Cert::X509PKCS7>::ConvertFrom(x509);
						Crypto::Hash::HashType hashType = pkcs7->GetDigestType();
						NN<Crypto::Hash::HashAlgorithm> hash;
						if (!pkcs7->GetMessageDigest(dataSize).SetTo(data))
						{
							this->txtSignState->SetText(CSTR("Message Digest not found"));
						}
						else if (hashType == Crypto::Hash::HashType::Unknown)
						{
							this->txtSignState->SetText(CSTR("Unknown Digest Type"));
						}
						else if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
						{
							this->txtSignState->SetText(CSTR("Digest Type not supported"));
						}
						else
						{
							UOSInt buffSize;
							UnsafeArray<const UInt8> dataBuff = mstm.GetBuff(buffSize);
							if (buffSize > 2 && dataBuff[buffSize - 2] == 13 && dataBuff[buffSize - 1] == 10)
							{
								buffSize -= 2;
							}
							hash->Calc(dataBuff, buffSize);
							buffSize = hash->GetResultSize();
							hash->GetValue(hashBuff);
							hash.Delete();
							if (!Text::StrEqualsC(data, dataSize, hashBuff, buffSize))
							{
								this->txtSignState->SetText(CSTR("Digest Mismatch"));
							}
							else
							{
								UOSInt encLen;
								UnsafeArray<const UInt8> encDigestData;
								if (!pkcs7->GetEncryptedDigest(encLen).SetTo(encDigestData))
								{
									this->txtSignState->SetText(CSTR("Signature data not found"));
								}
								else
								{
									NN<Crypto::Cert::X509Cert> crt;
									if (!pkcs7->GetNewCert(0).SetTo(crt))
									{
										this->txtSignState->SetText(CSTR("Cert not found"));
									}
									else
									{
										NN<Crypto::Cert::X509Key> key;
										if (!crt->GetNewPublicKey().SetTo(key))
										{
											this->txtSignState->SetText(CSTR("Public key not found"));
										}
										else
										{
											NN<Net::SSLEngine> ssl;
											if (!Net::SSLEngineFactory::Create(core->GetTCPClientFactory(), false).SetTo(ssl))
											{
												this->txtSignState->SetText(CSTR("Error in initializing SSL Engine"));
											}
											else
											{
												Text::StringBuilderUTF8 sb;
												sb.AppendC(UTF8STRC("Hash:\r\n"));
												sb.AppendHexBuff(hashBuff, buffSize, ' ', Text::LineBreakType::CRLF);

												Crypto::Cert::DigestInfo digestInfo;
												Bool match = false;
												UInt8 decBuff[256];
												UOSInt decLen = ssl->Decrypt(key, decBuff, Data::ByteArrayR(encDigestData, encLen), Crypto::Encrypt::RSACipher::Padding::PKCS1);
												if (decLen > 0)
												{
													if (Crypto::Cert::X509File::ParseDigestType(digestInfo, decBuff, decBuff + decLen))
													{
														sb.AppendC(UTF8STRC("\r\nHash Type: "));
														sb.Append(Crypto::Hash::HashTypeGetName(digestInfo.hashType));
														sb.AppendC(UTF8STRC("\r\nHash Value:\r\n"));
														sb.AppendHexBuff(digestInfo.hashVal, digestInfo.hashLen, ' ', Text::LineBreakType::CRLF);
														match = Text::StrEqualsC(digestInfo.hashVal, digestInfo.hashLen, hashBuff, buffSize);
													}
												}
												printf("%s\r\n", sb.ToPtr());
												if (match)
												{
													this->txtSignState->SetText(CSTR("Signature valid"));
												}
												else
												{
													this->txtSignState->SetText(CSTR("Signature invalid"));
												}
												ssl.Delete();
											}

											key.Delete();
										}
										crt.Delete();
									}
								}
							}
						}
					}
					else
					{
						this->txtSignState->SetText(CSTR("Signature is not in PKCS7 format"));
					}
					x509.Delete();
				}
				else
				{
					this->txtSignState->SetText(CSTR("Error in parsing Signature file"));
				}
			}
			else
			{
				this->txtSignState->SetText(CSTR("Signature file not found"));
			}
		}
		else
		{
			this->txtSignState->SetText(CSTR("Unexpected Multipart format"));
		}
	}
	NN<UI::GUITabPage> tp;
	NN<SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer> viewer;
	this->tcParts = ui->NewTabControl(ctrl);
	this->tcParts->SetDockType(UI::GUIControl::DOCK_FILL);

	NN<Text::String> defMsg;
	if (obj->GetDefMsg().SetTo(defMsg) && defMsg->v[0])
	{
		NN<UI::GUITextBox> txt;
		tp = this->tcParts->AddTabPage(CSTR("Default"));
		txt = ui->NewTextBox(tp, defMsg->ToCString(), true);
		txt->SetDockType(UI::GUIControl::DOCK_FILL);
		txt->SetReadOnly(true);
	}

	i = 0;
	j = obj->GetPartCount();
	while (i < j)
	{
		if (obj->GetPartContent(i).SetTo(subObj))
		{
			tp = this->tcParts->AddTabPage(subObj->GetSourceNameObj());
			if (SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(core, ui, tp, sess, subObj).SetTo(viewer))
				this->subViewers.Add(viewer);
		}
		i++;
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMultipartViewer::~AVIRMultipartViewer()
{
	this->subViewers.DeleteAll();
}
