#include <vcl.h>
#pragma hdrstop
// ---------------------------------------------------------------------------
#include "ConvDW.h"
#include <io.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <dir.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <process.h>

wchar_t LANGUAGELITERA = L'R';
extern wchar_t Inf[], LHA[];

// ---------------------------------------------------------------------------
char ChConvChrDOStoWIN(unsigned char B) {
 if(B>127&&B<176) B+=64;
 else if(B>223&&B<240) B+=16;
 else switch(B) {
  case 179: B='|'; break; //�
  case 180: B='+'; break; //�
  case 191: B='+'; break; //�
  case 192: B='+'; break; //�
  case 193: B='+'; break; //�
  case 194: B='+'; break; //�
  case 195: B='+'; break; //�
  case 196: B='-'; break; //�
  case 197: B='+'; break; //�
  case 217: B='+'; break; //�
  case 218: B='+'; break; //�
  case 240: B=168; break; //�
  case 241: B=184; break; //�
  case 242: B=170; break; //�
  case 243: B=186; break; //�
  case 244: B=175; break; //�
  case 245: B=191; break; //�
  case 248: B=176; break; //������
// case 251: B=1??; break; //������
  case 252: B=185; break; //�����
  case 253: B=164; break; //��������
 }
 return B;
}
//---------------------------------------------------------------------------
char *ChStrDOStoWIN(char *S) {
 for(char *s=S;*s;s++) *s=ChConvChrDOStoWIN(*s); return S;
}
//---------------------------------------------------------------------------
wchar_t ConvChrDOStoWIN(wchar_t B) { // unsigned wchar_t ???
	if (B > 127 && B < 176)
		B += 64;
	else if (B > 223 && B < 240)
		B += 16;
	else
		switch(B) {
		case 179:
			B = L'|';
			break; // �
		case 180:
			B = L'+';
			break; // �
		case 191:
			B = L'+';
			break; // �
		case 192:
			B = L'+';
			break; // �
		case 193:
			B = L'+';
			break; // �
		case 194:
			B = L'+';
			break; // �
		case 195:
			B = L'+';
			break; // �
		case 196:
			B = L'-';
			break; // �
		case 197:
			B = L'+';
			break; // �
		case 217:
			B = L'+';
			break; // �
		case 218:
			B = L'+';
			break; // �
		case 240:
			B = 168;
			break; // �
		case 241:
			B = 184;
			break; // �
		case 242:
			B = 170;
			break; // �
		case 243:
			B = 186;
			break; // �
		case 244:
			B = 175;
			break; // �
		case 245:
			B = 191;
			break; // �
		case 248:
			B = 176;
			break; // ������
			// case 251: B=1??; break; //������
		case 252:
			B = 185;
			break; // �����
		case 253:
			B = 164;
			break; // ��������
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t ConvChrWINtoDOS(wchar_t B) {
	if (B > 191 && B < 240)
		B -= 64;
	else if (B > 239)
		B -= 16;
	else
		switch(B) {
		case 178:
			B = 73;
			break; // I
		case 179:
			B = 105;
			break; // i
		case 168:
			B = 240;
			break; // �
		case 184:
			B = 241;
			break; // �
		case 170:
			B = 242;
			break; // �
		case 186:
			B = 243;
			break; // �
		case 175:
			B = 244;
			break; // �
		case 191:
			B = 245;
			break; // �
		case 176:
			B = 248;
			break; // ������
		case 185:
			B = 252;
			break; // �����
		case 164:
			B = 253;
			break; // ��������
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t ConvChrWINtoPRN(wchar_t B) {
	if (B > 191 && B < 240)
		B -= 64;
	else if (B > 239)
		B -= 16;
	else
		switch(B) {
		case 178:
			B = 73;
			break; // I
		case 179:
			B = 105;
			break; // i
		case 168:
			B = 240;
			break; // �
		case 184:
			B = 241;
			break; // �
		case 170:
			B = 242;
			break; // �
		case 186:
			B = 243;
			break; // �
		case 175:
			B = 244;
			break; // �
		case 191:
			B = 245;
			break; // �
		case 176:
			B = 248;
			break; // ������
		case 185:
			B = 252;
			break; // �����
		case 164:
			B = 253;
			break; // ��������
		}
	return B;
}

// ---------------------------------------------------------------------------
wchar_t *StrDOStoWIN(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrDOStoWIN(*s);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *StrWINtoDOS(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrWINtoDOS(*s);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *StrWINtoPRN(wchar_t *S) {
	for (wchar_t*s = S; *s; s++)
		* s = ConvChrWINtoPRN(*s);
	return S;
}

// ---------------------------------------------------------------------------
double Round(double D, double R) {
	return R*(D >= 0.0 ? floor(D / R + 0.5) : ceil(D / R - 0.5));
}

// ---------------------------------------------------------------------------
double Round(double D) {
	return Round(D, 1.0);
}

// ---------------------------------------------------------------------------
wchar_t *Fgets(wchar_t *s, int n, FILE *stream) {
	wchar_t *Dest = fgetws(s, n, stream);
	return Dest ? StrDOStoWIN(s) : NULL;
}

// ---------------------------------------------------------------------------
int Fputs(wchar_t *s, FILE *stream) {
	int Rez = 0;
	wchar_t Buf[2048];
	if (s && *s) {
		wcscpy(Buf, s);
		Rez = fputws(StrWINtoDOS(Buf), stream);
	}
	return Rez;
}

// ---------------------------------------------------------------------------
int Fgetc(FILE *F) {
	return(int)ConvChrDOStoWIN((wchar_t)fgetwc(F));
}

// ---------------------------------------------------------------------------
int Fputc(wchar_t B, FILE *F) {
	return fputwc((int)ConvChrWINtoDOS(B), F);
}

// ---------------------------------------------------------------------------
wchar_t IntToChar(int i) {
	if (i < 36)
		return(wchar_t)(i + (i < 10 ? 48 : 87));
	switch(i) {
	case 36:
		return L'#';
	case 37:
		return L'$';
	case 38:
		return L'%';
	case 39:
		return L'^';
	case 40:
		return L'&';
	case 41:
		return L'(';
	case 42:
		return L')';
	case 43:
		return L'-';
	case 44:
		return L'_';
	case 45:
		return L'@';
	case 46:
		return L'!';
	case 47:
		return L'"';
	case 48:
		return L'~';
	default:
		return L'0';
	}
}

// ---------------------------------------------------------------------------
int CharToInt(wchar_t C) {
	if (C >= L'0' && C <= L'9')
		return(int)(C - L'0');
	if (C >= L'a' && C <= L'z')
		return(int)(C - L'W');
	if (C >= L'A' && C <= L'Z')
		return(int)(C - L'A') + 10;
	switch(C) {
	case L'#':
		return 36;
	case L'$':
		return 37;
	case L'%':
		return 38;
	case L'^':
		return 39;
	case L'&':
		return 40;
	case L'(':
		return 41;
	case L')':
		return 42;
	case L'-':
		return 43;
	case L'_':
		return 44;
	case L'@':
		return 45;
	case L'!':
		return 46;
	case L'"':
		return 47;
	case L'~':
		return 48;
	default:
		return 0;
	}
}

// ---------------------------------------------------------------------------
int KSimbStr(wchar_t *S, wchar_t C) {
	int K = 0;
	while (*S)
		if (*S++ == C)
			K++;
	return K;
}

// ---------------------------------------------------------------------------
bool isLargeCiril(wchar_t c) {
	switch(c) {
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool isLargeLatin(wchar_t c) {
	switch(c) {
	case L'A':
		return true;
	case L'B':
		return true;
	case L'C':
		return true;
	case L'D':
		return true;
	case L'E':
		return true;
	case L'F':
		return true;
	case L'G':
		return true;
	case L'H':
		return true;
	case L'I':
		return true;
	case L'J':
		return true;
	case L'K':
		return true;
	case L'L':
		return true;
	case L'M':
		return true;
	case L'N':
		return true;
	case L'O':
		return true;
	case L'P':
		return true;
	case L'Q':
		return true;
	case L'R':
		return true;
	case L'S':
		return true;
	case L'T':
		return true;
	case L'U':
		return true;
	case L'V':
		return true;
	case L'W':
		return true;
	case L'X':
		return true;
	case L'Y':
		return true;
	case L'Z':
		return true;
	}
	return false;
}

// ---------------------------------------------------------------------------
bool isLarge(wchar_t c) {
	return isLargeLatin(c) || isLargeCiril(c);
}

// ---------------------------------------------------------------------------
bool isciril(wchar_t c) {
	switch(c) {
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true;
	case L'�':
		return true; // case L'\'': return true;
	default:
		return false;
	}
}

// ---------------------------------------------------------------------------
struct FileMasStr {
	FileMasStr *Sled;
	wchar_t *File, **MasStr;
	int K;
}*RootFMS;

// ---------------------------------------------------------------------------
int FindFile(wchar_t *NameFile, FileMasStr **F) {
	if (!(*F = RootFMS))
		return 0;
	if (wcscmp(NameFile, RootFMS->File) < 0) {
		*F = NULL;
		return 0;
	}
	for (; ; *F = (*F)->Sled) {
		if (!wcscmp((*F)->File, NameFile))
			return 1;
		if (!((*F)->Sled))
			return 0;
		if (wcscmp((*F)->Sled->File, NameFile) > 0)
			return 0;
	}
}

// ---------------------------------------------------------------------------
FileMasStr *TakeFileMasStr(wchar_t *NameFile) {
	FileMasStr *F, *f;
	int i = 0, K = 0;
	FILE *Fi;
	wchar_t S[1024], *s;
	if (FindFile(NameFile, &F))
		return F;
	f = (FileMasStr*)calloc(1, sizeof(FileMasStr));
	f->File = (wchar_t*)calloc(wcslen(NameFile)+1,sizeof(wchar_t));
	wcscpy(f->File,NameFile);
	if (_waccess(NameFile, 0)) {
		switch(LANGUAGELITERA) {
		case L'U':
			swprintf(Inf, L"�� �������� ����� ��������� ����������\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"�������� �������!", MB_ICONERROR);
			break;
		case L'E':
			swprintf(Inf, L"The file of text messages is not found\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"System error!", MB_ICONERROR);
			break;
		default:
			swprintf(Inf, L"�� ������ ���� ��������� ���������\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"��������� ������!", MB_ICONERROR);
		}
		return NULL;
	}
	if (!(Fi = _wfopen(NameFile, L"r"))) {
		switch(LANGUAGELITERA) {
		case L'U':
			swprintf(Inf, L"�� ������� ����� ��������� ����������\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"�������� �������!", MB_ICONERROR);
			break;
		case L'E':
			swprintf(Inf, L"The file of text messages is not opened\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"System error!", MB_ICONERROR);
			break;
		default:
			swprintf(Inf, L"�� ������ ���� ��������� ���������\n<%s>",
				NameFile);
			Application->MessageBox(Inf, L"��������� ������!", MB_ICONERROR);
		}
		return NULL;
	}
	while (fgetws(S, 1023, Fi))
		K++;
	f->MasStr = (wchar_t**)calloc(K, sizeof(wchar_t*));
	f->K = K;
	rewind(Fi);
	while (fgetws(S, 1023, Fi)) {
	 if (s = wcsrchr(S, L'\n')) *s = L'\0';
	 f->MasStr[i++]=wcscpy((wchar_t*)calloc(wcslen(S)+1,sizeof(wchar_t)),S);
	}
	fclose(Fi);
	if (!F) {
	 f->Sled = RootFMS;
	 return RootFMS = f;
	}
	else {
		f->Sled = F->Sled;
		return F->Sled = f;
	}
}

// ---------------------------------------------------------------------------
wchar_t *TextFromFile(wchar_t *Dir,wchar_t *NameFile,
  TStringList **SL,int N){
 if(!(*SL)){
  wchar_t File[256];
  *SL = new TStringList;
  if(Dir&&*Dir&&!_waccess(Dir,0))
   swprintf(File,L"%s\\%s%c.txt",Dir,NameFile,LANGUAGELITERA);
  else
   swprintf(File,L"%s%c.txt",NameFile,LANGUAGELITERA);
  if(!_waccess(File,0))
   (*SL)->LoadFromFile(File);
  else{
   switch(LANGUAGELITERA){
	case 'U':
	 swprintf(Inf,L"����� <%sU> �� ����",NameFile);
	 Application->MessageBox(Inf,L"�������� �������!",MB_OK);
	 break;
	case 'E':
	 swprintf(Inf,L"File <%sE> does not exist",NameFile,N);
	 Application->MessageBox(Inf,L"System error!",MB_OK);
	 break;
	default:
	 swprintf(Inf,L"���� <%sR> �� ����������",NameFile);
	 Application->MessageBox(Inf,L"!",MB_OK);
   }
   return NULL;
  }
  if((*SL)->Count<N){
   switch(LANGUAGELITERA){
	case 'U':
	 swprintf(Inf,L"� ���� <%sU> ���� ������ �� ������� - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"�������� �������!",MB_OK);
	 break;
	case 'E':
	 swprintf(Inf,L"In the file <%sE> there is no line number - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"System error!",MB_OK);
	 break;
	default:
	 swprintf(Inf,L"� ����� <%sR> ��� ������ ����� - <%d>",NameFile,N);
	 Application->MessageBox(Inf,L"��������� ������!",MB_OK);
   }
   return NULL;
 }}
 return (*SL)->Strings[N-1].w_str();
}
// ---------------------------------------------------------------------------
wchar_t *ToLat(wchar_t *S) {
	wchar_t *s = S;
	while (*s) {
		if (isciril(*s))
			switch(*s) {
			case L'�':
				*s = L'A';
				break;
			case L'�':
				*s = L'P';
				break;
			case L'�':
				*s = L'B';
				break;
			case L'�':
				*s = L'R';
				break;
			case L'�':
				*s = L'V';
				break;
			case L'�':
				*s = L'S';
				break;
			case L'�':
				*s = L'G';
				break;
			case L'�':
				*s = L'T';
				break;
			case L'�':
				*s = L'D';
				break;
			case L'�':
				*s = L'U';
				break;
			case L'�':
				*s = L'E';
				break;
			case L'�':
				*s = L'F';
				break;
			case L'�':
				*s = L'E';
				break;
			case L'�':
				*s = L'H';
				break;
			case L'�':
				*s = L'G';
				break;
			case L'�':
				*s = L'C';
				break;
			case L'�':
				*s = L'Z';
				break;
			case L'�':
				*s = L'C';
				break;
			case L'�':
				*s = L'I';
				break;
			case L'�':
				*s = L'S';
				break;
			case L'�':
				*s = L'J';
				break;
			case L'�':
				*s = L'S';
				break;
			case L'�':
				*s = L'K';
				break;
			case L'�':
				*s = L'Y';
				break;
			case L'�':
				*s = L'L';
				break;
			case L'�':
				*s = L'Y';
				break;
			case L'�':
				*s = L'M';
				break;
			case L'�':
				*s = L'X';
				break;
			case L'�':
				*s = L'N';
				break;
			case L'�':
				*s = L'E';
				break;
			case L'�':
				*s = L'O';
				break;
			case L'�':
				*s = L'U';
				break;
			case L'�':
				*s = L'A';
				break;
			case L'�':
				*s = L'I';
				break;
			case L'�':
				*s = L'I';
				break;
			case L'�':
				*s = L'E';
				break;
			case L'�':
				*s = L'a';
				break;
			case L'�':
				*s = L'p';
				break;
			case L'�':
				*s = L'b';
				break;
			case L'�':
				*s = L'r';
				break;
			case L'�':
				*s = L'v';
				break;
			case L'�':
				*s = L's';
				break;
			case L'�':
				*s = L'g';
				break;
			case L'�':
				*s = L't';
				break;
			case L'�':
				*s = L'd';
				break;
			case L'�':
				*s = L'u';
				break;
			case L'�':
				*s = L'e';
				break;
			case L'�':
				*s = L'f';
				break;
			case L'�':
				*s = L'e';
				break;
			case L'�':
				*s = L'h';
				break;
			case L'�':
				*s = L'g';
				break;
			case L'�':
				*s = L'c';
				break;
			case L'�':
				*s = L'z';
				break;
			case L'�':
				*s = L'c';
				break;
			case L'�':
				*s = L'i';
				break;
			case L'�':
				*s = L's';
				break;
			case L'�':
				*s = L'j';
				break;
			case L'�':
				*s = L's';
				break;
			case L'�':
				*s = L'k';
				break;
			case L'�':
				*s = L'y';
				break;
			case L'�':
				*s = L'l';
				break;
			case L'�':
				*s = L'y';
				break;
			case L'�':
				*s = L'm';
				break;
			case L'�':
				*s = L'x';
				break;
			case L'�':
				*s = L'n';
				break;
			case L'�':
				*s = L'e';
				break;
			case L'�':
				*s = L'o';
				break;
			case L'�':
				*s = L'u';
				break;
			case L'�':
				*s = L'a';
				break;
			case L'�':
				*s = L'i';
				break;
			case L'�':
				*s = L'i';
				break;
			case L'�':
				*s = L'e';
				break;
			}
		s++;
	}
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *LowToUpp(wchar_t *S) {
	for (wchar_t*s = S; *s; s++) {
		if (isalpha(*s)) {
			if (*s > 96)
				* s -= 32;
		}
		else {
			if (isciril(*s))
				switch(*s) {
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				}
		}
	}
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *UppToLow(wchar_t *S) {
	for (wchar_t*s = S; *s; s++) {
		if (isalpha(*s)) {
			if (*s <= 96)
				* s += 32;
		}
		else {
			if (isciril(*s))
				switch(*s) {
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				case L'�':
					*s = L'�';
					break;
				}
		}
	}
	return S;
}
// ---------------------------------------------------------------------------
int max(int a, int b) {
	return a > b ? a : b;
}

// ---------------------------------------------------------------------------
int min(int a, int b) {
	return a > b ? b : a;
}

// ---------------------------------------------------------------------------
wchar_t *NameToFile(wchar_t *S) {
	wchar_t Buf[256], *b = Buf, *s = S;
	for (; *s; s++) {
		switch(*s) {
		case L'.':
			*b++ = L'_';
		case L'/':
			*b++ = L'_';
		case L' ':
		case L',':
			break;
		default:
			*b++ = *s;
		}
	}
	*b = L'\0';
	ToLat(Buf);
	wcscpy(S, Buf);
	return S;
}

// ---------------------------------------------------------------------------
wchar_t *NameToFileRus(wchar_t *S) {
	wchar_t Buf[256], *b = Buf, *s = S;
	for (; *s; s++) {
		switch(*s) {
		case L'.':
			*b++ = L'_';
		case L'/':
			*b++ = L'_';
		case L' ':
		case L',':
			break;
		default:
			*b++ = *s;
		}
	}
	*b = L'\0';
	wcscpy(S, Buf);
	return S;
}

// ---------------------------------------------------------------------------
void Unlink(wchar_t *File) {
	int Rez;
	FILE *F = _wfopen(File, L"w");
	if (!F) {
		swprintf(Inf,
			L"���� <%s> �� ����� ���� ������.\n" L"���������� ����� ���������.", File);
		Application->MessageBox(Inf, L"��������� ������!",
			MB_OK | MB_ICONWARNING);
	}
	fclose(F);
}

// ---------------------------------------------------------------------------
bool GrandToLitlSimbKiril(wchar_t &B) { // ������� true, ���� ���� ������
	switch(B) {
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	default:
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
bool LitlToGrandSimbKiril(wchar_t &B) { // ������� true, ���� ���� ������
	switch(B) {
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	case L'�':
		B = L'�';
		break;
	default:
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
void CopyFromArhiv(wchar_t *Arhiv, wchar_t *File) {
	long Ta = 0l, Tf = 0l; AnsiString ArhFileA,FileA;
	struct _stat St; wchar_t ArhFile[256], *t, Bak[256];
	swprintf(ArhFile, L"%s\\%s", Arhiv, File);
	if (!_waccess(ArhFile, 0)) {
		_wstat(ArhFile, &St);
		Ta = St.st_mtime;
	}
	if (!_waccess(File, 0)) {
		_wstat(File, &St);
		Tf = St.st_mtime;
	}
	if (Ta > Tf) {
		if (!_waccess(File, 0)) {
			wcscpy(Bak, File);
			if (t = wcsrchr(Bak, L'.'))
				wcscpy(t + 1, L"bak");
			else
				wcscat(Bak, L".bak");
			_wunlink(Bak);
			_wrename(File, Bak);
		}
		ArhFileA=ArhFile; FileA=File;
		CopyFile(ArhFileA.c_str(),FileA.c_str(),false);
	}
}

// ---------------------------------------------------------------------------
void CopyToArhiv(wchar_t *Arhiv, wchar_t *File) {
	long Ta = 0l, Tf = 0l; AnsiString ArhFileA,FileA;
	struct _stat St; wchar_t ArhFile[256], *t, Bak[256];
	swprintf(ArhFile, L"%s\\%s", Arhiv, File);
	if (!_waccess(ArhFile, 0)) {
		_wstat(ArhFile, &St);
		Ta = St.st_mtime;
	}
	if (!_waccess(File, 0)) {
		_wstat(File, &St);
		Tf = St.st_mtime;
	}
	if (Tf > Ta) {
		if (!_waccess(ArhFile, 0)) {
			wcscpy(Bak, ArhFile);
			if (t = wcsrchr(Bak, L'.'))
				wcscpy(t + 1, L"bak");
			else
				wcscat(Bak, L".bak");
			_wunlink(Bak);
			_wrename(ArhFile, Bak);
		}
		ArhFileA=ArhFile; FileA=File;
		CopyFile(ArhFileA.c_str(),FileA.c_str(),false);
	}
}

// ---------------------------------------------------------------------------
bool CreateDir(wchar_t *Dir) {
	if (_waccess(Dir, 0)) {
		wchar_t NadDir[256], *s;
		wcscpy(NadDir, Dir);
		if (s = wcsrchr(NadDir, L'\\')) {
			*s = L'\0';
			if (CreateDir(NadDir)) {
				*s = L'\\';
				return !_wmkdir(Dir);
			}
			*s = L'\\';
		}
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
void Pack(wchar_t *Kod, wchar_t *Path, wchar_t *Arc, wchar_t *File) {
	wchar_t Arhiv[256], Dir[256];
	if (_waccess(Path, 0)) {
		_wgetcwd(Dir, 255);
		_wmkdir(Path);
		_wchdir(Dir);
	}
	swprintf(Arhiv, L"%s\\%s", Path, Arc);
	_wspawnl(P_WAIT, LHA, LHA, Kod, Arhiv, File, NULL);
}

// ---------------------------------------------------------------------------
int LLat(TCanvas *Canvas, wchar_t *S, wchar_t b) {
	int Size = Canvas->Font->Size, L = 0;
	wchar_t *s, C = L'\0', B[2];
	if (!isdigit(S[0])) {
		for (s = S; *s; s++)
			if (isdigit(*s) && (s[-1] != L'[') && !isdigit(s[-1])) {
				C = *s;
				*s = L'\0';
				break;
			}
	}
	if (!Canvas->Font->Style.Contains(fsItalic)) {
		L += Canvas->TextWidth(S);
	}
	else
		L += Canvas->TextWidth(S);
	if (C) {
		Canvas->Font->Size = Round(3 * Canvas->Font->Size / 5);
		*s = C;
		L += Canvas->TextWidth(s);
		Canvas->Font->Size = Size;
	}
	B[0] = b;
	B[1] = L'\0';
	L += Canvas->TextWidth(B);
	return L;
}

// ---------------------------------------------------------------------------
int LGrekLat(TCanvas *Canvas, int Kod, wchar_t *s, wchar_t B) {
	AnsiString NameFont = Canvas->Font->Name, S = wchar_t(Kod);
	int L;
	Canvas->Font->Name = "Symbol";
	L = Canvas->TextWidth(S);
	Canvas->Font->Name = NameFont;
	S = s;
	S += B;
	L += Canvas->TextWidth(S);
	return L;
}

// ---------------------------------------------------------------------------
int LGrek(TCanvas *Canvas, int Kod, bool Index) {
	AnsiString NameFont = Canvas->Font->Name, S = wchar_t(Kod);
	int L, Size = Canvas->Font->Size;
	bool ExFont = Canvas->Font->Style.Contains(fsBold),
	ItFont = Canvas->Font->Style.Contains(fsItalic),
	UnFont = Canvas->Font->Style.Contains(fsUnderline);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() >> fsBold;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() >> fsUnderline;
	Canvas->Font->Name = "Symbol";
	if (Index)
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	L = Canvas->TextWidth(S);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() << fsBold;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() << fsUnderline;
	Canvas->Font->Name = NameFont;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int LLiter(TCanvas *Canvas, wchar_t B, bool Index) {
	AnsiString S = B;
	int L, Size = Canvas->Font->Size;
	bool ExFont = Canvas->Font->Style.Contains(fsBold),
	ItFont = Canvas->Font->Style.Contains(fsItalic),
	UnFont = Canvas->Font->Style.Contains(fsUnderline);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() >> fsBold;
	if (ItFont && (isciril(B) || IsCifra(B)))
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() >> fsUnderline;
	if (Index)
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	L = Canvas->TextWidth(S);
	if (ExFont)
		Canvas->Font->Style = TFontStyles() << fsBold;
	if (ItFont && (isciril(B) || IsCifra(B)))
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (UnFont)
		Canvas->Font->Style = TFontStyles() << fsUnderline;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int DrawLat(TCanvas *Canvas, int Left, int Top, wchar_t *S, wchar_t b) {
	int Size = Canvas->Font->Size, H = -Canvas->Font->Height;
	wchar_t *s, C = L'\0', B[2];
	if (!isdigit(S[0]) && ((S[0] != L'-') || !isdigit(S[1]))) {
		for (s = S; *s; s++)
			if (isdigit(*s) && (s[-1] != L'[') && !isdigit(s[-1]) || (*s == L'$')
				) {
				C = *s;
				*s = L'\0';
				break;
			}
	}
	/* if(!Canvas->Font->Style.Contains(fsItalic)){  //�������� �������
	Canvas->Font->Style=TFontStyles()<<fsItalic;
	Canvas->TextOut(Left,Top,S); Left+=Canvas->TextWidth(S);
	Canvas->Font->Style=TFontStyles()>>fsItalic;
	}
	else{
	Canvas->TextOut(Left,Top,S); Left+=Canvas->TextWidth(S);
	} */
	Canvas->TextOut(Left, Top, S);
	Left += Canvas->TextWidth(S);
	if (C) {
		Canvas->Font->Size = Round(3 * Canvas->Font->Size / 5);
		*s = C;
		if (*s == L'$')
			s++;
		Canvas->TextOut(Left, Top + Round(H / 2), s);
		Left += Canvas->TextWidth(s);
		Canvas->Font->Size = Size;
	}
	B[0] = b;
	B[1] = L'\0';
	Canvas->TextOut(Left, Top, B);
	Left += Canvas->TextWidth(B);
	return Left;
}

// ---------------------------------------------------------------------------
int KodGrek(wchar_t *S, int &Kod) { // ���������� ��� � ����� ��������� �����
	if (wcsstr(S, L"alfa") == S) {
		Kod = 97;
		return 4;
	}
	if (wcsstr(S, L"alpha") == S) {
		Kod = 97;
		return 5;
	}
	if (wcsstr(S, L"beta") == S) {
		Kod = 98;
		return 4;
	}
	if (wcsstr(S, L"betta") == S) {
		Kod = 98;
		return 5;
	}
	if (wcsstr(S, L"gamma") == S) {
		Kod = 103;
		return 5;
	}
	if (wcsstr(S, L"gama") == S) {
		Kod = 103;
		return 5;
	}
	if (wcsstr(S, L"delta") == S) {
		Kod = 100;
		return 5;
	}
	if (wcsstr(S, L"epsilon") == S) {
		Kod = 101;
		return 7;
	}
	if (wcsstr(S, L"dzeta") == S) {
		Kod = 122;
		return 5;
	}
	if (wcsstr(S, L"dseta") == S) {
		Kod = 122;
		return 5;
	}
	if (wcsstr(S, L"teta") == S) {
		Kod = 113;
		return 4;
	}
	if (wcsstr(S, L"eta") == S) {
		Kod = 104;
		return 3;
	}
	if (wcsstr(S, L"jota") == S) {
		Kod = 105;
		return 4;
	}
	if (wcsstr(S, L"iota") == S) {
		Kod = 105;
		return 4;
	}
	if (wcsstr(S, L"kappa") == S) {
		Kod = 107;
		return 5;
	}
	if (wcsstr(S, L"lamda") == S) {
		Kod = 108;
		return 5;
	}
	if (wcsstr(S, L"lambda") == S) {
		Kod = 108;
		return 6;
	}
	if (wcsstr(S, L"mu") == S) {
		Kod = 109;
		return 2;
	}
	if (wcsstr(S, L"nu") == S) {
		Kod = 110;
		return 2;
	}
	if (wcsstr(S, L"ksi") == S) {
		Kod = 120;
		return 3;
	}
	if (wcsstr(S, L"omicron") == S) {
		Kod = 111;
		return 7;
	}
	if (wcsstr(S, L"omikron") == S) {
		Kod = 111;
		return 7;
	}
	if (wcsstr(S, L"pi") == S) {
		Kod = 112;
		return 2;
	}
	if (wcsstr(S, L"ro") == S) {
		Kod = 114;
		return 2;
	}
	if (wcsstr(S, L"sigma") == S) {
		Kod = 115;
		return 5;
	}
	if (wcsstr(S, L"tau") == S) {
		Kod = 116;
		return 3;
	}
	if (wcsstr(S, L"ipsilon") == S) {
		Kod = 117;
		return 7;
	}
	if (wcsstr(S, L"fi") == S) {
		Kod = 106;
		return 2;
	}
	if (wcsstr(S, L"hi") == S) {
		Kod = 99;
		return 2;
	}
	if (wcsstr(S, L"psi") == S) {
		Kod = 121;
		return 3;
	}
	if (wcsstr(S, L"omega") == S) {
		Kod = 119;
		return 5;
	}
	if (wcsstr(S, L"ALFA") == S) {
		Kod = 65;
		return 4;
	}
	if (wcsstr(S, L"ALPHA") == S) {
		Kod = 65;
		return 5;
	}
	if (wcsstr(S, L"BETA") == S) {
		Kod = 66;
		return 4;
	}
	if (wcsstr(S, L"BETTA") == S) {
		Kod = 66;
		return 5;
	}
	if (wcsstr(S, L"GAMMA") == S) {
		Kod = 71;
		return 5;
	}
	if (wcsstr(S, L"DELTA") == S) {
		Kod = 68;
		return 5;
	}
	if (wcsstr(S, L"EPSILON") == S) {
		Kod = 69;
		return 7;
	}
	if (wcsstr(S, L"DZETA") == S) {
		Kod = 90;
		return 5;
	}
	if (wcsstr(S, L"DSETA") == S) {
		Kod = 90;
		return 5;
	}
	if (wcsstr(S, L"ETA") == S) {
		Kod = 72;
		return 3;
	}
	if (wcsstr(S, L"TETA") == S) {
		Kod = 81;
		return 4;
	}
	if (wcsstr(S, L"JOTA") == S) {
		Kod = 73;
		return 4;
	}
	if (wcsstr(S, L"IOTA") == S) {
		Kod = 73;
		return 4;
	}
	if (wcsstr(S, L"KAPPA") == S) {
		Kod = 75;
		return 5;
	}
	if (wcsstr(S, L"LAMDA") == S) {
		Kod = 76;
		return 5;
	}
	if (wcsstr(S, L"LAMBDA") == S) {
		Kod = 76;
		return 6;
	}
	if (wcsstr(S, L"MU") == S) {
		Kod = 77;
		return 2;
	}
	if (wcsstr(S, L"NU") == S) {
		Kod = 78;
		return 2;
	}
	if (wcsstr(S, L"KSI") == S) {
		Kod = 88;
		return 3;
	}
	if (wcsstr(S, L"OMICRON") == S) {
		Kod = 79;
		return 7;
	}
	if (wcsstr(S, L"OMIKRON") == S) {
		Kod = 79;
		return 7;
	}
	if (wcsstr(S, L"PI") == S) {
		Kod = 80;
		return 2;
	}
	if (wcsstr(S, L"RO") == S) {
		Kod = 82;
		return 2;
	}
	if (wcsstr(S, L"SIGMA") == S) {
		Kod = 83;
		return 5;
	}
	if (wcsstr(S, L"TAU") == S) {
		Kod = 84;
		return 3;
	}
	if (wcsstr(S, L"IPSILON") == S) {
		Kod = 85;
		return 7;
	}
	if (wcsstr(S, L"FI") == S) {
		Kod = 70;
		return 2;
	}
	if (wcsstr(S, L"HI") == S) {
		Kod = 67;
		return 2;
	}
	if (wcsstr(S, L"PSI") == S) {
		Kod = 89;
		return 3;
	}
	if (wcsstr(S, L"OMEGA") == S) {
		Kod = 87;
		return 5;
	}
	Kod = 0;
	return 0;
}

// ---------------------------------------------------------------------------
int LengthGrek(TCanvas *Canvas, wchar_t *S, wchar_t SimbInd) {
	// ���������� ����� ������ � ��������
	int Rez = 0, d, Kod;
	bool Index = false;
	for (wchar_t*s = S; *s; ) {
		if (d = KodGrek(s, Kod)) {
			Rez += LGrek(Canvas, Kod, Index);
			s += d;
		}
		else {
			if (*s == SimbInd)
				Index = true;
			else if (*s == L'\'') {
				Index = false;
				Rez += LLiter(Canvas, SimbInd, Index);
			}
			else
				Rez += LLiter(Canvas, *s, Index);
			s++;
		}
	}
	return Rez;
}

// ---------------------------------------------------------------------------
int DrawGrek(TCanvas *Canvas, int Left, int Top, int Kod, bool Index) {
	AnsiString S, NameFont = Canvas->Font->Name;
	int Size = Canvas->Font->Size, L;
	bool ItFont = Canvas->Font->Style.Contains(fsItalic);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	Canvas->Font->Name = "Symbol";
	if (Index) {
		Top -= Canvas->Font->Height / 2;
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	}
	else
		Top -= Canvas->Font->Height / 12;
	S = wchar_t(Kod);
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	Canvas->Font->Name = NameFont;
	if (Index)
		Canvas->Font->Size = Size;
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	return L;
}

// ---------------------------------------------------------------------------
int DrawCiril(TCanvas *Canvas, int Left, int Top, wchar_t *S, bool Index) {
	int Size = Canvas->Font->Size, L;
	bool ItFont = Canvas->Font->Style.Contains(fsItalic);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() >> fsItalic;
	if (Index) {
		Top -= Round(3.0 * Canvas->Font->Height / 4.0);
		Canvas->Font->Size = Round(3.0 * Size / 4.0);
	}
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	if (ItFont)
		Canvas->Font->Style = TFontStyles() << fsItalic;
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
int DrawLat(TCanvas *Canvas, int Left, int Top, wchar_t *S, bool Index) {
	int Size = Canvas->Font->Size, L;
	if (Index) {
		Top -= Canvas->Font->Height / 2;
		Canvas->Font->Size = Round(2.0 * Size / 3.0);
	}
	L = Canvas->TextWidth(S);
	Canvas->TextOut(Left, Top, S);
	if (Index)
		Canvas->Font->Size = Size;
	return L;
}

// ---------------------------------------------------------------------------
void DrawGrekText(TCanvas *Canvas, int Left, int Top, int Width, wchar_t *S,
	wchar_t SimbInd) {
	wchar_t *s, B, Buf[256], *b = Buf;
	int d, Kod, L;
	bool Index = false, Grek = false;
	if (Width > 0) {
		L = LengthGrek(Canvas, S, SimbInd);
		Left += (Width - L) / 2;
	}
	else
		Left += Round(0.5 * Canvas->TextWidth(L"M"));
	for (wchar_t*s = S; *s; ) {
		if (d = KodGrek(s, Kod)) {
			L = DrawGrek(Canvas, Left, Top, Kod, Index);
			Left += Round(5.0 * L / 4.0);
			s += d;
			Grek = true;
		}
		else {
			if (*s == SimbInd)
				Index = true;
			else {
				if (*s == L'\'' || *s == L'(' || *s == L',' || *s == L')') {
					if (Index)
						Index = false;
					B = s[1];
					s[1] = L'\0';
					L = DrawLat(Canvas, Left, Top, s, Index);
					s[1] = B;
				}
				else {
					if (isciril(*s)) {
						while (!KodGrek(s, Kod) && (isciril(*s) || IsCifra(*s)))
							* b++ = *s++;
						*b = L'\0';
						s--;
						L = DrawCiril(Canvas, Left, Top, Buf, Index);
						b = Buf;
					}
					else if (IsCifra(*s)) {
						while (!KodGrek(s, Kod) && IsCifra(*s))
							* b++ = *s++;
						*b = L'\0';
						s--;
						if (Grek) {
							Index = true;
							Grek = false;
						}
						L = DrawCiril(Canvas, Left, Top, Buf, Index);
						Index = false;
						b = Buf;
					}
					else if (isalpha(*s)) {
						while (!KodGrek(s, Kod) && isalpha(*s))
							* b++ = *s++;
						*b = L'\0';
						s--;
						L = DrawLat(Canvas, Left, Top, Buf, Index);
						if (IsCifra(s[1]))
							Index = true;
						b = Buf;
					}
					else {
						B = s[1];
						s[1] = L'\0';
						Grek = false;
						L = DrawCiril(Canvas, Left, Top, s, false);
						s[1] = B;
					}
				}
				Left += L;
			}
			s++;
		}
	}
}

// ---------------------------------------------------------------------------
void DrawGrekText(TCanvas *Canvas, const TRect &Rect, wchar_t *S,
	wchar_t SimbInd) {
	DrawGrekText(Canvas, Rect.Left, Rect.Top, Rect.Width(), S, SimbInd);
}

// ---------------------------------------------------------------------------
wchar_t *CleanStr(wchar_t *SIn, wchar_t *SOut) {
	// ������� ������ �� ��������, �������� ��������� � �������� ����� � ������ � � ����� ������
	wchar_t *s;
	*SOut = L'\0'; // ��� ����� �������� ������� ��������� ������
	if (!SOut)
		return NULL;
	if (!SIn)
		return SOut;
	while (*SIn&&((*SIn==L' ')||(*SIn==L'\t')||
		  (*SIn==L'\n')))
		SIn++;
	wcscpy(SOut, SIn);
	for (s = SOut + wcslen(SOut) - 1;
		(*s == L' ') || (*s == L'\t') || (*s == L'\n'); s--);
	*++s = L'\0';
	return SOut;
}

// ---------------------------------------------------------------------------
int StrCmp(wchar_t *s1, wchar_t *s2) {
	wchar_t S1[256], S2[256];
	if (!s1)
		if (!s2 || !*s2)
			return 0;
		else
			return -1;
	if (!s2)
		if (!*s1)
			return 0;
		else
			return 1;
	CleanStr(s1, S1);
	CleanStr(s2, S2);
	return wcscmp(S1, S2);
}

// ---------------------------------------------------------------------------
bool CopyFiles(wchar_t *Fi, wchar_t *Fo) {
	wchar_t Sys[1024];
	int Rez=0;
//	Rez = CopyFile(Fi, Fo, false);
	if (!Rez) {
		swprintf(Sys,
			L"����������� ����� <%s>\n� ���� <%s> ������ � �������\n" "��� ������ - %d", Fi, Fo, GetLastError());
		Application->MessageBox(Sys, L"������!", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

// ---------------------------------------------------------------------------
wchar_t *Pole(wchar_t *S) {
	wchar_t *s;
	for (s = S; *s && (*s != L'\xb3') && (*s != L'\n'); s++);
	*s++ = L'\0';
	return s;
}

// ---------------------------------------------------------------------------
bool AllCifry(wchar_t *S) {
	if (!S)
		return true;
	while (*S && isdigit(*S))
		S++;
	if (*S)
		return false;
	return true;
}

// ---------------------------------------------------------------------------
bool Whole(wchar_t *S) {
	if ((*S == L'+') || (*S == L'-'))
		S++;
	if (!isdigit(*S) || ((*S == L'0') && S[1]))
		return false;
	if (!S[1] || AllCifry(S + 1))
		return true;
	return false;
}

// ---------------------------------------------------------------------------
bool Fractional(wchar_t *S) {
	wchar_t *s = wcschr(S, L'.');
	if (KSimbStr(S, L'.') != 1)
		return false;
	*s = L'\0';
	if ((Whole(S)) && (!s[1] || AllCifry(s + 1))) {
		*s = L'.';
		return true;
	}
	*s = L'.';
	return false;
}

// ---------------------------------------------------------------------------
bool Number(wchar_t *S) {
	wchar_t *s, E;
	bool W, F, w;
	if ((s = wcschr(S, L'e')) || (s = wcschr(S, L'E'))) {
		E = *s;
		*s = L'\0';
		if ((Whole(S) || Fractional(S)) && Whole(s + 1)) {
			*s = E;
			return true;
		}
		*s = E;
		return false;
	}
	if (Whole(S) || Fractional(S))
		return true;
	return false;
}

// ---------------------------------------------------------------------------
bool IsLiters(wchar_t *S) {
	for (; *S; S++)
		if ((isciril(*S) || isalpha(*S)) && (*S != L'e') && (*S != L'e'))
			return true;
	return false;
}

// ---------------------------------------------------------------------------
bool IsCifra(wchar_t B) {
	return(B > 47 && B < 58);
}

// ---------------------------------------------------------------------------
void FormSizes(int &Left, int &Top, int &Width, int &Height) {
	HWND Window;
	TRect TaskBarWind;
	Window = FindWindow("Shell_trayWnd", NULL);
	GetWindowRect(Window, &TaskBarWind);
	if (TaskBarWind.Left <= 0 && (TaskBarWind.Width() >= Screen->Width)) {
		// ������ ��� �����
		Width = Screen->Width;
		Height = Screen->Height - TaskBarWind.Height();
		Left = 0;
		if (TaskBarWind.Top)
			Top = 0; // TaskBar �����
		else
			Top = TaskBarWind.Bottom + 1; // TaskBar ������
	}
	else { // ������ ��� �����
		Width = Screen->Width - TaskBarWind.Width();
		Height = Screen->Height;
		Top = 0;
		if (TaskBarWind.Left > 0)
			Left = 0;
		else
			Left = TaskBarWind.Right + 1;
	}
}

// ---------------------------------------------------------------------------
void CopyZam(FILE *F, wchar_t *Target) {
	int l = 0, Lmax = 72;
	bool NewLine = false;
	for (wchar_t *t = Target; *t; t++) {
		switch(*t) {
		case L'@':
			l += fwprintf(F, L"_dog_");
			break;
		case L'$':
			l += fwprintf(F, L"_bax_");
			break;
		case L'\'':
			l += fwprintf(F, L"_po_");
			break;
		case L'�':
			l += fwprintf(F, L"_g");
			break;
		case L'+':
		case L'-':
		case L'*':
		case L'/':
		case L';':
		case L',':
			l++;
			fputwc(*t, F);
			if (NewLine && (t[1] != L'\n')) {
				l = 0;
				fputws(L"\n   ", F);
				NewLine = false;
			}
			break;
		default:
			l++;
			fputwc(*t, F);
		}
		if (l > Lmax)
			NewLine = true;
	}
}
// ---------------------------------------------------------------------------
