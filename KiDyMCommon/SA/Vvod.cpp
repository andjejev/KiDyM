#include "CA.h"
#include "ConvDW.h"
#include <string.h>
#include <alloc.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <comctrls.hpp>
#include <io.h>
#include "OperMess.h"
#include <process.h>
#include <stdio.h>
#include <jpeg.hpp>

extern FILE *INFILE,*ERRFILE,*HTMFILE,*OPRFILE,*TMPFILE;
extern bool IsError,IsSyntError,ISRAND,ISINCOMPLETEIF,ISRAZM;
extern int MAXZNAC,MAXFUNC,NYear,N;
extern wchar_t *NameFunc[],Inf[],ErrFile[],InpFile[],HtmFile[],
 OprFile[],OutFile[],WORKDIR[],CurrDir[],Autor[],SimbInd,
 ANSIFile[],Stmp[],*stmp,Serr[],*serr,Shtm[],*shtm;
extern TStringList *SLT,*SLE,*SLH,*SLK; extern String SF;
extern Cnst *Pust,*Nul,*Odin,*Dva,*Tri; extern List *L,*LBeg;
extern long DateInp;

		   /*  Объявления объектов:  */
long Lfile,PozFile; Magazine *Mtp,*MUnit;
int PUNKT=0,StrLong,NStrCalc;
bool RRR,SMALLFONT,BodysIsTreated,LOGIC; TColor ColorModel=(TColor)255;
TImage *ImageVvod,**Images;
int Line=1,Com,NumbTask,KolErr,KolMes,KOper=23,Kblank=1;
static wchar_t B,ENTERSIMBOL,S[256];
wchar_t *ZNAC,LOGZNAC[]=L" <>\\",WordErr[]=L"* ОШИБКА * ",
 Buf[1024],SP[10240],*sp=SP,*Head,*inf,
 *StrOp[] = {L"УСРЕДНИТЬ",L"ПРИВЕСТИ СИЛЫ",
  L"СТРУКТУРЫ",L"РАЗМНОЖИТЬ",L"ЗАКРЫТЬ",L"ОТКРЫТЬ",L"ТВЕРДЫЕ ТЕЛА",
  L"ТВЕРДОЕ ТЕЛО",L"ТВЁРДЫЕ ТЕЛА",L"ТВЁРДОЕ ТЕЛО",L"УПРУГИЕ СТЕРЖНИ",
  L"УПРУГИЙ СТЕРЖЕНЬ",L"КВАТЕРНИОН",L"СЧИТАТЬ МАЛЫМИ",L"УПРАВЛЕНИЕ COF",
  L"РАССЧИТАТЬ COM",L"РАССЧИТАТЬ COM ПО ФУРЬЕ",
  L"РАСЧЕТ УГЛОВ ПРИ ВОЗМУЩЕНИИ КООРДИНАТ COF И COM",
  L"КРАУТ",L"ПОЛИНОМ",L"ПОЛИНОМЫ COF И COM",L"ПРОЧИТАТЬ МАССИВЫ ИЗ ФАЙЛА"
 },
 HeadCalc[256],HeadWork[256],HeadVersion[256],Autor[256],*t,
 Blank[10240],*blank,Name[256],NameElem[256],*NameCord,
 NameRoot[50],SymbMQ=(wchar_t)0x25cb,SymbMM=(wchar_t)0x2022,
 SymbMV=(wchar_t)0x02df,SymbMC=(wchar_t)0x2219;

bool(*OP[])(Parm *,TRichEdit *,TCanvas *)={
 MidlElem,LeadFors,Structurs,MultiPly,ToClose,ToOpen,Solids,
 Solids,Solids,Solids,ElasticRods,ElasticRods,Quaternion,LitlCount,
 ContrCOF,CalcsCOM,CalcsCOMpoFurie,Perturbation,Kraut,Polinom,
 PolinomsCOFCOM,ReadArrFromFile};

int FindOper(wchar_t *Name){ int i;
 for(i=0;i<KOper;i++){
  wcscpy(Inf,Name); LowToUpp(Inf);
  if(!wcscmp(Inf,StrOp[i]))
   return i+1;
 }
 return 0;
}

void Error(int Line,int Pos,bool Mess){
// Процедура выводит в файл ERRFILE информацию из строки Inf (глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом * ОШИБКА  *.
// Если флаг Mess установлен в true, то сообщение выведется еще и в окно.
// Кроме того, глобальному параметру IsError присвоится значение true,
// а KolErr++.
 wchar_t Buf[2048];
 IsError=true; KolErr++;
 if(Mess) Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
// if(_waccess(ErrFile,0))
//  ERRFILE=_wfopen(ErrFile,L"w");
// else
//  ERRFILE=_wfopen(ErrFile,L"a");
// if(ERRFILE){
//  swprintf(Buf,L"%3d%3d *  ОШИБКА  * %s\n",Line,Pos,Inf);
//  Fputs(Buf,ERRFILE); fclose(ERRFILE); ERRFILE=NULL;
// }
 if(!SLE){ SLE=new TStringList; Serr[0]='\0'; serr=Serr; }
 if(SLE){
  serr+=swprintf(serr,L"%3d%3d *  ОШИБКА  * %s",Line,Pos,Inf);
  SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
  SLE->SaveToFile(ErrFile);
}}

void ErrorSynt(int Line,int Pos){
// Процедура выводит в файл ERRFILE информацию из строки Inf(глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом * ОШИБКА  *.
// Кроме того, глобальному параметру IsSyntError присвоится значение true,
// а KolErr++.
 wchar_t Buf[2048];
 IsSyntError=true; KolErr++;
// if(ERRFILE=_wfopen(ErrFile,L"a")){
//  swprintf(Buf,L"%3d%3d *  ОШИБКА  * %s\n",Line,Pos,Inf);
//  Fputs(Buf,ERRFILE); fclose(ERRFILE);
// }
 if(SLE){
  serr+=swprintf(Serr,L"%3d%3d *  ОШИБКА  * %s",Line,Pos,Inf);
  SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
  SLE->SaveToFile(ErrFile);
}}

void Atten(int Line,int Pos){
// Процедура выводит в файл ERRFILE информацию из строки Inf (глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом * ВНИМАНИЕ *.
// Кроме того, счетчик предупреждений увеличится на 1( KolMes++ ).
 wchar_t Buf[2048]; KolMes++;
 Application->MessageBox(Inf,L"Внимание!",MB_OK|MB_ICONWARNING);
// if(ERRFILE=_wfopen(ErrFile,L"a")){
//  swprintf(Buf,L"%3d%3d * ВНИМАНИЕ * %s\n",Line,Pos,Inf);
//  Fputs(Buf,ERRFILE); fclose(ERRFILE);
// }
 if(SLE){
  serr+=swprintf(Serr,L"%3d%3d * ВНИМАНИЕ * %s",Line,Pos,Inf);
  SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
  SLE->SaveToFile(ErrFile);
}}

wchar_t *Compress(wchar_t *S){ wchar_t *s=S;
 if(s){
  for(;*s;s++); --s; for(;(*s==' ')||(*s=='\t');s--); *(++s)='\0';
 }
 return S;
}

int Znac(wchar_t B){
 int i=MAXZNAC;
 while(i>0&&B!=ZNAC[i])
  i--;
 return i;
}

int Func(wchar_t *S){
 int i=MAXFUNC;
 for(;i>0&&wcscmp(NameFunc[i],S)!=0;i--);
 return i;
}

void PosLine(TRichEdit *RE){
 wchar_t *b; blank=Blank; Blank[0]='\0';
 *sp='\0'; sp=SP;
 swprintf(Inf,L"%4d.  %s",Line++,SP);
 if(b=wcsrchr(Inf,'\n')) *b='\0';
 RE->Lines->Add(Inf);
 stmp+=swprintf(stmp,L"<br>");
 SLT->Add(Stmp); Stmp[0]='\0'; stmp=Stmp;
}

bool ReadChar(void){
 if(PozFile<Lfile){
  if(ENTERSIMBOL){ B=ENTERSIMBOL; ENTERSIMBOL='\0'; }
  else{
   if(!(*t)) return false;
   B=*t++;///*F*/fgetwc(INFILE);
   if(B=='\t'){
	while(B=='\t'){ //int Ls=wcslen(S);
	 *sp++=B;
	 B=*t++; for(int i=0;i<6;i++) *blank++=' '; *blank='\0';
	 PozFile++;
	}
	PozFile--; sp--; blank--; *blank='\0';
   }
   if(B=='\r'){ B=*t++; PozFile++; }
   *sp++=B;
   PozFile++;
  }
  if(B==' '){ *blank++=' '; *blank='\0'; }
  return true;
 }
 return false;
}

POINT Pt;

void ErrorSynt(TRichEdit *RE,wchar_t *S,int Cod){
// Процедура работает внутри Vvod. Выводит на экран ошибочную строку,
// позицию ошибки в ней и текст ошибки. Выводит в файл ERRFILE номер
// ошибочной строки, номер позиции ошибки в ней и текст.
 int i,j,PosErr=(int)((long)sp-(long)SP),Kt=0,l;
 wchar_t *s=SP,Pusto[1024],Buf[256]; TColor TC;
 TTabControl *T; if(RE) T=(TTabControl *)RE->Parent;
 *sp='\0';
 for(;*s!=*sp;) if(*s++=='\t') Kt++;
 IsSyntError=Cod;
 PosLine(RE);
 T->Canvas->Font->Assign(RE->Font);
 l=T->Canvas->TextWidth(SP);
 if(ENTERSIMBOL){ B=ENTERSIMBOL; ENTERSIMBOL='\0'; }
// for(j=0;B!=';'&&!feof(INFILE);j++){
//  B=Fgetc(INFILE); if(j<500) *sp++=B;
// }
 for(Pusto[0]='\0',i=0;T->Canvas->TextWidth(Pusto)<l;){
  Pusto[i++]=' '; Pusto[i]='\0';
 }
 Pusto[i-1]='^'; Pusto[i]='\0';
 swprintf(Buf,L"       %s %s %s\n",Pusto,WordErr,S);
 TC=RE->SelAttributes->Color;
 RE->SelAttributes->Color=clRed;
 Pt=RE->CaretPos;
 l=wcslen(Buf); if(Buf[l-1]=='\n') Buf[l-1]='\0';
 RE->Lines->Add(Buf);
 RE->SelAttributes->Color=TC;
// if(ERRFILE=_wfopen(ErrFile,L"a")){
//  swprintf(Inf,L"%3d%3d *  ОШИБКА  * %s\n",Line-1,PosErr+Kt*7,S);
//  Fputs(Inf,ERRFILE); fclose(ERRFILE);
// }
 if(SLE){
  swprintf(Serr,L"%3d%3d *  ОШИБКА  * %s\n",Line-1,PosErr+Kt*7,S);
  SLE->Add(Serr); Serr[0]='\0'; serr=Serr;
  SLE->SaveToFile(ErrFile);
}}

bool IsLogZnac(wchar_t *S){
 return
  wcschr(S,'<')||wcschr(S,'>')||wcsstr(S,L"\\=")||wcsstr(S,L"==");
}

int LogZnac(TRichEdit *RE){
 int i; wchar_t B1=B; //long Pos;
 if(!ReadChar()) return -1;
 switch(B1){
  case '<': if(B=='=') i=3; else if(B=='<') i=7; else i=1; break;
  case '>': if(B=='=') i=4; else if(B=='>') i=8; else i=2; break;
  case '\\':if(B=='=') i=5; else i=0; break;
  case '=':
   if(B=='=') i=6;
   else
	ErrorSynt(RE,L"Ожидалось: '<', '>', '<=', '>=', '\=', '==', '<<', '>>'.");
	i=0;
   break;
  default :
   ErrorSynt(RE,L"Ожидалось: '<', '>', '<=', '>=', '\=', '==', '<<', '>>'."); i=0;
 }
// Pos=ftell(INFILE);
 if(i<3){ ENTERSIMBOL=B; B=B1; }
 return i;
}

bool ReadComm(TRichEdit *RE){
// fputws(L"<font color=\"#008080\">",TMPFILE); fputwc('#',TMPFILE);
 stmp+=swprintf(stmp,L"<font color=\"#008080\">#");
 while(ReadChar()) switch(B){
  case '#':
//   fputwc('#',TMPFILE); fputws(L"</font>",TMPFILE);
   stmp+=swprintf(stmp,L"#</font>");
   return true;
  case ' ':
//   fputws(L"&nbsp;",TMPFILE);
   stmp+=swprintf(stmp,L"&nbsp;");
   continue;
  case '\t':
//   for(int i=0;i<6;i++) fputws(L"&nbsp;",TMPFILE);
   for(int i=0;i<6;i++) stmp+=swprintf(stmp,L"&nbsp;");
   continue;
  case '\n':
//   PosLine(RE); fputws(L"&nbsp;",TMPFILE);
   PosLine(RE); stmp+=swprintf(stmp,L"&nbsp;");
   continue;
//  default: fputwc(B,TMPFILE);
  default: *stmp++=B; *stmp='\0';
 }
 ErrorSynt(RE,L"Не завершен комментарий!"); return false;
}

bool EatBlock(TRichEdit *RE){
 stmp+=swprintf(stmp,L"<font color=\"#008080\">{");
 while(ReadChar())
  switch(B){
   case '}' :
	Com--;
	if(!Com){
	 stmp+=swprintf(stmp,L"}</font>");
	 return true;
	}
	else
	 *stmp++='}';
	break;
   case '{' :
	*stmp++='{'; *stmp='\0';
	Com++; break;
   case '\n':
	PosLine(RE); stmp+=swprintf(stmp,L"&nbsp;"); continue;
   case ' ':  stmp+=swprintf(stmp,L"&nbsp;"); continue;
   default: *stmp++=B; *stmp='\0';
  }
 stmp+=swprintf(stmp,L"</font>");
 ErrorSynt(RE,L"Не завершен комментарий!"); return false;
}

bool ReadComm(TRichEdit *RE,TextComm *tt){
 wchar_t *Buf; bool Rez;
 for(int i=0;i<wcslen(Blank);i++) stmp+=swprintf(stmp,L"&nbsp;");
 stmp+=swprintf(stmp,L"<font color=\"#008080\">");
 Blank[0]='\0'; blank=Blank;
 do{
  Rez=ReadChar();
  tt->S+=B;
 }while(Rez&&(B!='#'));
 Buf=(wchar_t *)calloc(tt->S.Length()+1,SzC);
 wcscpy(Buf,tt->S.w_str());
 Buf=LiterGrekToHtm(Buf);
 for(int i=0,Beg=i,End;B=Buf[i];i++){
  switch(B){
   case ' ': stmp+=swprintf(stmp,L"&nbsp;"); continue;
   case '\t':
	for(int i=0;i<6;i++) stmp+=swprintf(stmp,L"&nbsp;");
	continue;
   case '\n':
	End=i; Buf[End]='\0';
	swprintf(Inf,L"%4d.  %s",Line++,Buf+Beg);
	RE->Lines->Add(Buf+Beg);
	stmp+=swprintf(stmp,L"<br>");
	SLT->Add(Stmp); Stmp[0]='\0'; stmp=Stmp;
	stmp+=swprintf(stmp,L"&nbsp;");
	Buf[End]='\n'; Beg=i+1;
   continue;
   default: *stmp++=B; *stmp='\0';
 }}
 stmp+=swprintf(stmp,L"</font>"); tt->S=Buf; free(Buf);
 Blank[0]='\0'; blank=Blank;
 if(Rez) return true;
 ErrorSynt(RE,L"Не завершен комментарий!"); return false;
}

bool EatBlock(TRichEdit *RE,TCGauge *CGauge,TextComm *tt){
 wchar_t *BufH,*BufR; bool Rez;
 for(int i=0;i<wcslen(Blank);i++) stmp+=swprintf(stmp,L"&nbsp;");
 stmp+=swprintf(stmp,L"<font color=\"#008080\">");
 Blank[0]='\0'; blank=Blank;
 do{
  Rez=ReadChar();
  if(B=='{') Com++;
  else if(B=='}')
   Com--;
  else if(B=='\n'){
   SP[0]='\0'; sp=SP; Blank[0]='\0'; blank=Blank;
  }
  tt->S+=B;
  if(CGauge) CGauge->Progress=PozFile;
 }while(Rez&&Com);
 BufH=(wchar_t *)calloc(tt->S.Length()+1,SzC);
 BufR=(wchar_t *)calloc(tt->S.Length()+1,SzC);
 wcscpy(BufH,tt->S.c_str()); wcscpy(BufR,tt->S.c_str());
 BufH=LiterGrekToHtm(BufH);
 for(int i=0,Beg=i,End;B=BufH[i];i++){
  switch(B){
   case ' ':  stmp+=swprintf(stmp,L"&nbsp;"); continue;
   case '\t':
	for(int j=0;j<6;j++) stmp+=swprintf(stmp,L"&nbsp;");  continue;
   case '\n':
	End=i; BufH[End]='\0';
	stmp+=swprintf(stmp,L"<br>");
	SLT->Add(Stmp);
	Stmp[0]='\0'; stmp=Stmp;
//	stmp+=swprintf(stmp,L"&nbsp;");
	BufH[End]='\n'; Beg=i+1;
	continue;
   default:   *stmp++=B; *stmp='\0';
 }}
 stmp+=swprintf(stmp,L"</font>");
 tt->S=BufH;
 free(BufH);
 for(int i=0,Beg=i,End;B=BufR[i];i++){
  if(B=='\n'){
   End=i; BufR[End]='\0';
   swprintf(Inf,L"%4d.  %s",Line++,BufR+Beg);
   RE->Lines->Add(Inf);
   BufR[End]='\n'; Beg=i+1;
 }}
 free(BufR);
 Blank[0]='\0'; blank=Blank;
 if(Rez) return true;
 ErrorSynt(RE,L"Не завершен комментарий!"); return false;
}

int ReadStr(int Chto,TRichEdit *RE){
 bool Numb,Quot=false; int KT=0,KE=0,i=0; wchar_t C;
 blank=Blank;
M:if(ReadChar()){
  switch(B){
   case  '#': case  '{':
	return TEXTCOMM;
   case '\n': PosLine(RE); goto M;
   case  '+': goto M;
   case '\t':
//	for(int i=0;i<6;i++) fputws(L"&nbsp;",TMPFILE);
	for(int i=0;i<6;i++) stmp+=swprintf(stmp,L"&nbsp;");
	goto M;
   case  ' ':// stmp+=swprintf(stmp,L"&nbsp;");
	goto M;
   case  ';':
//	S[i++]=B; S[i]='\0'; fputwc(B,TMPFILE);
	S[i++]=B; S[i]='\0'; *stmp++=B; *stmp='\0';
	return PUST;
   case  '-': S[i++]=B; S[i]='\0'; return '-';
   case  '~': S[i++]=B; S[i]='\0'; return '~';
   case  '(': S[i++]=B; S[i]='\0'; return '(';
   case  ')': S[i++]=B; S[i]='\0'; return ')';
   case  '=':
//	S[i++]=B; S[i]='\0'; fputwc(B,TMPFILE);
	S[i++]=B; S[i]='\0'; *stmp++=B; *stmp='\0';
    return INST;
   case  '.': Chto=Double; Numb=true; S[i++]=B; /*fputwc(B,TMPFILE);*/ break;
   case  '"':
    Chto=NAME; Numb=false; Quot=!Quot;
    break;
   case '[':
    S[i++]=B; S[i]='\0'; return '[';
   default  :
    if(isdigit(B)){
     if(Chto!=Double) Chto=Long; Numb=true; S[i++]=B; break;
    }
    if(isalpha(B)||isciril(B)||B=='_'||B=='\x14'){
     if(!Chto) Chto=NAME; Numb=false; S[i++]=B; break;
    }
    else{
	 ErrorSynt(RE,L"Запрещенный символ."); return -1;
 }}}
 else return 0;
L:if(ReadChar()){
//  wchar_t Sun=int('¤');
//  if(B==Sun){
//   if(Quot){ S[i++]=B; goto L; }
//   S[i]='\0';//03.02.2019
//   return Chto;
//  }
  switch(B){
   case  '#': if(!Quot){ if(ReadComm(RE)) goto M; else return -1; } break;
   case  ']':
	if(!Quot&&(Chto==ARRA)) Chto=NAME;
	break;
   case  '[':
	if(!Quot){
	 if(Numb){ ErrorSynt(RE,L"Ожидалось имя."); return -1; } Chto=ARRA;
	}
	break;
//   case '\\': Chto=STRINGER; break;
//    if(Quot||(Chto==ARR)){ S[i++]=B; goto L; }
//    else{ if(EatName(RE)) goto M; else return -1; }
   case '\n': Quot=false; PosLine(RE); goto L;
   case  ' ':
	S[i++]=B; goto L;
   case  '\t':
//	S[i++]=B; for(int i=0;i<6;i++) fputws(L"&nbsp;",TMPFILE);
	S[i++]=B; for(int i=0;i<6;i++) stmp+=swprintf(stmp,L"&nbsp;");
	goto L;
   case  '"': Quot=!Quot; goto L;
   case  ':':
	if(Quot||(Chto==ARRA)){ S[i++]=B; goto L; }
	else{
	 if(Numb){ ErrorSynt(RE,L"Ожидалось имя."); return -1; }
	 if(ReadChar()&&B=='=') Chto=INST;
	 else{
	  Chto=ROUT;
//      fseek(INFILE,-1,1);
	  ENTERSIMBOL=B;
	 }
	 Numb=false;
	}
	break;
   case  '(':
	if(!Quot){ if(Numb){ ErrorSynt(RE,L"Ожидалось имя."); return -1; } }
	if(S[i-1]==SimbInd){
	 for(S[i++]=B;ReadChar()&&(B!=')');) S[i++]=B; S[i++]=B; goto L; //!!!
	}
	break;
   case ')': case ',': case '*': case '/': case '%': case '^':
   case ';': case '<': case '>': case '\\': case '!': case '?':
	if(Quot){ S[i++]=B; goto L; } break;
   case  '+': case  '-':
	if(!Quot){
	 if(Chto!=INST){
	   S[i]='\0';
	   Compress(S);
	   if(((C=S[i-1])=='E'||C=='e')&&Chto==Double){ S[i++]=B; goto L; }
	}}
	else { S[i++]=B; goto L; }
	break;
   case  '=':
	if(!Quot){
	 S[i]='\0';
	 if(!wcschr(S,'.')) Chto=VARY; else Chto=ELEM; Numb=false;
	}
	break;
   case  'E': case  'e':
	if(!Quot){
	 if(Numb){ Chto=Double;
	  if(++KE>1){ ErrorSynt(RE,L"В числе 2 буквы 'E'."); return -1; }
	}}
	S[i++]=B; goto L;
   case  '.':
	if(!Quot&&(Chto!=ARRA)){
	 if(Numb){ Chto=Double;
	  if(++KT>1){
	   ErrorSynt(RE,L"В числе 2 точки."); return -1;
	  }
	 }
	 else Chto=ELEM;
	}
	S[i++]=B; goto L;
   case '|': Chto=BODY; break;
   default  :
	if((B==SymbMC)||(B==SymbMQ)||(B==SymbMV)||(B==SymbMM)){
	 if(Quot){ S[i++]=B; goto L; }
	 break;
	}
	if(!Quot){
	 if((isalpha(B)||isciril(B))&&(B!='e')&&(B!='E')&&Numb){
	  ErrorSynt(RE,
	   L"Если это число, то ожидалась цифра или символы: '.', 'E', 'e', ';'.");
	  return -1;
	}}
	S[i++]=B; goto L;
  }
  S[i]='\0';
 }
 else return 0;
 return Chto;
}

Arra *TakeArrDeriv(Arra *A,wchar_t *S){//02.02.2019
 int N=A->N,Type=A->Type; wchar_t *Name=A->Name;
 if(A->F){
  Form *FA=A->F;
  A=TakeArra(S); A->N=N; A->Type=Type;
  A->F=(Form *)calloc(N,sizeof(Form));
  for(int i=0;i<N;i++){ Vary *V;
   if(FA[i].C->Atr==VARY){
	V=FA[i].V;
   }
   else{
	swprintf(Inf,L"%s_%d",Name,i);
	V=TakeVary(Inf); V->Znach=FA[i];
   }
   swprintf(Inf,L"%s'%s",V->Name,L->Time->Name);
   A->F[i].V=TakeVary(Inf);
  }
  return A;
 }
 return NULL;
}

Form ReadForm(int Prio,TRichEdit *RE){
 Cnst *C; Form F,U,V,FT,FF; signed int Oper; Vary *VV; Arra *A,*AA;
 int KS=0,KKS=0; wchar_t *Shtr,tst[16];
 swprintf(tst,L"%s'%s",L->Time->Name,L->Time->Name);
 switch(ReadStr(0,RE)){
  case '-': U=ReadForm(6,RE); if(U.C==Pust) return U; F=-U; break;
  case '~':
   U=ReadForm(6,RE); if(U.C==Pust) return U;
   if(U.C->Atr==ARRA){ Arra *A;
	if(U.A->N!=2&&U.A->N!=4){
	 swprintf(Inf,L"Нельзя сопрягать массив размерностью %d",U.A->N);
	 ErrorSynt(RE,Inf); F.C=Pust;
	 return F;
	}
	swprintf(Inf,L"%s~",U.A->Name);
	A=TakeArra(Inf);
	if(!A->N){
	 A->N=U.A->N;
//Выделим память, перепишем первый элемент, остальные обратим
	 if(U.A->A){
	  A->A=(double *)calloc(A->N,SzD); A->A[0]=U.A->A[0];
	  for(int i=1;i<A->N;i++) A->A[i]=-U.A->A[i];
	 }
	 if(U.A->F){
	  A->F=(Form *)calloc(A->N,sizeof(Form)); A->F[0]=U.A->F[0];
	  for(int i=1;i<A->N;i++) A->F[i]=-U.A->F[i];
	}}
	F.A=A;
   }
   else{
	ErrorSynt(RE,L"Попытка сопряжения несопрягаемой формулы");
	F.C=Pust;
   }
   break;
  case '(': KS++;  F=ReadForm(0,RE); if(F.C==Pust) return F; break;
  case '[': KKS++; F=ReadForm(0,RE); if(F.C==Pust) return F; break;
  case  NAME:
   if(B!='('){
	if(FindCnst(S,&C)) F.C=C;
	else if(FindArra(S,&A)) F.A=A;
	else{ bool IsArra=false; //02.02.2019
	 for(Shtr=S;Shtr=wcschr(Shtr+1,'\'');*Shtr='\''){
	  *Shtr='\0';
	  if((wcsstr(Shtr+1,tst)==(Shtr+1))||
		 (wcsstr(Shtr+1,L->Time->Name)==(Shtr+1))){
	   if(IsArra||FindArra(S,&A)){//02.02.2019
		if(IsArra){
		 TakeArrDeriv(A,S); //03.02.2019
		}
		if(!A->Line) A->Line=Line;//02.02.2019
		IsArra=true;              //02.02.2019
	   }                          //02.02.2019
	   else{
		VV=TakeVary(S);
		if(!(VV->Line)) VV->Line=Line;
	 }}}
	 if(IsArra){  //02.02.2019//24.10.2019
	  F.A=TakeArrDeriv(A,S); //03.02.2019
	  if(!F.A) F.C=Pust;
	  A->Znach.A=A;
	 }
	 else                         //02.02.2019
	  F.V=TakeVary(S);
   }}
   else{
	KS++;
	switch(Oper=Func(S)){
	 case  2:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=sqrt(U); break;
	 case  3:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=sin(U); break;
	 case  4:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=cos(U); break;
	 case  5:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=tg(U); break;
	 case  6:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=arcsin(U); break;
	 case  7:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=arctg(U); break;
	 case  8:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=exp(U); break;
	 case  9:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=ln(U); break;
	 case 10:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=sh(U); break;
	 case 11:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=ch(U); break;
	 case 12:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=arsh(U); break;
	 case 13:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=abs(U); break;
	 case 14:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=rand(U);
	  ISRAND=true; break;
	 case 15:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=ceil(U); break;
	 case 16:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=floor(U); break;
	 case 17:
	  U=ReadForm(0,RE); if(U.C==Pust) return U; F=arccos(U); break;
	 default:
	  if(!wcscmp(S,L"ipl")){
	   if(ReadStr(0,RE)<1) return F;
	   if(B==',') A=TakeArra(S);
	   U=ReadForm(0,RE); if(U.C==Pust) return U;
	   F.I=TakeIpll(1,A,U);
	  }
	  else if(!wcscmp(S,L"spln")){
	   if(ReadStr(0,RE)<1) return F;
	   if(B==',') A=TakeArra(S);
	   U=ReadForm(0,RE);
	   if(U.C==Pust) return U;
	   F.P=TakeSpln(1,A,U);
	  }
	  else if(!wcscmp(S,L"summ")){ int Rez;
	   FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0,RE),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		if(B==','){
		 U=ReadForm(0,RE); if(U.C==Pust) return U;
		 if(B==','){
		  V=ReadForm(0,RE); if(V.C==Pust) return V;
		 }
		 F.S=TakeSumm(FF,VV,U,V); break;
	   }}
	   return F;
	  }
	  else if(!wcscmp(S,L"intg")){ int Rez;
	   FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0,RE),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		if(S[0]=='d'){
		 if(!VarInForm(true,FF,VV)){ Vary *v;
		  v=TakeVary(S+1); if(v&&!(v->Line)) v->Line=Line;
		  if(VarInForm(true,FF,v)){ DelVary(S); VV=v; }
		}}
		if(B==','){
		 U=ReadForm(0,RE); if(U.C==Pust) return U;
		 if(B==','){
		  V=ReadForm(0,RE); if(V.C==Pust) return V;
		 }
		 F.G=TakeIntg(FF,VV,U,V); break;
		}
		else{
		 U.C=Nul; V.V=VV; F.G=TakeIntg(FF,VV,U,V); ReadChar();
	   }}
	   return F;
	  }
	  else if(!wcscmp(S,L"root")){ int Rez;
	   FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0,RE),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		if(B==','){
		 U=ReadForm(0,RE); if(U.C==Pust) return U;
		 if(B==','){
		  V=ReadForm(0,RE); if(V.C==Pust) return V;
		 }
		 F.R=TakeSolu(FF,VV,U,V); break;
	   }}
	   return F;
	  }
	  else if(!wcscmp(S,L"rest")){
	   U=ReadForm(0,RE); if(U.C==Pust) return U;
	   if(B==','){
		V=ReadForm(0,RE); if(V.C==Pust) return V;
		F.O=TakeRest(U,V); break;
	   }
	   return F;
	  }
	  else if(!wcscmp(S,L"diff")){
	   U=ReadForm(0,RE);
	   if(U.C==Pust){
		ErrorSynt(RE,L"Пустая формула."); F=U;
	   }
	   if(B==','){
		V=ReadForm(0,RE);
		if(V.C==Pust){
		 ErrorSynt(RE,L"Пустая формула."); F=V;
		}
		if((U.V->Atr==VARY)&&(V.V->Atr==VARY)){
		 F.D=(Diff *)calloc(1,sizeof(Diff)); F.D->Atr=DIFF; F.D->Y=U.V; F.D->X=V.V;
	   }}
	   else{
		ErrorSynt(RE,
		 L"     Функция Diff должна иметь 2 параметра - \n"
		 L"имена переменных, какая по какой диффернцируется."); F.C=Pust;
	   }
	   break;
	  }
	  else {
	   ErrorSynt(RE,L"Неопознанная унарная операция."); F.C=Pust; return F;
   }} }
   break;
  case Long: case Double:
   F.C=TakeCnst(S);
   break;
  case PUST: case -1: F.C=Pust; return F;
 }
 for(;;) switch(B){
  case '[':
   if(A=TakeArra(S)){
	F=ReadForm(0,RE);
	F.a=TakeArrI(A,F);
	if(!ReadChar()) return F.C=Pust,F;
   }
   break;
//  case ']': return F;
  case '\n': PosLine(RE);
  case '\t': case ' ':
   if(!ReadChar()) return F.C=Pust,F; else break;
  case ')':
   if(KS){
	KS--;
	if(!ReadChar())
	 F.C=Pust; break;
   }
   return F;
  case ']':
   if(KKS){
	KKS--;
	if(!ReadChar())
	 F.C=Pust; break;
   }
   return F;
  case  ';':
  case  ',':
   if(LOGIC) LOGIC=false;
   if(KS>0){
	ErrorSynt(RE,L"Ожидалось: ')'."); F.C=Pust;
   }
   if(KKS){
	U=ReadForm(3,RE); if(U.C==Pust) return U;
	if(F.C->Atr!=ARRA||U.C->Atr!=ARRA||F.A->N!=3||U.A->N!=3){
	 ErrorSynt(RE,
	  L"Операнды векторного произведения должны быть 3-мерными векторами.");
	 F.C=Pust; return F;
	}
	F.A->Type=U.A->Type=VECT;
	F=F*U; //Это должно быть векторное произведение!
	break;
   }
  case '?':
   return F;
  case '!':
   if(!LOGIC){
	F.U=TakeUnop(18,F);
	ReadChar();
   }
   return F;
  case  '=': //снова читаем переменную с формулой
   Compress(S);
   for(Shtr=S;Shtr=wcschr(Shtr+1,'\'');*Shtr='\''){
	*Shtr='\0';
	if((wcsstr(S,tst)==(Shtr+1))||(wcsstr(Shtr+1,L->Time->Name)==(Shtr+1))){
	 VV=TakeVary(S); if(!VV->Line) VV->Line=Line;
   }}
   FF.V=VV=TakeVary(S);
   if((VV->Znach.C!=Pust)&&(VV->Line>NStrCalc)){
	swprintf(Inf,
	 L"Повторный ввод формулы для переменной:\n%s\nв строке %d",S,Line);
	Atten(Line,0);
   }
   if(!VV->Line) VV->Line=Line;
   VV->Znach=ReadForm(0,RE); return FF;
  case '\\':
   if((Oper=LogZnac(RE))>0){
	V=ReadForm(0,RE); if(V.C==Pust) return V;
	if((B=='!')||(B=='?')){
	 FT=ReadForm(0,RE); if(FT.C==Pust) return FT;
	}
	else { ErrorSynt(RE,L"Ожидалось: '?'."); F.C=Pust; return F; }
	if(B=='!'){
	 FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
	}
	else{ String Su,Sv,St,S;
	 FF.C=Pust;
	 inf+=swprintf(inf=Inf,L"Тернарное предложение в строке %d:\n\n",Line);
	 FormToStr(F,&Su,true);
	 FormToStr(V,&Sv,true);
	 FormToStr(FT,&St,true);
	 S=Su;
	 switch(Oper){
	  case 1: S+=L" < ";  break; case 2: S+=L" > ";  break;
	  case 3: S+=L" <= "; break; case 4: S+=L" >= "; break;
	  case 5: S+=L" == "; break; case 6: S+=L" != "; break;
	  case 7: S+=L" << "; break; case 8: S+=L" >> "; break;
	 }
	 S+=Sv + L" ? " + St + L";"; inf+=swprintf(inf,S.c_str());
	 swprintf(inf,L"\n\nне завершено!"); ISINCOMPLETEIF=true;
	 Application->MessageBox(Inf,L"Внимание!",MB_OK);
	}
	F.L=TakeIfop(Oper,F,V,FT,FF); return F;
   }
  default:
   if((Oper=Znac(B))&&(Prio>=Oper)) return F;
   switch(Oper){
	case  1:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F+U; break;
	case  2:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F-U; break;
	case  3: case 7: case 8: case 9: case 10:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 if((F.C->Atr==ARRA)&&(U.C->Atr==ARRA)){
	  switch(Oper){
	   case  3:
		if(F.C->Atr==ARRA&&U.C->Atr==ARRA){
		 swprintf(Inf,
		  L"При умножении массивов в строке %d\n"
		  L" надо использовать нужный символ",Line);
		 ErrorSynt(RE,Inf); F.C=Pust; break;
		}
		break;
	   case  7:
		if(!F.A->Type) F.A->Type=QUAT;
		else if(F.A->Type!=QUAT){
		 swprintf(Inf,
		  L"Левым операндом умножения в строке %d"
		  L" должен быть кватернион",Line);
		 ErrorSynt(RE,Inf); F.C=Pust; break;
		}
		if(!U.A->Type) U.A->Type=QUAT;
		else if(U.A->Type!=QUAT){
		 swprintf(Inf,
		  L"Правым операндом умножения в строке %d"
		  L" должен быть кватернион",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		break;
	   case  8:
		if(!F.A->Type) F.A->Type=MATR;
		else if(F.A->Type!=MATR){
		 swprintf(Inf,
		  L"Левым операндом умножения в строке %d"
		  L" должна быть матрица",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		if(!U.A->Type) U.A->Type=MATR;
		else if(U.A->Type!=MATR){
		 swprintf(Inf,
		  L"Правым операндом умножения в строке %d"
		  L" должна быть матрица",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		break;
	   case  9:
		if(!F.A->Type) F.A->Type=VECT;
		else if(F.A->Type!=VECT){
		 swprintf(Inf,
		  L"Левым операндом умножения в строке %d"
		  L" должен быть 3-хмерный вектор",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		if(!U.A->Type) U.A->Type=VECT;
		else if(U.A->Type!=VECT){
		 swprintf(Inf,
		  L"Правым операндом умножения в строке %d"
		  L" должен быть 3-хмерный вектор",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		break;
	   case 10:
		if(!F.A->Type) F.A->Type=MATR;
		else if(F.A->Type!=MATR){
		 swprintf(Inf,
		  L"Левым операндом умножения в строке %d"
		  L" должна быть матрица",Line);
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		if(!U.A->Type) U.A->Type=/*U.A->N==3?VECT:*/CLMN;
		else if((U.A->Type!=VECT)&&(U.A->Type!=CLMN)){
		 swprintf(Inf,
		  L"Правым операндом умножения в строке %d"
		  L" должен быть %s",Line,
		  F.A->N==9?L"3-хмерный вектор":L"вектор-столбец");
		 ErrorSynt(RE,Inf); F.C=Pust;
		}
		break;
	 }}
	 if(F.C!=Pust)
	  F=F*U;
	 break;
	case  4:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F/U; break;
	case  5:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F%U; break;
	case  6:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F^U; break;
	default:
	 if((Oper=LogZnac(RE))<0) return F;
	 if(Oper){
	  LOGIC=true;
	  V=ReadForm(0,RE); if(V.C==Pust) return V;
	  if(B=='?'){
	   FT=ReadForm(0,RE); if(FT.C==Pust) return FT;
	  }
	  else { ErrorSynt(RE,L"Ожидалось: '?'."); F.C=Pust; return F; }
	  if(B=='!'){
	   FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
	  }
	  else{ String Su,Sv,St,S;
	   FF.C=Pust;
//       inf+=swprintf(inf=Inf,L"Тернарное предложение в строке %d:\n\n",Line);
	   inf+=swprintf(inf=Inf,L"   %d: ",Line);
	   FormToStr(F,&Su,true);
	   FormToStr(V,&Sv,true);
	   FormToStr(FT,&St,true);
	   S=Su;
	   switch(Oper){
		case 1: S+=L" < ";  break; case 2: S+=L" > ";  break;
		case 3: S+=L" <= "; break; case 4: S+=L" >= "; break;
		case 5: S+=L" == "; break; case 6: S+=L" != "; break;
		case 7: S+=L" << "; break; case 8: S+=L" >> "; break;
	   }
	   S+=Sv + L" ? " + St + L";"; inf+=swprintf(inf,S.c_str());
	   TakeMagazine(&Mtp,Inf);
//       swprintf(inf,L"\n\nне завершено!");
	   ISINCOMPLETEIF=true;
//       Application->MessageBox(Inf,L"Внимание!",MB_OK);
	  }
	  F.L=TakeIfop(Oper,F,V,FT,FF); break;
	 }
	 else { ErrorSynt(RE,L"Неопознанный символ."); F.C=Pust; return F; }
}}}

Form ReadRazm(int Prio,TRichEdit *RE){
 Cnst *C; Form F,U,V,FT,FF; signed int Oper; Vary *VV; Arra *A;
 switch(ReadStr(0,RE)){
  case  NAME:
   if(B!='('){
	if(FindVary(S,&VV)) F.V=VV;
	else{
	 swprintf(Inf,L"Введенная размерность \"%s\" отсутствует в списке.\n"
	 L"Чтение данных прекращается. \n Введите правильно размерность",S);
	 Error(0,0,true);
	 F.C=Pust;
   }}
   break;
  case Long: case Double:
   F.C=TakeCnst(S);
   break;
  case PUST: case -1: F.C=Pust; return F;
 }
 for(;;) switch(B){
  case '\n': PosLine(RE);
  case '\t': case ' ':
   if(!ReadChar()) return F.C=Pust,F; else break;
  case  ';':
   return F;
  default:
   if((Oper=Znac(B))&&(Prio>=Oper)) return F;
   switch(Oper){
	case  1:
	 ErrorSynt(RE,
	  L"Обозначение размерности не должно содержать знака '+'.");
	 break;
	case  2:
	 ErrorSynt(RE,
	  L"Обозначение размерности не должно содержать знака '-'.");
	 break;
	case  3:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F*U; break;
	case  4:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F/U; break;
	case  5:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F%U; break;
	case  6:
	 U=ReadForm(Oper,RE); if(U.C==Pust) return U;
	 F=F^U; break;
	default:
	 ErrorSynt(RE,L"Неопознанный символ."); F.C=Pust;
	 return F;
}}}

bool SptoArr(Arra *Arr,int &N,TRichEdit *RE){
 int i=0; Form *F;
 for(ReadChar();B!='=';ReadChar())
//  if(B==' ') fputws(L"&nbsp;",TMPFILE);
  if(B==' ') stmp+=swprintf(stmp,L"&nbsp;");
 for(ReadChar();B==' '||B=='\n'||B=='\v'||B=='\t';ReadChar()){
  switch(B){
//   case ' ': fputws(L"&nbsp;",TMPFILE); break;
   case ' ': stmp+=swprintf(stmp,L"&nbsp;"); break;
//   case '\n': case '\v': fputws(L"\n<br>",TMPFILE); Line++; break;
   case '\n': case '\v': stmp+=swprintf(stmp,L"<br>");
	SLT->Add(Stmp); Stmp[0]='\0'; stmp=Stmp; Line++; break;
//   default: for(int i=0;i<6;i++) fputws(L"&nbsp;",TMPFILE); break;
   default:
	for(int i=0;i<6;i++) stmp+=swprintf(stmp,L"&nbsp;"); break;
 }}
 F=(Form *)calloc(N+1,sizeof(Form));
// fseek(INFILE,-1L,1);
 ENTERSIMBOL=B;
 do{
  if(i<N){
   F[i]=ReadForm(0,RE);
//   PrintForm(false,TMPFILE,NULL,F[i]); fputwc(B,TMPFILE);
   stmp=FormToStringList(F[i],NULL,Stmp,stmp,SLT,false,&SF);
   stmp+=swprintf(stmp,L"%c ",B);
   i++;
  }
  else{
   if(Arr->Name)
	swprintf(Inf,L"Массив %s больше объявленной размерности (%d).",
	 Arr->Name?Arr->Name:L"\0",N);
   ErrorSynt(RE,Inf);
   free(F); F=NULL; return false;
  }
 }while(B!=';');
 if(i<N){
   swprintf(Inf,L"Массив %s меньше объявленной размерности (%d).",
	Arr->Name?Arr->Name:L"\0",N);
   ErrorSynt(RE,Inf);
  free(F); F=NULL; return false;
 }
 else{
  Arr->N=N;
  for(i=0;i<N;i++) if(F[i].C->Atr!=3){ Arr->F=F; return true; }
  Arr->A=(double *)calloc(N,sizeof(double));
  for(i=0;i<N;i++){ Arr->A[i]=F[i].C->Val; } free(F);
  return true;
}}

bool NoName(wchar_t *S){
 bool T=false,T2=false,E=false,C=false; wchar_t *s;
 if(!S) return true;
 for(s=S;*s&&((*s==' ')||(*s=='\t')||(*s=='\n'));s++);
 if(!*s) return true;
 switch(*s){
  case '.': T=true;
  case '+':case '-':
  case '0':case '1':case '2':case '3':case '4':case '5':case '6':
  case '7':case '8':case '9':
   if(isdigit(*s)) C=true;
   for(s++;*s;s++)
	switch(*s){
	 case 'E':case 'e':
      if(!C){
       if(!T) return false;
       Application->MessageBox(
		L"Неправильно записано экспоненциальное представление числа",
		L"ОШИБКА!",MB_OK|MB_ICONERROR);
	   return false;
	  }
	  if(E){
	   Application->MessageBox(
		L"Неправильно записано экспоненциальное представление числа",
		L"ОШИБКА!",MB_OK|MB_ICONERROR);
	   return false;
	  }
	  E=true;
	  break;
	 case '+':case '-':
	  if((*(s-1)!='E')&&(*(s-1)!='e')) return true;
	  break;
	 case '.':
	  if(T) T2=true;
	  break;
	 case '0':case '1':case '2':case '3':case '4':case '5':case '6':
	 case '7':case '8':case '9':
	  break;
	 default: return false;
	}
   break;
  default: return false;
 }
 if(C&&T2){
  Application->MessageBox(L"В числе 2 точки!",L"ОШИБКА!",MB_OK|
MB_ICONERROR);
  return false;
 }
 return true;
}

bool IsUnOp(wchar_t *S){ return Func(S); }

bool IsZnaki(wchar_t *S){
 for(wchar_t *s=S;*s;s++) if(Znac(*s)) return true; return false;
}

int ChtoEto(wchar_t *S){
 while((*S==' ')||(*S=='\n')||(*S=='\t')||(*S=='\v')) S++;
 if(isalpha(*S)||isciril(*S)||*S=='_'){ wchar_t *Sk;
  if(Sk=wcschr(S,'='))
   return NAMELIST;
  if(Sk=wcschr(S,'(')){ *Sk='\0';
   if(IsUnOp(S)||IsZnaki(S)){ *Sk='('; return FORMULA; }
   *Sk='('; return NAMELIST;
  }
  if(IsZnaki(S)) return FORMULA;
  if(Sk=wcschr(S,'[')) return FORMULA;
  return NAME;
 }
 if(IsZnaki(S)) return FORMULA;
 for(;*S;S++) if((isalpha(*S)||isciril(*S))&&(*S!='e')&&(*S!='E')) return NAME;
 return NUMB;
}

Parm *StrToSp(wchar_t *Buf,int Ln){
 Parm *R=NULL,**Pr=&R,*P; wchar_t *b,*b0; Magazine *m,*M=NULL; int Ksk=0;
 for(b=Buf,b0=b;*b0;b0++){
  switch(*b0){
   case '(': Ksk++; break;
   case ')': Ksk--; break;
   case ',':
    if(!Ksk){
     *b0='\0';
     while((*b==' ')||(*b=='\n')||(*b=='\t')||(*b=='\v')) b++;
	 AddMagazine(&M,b);
	 *b0=','; b=b0+1;
	}
	break;
 }}
 if(!Ksk){
  while((*b==' ')||(*b=='\n')||(*b=='\t')||(*b=='\v')) b++;
  AddMagazine(&M,b);
 }
 for(m=M;m;m=m->Sled){
  P=*Pr=(Parm *)calloc(1,sizeof(Parm)); Pr=(Parm **)P; P->Line=Ln;
  P->F.C=Pust;
  switch(ChtoEto(m->S)){
   case NUMB: P->Val=_wtof(m->S); break;
   case NAME:
    P->Nam=wcscpy((wchar_t *)malloc((wcslen(m->S)+1)*SzC),m->S);
    Compress(P->Nam);
    break;
   case FORMULA:
    P->F=StrToForm(m->S); break;
   case NAMELIST:
    for(b=m->S;*b;b++){
	 if(*b==L'='){ *b++=L'\0'; break; }
	 if(*b==L'('){
	  *b++=L'\0'; b0=b+wcslen(b)-1; if(*b0==L')') *b0='\0'; break;
	}}
	P->Nam=wcscpy((wchar_t *)malloc((wcslen(m->S)+1)*SzC),m->S);
	Compress(P->Nam);
	P->R=StrToSp(b,Ln);
	break;
 }}
 DelAllMagazine(&M);
 return R;
}

Parm *ReadSp(int Ln,wchar_t &Delim){
 wchar_t Buf[8192],*b=Buf; int Ksk=0;
 while(ReadChar()&&((B==' ')||(B=='\n')||(B=='\t')||(B=='\v')));
 *b++=B;
 while(ReadChar()&&(B!=';')&&(B!='|')&&(Ksk>-1)){
  switch(*b++=B){
   case '(': Ksk++; break;
   case ')': Ksk--; break;
 }}
 if(Ksk==-1){ b[-1]='\0'; Delim=')'; } else{ *b='\0'; Delim=B; }
 Compress(Buf);
 return StrToSp(Buf,Ln);
}

void DelVary(Vary **R,Vary *V){ Vary *v=*R;
 if(V==*R){ *R=v->Sled; free(V); return; }
 for(;v;v=v->Sled)
  if(v->Sled==V){ v->Sled=V->Sled; free(V); return; }
}

bool ToWithdraw(Root *I){
 int N=1; Elem *E; Vary *V;
 for(Parm *P=I->First;P;P=P->Sled,N++){
  if(!P->Nam){
   swprintf(Inf,L"%d-й параметр инструкции %s %d-й строки\n"
	L"должен содержать имя удаляемой инструкции, переменного элемента!",
	N,I->Name,I->Line);
   Error(I->Line,0,Inf); IsSyntError=true;
  }
  if(P->R){
   swprintf(Inf,L"%d-й параметр инструкции %s %d-й строки\n"
    L"не должен содержать внутренний список!",N,I->Name,I->Line);
   Error(I->Line,0,Inf); IsSyntError=true;
  }
  if(wcschr(P->Nam,'.')){
   if(!FindElem(P->Nam,&E)){
    swprintf(Inf,L"%d-й параметр инструкции %s %d-й строки\n"
	 L"содержит имя, отсутствующее среди элементов, или уже удаленное!",
     N,I->Name,I->Line);
    Error(I->Line,0,Inf); IsSyntError=true;
   }
   else
    DelStruct((void **)&(L->E),E);
  }
  else{ Root *i;
   if(FindVary(P->Nam,&V)) DelVary(&(L->V),V);
   else if(wcscpy(Inf,P->Nam),i=FindInstLast(LowToUpp(Inf)))
	DelStruct((void **)&(L->Inst),i);
   else if(i=FindOperLast(LowToUpp(Inf)))
	DelStruct((void **)&(L->Oper),i);
   else{
	swprintf(Inf,L"%d-й параметр инструкции %s %d-й строки\n"
	 L"содержит имя, отсутствующее среди переменных и инструкций, или уже удаленное!",
	 N,I->Name,I->Line);
	Error(I->Line,0,Inf); IsSyntError=true;
 }}}
 return IsSyntError;
}

void Vvod(TRichEdit *RE,TCGauge *CGauge,TStatusBar *StatusBar,
  TImageKDM *ImageKDM){
 wchar_t *s,*Shtr,tst[256],KONEC[]=L"КОНЕЦ",SBold[256],Delim,*Buf;
 bool END,MAKE; struct _stat SB; Elem *E;
 Root *I,*O,**PI=&L->Inst; Body *b; Stringer *r; TextComm *tt;
 int Flag,N,Ln,Rez; Arra *A; Vary *V; Form F; Parm *P; Cord *K;
 _wstat(InpFile,&SB); DateInp=SB.st_mtime;
 NumbTask++; NameRoot[0]='\0';
 swprintf(tst,L"%s'%s",L->Time->Name,L->Time->Name);
 if(CGauge){
  CGauge->ForeColor=ColorModel; CGauge->Progress=0;
  CGauge->MaxValue=Lfile;
 }
 wcscpy(SBold,StatusBar->SimpleText.c_str());
 StatusBar->SimpleText=L"Чтение файла исходных данных";
M:Flag=ReadStr(0,RE);
 if(CGauge) CGauge->Progress=PozFile;
// StatusBar->SimpleText=S;
 switch(Flag){
  case Double: case Long:
   ErrorSynt(RE,L"Объект ввода не может быть отдельным числом.");
   goto M;
  case PUST:
   if(PozFile<Lfile){
	ErrorSynt(RE,L"Пустое выражение."); goto M;
   }
   StatusBar->SimpleText=SBold;
   return;
  case ELEM: END=false;
   Compress(S);
   wcscpy(Name,S); s=Name; while(*s++!='.'); *(s-1)='\0'; NameCord=s++;
   wcscpy(NameElem,Name); wcscat(NameElem,L".");
   while(!END){
	MAKE=true;
	while(MAKE) switch(*s){
	  case  '.':  *s='\0'; MAKE=false; break;
	  case '\0': END=true; MAKE=false; break;
	  default:   s++;
	}
	swprintf(NameElem,L"%s.%s",Name,NameCord);
	if(FindElem(NameElem,&E)&&(E->Line>NStrCalc)){
	 swprintf(Inf,L"Повторный ввод элемента:\n%s\nв строке %d",
	 NameElem,Line);
	 Atten(Line,0);
	}
	else E=TakeElem(NameElem);
	E->K=TakeVary(NameCord);
	if(B!=';') F=ReadForm(0,RE);
	E->Znach=F; NameCord=++s;
//	swprintf(Inf,L"%s",E->Name); fputws(Blank,TMPFILE);
//	PrintForm(false,TMPFILE,Inf,E->Znach);
	stmp+=swprintf(stmp,Blank);
	Blank[0]='\0'; blank=Blank;
	stmp=FormToStringList(E->Znach,E->Name,Stmp,stmp,SLT,false,&SF);
//	fputws(L"&nbsp;",TMPFILE);
	stmp+=swprintf(stmp,L"&nbsp;");
   }
   if(ImageKDM) ImageKDM->Add(E);
   goto L;
  case VARY: Compress(S);
   for(Shtr=S;Shtr=wcschr(Shtr+1,'\'');*Shtr='\''){
	*Shtr='\0';
	if((wcsstr(S,tst)==(Shtr+1))||
	   (wcsstr(Shtr+1,L->Time->Name)==(Shtr+1))){
	 V=TakeVary(S); if(!V->Line) V->Line=Line;
   }}
   if(!FindArra(S,&A)){
	wchar_t Name[256]; wcscpy(Name,S);
	Form F=ReadForm(0,RE);
	if(F.C->Atr==ARRA){ //03.02.2019//24.10.2019
	 A=TakeArra(Name);
	 switch(A->N=F.A->N){
	  case 3: A->Type=VECT; break;
	  case 4: A->Type=QUAT; break;
	  default: A->Type=CLMN;
	 }
	 if(F.A->F) A->F=F.A->F;
	 if(F.A->A) A->A=F.A->A;
	 A->Znach.A=A;
//	 fputws(Blank,TMPFILE);
//	 PrintForm(false,TMPFILE,A->Name,A->Znach);
//	 fputws(L"&nbsp;",TMPFILE);
	 stmp+=swprintf(stmp,Blank);
	 Blank[0]='\0'; blank=Blank;
	 stmp=FormToStringList(A->Znach,A->Name,Stmp,stmp,SLT,false,&SF);
	 if(ImageKDM) ImageKDM->Add(A);
	}
	else{
	 V=TakeVary(Name);
	 if((V->Znach.C!=Pust)&&(V->Line>NStrCalc)){
	  swprintf(Inf,
	   L"Повторный ввод формулы для переменной:\n%s\nв строке %d",
	   Name,Line);
	  Atten(Line,0);
	 }
	 if(!V->Line) V->Line=Line;
	 V->Znach=F;
	 if(B==','){
	  List *Lo=L; L=TakeList(L"РАЗМЕРНОСТИ");
	  V->Razm=ReadRazm(0,RE);
	  L=Lo;
	  if(V->Razm.C!=Pust) ISRAZM=true;
	  else return;
	 }
//	 fputws(Blank,TMPFILE);
//	 PrintForm(false,TMPFILE,V->Name,V->Znach);
//	 fputws(L"&nbsp;",TMPFILE);
//	 FormToStr(V->Znach,&SF);
	 stmp+=swprintf(stmp,Blank);
	 Blank[0]='\0'; blank=Blank;
	 stmp=FormToStringList(V->Znach,V->Name,Stmp,stmp,SLT,false,&SF);
	 if(ImageKDM) ImageKDM->Add(V);
   }}
   else{//нашли такой массив
	F=ReadForm(0,RE);
	switch(F.C->Atr){
	 case    ARRA:
	  for(int i=0;i<F.A->N;i++)
	   (A->F[i].C->Atr==VARY ? A->F[i].V->Znach : A->F[i])=F.A->F[i];
//	  A->Znach.A=A;
	  A->Znach=F;//25.11.2019
//	  fputws(Blank,TMPFILE);
//	  PrintForm(false,TMPFILE,A->Name,A->Znach);
//	  FormToStr(A->Znach,&SF);
	  stmp+=swprintf(stmp,Blank);
	  Blank[0]='\0'; blank=Blank;
	  stmp=FormToStringList(A->Znach,A->Name,Stmp,stmp,SLT,true,&SF);
//	  A->Znach.C=Pust; //25.11.2019
	  break;
	 default:
	  ErrorSynt(RE,L"Неправильное присвоение формулы массиву");
   }}
   goto L;
  case    ARRA:
//   for(int j=0;j<wcslen(Blank);j++)
//   fputws(L"&nbsp;",TMPFILE);
//   stmp+=swprintf(stmp,L"&nbsp;");
//   blank=Blank; *Blank='\0';
   Compress(S); A=TakeArra(S);
   Buf=(wchar_t *)calloc(wcslen(S)+1,SzC);
   wcscpy(Buf,S);
   Buf=LiterGrekToHtm(Buf);//Grek(S);
//   fwprintf(TMPFILE,L"<font face=\"Courier New\">%s[",Buf);
   stmp+=swprintf(stmp,L"<font face=\"Courier New\">%s[",Buf);
   free(Buf);
   Rez=ReadStr(ARRA,RE);
   if(Rez==Long){
	N=_wtoi(S);
	if(A->A) free(A->A);
//	fwprintf(TMPFILE,L"%d]=L",N);
	stmp+=swprintf(stmp,L"%d]=",N);
	if(SptoArr(A,N,RE))
	 TakeSplns(1,A);
//	fputws(L"</font>",TMPFILE);
	stmp+=swprintf(stmp,L"</font>");
	goto L;
   }
   else if(Rez==NAME){
	FILE *F; long DateFile; struct _stat SB; wchar_t File[256];
	for(ReadChar();B!=';';ReadChar());
	if(wcsstr(S,L".\\")==S)
	 swprintf(File,L"%s%s",CurrDir,S+1);
	else if(wcsstr(S,L"..\\")==S){
	 wchar_t *s=wcsrchr(CurrDir,'\\');
	 *s='\0';
	 swprintf(File,L"%s%s",CurrDir,S+2);
	 *s='\\';
	}
	else wcscpy(File,S);
	if(_waccess(File,0)){
	 ErrorSynt(RE,L"Файл не найден!");
	 StatusBar->SimpleText=SBold;
	 return;
	}
	_wstat(File,&SB); DateFile=SB.st_mtime;
	if(DateFile>DateInp) DateInp=DateFile;
	if(F=_wfopen(File,L"rb")){
	 fread(&N,SzI,1,F); A->N=N;
	 A->A=(double *)realloc(A->A,N*SzD);
	 fread(A->A,SzD,N,F);
//	 fwprintf(TMPFILE,L"%s];",S);
	 stmp+=swprintf(stmp,L"%s];",S);
	 TakeSplns(1,A);
//	 fputws(L"</font>",TMPFILE);
	 stmp+=swprintf(stmp,L"</font>");
	 fclose(F);
	 goto L;
	}
	else{
	 ErrorSynt(RE,L"Файл не открывается!");
	 StatusBar->SimpleText=SBold;
	 return;
   }}
   else{
//	fputws(L"</font>",TMPFILE);
	stmp+=swprintf(stmp,L"</font>");
	if(!Rez){
	 StatusBar->SimpleText=SBold;
	 return;
	}
	if(ImageKDM) ImageKDM->Add(I);
	goto M;
   }
  case NAME:
   wcscpy(Inf,S); LowToUpp(Inf);
   if(!wcscmp(Inf,L"КОНЕЦ")){
	if(CGauge) CGauge->Progress=CGauge->MaxValue;
	StatusBar->SimpleText=SBold;
	return;
   }
   I=*PI=(Root *)calloc(1,sizeof(Root)); PI=(Root **)I; I->Line=Ln;
   I->Name=wcscpy((wchar_t *)malloc(SzC*(wcslen(Inf)+1)),Inf);
//   fwprintf(TMPFILE,L"%s;",S);
   stmp+=swprintf(stmp,L"%s;",S);
   if(ImageKDM) ImageKDM->Add(I);
   break;
  case   INST:
   wcscpy(NameRoot,S);
   Compress(NameRoot);
   LowToUpp(NameRoot); Ln=Line;
   if(!wcscmp(NameRoot,L"РАСЧЕТ")||!wcscmp(NameRoot,L"РАСЧЁТ"))
	NStrCalc=Line;
   if(!wcscmp(NameRoot,L"ЛИСТ")){
	P=B=='='?ReadSp(Line,Delim):NULL;
	if(!P||(KolElem(P)>1)||P->R){
	 swprintf(Inf,L"Инструкция \"ЛИСТ:=\" должна содержать\n"
	  L"справа только наименование листа.");
	 ErrorSynt(RE,Inf); goto L;
	}
	L=TakeList(P->Nam); free(P->Nam); free(P);
//	fwprintf(TMPFILE,L"%s:=",NameRoot);
	stmp+=swprintf(stmp,L"%s:=",NameRoot);
   }
   else{
	I=*PI=(Root *)calloc(1,sizeof(Root)); PI=(Root **)I; I->Line=Ln;
	I->Name=wcscpy((wchar_t *)malloc(SzC*(wcslen(NameRoot)+1)),NameRoot);
	if(!wcscmp(NameRoot,L"РАБОТА")||wcsstr(NameRoot,L"ВЫПОЛНИЛ")){
	 wchar_t *s=Inf; while(ReadChar()&&(B==' ')); *s++=B;
	 while(ReadChar()&&(B!=';')) *s++=B; *s='\0';
	 if(I->First=P=(Parm *)calloc(1,sizeof(Parm))){
	  P->Nam=wcscpy((wchar_t *)calloc(wcslen(Inf)+1,SzC),Inf);
//	  fwprintf(TMPFILE,L"%s := ",S);
	  stmp+=swprintf(stmp,L"%s := ",S);
//	  ReadAllRight(I->First,TMPFILE,false,false);
	  stmp=ReadAllRight(I->First,Stmp,stmp,SLT,&SF,false,false);
	}}
	else{
	 if(B=='='){
	  I->First=P=ReadSp(Line,Delim);
	  wcscpy(S,NameRoot);
	 }
	 else
	  I->First=P=NULL;
	 if(I->First){ int k;
//	  fwprintf(TMPFILE,L"%s := ",S);
	  stmp+=swprintf(stmp,L"%s := ",S);
	  if(!wcscmp(NameRoot,L"ПЕЧАТАТЬ")||
		 !wcscmp(NameRoot,L"ПОКАЗАТЬ"))
//	   ReadAllRight(I->First,TMPFILE,true,false);
	  stmp=ReadAllRight(I->First,Stmp,stmp,SLT,&SF,true,false);
	  else if(wcsstr(NameRoot,L"РИСУН"))
//	   ReadAllRight(I->First,TMPFILE,false,true);
	  stmp=ReadAllRight(I->First,Stmp,stmp,SLT,&SF,false,true);
	  else{
//	   ReadAllRight(I->First,TMPFILE,true,true);
	   stmp=ReadAllRight(I->First,Stmp,stmp,SLT,&SF,true,true);
	 }}
//	 else fwprintf(TMPFILE,L"%s",S);
	 else stmp+=swprintf(stmp,L"%s",S);
	}
	if(!wcscmp(NameRoot,L"ВРЕМЯ")){
	 if(!I->First){
	  swprintf(Inf,L"Инструкция \"%s\" должна содержать:\n"
	   L"три члена списка - обозначение переменной \"время\",\n"
	   L"ее начальное и конечное значение в виде чисел или\n"
	   L"переменных - в скобках или подряд через запятую",I->Name);
	  Error(I->Line,0,true);
	 }
	 else if((KolElem(I->First->R)!=2) &&
			 (!I->First->Sled||!I->First->Sled->Sled)){
	  swprintf(Inf,L"Инструкция \"%s\" должна содержать:\n"
	   L"три члена списка - обозначение переменной \"время\",\n"
	   L"ее начальное и конечное значение в виде чисел или\n"
	   L"переменных - в скобках или подряд через запятую",I->Name);
	  Error(I->Line,0,true);
	}}
	if(!wcscmp(NameRoot,L"ВАРЬИРОВАТЬ")){
	 int K=KolElem(I->First);
	 swprintf(Inf,L"Инструкция \"%s\" должна содержать\n"
	  L"одну или несколько варьируемых переменных\n"
	  L"с границами (и, может быть, шагом) изменения\n",I->Name);
	 if(!K||
		(K==1)&&(KolElem(I->First->R)<2)||
		(K==2)&&
		 ((KolElem(I->First->R)<3)||(KolElem(I->First->Sled->R)<2)))
	  Error(I->Line,0,true);
	}
	if(ImageKDM) ImageKDM->Add(I);
//	fputwc(';',TMPFILE);
	*stmp++=';'; *stmp='\0';
   }
   if(!wcscmp(NameRoot,L"ИЗЬЯТЬ")){
	wcscpy(Inf,L"Правильное название инструкции - \"ИЗЪЯТЬ\"!");
	Atten(Line,0);
	if(ToWithdraw(I)) goto L;
   }
   if(!wcscmp(NameRoot,L"ИЗЪЯТЬ"))
	if(ToWithdraw(I)) goto L;
   break;
  case ROUT:
   wcscpy(NameRoot,Compress(S));
   if(!(P=ReadSp(Ln=Line,Delim))) goto M;
   O=TakeOper(NameRoot); O->Line=Ln; O->First=P;
//   fwprintf(TMPFILE,L"%s : ",NameRoot);
   stmp+=swprintf(stmp,L"%s : ",NameRoot);
   wcscpy(Inf,O->Name); LowToUpp(Inf);
   if(!wcscmp(Inf,L"ТЕНЗОР ИНЕРЦИИ"))
//	ReadAllRight(O->First,TMPFILE,true,true);
	stmp=ReadAllRight(O->First,Stmp,stmp,SLT,&SF,true,true);
   else
//	ReadAllRight(O->First,TMPFILE,true,false);
	stmp=ReadAllRight(O->First,Stmp,stmp,SLT,&SF,true,false);
//   fputwc(';',TMPFILE);
   *stmp++=';'; *stmp='\0';
   if(!wcscmp(Inf,L"КВАТЕРНИОН")&&O->First&&O->First->R&&
	  (KolElem(O->First->R)==4)&&O->First->R->Nam&&
	  O->First->R->Sled->Nam&&O->First->R->Sled->Sled->Nam&&
	  O->First->R->Sled->Sled->Sled->Nam){
	Arra *A=TakeArra(O->First->Nam); int i; Parm *p;
	A->N=4; A->Type=QUAT;
	A->F=(Form *)calloc(4,sizeof(Form));
	for(i=0,p=O->First->R;i<4;i++,p=p->Sled)
	 A->F[i].V=TakeVary(p->Nam);
   }
   goto L;
//  case BODY: fwprintf(TMPFILE,L"%s | ",S);
  case BODY: stmp+=swprintf(stmp,L"%s | ",S);
   CleanStr(S,Inf);
   b=TakeBody(Inf,wcschr(Inf,'~'));
   if(b){
	b->KoordSys=ReadSp(Line,Delim);
	if(Delim=='|')
	 b->Inert=ReadSp(Line,Delim);
   }
   if(b->KoordSys){
//	ReadAllRight(b->KoordSys,TMPFILE,true,false);
	stmp=ReadAllRight(b->KoordSys,Stmp,stmp,SLT,&SF,true,false);
//	fputws(L" | ",TMPFILE);
	stmp+=swprintf(stmp,L" | ");
   }
//   if(b->Inert) ReadAllRight(b->Inert,TMPFILE,true,false);
   if(b->Inert)
	stmp=ReadAllRight(b->Inert,Stmp,stmp,SLT,&SF,true,false);
//   fputwc(';',TMPFILE);
   *stmp++=';'; *stmp='\0';
   if(ImageKDM) ImageKDM->Add(b);
   goto L;
//  case STRINGER: fwprintf(TMPFILE,L"%s \\ ",S);
  case STRINGER: stmp+=swprintf(stmp,L"%s \\ ",S);
   CleanStr(S,Inf);
   r=TakeStringer(Inf);
   if(r)
	r->Parms=ReadSp(Line,Delim);
   for(Parm *P=r->Parms;P;P=P->Sled){
	if(P->R){ Cnst *C;
	 for(Parm *p=P->R;p;p=p->Sled){
	  if(!FindCnst(*p->Nam=='-'?p->Nam+1:p->Nam,&C)){
	  V=TakeVary(*p->Nam=='-'?p->Nam+1:p->Nam);
	  if(!(V->Line)) V->Line=Line;
   }}}}
//   if(r->Parms) ReadAllRight(r->Parms,TMPFILE,true,false);
   if(r->Parms)
	stmp=ReadAllRight(r->Parms,Stmp,stmp,SLT,&SF,true,false);
//   fputwc(';',TMPFILE);
   *stmp++=';'; *stmp='\0';
   if(ImageKDM) ImageKDM->Add(r);
   goto L;
  case TEXTCOMM:
   tt=(TextComm *)calloc(1,sizeof(TextComm));
   tt->Atr=TEXTCOMM; tt->S=wchar_t(B); tt->Line=Line;
   if(B=='#'){
	if(ReadComm(RE,tt)){
	 if(ImageKDM) ImageKDM->Add(tt); goto M;
	}
	else{
	 StatusBar->SimpleText=SBold;
	 return;
   }}
   else{
	Com++;
	if(EatBlock(RE,CGauge,tt)){
	 if(ImageKDM) ImageKDM->Add(tt); goto M;
	}
	else{
	 StatusBar->SimpleText=SBold;
	 return;
 } }}
 switch(B){
  case   ';':
   LowToUpp(S);
   if(!wcscmp(S,KONEC)){
	StatusBar->SimpleText=SBold;
	return;
   }
   goto M;
  case 255:
   StatusBar->SimpleText=SBold;
   return;
 }
L:goto M;
}

void AllToCollect(wchar_t *S,Parm *R){
 wchar_t *h=S;
 for(Parm *p=R;p;p=p->Sled){
  wchar_t *n=p->Nam;
  if(n){
   while(*n){
	if((*n=='\n')||(*n==' ')){
	 while(*n&&(*n=='\n')||(*n==' ')) n++; *h++=' ';
	}
    else *h++=*n++;
  }}
  if(p->R){
   *h+='(';
   for(Parm *r=p->R;r;r=r->Sled){
    if(r->Nam) h+=swprintf(h,r->Nam); else h+=swprintf(h,L"%g",r->Val);
    if(r->Sled){ *h++=','; *h++=' '; } else *h++=')';
  }}
  if(p->Sled){ *h++=','; *h++=' '; } else *h++='\0';
}}
//---------------------------------------------------------------------------
struct OperBodyStringer{
 OperBodyStringer *Sled; int Atr,Line; void *Robs;
} *OBS;
//---------------------------------------------------------------------------
int FindOperBodyStringer(OperBodyStringer *Rout,int L,
	 OperBodyStringer **M){
 if(!(*M=Rout)) return 0;
 if(L<Rout->Line){ *M=NULL; return 0; }
 for(;;*M=(*M)->Sled){
  if((*M)->Line==L) return 1;
  if(!((*M)->Sled)) return 0;
  if((*M)->Sled->Line>L) return 0;
}}
//---------------------------------------------------------------------------
OperBodyStringer *TakeOperBodyStringer(OperBodyStringer **Rout,Root 
*R){
 OperBodyStringer *M,*m; if(!R) return NULL;
 if(FindOperBodyStringer(*Rout,R->Line,&M)) return M;
 m=(OperBodyStringer *)calloc(1,sizeof(OperBodyStringer));
 m->Line=R->Line; m->Atr=ROUT; m->Robs=R;
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
OperBodyStringer *TakeOperBodyStringer(OperBodyStringer **Rout,Body 
*R){
 OperBodyStringer *M,*m; if(!R) return NULL;
 if(FindOperBodyStringer(*Rout,R->Line,&M)) return M;
 m=(OperBodyStringer *)calloc(1,sizeof(OperBodyStringer));
 m->Line=R->Line; m->Atr=BODY; m->Robs=R;
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
OperBodyStringer *TakeOperBodyStringer(OperBodyStringer **Rout,
 Stringer *R){
 OperBodyStringer *M,*m; if(!R) return NULL;
 if(FindOperBodyStringer(*Rout,R->Line,&M)) return M;
 m=(OperBodyStringer *)calloc(1,sizeof(OperBodyStringer));
 m->Line=R->Line; m->Atr=STRINGER; m->Robs=R;
 if(!M){ m->Sled=*Rout;   return   *Rout=m; }
 else  { m->Sled=M->Sled; return M->Sled=m; }
}
//---------------------------------------------------------------------------
Form SetRazmByName(wchar_t *Name){
 Form F; Vary *V; wchar_t *s;
 if(s=wcschr(Name,'\'')) *s='\0';
 if(!wcscmp(Name,L->Time->Name)) F=L->Time->Razm;
 else if(FindVary(Name,&V)&&(V->Razm.C!=Pust)) F=V->Razm;
 else F=SetRazmByForm(V->Znach);
 if(s){ *s='\''; return F/SetRazmByName(s+1); }
 return F;
}
//---------------------------------------------------------------------------
Form SetRazmByForm(Form Znach){
 Form F,Rl,Rr; List *Lo; Vary *V;
 switch(Znach.C->Atr){
  case CNST:
   F.C=Pust; return F;
  case VARY:
   if(Znach.V->Razm.C!=Pust)
    return Znach.V->Razm;
   if(wcschr(Znach.V->Name,'\''))
    return SetRazmByName(Znach.V->Name);
   else
    return SetRazmByForm(Znach.V->Znach);
  case UNOP:
   switch(Znach.U->Func){
	case 3: case 4: case 5:   //"sin", "cos", "tg"
     F.C=Pust; return F;
    case 8: case 9: case 10:
	case 11: case 12: case 18://"exp","ln","sh","ch","arsh","!"
	 F.C=Pust; return F;
    case 6: case 7: case 17://"arcsin","arctg","arccos"
	 if(FindList(L"РАЗМЕРНОСТИ",&L)&&
		FindVary(L"рад",&V)){//ищем переменную "рад"
      F.V=V; L=Lo;
     }
     else F.C=Pust;
      Lo=L;
     return F;
    case 1:case 13:case 14:case 15:case 16://"-","abs","rand","ceil","floor"
     return SetRazmByForm(Znach.U->Arg);
    case 2: //"sqrt"
     F=SetRazmByForm(Znach.U->Arg);
     if(F.C!=Pust) return sqrt(F);
     F.C=Pust; return F;
   }
  case BNOP:
   Rl=SetRazmByForm(Znach.B->L);
   Rr=SetRazmByForm(Znach.B->R);
   switch(Znach.B->Oper){
    case 1: case 2: case 5://+, - или %
     if(Rl==Pust) return Rr;
     if(Rr==Pust) return Rl;
     if(Rl==Rr) return Rl;
     F.C=Pust; return F;//диагностика
    case 3://*
     if(Rl==Pust) return Rr;
     if(Rr==Pust) return Rl;
     return Rl*Rr;
    case 4://:
     if(Rl==Pust) return F.C=Odin,F;
     if(Rr==Pust) return Rl;
     if(Rl==Rr) return F.C=Pust,F;
     return Rl/Rr;
    case 6://^
     if(Rr!=Pust) return F.C=Pust,F;//диагностика
     if(Znach.B->R.C==Odin) return Rl;
     if(Znach.B->R.C==Dva) return Rl*Rl;
     if(Znach.B->R.C==Tri) return Rl*Rl*Rl;
     return F.C=Pust,F;//диагностика
   }
  default: F.C=Pust; return F;
}}
//---------------------------------------------------------------------------
Cord *TakeCord(Vary *V,Cord **R){
 Cord *K,*C=(Cord *)calloc(1,sizeof(Cord));
 C->V=V; K=*R;
 if(!K) *R=C;
 else { for(;K->Sled;K=K->Sled); K->Sled=C; }
 return C;
}
//---------------------------------------------------------------------------
void FormTo2List(Form F,Cord **Ch,Cord **Zn){
 switch(F.C->Atr){
  case VARY:
    TakeCord(F.V,Ch);
   break;
  case BNOP:
   switch(F.B->Oper){
    case 3:  //умножение
     FormTo2List(F.B->L,Ch,Zn);
     FormTo2List(F.B->R,Ch,Zn);
     break;
    case 4: //деление
     FormTo2List(F.B->L,Ch,Zn);
     FormTo2List(F.B->R,Zn,Ch);
     break;
    case 6: //возведение в степень
     int K=Round(F.B->R.C->Val);
     for(int i=0;i<K;i++)
      FormTo2List(F.B->L,Ch,Zn);
     break;
   }
   break;
}}
//---------------------------------------------------------------------------
Form TwoListToForm(Cord *Ch,Cord *Zn){
 Form L,R;
 if(Ch){
  L.V=Ch->V; for(Cord *q=Ch->Sled;q;q=q->Sled){ R.V=q->V; L=L*R; }
 }
 else L.C=Odin;
 if(Zn){ for(Cord *q=Zn;q;q=q->Sled){ R.V=q->V; L=L/R; }}
 return L;
}
//---------------------------------------------------------------------------
void MessageRazmNotEcv(wchar_t *Buf, Form F1,Form F2){
 String S1,S2;
 FormToStr(F1,&S1,false);
 FormToStr(F2,&S2,false);
 swprintf(Inf,L"%s\nтребуется размерность %s,\n"
  L"а указана размерность %s",Buf,S2,S1);
 Application->MessageBoxA(Inf,L"Ошибка в размерности",MB_ICONERROR);
}
//---------------------------------------------------------------------------
//Сокращаем размерность в числителе и знаменателе
Form TrunCateRazmNmDn (Form Razm){
 Cord *Nm=NULL,*Dn=NULL,*Rz=NULL;
 FormTo2List(Razm,&Nm,&Dn);
 for(Cord *q=Nm;q;q=q->Sled)
  if(FindCord(q->V,Dn)){
   TakeCord(q->V,&Rz);
   DelCord(&Dn,q);
  }
 for(Cord *q=Rz;q;q=q->Sled)
  DelCord(&Nm,q);
 Razm=TwoListToForm(Nm,Dn);
 DelAllList((void **)&Nm);
 DelAllList((void **)&Dn);
 DelAllList((void **)&Rz);
 return Razm;
}
//---------------------------------------------------------------------------
void DiagnosRazm(Form F,Form R){
 Form Rl,Rr,Znach,RazmArg,RazmForm,RazmR,RazmL;
 List *Lo; Vary *V; wchar_t Buf[256]; String B1,B2;
 switch(F.C->Atr){
  case CNST:
   return;
  case VARY:
   if(F.V->Razm.C!=Pust){
	if(F.V->Razm/R!=Odin){
	 swprintf(Buf,L"Для переменной %s",F.V->Name);
	 MessageRazmNotEcv(Buf,F.V->Razm,R);
   }}
   return;
  case UNOP:
   switch(F.U->Func){
	case 3: case 4: case 5:   //"sin","cos","tg"
	 F.C=Pust; return;// F;
	case 8: case 9: case 10:
	case 11: case 12: case 18://"exp","ln","sh","ch","arsh","!"
	 F.C=Pust; return;// F;
	case 6: case 7: case 17://"arcsin","arctg","arccos"
	 if(FindList(L"РАЗМЕРНОСТИ",&L)&&
		FindVary(L"рад",&V)){//ищем переменную "рад"
	  F.V=V; L=Lo;
	 }
	 else F.C=Pust;
	 Lo=L;
	 return;
	case 1:case 13:case 14:case 15:case 16://"-","abs","rand","ceil","floor"
	 DiagnosRazm(F.U->Arg, R);
	 return;
	case 2: //"sqrt"
     R=R*R;
     DiagnosRazm(F.U->Arg, R);
     return;
   }
  case BNOP:
   if(F.B->L.C->Atr==3) return;
   if((F.B->Oper!=6)&&(F.B->R.C->Atr==3)) return;
   switch(F.B->Oper){
    case 6:
     RazmArg=SetRazmByForm(F.B->L),RazmForm=RazmArg^F.B->R;
	 if(RazmArg!=R){
	  FormToStr(F,&B1,false);
	  swprintf(Buf,L"Для формулы \n%s\n",B1);
	  MessageRazmNotEcv(Buf,RazmArg,R);
	 } break;
//проверка размерности правого и левого операнда
	case 1: case 2:
	 RazmL=SetRazmByForm(F.B->L);
	 if(RazmL/R!=Odin){
	  FormToStr(F.B->L,&B1,false);
	  FormToStr(F,&B2,false);
	  swprintf(Buf,L"Для левого операнда \n%s\n формулы \n%s\n ",
	   B1,B2);
	  MessageRazmNotEcv(Buf,RazmL,R);
	 }
	 RazmR=SetRazmByForm(F.B->R);
	 if(RazmR/R!=Odin){
	  FormToStr(F.B->R,&B1,false);
	  FormToStr(F,&B2,false);
	  swprintf(Buf,L"Для правого операнда \n%s\n формулы \n%s\n ",
	   B1,B2);
	  MessageRazmNotEcv(Buf,RazmR,R);
	 } break;
	case 3: case 4: case 5:
	 RazmL=SetRazmByForm(F.B->L);
	 RazmR=SetRazmByForm(F.B->R);
	 if(F.B->Oper==3) RazmL=RazmL*RazmR;
	 else RazmL=RazmL/RazmR;
	  RazmL=TrunCateRazmNmDn (RazmL);
	  if(RazmL/R!=Odin){
	   FormToStr(F.B->L,&B1,false);
	   FormToStr(F.B->R,&B2,false);
	  if (F.B->Oper==3)
	   swprintf(Buf,L"Для произведения \n%s * %s\n",B1,B2);
	  else
	   swprintf(Buf,L"Для дроби  \n%s/%s",B1,B2);
	  MessageRazmNotEcv(Buf,RazmL,R);
	 }
	 break;
	}
	return;
  default: F.C=Pust; return;// F;
}}
//---------------------------------------------------------------------------
void SetRazm(void){
 for(Vary *V=L->V; V; V=V->Sled){
  Cord *Nm=NULL,*Dn=NULL,*Rz=NULL;
  if(V->Razm.C==Pust){
   if(wcschr(V->Name,'\''))
    V->Razm=SetRazmByName(V->Name);
   else
    V->Razm=SetRazmByForm(V->Znach);
  }
//Сокращаем размерность в числителе и знаменателе
  FormTo2List(V->Razm,&Nm,&Dn);
  for(Cord *q=Nm;q;q=q->Sled)
   if(FindCord(q->V,Dn)){
	TakeCord(q->V,&Rz);
	DelCord(&Dn,q);
   }
  for(Cord *q=Rz;q;q=q->Sled)
   DelCord(&Nm,q);
  V->Razm=TwoListToForm(Nm,Dn);
  DelAllList((void **)&Nm);
  DelAllList((void **)&Dn);
  DelAllList((void **)&Rz);
}}
//---------------------------------------------------------------------------
bool VvodJob(TStringList *SLM,TRichEdit *Ri,TCGauge *CGauge,
  TStatusBar *StatusBar,TImageKDM *ImageKDM,
  TTabControl *TabControl,TCanvas *Canvas){
 int i,l,K=0; Vary *V; Root *O,*I;// struct _stat St;
 wchar_t NameRoot[256],Buf[256],SBold[256];
 Magazine *Mi=NULL,*m; OperBodyStringer *ROBS=NULL,*robs;
// if(_wstat(InpFile,&St)) return false;
// Lfile=Ri->Text.Length(); St.st_size;
// INFILE=_wfopen(ANSIFile/*InpFile*/,L"r");
// Ri->Lines->LoadFromFile(ANSIFile);
// while(fgetws(Inf,1023,INFILE)&&wcsrchr(Inf,'\n')) Lfile--;
// rewind(INFILE);
Beg:
 IsSyntError=false; PozFile=NumbTask=0; S[0]=SP[0]='\0'; Line=1; sp=SP;
// rewind(INFILE);
 for(int i=0;i<SLM->Count;i++)
// while(Fgets(Inf,255,INFILE))
  if((l=SLM->Strings[i].Length())>StrLong)
   StrLong=l;
// rewind(INFILE);
 t=SLM->Text.w_str(); Lfile=SLM->Text.Length();
// TMPFILE=_wfopen(L"tmp.htm",L"w");
 Stmp[0]='\0'; stmp=Stmp; SLT=new TStringList;
 Vvod(Ri,CGauge,StatusBar,ImageKDM);
 if(CGauge) CGauge->Progress=0;
// fclose(TMPFILE);
 SLT->SaveToFile("tmp.htm");
// HTMFILE=_wfopen(HtmFile,_waccess(HtmFile,0)?L"w":L"a");
// if(!HTMFILE){
//  swprintf(Inf,L"Нельзя открыть файл для формирования отчета!");
//  Application->MessageBox(Inf,L"Системная ошибка!",MB_OK|
//MB_ICONERROR);
//  return false;
// }
// else{
 if(!SLH) SLH=new TStringList;
 if(SLH){
//  fprintf(HTMFILE,"<html>\n<head>\n<title>О Т Ч Е Т   К И Д И М</title>\n"
//   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
//   "wchar_tset=windows-1251\">\n</head>\n"
//   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
//   " background=\"file:///%s\\fon.jpg\""
//   " font face=\"Times New Roman, Times, serif\">\n",
//	AnsiString(WORKDIR).c_str());
//  fwprintf(HTMFILE,L"<h3 align=\"center\"><font color=\"#990000\">"
//   L"О Т Ч Е Т</font></h3>\n");
  SLH->Add(L"<html>"); SLH->Add(L"<head>");
  SLH->Add(L"<title>О Т Ч Е Т   К И Д И М</title>");
  SLH->Add(L"<meta http-equiv=\"Content-Type\""
   L" content=\"text/html; charset=windows-1251\">");
  SLH->Add(L"</head>");
  shtm+=swprintf(shtm=Shtm,
   L"<body bgcolor=\"#FFFFFF\" text=\"#800000\"");
  shtm+=swprintf(shtm,L" background=\"file:///%s\\fon.jpg\"",
		  WORKDIR);
  shtm+=swprintf(shtm,L" font face=\"Times New Roman, Times, serif\">");
  SLH->Add(Shtm); *Shtm='\0'; shtm=Shtm;
  SLH->Add(L"<h3 align=\"center\"><font color=\"#990000\">"
   L"О Т Ч Е Т</font></h3>");
 }
 if(IsSyntError) goto AvExit;
 if(ISRAZM){
  SetRazm();
  for(Vary *V=L->V; V; V=V->Sled){
   DiagnosRazm(V->Znach,V->Razm);
   V->Krazm=ToBaseUnit(V->Razm);
 }}
 if(Mtp){
  wcscpy(Inf,L"В исходных данных есть незавершенные\n"
   L"тернарные предложения.\nСм. закладку \"Ошибки\"");
  Application->MessageBox(Inf,L"ВНИМАНИЕ!",MB_OK|MB_ICONWARNING);
//  if(ERRFILE=_wfopen(ErrFile,L"a")){
//   inf+=swprintf(inf=Inf,
//	L"* ВНИМАНИЕ * В следующих строках исходных данных\n"
//	L"есть незавершенные тернарные предложения:\n");
//   for(Magazine *m=Mtp;m;m=m->Sled)
//	inf+=swprintf(inf,L"%s\n",m->S);
//   Fputs(Inf,ERRFILE); fclose(ERRFILE);
//  }
  if(SLE){
   wcscpy(Serr,
	L"* ВНИМАНИЕ * В следующих строках исходных данных\n"
	L"есть незавершенные тернарные предложения:");
   SLE->Add(Serr);
   for(Magazine *m=Mtp;m;m=m->Sled)
	SLE->Add(m->S);
   SLE->SaveToFile(ErrFile);
 }}
 if(ISRAND) randomize();
 L=LBeg;
//Уберем последнюю инструкцию РАСЧЕТ
 for(I=L->Inst;I&&I->Sled&&I->Sled->Sled;I=I->Sled);
 if(I&&I->Sled){
  wcscpy(Inf,I->Sled->Name); LowToUpp(Inf);
  if(!wcscmp(Inf,L"РАСЧЕТ")||!wcscmp(Inf,L"РАСЧЁТ")){
   if(I->Sled->First){
	if(I->Sled->First->Nam) free(I->Sled->First->Nam);
	free(I->Sled->First);
   }
   if(I->Sled->Name) free(I->Sled->Name);
   free(I->Sled); I->Sled=NULL;
 }}
//Зафиксируем № строки с предыдущей инструкцией РАСЧЕТ
 for(NStrCalc=0,I=L->Inst;I;I=I->Sled){
  wcscpy(Inf,I->Name); LowToUpp(Inf);
  if(!wcscmp(Inf,L"РАСЧЕТ")||!wcscmp(Inf,L"РАСЧЁТ"))
   NStrCalc=I->Line;
 }
//Проверим инструкции на повторение
 for(I=L->Inst;I;I=I->Sled){
  if(I->Line>NStrCalc){
   wcscpy(NameRoot,I->Name); LowToUpp(NameRoot);
   if(wcscmp(NameRoot,L"АНИМАЦИЯ")&&wcscmp(NameRoot,L"ФОН")&&
	  wcscmp(NameRoot,L"АНИМАЦИЯ3D")&&wcscmp(NameRoot,L"ФОН3D")&&
	  wcscmp(NameRoot,L"СРАВНИТЬ")&&
	  wcscmp(NameRoot,L"СОВМЕСТИТЬ")&&
	  FindMagazine(Mi,NameRoot,&m)){
	swprintf(Inf,
	 L"Инструкция \"%s\" повторно записана в строке %d",
	 NameRoot,I->Line);
	Application->MessageBox(Inf,L"ВНИМАНИЕ!",MB_OK|
MB_ICONWARNING);
   }
   else TakeMagazine(&Mi,NameRoot);
 }}
 if(I=FindInstLast(L"КОДИРОВКА")){
  int l=wcslen(I->First->Nam);
  if(I->First->Nam&&(l==5))
   CodElem(I->First->Nam);
 }
 if((I=FindInstLast(L"ЕДИНИЦЫ ИЗМЕРЕНИЯ"))||
	(I=FindInstLast(L"РАЗМЕРНОСТИ"))){
  if(MUnit) MUnit=(Magazine *)realloc(MUnit,3*sizeof(Magazine));
  else MUnit=(Magazine *)calloc(3,sizeof(Magazine));
  MUnit->Sled=MUnit+1; MUnit->Sled->Sled=MUnit+2;
  for(Parm *P=I->First;P;P=P->Sled){
   wchar_t *S=(wchar_t *)calloc(wcslen(P->R->Nam)+1,SzC);
   switch(P->Nam[0]){
	case 'm':
	 MUnit[0].S=wcscpy(S,P->R->Nam);
	 break;
	case 'l':
	 MUnit[1].S=wcscpy(S,P->R->Nam);
	 break;
	case 't':
	 MUnit[2].S=wcscpy(S,P->R->Nam);
	 break;
	default:
	 swprintf(Inf,
	  L"Задан неизвестный тип размерности %s",P->R->Nam);
	 Application->MessageBoxA(Inf,I->Name,MB_OK);
	 free(S);
  }}
 }
 if(!(I=FindInstLast(L"РАБОТА"))){
  swprintf(Inf,
   L"В исходных данных нет инструкции \"РАБОТА:=<Название работы>;\"");
  Atten(0,0); wcscpy(HeadWork,L"Нет названия работы");
 }
 else if(!I->First||!I->First->Nam){
  swprintf(Inf,
   L"В инструкции \"%s\" в правой части надо указать название работы",
   I->Name);
  Atten(I->Line,0);
 }
 else{
  AllToCollect(HeadWork,I->First);
//  if(HTMFILE){
//   wchar_t *Buf=(wchar_t *)calloc(wcslen(HeadWork)+1,SzC);
//   wcscpy(Buf,HeadWork);
//   Buf=LiterGrekToHtm(Buf);
//   fwprintf(HTMFILE,L"<h3 align=\"center\"><font color=\"#990000\">"
//	L"по работе \"%s\"</font></h3>\n",Buf);
//   free(Buf);
//  }
  if(SLH){
   wchar_t *Buf=(wchar_t *)calloc(wcslen(HeadWork)+1,SzC);
   wcscpy(Buf,HeadWork);
   Buf=LiterGrekToHtm(Buf);
   shtm+=swprintf(shtm=Shtm,
	L"<h3 align=\"center\"><font color=\"#990000\">"
	L"по работе \"%s\"</font></h3>",Buf);
   SLH->Add(Shtm); shtm=Shtm;
   wcscpy(HeadWork,Buf);
   free(Buf);
 }}
 if(I=FindInstLast(L"ВАРИАНТ")){
  if(!I->First||!I->First->Nam){
   swprintf(Inf,
	L"В инструкции \"%s\" в правой части надо указать название варианта работы",
	I->Name);
   Atten(I->Line,0);
  }
  else{
   AllToCollect(HeadVersion,I->First);
//   if(HTMFILE)
//	fwprintf(HTMFILE,L"<h4 align=\"center\"><font color=\"#000099\">"
//	 L"Вариант: \"%s\"</font></h4>\n",HeadVersion);
   if(SLH)
	swprintf(Shtm,L"<h4 align=\"center\"><font color=\"#000099\">"
	 L"Вариант: \"%s\"</font></h4>",HeadVersion);
	SLH->Add(Shtm);
 }}
 if(!(I=FindInstLast(L"ВЫПОЛНИЛ"))&&!(I=FindInstLast(L"ВЫПОЛНИЛА"))&&
	!(I=FindInstLast(L"ВЫПОЛНИЛИ"))){
  swprintf(Inf,L"В исходных данных нет инструкции \"ВЫПОЛНИЛ\"\n"
   L"(\"ВЫПОЛНИЛА\", \"ВЫПОЛНИЛИ\")");
  Atten(0,0); wcscpy(Autor,L"Нет автора");
 }
 else{
  if(I->First&&I->First->Nam){
   AllToCollect(Autor,I->First);
//   if(HTMFILE)
//	fwprintf(HTMFILE,
//	 L"<h3 align=\"center\"><font color=\"#990000\">%s: \"%s\"</font></h3>\n",
//	 !wcscmp(I->Name,L"ВЫПОЛНИЛ")?"Выполнил":
//	 !wcscmp(I->Name,L"ВЫПОЛНИЛА")?"Выполнила":"Выполнили",Autor);
   if(SLH){
	swprintf(Shtm,
	 L"<h3 align=\"center\"><font color=\"#990000\">%s: \"%s\"</font></h3>\n",
	 !wcscmp(I->Name,L"ВЫПОЛНИЛ")?L"Выполнил":
	 !wcscmp(I->Name,L"ВЫПОЛНИЛА")?L"Выполнила":L"Выполнили",
	 Autor);
	SLH->Add(Shtm);
  }}
  else{
   swprintf(Inf,
	L"В инструкции \"%s\" в правой части надо указать авторство",
	I->Name);
   Atten(I->Line,0);
 }}
 //if((I=FindInstLast(L"РИСУНОК"))||(I=FindInstLast(L"РИСУНКИ"))&&
 //	HTMFILE){
 if((I=FindInstLast(L"РИСУНОК"))||(I=FindInstLast(L"РИСУНКИ"))&&
	SLH){
  wchar_t DrawsFile[256]=L"\0";
  if(!I->First||!I->First->Nam){
   swprintf(Inf,
	L"В инструкции \"%s\" в правой части надо указать имя файла с рисунком",
	I->Name);
   Atten(I->Line,0);
  }
  else{ wchar_t *s,Ris[256],*ss; int KolRis=0;
   for(Parm *P=I->First;P;P=P->Sled){
	if(wcsrchr(P->Nam,'\\')){
	 if(s=wcsstr(P->Nam,L"..\\")){
	  if(ss=wcsrchr(CurrDir,'\\')){
	   *ss='\0'; swprintf(DrawsFile,L"%s\\%s",CurrDir,s+3); *ss='\\';
	 }}
	 else wcscpy(DrawsFile,*P->Nam=='"'?P->Nam+1:P->Nam);
	}
	else if((s=wcsstr(P->Nam,L"../"))||(s=wcsstr(P->Nam,L"..\\"))){
	 if(ss=wcsrchr(CurrDir,'\\')){
	  *ss='\0'; swprintf(DrawsFile,L"%s\\%s",CurrDir,s+3); *ss='\\';
	}}
	else
	 swprintf(DrawsFile,L"%s\\%s",CurrDir,*P->Nam=='"'?P->Nam+1:P->Nam);
	if(s=wcsrchr(DrawsFile,'"')) *s='\0';
	if(!(DrawsFile[0])){
	 swprintf(Inf,L"В инструкции \"%s\" не указано имени файла с рисунком.",
	  I->Name);
	 Atten(I->Line,0);
	}
	else if(_waccess(DrawsFile,0)){
	 if(s=wcsrchr(InpFile,'\\')){
	  wcscpy(Ris,InpFile); s=wcsrchr(Ris,'\\'); *s='\0';
	  swprintf(Inf,L"%s\\%s",Ris,DrawsFile);
	  if(!wcsrchr(Inf,'.')) wcscat(Inf,L".BMP");
	  if(!_waccess(Inf,0)){
	   wcscpy(DrawsFile,Inf); KolRis++;
	   if(wcscmp(P->Nam,DrawsFile)){
		free(P->Nam);
		P->Nam=wcscpy((wchar_t *)calloc(wcslen(DrawsFile)+1,SzC),DrawsFile);
	  }}
	  else{
	   swprintf(Inf,
		L"В инструкции \"%s\" ссылка на файл с рисунком недействительна.\n"
		L"Файл %s не найден",I->Name,P->Nam);
	   Atten(I->Line,0);
	 }}
	 else{
	  swprintf(Inf,
	   L"В инструкции \"%s\" ссылка на файл с рисунком недействительна.\n"
	   L"Файл %s не найден",I->Name,P->Nam);
	  Atten(I->Line,0);
	}}
	else{
	 KolRis++;
	 if(wcscmp(P->Nam,DrawsFile)){
	  free(P->Nam);
	  P->Nam=wcscpy((wchar_t *)calloc(wcslen(DrawsFile)+1,SzC),DrawsFile);
   }}}
   if(KolRis>1){
//	fputws(L"<p align=\"center\">\n<table width=\"100%\" border=\"0\">\n <tr>\n",
//	 HTMFILE);
	SLH->Add(L"<p align=\"center\">");
	SLH->Add(L"<table width=\"100%\" border=\"0\">");
	SLH->Add(L"<tr>\n");
   }
   for(Parm *P=I->First;P;P=P->Sled){
	if(wcsrchr(P->Nam,'\\')) wcscpy(DrawsFile,*P->Nam=='"'?P->Nam+1:P->Nam);
	else swprintf(DrawsFile,L"%s\\%s",CurrDir,*P->Nam=='"'?P->Nam+1:P->Nam);
	if(s=wcsrchr(DrawsFile,'"')) *s='\0';
	if(DrawsFile[0]){
	 if(!_waccess(DrawsFile,0)){
	  if(KolRis>1)
//	   fwprintf(HTMFILE,L"  <th><img src=\"%s\"></th>\n",DrawsFile);
	   swprintf(Shtm,L"  <th><img src=\"%s\"></th>",DrawsFile);
	  else
//	   fwprintf(HTMFILE,L"<p align=\"center\"><img src=\"%s\"></p>",DrawsFile);
	   swprintf(Shtm,L"<p align=\"center\"><img src=\"%s\"></p>",
		DrawsFile);
	  SLH->Add(Shtm);
	 }
	 else{
	  if(s=wcsrchr(InpFile,'\\')){
	   wcscpy(Ris,InpFile); s=wcsrchr(Ris,'\\'); *s='\0';
	   swprintf(Inf,L"%s\\%s",Ris,DrawsFile);
	   if(!_waccess(Inf,0)){
		if(KolRis>1)
//		 fwprintf(HTMFILE,L"  <th><img src=\"%s\"></th>\n",Inf);
		 swprintf(Shtm,L"  <th><img src=\"%s\"></th>\n",Inf);
		else
//		 fwprintf(HTMFILE,L"<p align=\"center\"><img src=\"%s\"></p>",Inf);
		 swprintf(Shtm,L"<p align=\"center\"><img src=\"%s\"></p>",Inf);
		SLH->Add(Shtm);
   }}}}}
//   if(KolRis>1) fputws(L" </tr>\n</table>\n</p>\n",HTMFILE);
   if(KolRis>1){
	SLH->Add(L" </tr>"); SLH->Add(L"</table>"); SLH->Add(L"</p>");
 }}}
 if(SLH){
  swprintf(Shtm,L"<h4 align=\"left\"><font color=\"#009900\">"
   L"%d. Исходные данные</font></h4>",++PUNKT);
  SLH->Add(Shtm);
  swprintf(Shtm,L"<p align=\"left\"><font face=\"%s\">",Ri->Font->Name);
  SLH->Add(Shtm);
  if(StrLong>75){
//   fwprintf(HTMFILE,L"<font size=-%d>",1);
   SLH->Add(L"<font size=-1>"); StrLong=104; SMALLFONT=true;
  }
//  SLT->SaveToFile(L"tmp.htm");
//  if(TMPFILE=_wfopen(L"tmp.htm",L"r")){
//   struct _stat St; wchar_t B;
//   _wstat(L"tmp.htm",&St);
//   for(int i=0;i<St.st_size;i++){
//	B=fgetwc(TMPFILE); if(B=='\n') i++; fputwc(B,HTMFILE);
//   }
//   fclose(TMPFILE);
//  }
//  SLT->LoadFromFile(L"tmp.htm");
  for(int i=0;i<SLT->Count;i++)
   SLH->Add(SLT->Strings[i]);
  if(StrLong>75) SLH->Add(L"</font>");
  SLH->Add(L"</font></p>");
 }
 for(L=LBeg;L;L=L->Sled){ int Kr=0;
  for(O=L->Oper;O;O=O->Sled){
   wcscpy(NameRoot,O->Name); LowToUpp(NameRoot);
   if(!wcscmp(NameRoot,L"РАЗМНОЖИТЬ")) Kr++;
  }
  if(Kr>1){
   swprintf(Inf,
	L"Оператор \"РАЗМНОЖИТЬ\" должен быть в одном экземпляре,\n"
	L"объедините содержимое таких операторов в один!");
   Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
//   ErrorSynt(O->Line,0); fclose(HTMFILE); goto AvExit;
   ErrorSynt(O->Line,0);
//   fclose(HTMFILE);
   SLH->SaveToFile(HtmFile);
   goto AvExit;
  }
//расставим операторы и твердые тела со стержнями в порядке записи в и/д
  for(O=L->Oper;O;O=O->Sled)
   TakeOperBodyStringer(&ROBS,O);
  for(Body *B=L->B;B;B=B->Sled)
   TakeOperBodyStringer(&ROBS,B);
  for(Stringer *S=L->T;S;S=S->Sled)
   TakeOperBodyStringer(&ROBS,S);
  BodysIsTreated=false;
  if(CGauge){
   CGauge->Progress=0; CGauge->MaxValue=KolElem(ROBS);
  }
  wcscpy(SBold,StatusBar->SimpleText.c_str());
//  if(ROBS)
//   TabControl->Tabs->Add(L"Операторы");
//  N=Coord(L,CGauge,StatusBar);//вставлено 04.05.2017
Root *I;
  StatusBar->SimpleText=L"Обрабатываем операторы, твердые тела";
  for(robs=ROBS;robs;robs=robs->Sled){
   if(CGauge) CGauge->Progress++;
   switch(robs->Atr){
	case ROUT:
	 if(*((Root *)robs->Robs)->Name=='@') continue;
	 if(i=FindOper(((Root *)robs->Robs)->Name)){ i--;
	  if(!OP[i](((Root *)robs->Robs)->First,NULL,Canvas))
	   IsSyntError=true;
	 }
	 else{
	  swprintf(Inf,
	   L"Обработчик оператора \"%s\", записанного в строке №%d, не найден.",
	   ((Root *)robs->Robs)->Name,robs->Line);
	  Application->MessageBox(Inf,L"Ошибка!",MB_OK|MB_ICONERROR);
	  ErrorSynt(robs->Line,0);
//	  fclose(HTMFILE);
	  SLH->SaveToFile(HtmFile);
	  goto AvExit;
	 }
//  I=FindInstLast(L"ПЕЧАТАТЬ");
	 break;
	case BODY:
	 if(L->B&&!BodysIsTreated){
	  if(*((Body *)robs->Robs)->Name=='@') continue;
	  Bodys(L->B,StatusBar,TabControl,CGauge); BodysIsTreated=true;
	 }
	 break;
	case STRINGER:
	 if(*((Stringer *)robs->Robs)->Name=='@') continue;
	 Stringers(L->T);
	 break;
  }}
//  if(L->B) Bodys(L->B,StatusBar);
//  if(L->T) Stringers(L->T);
  StatusBar->SimpleText=SBold;
  wcscpy(SBold,StatusBar->SimpleText.c_str());
  StatusBar->SimpleText=L"Поиск колец в формулах";
  if(CGauge){
   for(Vary *V=L->V;V;V=V->Sled) K++;
   CGauge->MaxValue=K; CGauge->Progress=0; K=0;
  }
  Magazine *Mrez=NULL;
//_wunlink(L"Cykl.txt");
  for(Vary *V=L->V;V;V=V->Sled,K++){
   Magazine *Mv=NULL;
//FILE *Fcykl;
//Fcykl=_wfopen(L"Cykl.txt",L"a+"); fwprintf(Fcykl,L"%s\n",V->Name); fclose(Fcykl);
   TakeMagazine(&Mv,V->Name);
   if(CGauge) CGauge->Progress++;
   if(IsCykl(Mv,V->Znach,&Mrez)){ wchar_t *inf;
	inf+=swprintf(inf=Inf,L"Переменная \"%s\" или входящие в ее формулу\n"
	 L"переменные зависят сами от себя.\n",V->Name);
	inf+=swprintf(inf,L"Последовательность проверки:\n");
	for(Magazine *m=Mrez;m;m=m->Sled)
	 inf+=swprintf(inf,L"%s%s",m->S,m->Sled?",\n":".\n");
	swprintf(inf,L"Зацикливание.");
	Application->MessageBox(Inf,L"ОШИБКА!",MB_OK|MB_ICONERROR);
	goto AvExit;
   }
   DelAllMagazine(&Mv);
  }
  if(CGauge) CGauge->Progress=0;
  StatusBar->SimpleText=SBold;
 }
AvExit: L=LBeg;
 return !IsSyntError;
}

int ReadStr(int Chto){
 bool Numb; int KT=0,KE=0,i=0,j,Ks; wchar_t C;
M:if(B=*sp++){
  switch(B){
   case  '+': goto M;
   case  ' ': goto M;
   case  '\t': goto M;
   case  ';': case  '#': return PUST;
   case  '-': S[i++]=B; S[i]='\0'; return '-';
   case  '(': S[i++]=B; S[i]='\0'; return '(';
   case  ')': S[i++]=B; S[i]='\0'; return ')';
   case  '=': return PUST;
   case  '.': Chto=Double; Numb=true; S[i++]=B; break;
   case  '"': return PUST;
   default  :
    if(isdigit(B)){
     if(Chto!=Double) Chto=Long; Numb=true; S[i++]=B; break;
    }
    if(isalpha(B)||isciril(B)||B=='_'||B=='\x14'){
     if(!Chto) Chto=NAME; Numb=false; S[i++]=B; break;
    }
    else{
	 swprintf(Inf,L"Запрещенный символ - %c",B);
     Application->MessageBox(Inf,L"Ошибка!",MB_OK); return -1;
 }}}
 else return PUST;
L:if(B=*sp++){
  switch(B){
   case  ' ':
   case  '\t': S[i++]=B; goto L;
   case  '(':
    if(Numb){
     swprintf(Inf,L"Ожидалось имя");
     Application->MessageBox(Inf,L"Ошибка!",MB_OK); return -1;
    }
    else
     if(S[i-1]==SimbInd){
      for(S[i++]=B;(B=*sp++)&&(B!=')');) S[i++]=B;
      S[i++]=B;
      goto L; //!!!
     }
    break;
   case ')': case ',': case '*': case '/': case '%': case '^': case ';':
   case '<': case '>': case '\\': case '!': case '?':
    break;
   case  '+': case  '-':
    S[i]='\0';
    Compress(S);
    if(((C=S[i-1])=='E'||C=='e')&&Chto==Double){ S[i++]=B; goto L; }
    break;
   case  '=': S[i]='\0'; Chto=VARY; Numb=false; break;
   case  'E': case  'e':
    if(Numb){
     Chto=Double;
     if(++KE>1){
      swprintf(Inf,L"В числе 2 буквы 'E'");
      Application->MessageBox(Inf,L"Ошибка!",MB_OK); return -1;
    }}
    S[i++]=B; goto L;
   case  '.':
    if(Numb){ Chto=Double;
     if(++KT>1){
      swprintf(Inf,L"В числе 2 точки");
      Application->MessageBox(Inf,L"Ошибка!",MB_OK); return -1;
    }}
    S[i++]=B; goto L;
   default  :
//    if((isalpha(B)||isciril(B))&&Numb){
//     swprintf(Inf,L"Если это число, то ожидалась цифра\n"
//      L"или символы: '.', 'E', 'e', ';'");
//     Application->MessageBox(Inf,L"Ошибка!",MB_OK); return -1;
//    }
    S[i++]=B; goto L;
  }
  S[i]='\0';
  Compress(S);
 }
 S[i]='\0';
 Compress(S);
 return Chto;
}

int LogZnac(){ int i; wchar_t B1=B;
 if(!(B=*sp++)) return -1;
 switch(B1){
  case '<':  if(B=='=') return 3; else if(B=='<') return 7; i=1; break;
  case '>':  if(B=='=') return 4; else if(B=='>') return 8; i=2; break;
  case '\\': if(B=='=') return 5; i=0; break;
  case '=':  if(B=='=') return 6; i=0; break;
  default :
   swprintf(Inf,
	L"Ожидалось: '%c', '%c', '%c%c', '%c%c', '%c%c', '%c%c', '%c%c', '%c%c'",
	LOGZNAC[1],LOGZNAC[2],LOGZNAC[1],LOGZNAC[4],LOGZNAC[2],LOGZNAC[4],
	LOGZNAC[3],LOGZNAC[4],LOGZNAC[4],LOGZNAC[4],LOGZNAC[1],LOGZNAC[1],
	LOGZNAC[2],LOGZNAC[2]);
   Application->MessageBox(Inf,L"Ошибка!",MB_OK);
   i=0;
 }
 B=B1; sp--; return i;
}

Form ReadForm(int Prio){
 Cnst *C; Form F,U,V,FT,FF; signed int Oper; Vary *VV; Arra *A; ArrI *a;
 int KS=0,KKS=0; wchar_t *Shtr,tst[16];
 swprintf(tst,L"%s'%s",L->Time->Name,L->Time->Name);
 switch(ReadStr(0)){
  case '-': U=ReadForm(7); if(U.C==Pust) return U; F=-U; break;
  case '(':
   KS++; F=ReadForm(0); if(F.C==Pust) return F;
   break;
  case '[': KKS++; F=ReadForm(0); if(F.C==Pust) return F; break;
  case NAME:
//   if(B!='('){
//    if(Shtr=wcschr(S,'[')){
//     *Shtr='\0';
//     Arra *Arr=TakeArra(S);
//     F.a=TakeArrI(Arr,StrToForm(Shtr+1));
//     *Shtr='[';
//     return F;
//    }
//    if(FindCnst(S,&C)) F.C=C;
//    else{
//     for(Shtr=S;Shtr=wcschr(Shtr+1,'\'');*Shtr='\''){
//      *Shtr='\0'; VV=TakeVary(S); if(!(VV->Line)) VV->Line=Line;
//     }
//     F.V=TakeVary(S); if(!(F.V->Line)) F.V->Line=Line;
//   }}
   if(B!='('){
	if(FindCnst(S,&C)) F.C=C;
	else if(FindArra(S,&A)) F.A=A;
	else{ bool IsArra=false; //02.02.2019
	 for(Shtr=S;Shtr=wcschr(Shtr+1,'\'');*Shtr='\''){
	  *Shtr='\0';
	  if((wcsstr(Shtr+1,tst)==(Shtr+1))||
		 (wcsstr(Shtr+1,L->Time->Name)==(Shtr+1))){
	   if(IsArra||FindArra(S,&A)){//02.02.2019
		if(IsArra)                //02.02.2019
		 A=TakeArra(S);           //02.02.2019
		if(!A->Line) A->Line=Line;//02.02.2019
		IsArra=true;              //02.02.2019
	   }                          //02.02.2019
	   else{
		VV=TakeVary(S);
		if(!(VV->Line)) VV->Line=Line;
	 }}}
	 if(IsArra)                   //02.02.2019
	  F.A=TakeArra(S);            //02.02.2019
	 else                         //02.02.2019
	  F.V=TakeVary(S);
   }}
   else{
	KS++;
	switch(Oper=Func(S)){
	 case  2:
	  U=ReadForm(0); if(U.C==Pust) return U; F=sqrt(U); break;
	 case  3:
	  U=ReadForm(0); if(U.C==Pust) return U; F=sin(U); break;
	 case  4:
	  U=ReadForm(0); if(U.C==Pust) return U; F=cos(U); break;
	 case  5:
	  U=ReadForm(0); if(U.C==Pust) return U; F=tg(U); break;
	 case  6:
	  U=ReadForm(0); if(U.C==Pust) return U; F=arcsin(U); break;
	 case  7:
	  U=ReadForm(0); if(U.C==Pust) return U; F=arctg(U); break;
	 case  8:
	  U=ReadForm(0); if(U.C==Pust) return U; F=exp(U); break;
	 case  9:
	  U=ReadForm(0); if(U.C==Pust) return U; F=ln(U); break;
	 case 10:
	  U=ReadForm(0); if(U.C==Pust) return U; F=sh(U); break;
	 case 11:
	  U=ReadForm(0); if(U.C==Pust) return U; F=ch(U); break;
	 case 12:
	  U=ReadForm(0); if(U.C==Pust) return U; F=arsh(U); break;
	 case 13:
	  U=ReadForm(0); if(U.C==Pust) return U; F=abs(U); break;
	 case 14:
	  U=ReadForm(0);
	  if(U.C==Pust) return U; F=rand(U); ISRAND=true; break;
	 case 15:
	  U=ReadForm(0); if(U.C==Pust) return U; F=ceil(U); break;
	 case 16:
	  U=ReadForm(0); if(U.C==Pust) return U; F=floor(U); break;
	 case 17:
	  U=ReadForm(0); if(U.C==Pust) return U; F=arccos(U); break;
	 default:
	  if(!wcscmp(S,L"ipl")){
	   if(ReadStr(0)<1) return F;
	   if(B==',') A=TakeArra(S);
	   U=ReadForm(0); if(U.C==Pust) return U;
	   F.I=TakeIpll(1,A,U);
	  }
	  else if(!wcscmp(S,L"spln")){
	   if(ReadStr(0)<1) return F;
	   if(B==',') A=TakeArra(S);
	   U=ReadForm(0); if(U.C==Pust) return U;
	   F.P=TakeSpln(1,A,U);
	  }
	  else if(!wcscmp(S,L"summ")){ int Rez;
	   FF=ReadForm(0); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		  if(B==','){
		 U=ReadForm(0); if(U.C==Pust) return U;
		   if(B==','){
		  V=ReadForm(0); if(V.C==Pust) return V;
		 }
		 F.S=TakeSumm(FF,VV,U,V); break;
	   }}
	   return F;
	  }
	  else if(!wcscmp(S,L"intg")){ int Rez;
	   FF=ReadForm(0); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		  if(B==','){
		 U=ReadForm(0); if(U.C==Pust) return U;
		   if(B==','){
		  V=ReadForm(0); if(V.C==Pust) return V;
		 }
		 F.G=TakeIntg(FF,VV,U,V); break;
		}
		else
		 F.G=TakeIntg(FF,VV);
	   }
	   return F;
	  }
	  else if(!wcscmp(S,L"root")){ int Rez;
	   FF=ReadForm(0); if(FF.C==Pust) return FF;
	   if(Rez=ReadStr(0),Rez<1) return F;
	   if(Rez==NAME){
		VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
		  if(B==','){
		 U=ReadForm(0); if(U.C==Pust) return U;
		   if(B==','){
		  V=ReadForm(0); if(V.C==Pust) return V;
		 }
		 F.R=TakeSolu(FF,VV,U,V); break;
		}
		else F.C=Pust;
	   }
	   return F;
	  }
	  else if(!wcscmp(S,L"rest")){
	   U=ReadForm(0); if(U.C==Pust) return U;
		 if(B==','){
		V=ReadForm(0); if(V.C==Pust) return V;
		F.O=TakeRest(U,V); break;
	   }
	   return F;
	  }
	  else{
	   swprintf(Inf,L"Неопознанная унарная операция");
	   Application->MessageBox(Inf,L"Ошибка!",MB_OK); F.C=Pust; return F;
   }} }
   break;
  case Long: case Double:
   if(IsLiters(S)){
	if(!FindCnst(S,&(F.C))){
	 swprintf(Inf,L"Если это число, то ожидалась цифра\n"
	  L"или символы: '.', 'E', 'e', ';'");
	 Application->MessageBox(Inf,L"Ошибка!",MB_OK);
	 F.C=Pust;
   }}
   else F.C=TakeCnst(S);
   break;
  case PUST: case -1:
   F.C=Pust;
   return F;
 }
 for(;;) switch(B){
  case '[':
   if(A=TakeArra(S)){
	F=ReadForm(0);
	F.a=TakeArrI(A,F);
	if(!ReadChar()) return F.C=Pust,F;
   }
   break;
  case ']':
   if(KKS){ KKS--; B=*sp++; while(B==' ') B=*sp++; break; }
   return F;
  case ')':
   if(KS){ KS--; B=*sp++; while(B==' ') B=*sp++; break; }
   return F;
  case  '?': case '!': return F;
  case  '=':
   if(B=*sp++,B=='=') goto L;
   wcscpy(Inf,
	L"Ожидалось: '+', '-', '*', '/', '%', '^',"
	L" '<', '>', '<=', '>=', '!=', '==', '<<', '>>',");
   Application->MessageBox(Inf,L"Ошибка!",MB_OK);
   F.C=Pust; return F;
  case  '<': case '>': case '\\':
L: if((Oper=LogZnac())>0){
	V=ReadForm(0);
	if(V.C==Pust) return V;
	if(B=='?'){
	 FT=ReadForm(0);
	 if(FT.C==Pust) return FT;
	}
	else{
	 swprintf(Inf,L"Ожидалось: '?'");
	 Application->MessageBox(Inf,L"Ошибка!",MB_OK);
	 F.C=Pust; return F;
	}
	if(B=='!'){
	 FF=ReadForm(0);
	 if(FF.C==Pust) return FF;
	}
	else{ String Su,Sv,St,S;
	 FF.C=Pust;
	 inf+=swprintf(inf=Inf,L"Тернарное предложение в строке %d:\n\n",Line);
	 FormToStr(F,&Su,true);
	 FormToStr(V,&Sv,true);
	 FormToStr(FT,&St,true);
	 S=Su;
     switch(Oper){
	  case 1: S+=L" < ";  break; case 2: S+=L" > ";  break;
	  case 3: S+=L" <= "; break; case 4: S+=L" >= "; break;
	  case 5: S+=L" == "; break; case 6: S+=L" != "; break;
	  case 7: S+=L" << "; break; case 8: S+=L" >> "; break;
	 }
     S+=Sv + L" ? " + St + L";"; inf+=swprintf(inf,S.c_str());
     swprintf(inf,L"\n\nне завершено!"); ISINCOMPLETEIF=true;
     Application->MessageBox(Inf,L"Внимание!",MB_OK);
    }
    F.L=TakeIfop(Oper,F,V,FT,FF); return F;
   }
  default:
   if(Oper=Znac(B)){
	if(Prio>=Oper) return F;
	switch(Oper){
	 case  1: U=ReadForm(Oper); if(U.C==Pust) return U; F=F+U; break;
	 case  2: U=ReadForm(Oper); if(U.C==Pust) return U; F=F-U; break;
	 case  3: U=ReadForm(Oper); if(U.C==Pust) return U; F=F*U; break;
	 case  4: U=ReadForm(Oper); if(U.C==Pust) return U; F=F/U; break;
	 case  5: U=ReadForm(Oper); if(U.C==Pust) return U; F=F%U; break;
	 case  6: U=ReadForm(Oper); if(U.C==Pust) return U; F=F^U; break;
	 default:
	  swprintf(Inf,L"Неопознанный символ - '%c'",B);
	  Application->MessageBox(Inf,L"Ошибка!",MB_OK); F.C=Pust; return F;
   }}
   else return F;
}}
//------------------------------------------------------------------------
Form StrToForm(wchar_t *Sp){
 Form F; int Kl,Kp; wchar_t Buf[80]=L"Нарушен баланс скобок",*spOld=sp;
sp=Sp;
 if(Sp&&*Sp){
  Kl=KSimbStr(Sp,'('); Kp=KSimbStr(Sp,')');
  if(Kl!=Kp){
   if(Kl>Kp) swprintf(Inf,L"Левых скобок больше, чем правых:\n%s",Sp);
   else      swprintf(Inf,L"Правых скобок больше, чем левых:\n%s",Sp);
   Application->MessageBox(Inf,Buf,MB_OK); IsError=true; F.C=Pust;
  }
  else F=ReadForm(0);
 }
 sp=spOld;
 return F;
}
//------------------------------------------------------------------------
int FormHeight(Form F,TCanvas *Canvas){
 int hf,Hf,Size,h;
 switch(F.C->Atr){
  case PUST:
   hf=Canvas->TextHeight(L"'$'"); break;
  case CNST:
   hf=Canvas->TextHeight(F.C->Val); break;
  case 4:
   hf=Canvas->TextHeight(F.V->Name); break;
  case UNOP:
   hf=FormHeight(F.U->Arg,Canvas);
   switch(F.U->Func){
    case 2:
     hf*=1.5;
     break; //корень
    case 8: Hf=Canvas->TextHeight(L"M"); hf+=2*Hf/3; break; //экспонента
    default:
     Size=Canvas->Font->Size;
     while((Hf=Canvas->TextHeight(L"M"))<hf) Canvas->Font->Size++;
     if(Hf > hf) hf=Hf;
     Canvas->Font->Size=Size;
   }
   break;
  case BNOP:
   hf=FormHeight(F.B->L,Canvas); Hf=FormHeight(F.B->R,Canvas);
   switch(F.B->Oper){
    case 4://деление
     if(hf>Hf) hf+=hf; else hf=2*Hf;
     hf+=Round(0.5*Canvas->TextHeight(L"M"));
     break;
    case 6://в степень
     Size=Canvas->Font->Size;
     Canvas->Font->Size=Round(2.0*Canvas->Font->Size/3.0);
     Hf=FormHeight(F.B->R,Canvas); hf+=Hf;
     Canvas->Font->Size=Size;
     break;
    default:
     if(Hf>hf) hf=Hf; break;
   }
   break;
  case IFOP:
   hf=FormHeight(F.L->U,Canvas);
   Hf=FormHeight(F.L->V,Canvas);  if(Hf>hf) hf=Hf;
   Hf=FormHeight(F.L->FT,Canvas); if(Hf>hf) hf=Hf;
   Hf=FormHeight(F.L->FF,Canvas); if(Hf>hf) hf=Hf;
   Size=Canvas->Font->Size;
   while(Canvas->TextHeight(L"M")<hf)
    Canvas->Font->Size++;
   if(Canvas->TextHeight(L"?") > hf) hf=Canvas->TextHeight(L"?");
   if(Canvas->TextHeight(L"!") > hf) hf=Canvas->TextHeight(L"!");
   Canvas->Font->Size=Size;
   break;
  default:
   Application->MessageBoxA(L"Объект формулы не обрабатывается",
	L"Сообщение",MB_OK);
 }
 return hf;
}
//---------------------------------------------------------------------------
int FormWidth(Form F,TCanvas *Canvas){
 int df=0,dFL,dFR,Size;//df-ширина формулы а-отступ по вертикали
 switch(F.C->Atr){
  case PUST: df+=Canvas->TextWidth(L"'$'"); break;
  case CNST:
    if(F.C->Name){
    if(wcsstr(F.C->Name,L"PI")){
     String NameFont=Canvas->Font->Name;
     Canvas->Font->Name=L"Symbol";
     if(!wcscmp(F.C->Name,L"2PI"))         df+=Canvas->TextWidth(L"2\x70");
     else if(!wcscmp(F.C->Name,L"3PI"))    df+=Canvas->TextWidth(L"3\x70");
     else if(!wcscmp(F.C->Name,L"4PI"))    df+=Canvas->TextWidth(L"4\x70");
     else if(!wcscmp(F.C->Name,L"5PI"))    df+=Canvas->TextWidth(L"5\x70");
     else if(!wcscmp(F.C->Name,L"6PI"))    df+=Canvas->TextWidth(L"6\x70");
     else if(!wcscmp(F.C->Name,L"7PI"))    df+=Canvas->TextWidth(L"7\x70");
     else if(!wcscmp(F.C->Name,L"PI"))     df+=Canvas->TextWidth(L"\x70");
     else if(!wcscmp(F.C->Name,L"PIna2"))  df+=Canvas->TextWidth(L"\x70/2");
     else if(!wcscmp(F.C->Name,L"PIna3"))  df+=Canvas->TextWidth(L"\x70/3");
     else if(!wcscmp(F.C->Name,L"PIna4"))  df+=Canvas->TextWidth(L"\x70/4");
     else if(!wcscmp(F.C->Name,L"PIna6"))  df+=Canvas->TextWidth(L"\x70/6");
     else if(!wcscmp(F.C->Name,L"PIna8"))  df+=Canvas->TextWidth(L"\x70/8");
     else if(!wcscmp(F.C->Name,L"PIna10")) df+=Canvas->TextWidth(L"\x70/10");
     else if(!wcscmp(F.C->Name,L"PIna12")) df+=Canvas->TextWidth(L"\x70/12");
     Canvas->Font->Name=NameFont;
    }
    else df+=LengthGrek(Canvas,F.C->Name,SimbInd);
   }
   else  df+=Canvas->TextWidth(F.C->Val);
   break;
  case 4: df+=LengthGrek(Canvas,F.V->Name,SimbInd);; break;
  case UNOP:
   switch(F.U->Func) {
    case 1://унарный минус
     df+=Canvas->TextWidth(L"-");
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 ){
     // скобки ставим когда бинарная операция L"+" или L"-"
      Size=Canvas->Font->Size;
//      while (Img->Canvas->TextHeight(L"M")<h) Img->Canvas->Font->Size++;
      df+=Canvas->TextWidth(L"(");
      Canvas->Font->Size=Size;
     }
     break;
    case 2://корень
     df+=FormWidth(F.U->Arg,Canvas);
     dFR=FormHeight(F.U->Arg,Canvas);
     df+=Round(dFR*(3.0*tan(M_PI/6.0)+1.0/6.0));
     break;
    case 8: // exp
     break;
    default:
     df+=Canvas->TextWidth(NameFunc[F.U->Func]);
     Size=Canvas->Font->Size;
     df+=Canvas->TextWidth(L"(");
     Canvas->Font->Size=Size;
     break;
   }
   df+=FormWidth(F.U->Arg,Canvas);
   switch(F.U->Func) {
    case 2:
    break;
    case 8:
    break;
    case 1://унарный минус
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 )
     // скобки ставим когда бинарная операция L"+" или L"-"
      df+=Canvas->TextWidth(L")");
     break;
    default:
     df+=Canvas->TextWidth(L")");
     break;
   }
   break;
  case BNOP:
   switch(F.B->Oper){
    case 4://деление
     dFL=FormWidth(F.B->L,Canvas);
     dFR=FormWidth(F.B->L,Canvas);
     df=dFL>dFR ? dFL : dFR;
     break;
    case 6:
     Size=Canvas->Font->Size;
     Canvas->Font->Size=3*Canvas->Font->Size/4;
     Canvas->Font->Size=Size;
     df+=FormWidth(F.B->L,Canvas);
     Canvas->Font->Size=3*Canvas->Font->Size/4;
     df+=FormWidth(F.B->R,Canvas);
     Canvas->Font->Size=Size;
     break;
    default:
     df+=FormWidth(F.B->L,Canvas);
     if(F.B->Oper!=3) swprintf(Inf,L" %c ",ZNAC[F.B->Oper]);
     else if(!IsCnst(F.B->L)) swprintf(Inf,L"."); //умножение
     else Inf[0]='\0';
     df+=Canvas->TextWidth(Inf);
     df+=FormWidth(F.B->R,Canvas);
   }
   break;
  case IFOP:
   df+=FormWidth(F.L->U,Canvas);
   switch(F.L->Oper) {
    case 1: swprintf(Inf,L" < ");  break;
    case 2: swprintf(Inf,L" > ");  break;
    case 3: swprintf(Inf,L" <= "); break;
    case 4: swprintf(Inf,L" >= "); break;
    case 5: swprintf(Inf,L" \= "); break;
    case 6: swprintf(Inf,L" == "); break;
    case 7: swprintf(Inf,L" << "); break;
    case 8: swprintf(Inf,L" >> "); break;
   }
   df+=Canvas->TextWidth(Inf);
   df+=FormWidth(F.L->V,Canvas);
   swprintf(Inf,L" ? ");
   df+=Canvas->TextWidth(Inf);
   df+=FormWidth(F.L->FT,Canvas);
   swprintf(Inf,L" ! ");
   df+=Canvas->TextWidth(Inf);
   df+=FormWidth(F.L->FF,Canvas);
   break;
 }
 return df;
}
//---------------------------------------------------------------------------
int DrawForm(Form F,TCanvas *Canvas,int X,int Y){
 int df=0,dFL,dFR,Size,h,a,hl,hr,al,ar,ht,hF,wArg,hArg,dY;//df-ширина формулы
 switch(F.C->Atr){
  case PUST:
   df+=Canvas->TextWidth(L"'$'");
   Canvas->TextOut(X,Y,L"'$'");
   break;
  case CNST:
   if (F.C->Name){
//    df+=Canvas->TextWidth(F.C->Name);
    df+=LengthGrek(Canvas,F.C->Name,SimbInd);
//    Canvas->TextOut(X,Y,F.C->Name);
    if(wcsstr(F.C->Name,L"PI")){
     String NameFont=Canvas->Font->Name;
     Canvas->Font->Name=L"Symbol"; dY=Round(0.15*Canvas->Font->Size); 
Y-=dY;
     if(!wcscmp(F.C->Name,L"2PI"))         Canvas->TextOut(X,Y,L"2\x70");
     else if(!wcscmp(F.C->Name,L"3PI"))    Canvas->TextOut(X,Y,L"3\x70");
     else if(!wcscmp(F.C->Name,L"4PI"))    Canvas->TextOut(X,Y,L"4\x70");
     else if(!wcscmp(F.C->Name,L"5PI"))    Canvas->TextOut(X,Y,L"5\x70");
     else if(!wcscmp(F.C->Name,L"6PI"))    Canvas->TextOut(X,Y,L"6\x70");
     else if(!wcscmp(F.C->Name,L"7PI"))    Canvas->TextOut(X,Y,L"7\x70");
     else if(!wcscmp(F.C->Name,L"PI"))     Canvas->TextOut(X,Y,L"\x70");
     else if(!wcscmp(F.C->Name,L"PIna2"))  Canvas->TextOut(X,Y,L"\x70/2");
     else if(!wcscmp(F.C->Name,L"PIna3"))  Canvas->TextOut(X,Y,L"\x70/3");
     else if(!wcscmp(F.C->Name,L"PIna4"))  Canvas->TextOut(X,Y,L"\x70/4");
     else if(!wcscmp(F.C->Name,L"PIna6"))  Canvas->TextOut(X,Y,L"\x70/6");
     else if(!wcscmp(F.C->Name,L"PIna8"))  Canvas->TextOut(X,Y,L"\x70/8");
     else if(!wcscmp(F.C->Name,L"PIna10")) Canvas->TextOut(X,Y,L"\x70/10");
     else if(!wcscmp(F.C->Name,L"PIna12")) Canvas->TextOut(X,Y,L"\x70/12");
     Canvas->Font->Name=NameFont;
    }
    else DrawGrekText(Canvas,X,Y,0,F.C->Name,SimbInd);
   }
   else{
    df+=Canvas->TextWidth(F.C->Val);
    Canvas->TextOut(X,Y,F.C->Val);
   }
   break;
  case 4:
   df+=LengthGrek(Canvas,F.V->Name,SimbInd);
   DrawGrekText(Canvas,X,Y,0,F.V->Name,SimbInd);
   break;
  case UNOP:
   h=FormHeight(F.U->Arg,Canvas);
   a=Round(0.5*(h-Canvas->TextHeight(L"M")));
   switch(F.U->Func) {
    case 1://унарный минус
	 df+=Canvas->TextWidth(L"–");
     Canvas->TextOut(X,Y+a,L"–");
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 ){
     // скобки ставим когда бинарная операция L"+" или L"-"
      Size=Canvas->Font->Size;
      while(Canvas->TextHeight(L"M")<h) Canvas->Font->Size++;
	  df+=Canvas->TextWidth(L"(");
      Canvas->TextOut(X+df,Y,L"(");
      Canvas->Font->Size=Size;
     }
     df+=DrawForm(F.U->Arg,Canvas,X+df,Y);
     break;
    case 2://корень квадратный
     wArg=FormWidth(F.U->Arg,Canvas);
     hArg=FormHeight(F.U->Arg,Canvas);
     a=Round(3.0*hArg*tan(M_PI/18.0)/4.0);
     al=Round(3.0*hArg/4.0);
     Canvas->Pen->Width=2;
     Canvas->Pen->Color=Canvas->Font->Color;
     Canvas->MoveTo(X,Y+al); Canvas->LineTo(X+a,Y+al);
     Canvas->LineTo(X+2*a,Y+2*al); Canvas->LineTo(X+4*a,Y);
     Canvas->LineTo(X+4*a+wArg,Y);
     df+=wArg+4*a+Round(hArg/6.0);
     al=DrawForm(F.U->Arg,Canvas,X+4*a+Round(hArg/12.0),Y+Round
(hArg/6.0));
     break;
    case 8: // exp
     break;
    default:
     df+=Canvas->TextWidth(NameFunc[F.U->Func]);
     Canvas->TextOut(X,Y+a,NameFunc[F.U->Func]);
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 ){
      Size=Canvas->Font->Size;
      while(Canvas->TextHeight(L"M")<h) Canvas->Font->Size++;
	  Canvas->TextOut(X+df,Y,L"(");
      df+=Canvas->TextWidth(L"(");
      Canvas->Font->Size=Size;
     }
     else{
      df+=Canvas->TextWidth(L" ");
      Canvas->TextOut(X+df,Y,L" ");
     }
     df+=DrawForm(F.U->Arg,Canvas,X+df,Y);
     break;
   }
   switch(F.U->Func) {
    case 2:
    break;
    case 8:
    break;
    case 1://унарный минус
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 ){
     // скобки ставим когда бинарная операция L"+" или L"-"
      Canvas->TextOut(X+df,Y,L")");
      df+=Canvas->TextWidth(L")");
     }
     break;
    default:
     if(F.U->Arg.C->Atr==BNOP && F.U->Arg.B->Oper<3 ){
      Canvas->TextOut(X+df,Y,L")");
      df+=Canvas->TextWidth(L")");
     }
     break;
   }
   break;
  case BNOP:
   hl=FormHeight(F.B->L,Canvas);
   switch(F.B->Oper){
    case 4://деление
     hr=Canvas->TextHeight(L"M");
     Canvas->Pen->Color=Canvas->Font->Color;
     Canvas->MoveTo(X,Y+hl+Round(0.25*hr));
     dFL=FormWidth(F.B->L,Canvas);
     dFR=FormWidth(F.B->R,Canvas);
     df=dFL>dFR ? dFL : dFR;
     Canvas->LineTo(X+df,Y+hl+Round(0.25*hr));
     DrawForm(F.B->L,Canvas,X+(df-dFL)/2,Y);
     DrawForm(F.B->R,Canvas,X+(df-dFR)/2,Y+hl+0.25*hr);
     break;
    case 6:
     Size=Canvas->Font->Size;
     Canvas->Font->Size=Round(2.0*Canvas->Font->Size/3.0);
     hr=FormHeight(F.B->R,Canvas);
     Canvas->Font->Size=Size;
     al=Round(0.5*hr);
     df+=DrawForm(F.B->L,Canvas,X+df,Y+al);
     Canvas->Font->Size=Round(2.0*Canvas->Font->Size/3.0);
     df+=DrawForm(F.B->R,Canvas,X+df,Y);
     Canvas->Font->Size=Size;
     break;
    default:
     hr=FormHeight(F.B->R,Canvas);
     if(hl>hr) h=hl;
     else h=hr;
     al=Round((h-hl)/2); ar=Round((h-hr)/2);
     df+=DrawForm(F.B->L,Canvas,X+df,Y+al);
     if(F.B->Oper!=3){
      swprintf(Inf,L" %c ",F.B->Oper==2?'–':ZNAC[F.B->Oper]);
//      Canvas->TextOut(X+df,Y+(al+ar)/2,Inf);
      Canvas->TextOut(X+df,Y+Round(0.5*(h-Canvas->TextHeight(L"M"))),Inf);
      df+=Canvas->TextWidth(Inf);
     }
     else if(!IsCnst(F.B->L)){ //умножение
      swprintf(Inf,L".");
      Canvas->TextOut(X+df,Y+Round(0.5*h)-Round(0.75*Canvas->TextHeight
(L"M")),Inf);
      df+=Canvas->TextWidth(Inf);
     }
     df+=DrawForm(F.B->R,Canvas,X+df,Y+ar);
   }
   break;
  case IFOP:
   hl=FormHeight(F.L->U,Canvas);
   hr=FormHeight(F.L->V,Canvas);
   h=hl>hr ? hl : hr;
   ht=FormHeight(F.L->FT,Canvas);
   if(ht>h) h=ht;
   hF=FormHeight(F.L->FF,Canvas);
   if(hF>h) h=hF;
   a=h-hl;
   df+=DrawForm(F.L->U,Canvas,X+df,Y+a);
   switch(F.L->Oper) {
    case 1: swprintf(Inf,L" < ");  break;
    case 2: swprintf(Inf,L" > ");  break;
    case 3: swprintf(Inf,L" <= "); break;
    case 4: swprintf(Inf,L" >= "); break;
    case 5: swprintf(Inf,L" \= "); break;
    case 6: swprintf(Inf,L" == "); break;
    case 7: swprintf(Inf,L" << "); break;
	case 8: swprintf(Inf,L" >> "); break;
   }
   a=h-Canvas->TextHeight(Inf);
   Canvas->TextOut(X+df,Y+a,Inf);
   df+=Canvas->TextWidth(Inf);
   a=h-hr;
   df+=DrawForm(F.L->V,Canvas,X+df,Y+a);
   swprintf(Inf,L" ? ");
   a=h-Canvas->TextHeight(Inf);
   Canvas->TextOut(X+df,Y+a,Inf);
   df+=Canvas->TextWidth(Inf);
   a=h-ht;
   df+=DrawForm(F.L->FT,Canvas,X+df,Y+a);
   a=h-Canvas->TextHeight(Inf);
   if(F.L->FF.C!=Pust){
    swprintf(Inf,L" ! ");
    df+=Canvas->TextWidth(Inf);
    Canvas->TextOut(X+df,Y+a,Inf);
    a=h-hF;
    df+=DrawForm(F.L->FF,Canvas,X+df,Y+a);
   }
   break;
  //default:
 }
 return df;
}
//---------------------------------------------------------------------------
int *ListLength(Parm *P,int *l,int Ln,int Ind,int &k,TCanvas *Canvas){
 for(Parm *p=P;p;p=p->Sled){
  if(p->Line>Ln){
   Ln++; Ind++; k++; l=(int *)realloc(l,k*SzI); l[k-1]=0;
  }
  if(p->Nam){
    l[k-1]+=LengthGrek(Canvas,p->Nam,SimbInd);
   if(p->R){
    l[k-1]+=Canvas->TextWidth(L"(");
    l=ListLength(p->R,l,Ln,Ind,k,Canvas);
  }}
  else{
   swprintf(Inf,L"%g",p->Val); l[k-1]+=Canvas->TextWidth(Inf);
  }
  l[k-1]+=Canvas->TextWidth(p->Sled?", L":")");
 }
 return l;
}
//---------------------------------------------------------------------------
int TImageUnit::Width(TCanvas *Canvas,TSizesLines *SL){
 int hh,hp,Blank=2,Ln=Line,Ind=Line-1,k=1,*l,i=0; wchar_t *S;
 switch(Atr){
  case TEXTCOMM:
   wcscpy(S=Inf,UnitKDM.T->S.c_str());
   for(wchar_t *s=S;*s;s++){
    if(*s=='\n'){
     *s='\0';   SL[Ind+i].l+=LengthGrek(Canvas,S,SimbInd); S=s+1; i++;
   }}
     SL[Ind+i].l+=LengthGrek(Canvas,S,SimbInd);
   break;
  case VARY:
   L=FormWidth(UnitKDM.V->Znach,Canvas);
   L+=Canvas->TextWidth(L" = ");
   L+= LengthGrek(Canvas,UnitKDM.V->Name,SimbInd);
   L+=Blank*Canvas->TextWidth(L" ");
   SL[Ind].l+=L;
   break;
  case ELEM:
   L=FormWidth(UnitKDM.E->Znach,Canvas);
   L+=Canvas->TextWidth(L" = ");
   L+=LengthGrek(Canvas,UnitKDM.E->K->Name,SimbInd);
   L+=Canvas->TextWidth(L".");
   L+=LengthGrek(Canvas,UnitKDM.E->K->Name,SimbInd);
   L+=Blank*Canvas->TextWidth(L" ");
   SL[Ind].l+=L;
   break;
  case ROUT:
   l=(int *)calloc(1,SzI);
   l[0]=LengthGrek(Canvas,UnitKDM.R->Name,SimbInd);
   l[0]+=Canvas->TextWidth(L" := ");
   l[0]+=Blank*Canvas->TextWidth(L" ");
   if(UnitKDM.R->First)
    l=ListLength(UnitKDM.R->First,l,Ln,Ind,k,Canvas);
   l[k-1]+=Canvas->TextWidth(L";");
   SL[Ind].l+=(L=l[0]);
   for(int i=1;i<k;i++){
    SL[i+Ind].l=l[i];
    if(l[i]>L) L=l[i];
   }
   free(l);
   break;
  case ARRA:
   L=LengthGrek(Canvas,UnitKDM.A->Name,SimbInd);
   L+=Canvas->TextWidth(L" [ ");
   swprintf(Inf,L"%d",UnitKDM.A->N);
   L+=Canvas->TextWidth(Inf);
   L+=Canvas->TextWidth(L" ] ");
   L+=Canvas->TextWidth(L" = ");
   L+=FormWidth(UnitKDM.A->F[0],Canvas);
   for(int i=1;i<UnitKDM.A->N;i++){
    L+=Canvas->TextWidth(L", ");
    L+=FormWidth(UnitKDM.A->F[i],Canvas);
   }
   L+=Canvas->TextWidth(L";");
   SL[Ind].l+=L;
   break;
 }
 return L;
}
//---------------------------------------------------------------------------
int *ListHeight(Parm *P,int *h,int Ln,int Ind,int &k,TCanvas *Canvas){
 for(Parm *p=P;p;p=p->Sled){
  if(p->Line>Ln){
   Ln++; Ind++; k++; h=(int *)realloc(h,k*SzI); h[k-1]=0;
  }
  if (p->Nam){
   if(h[k-1]<Canvas->TextHeight(p->Nam))
    h[k-1]=Canvas->TextHeight(p->Nam);
   if(p->R)
    h=ListHeight(p->R,h,Ln,Ind,k,Canvas);
  }
  else{
   swprintf(Inf,L"%g",p->Val);
    if(h[k-1]< Canvas->TextHeight(Inf))
     h[k-1]=Canvas->TextHeight(Inf);
 }}
 return h;
}
//---------------------------------------------------------------------------
int TImageUnit::Height(TCanvas *Canvas,TSizesLines *SL){
 int hh,hp,Ln=Line,Ind=Line-1, k=1,*h; wchar_t *S;
 switch(Atr){
  case VARY:
   H=FormHeight(UnitKDM.V->Znach,Canvas);
   if(Canvas->TextHeight(UnitKDM.V->Name) > H)
    H=Canvas->TextHeight(UnitKDM.V->Name);
   if(SL[Ind].h<H) SL[Ind].h=H;
   break;
  case ELEM:
   H=FormHeight(UnitKDM.E->Znach,Canvas);
   if(Canvas->TextHeight(UnitKDM.E->Name) > H)
    H=Canvas->TextHeight(UnitKDM.E->Name);
   if(SL[Ind].h<H) SL[Ind].h=H;
   break;
  case ROUT:
   h=(int *)calloc(1,SzI);
   h[0]=Canvas->TextHeight(UnitKDM.R->Name);
    if(UnitKDM.R->First)
    h=ListHeight(UnitKDM.R->First,h,Ln,Ind,k,Canvas);
   H=h[0];
   if(h[0]>SL[Ind].h) SL[Ind].h=h[0];
   for(int i=1;i<k;i++){
    SL[i+Ind].h=h[i];
    H+=h[i];
   }
   free(h);
   break;
  case ARRA:
   H=Canvas->TextHeight(UnitKDM.A->Name);
   hh=Canvas->TextHeight(UnitKDM.A->N);
   if(hh>H) H=hh;
   if(Canvas->TextHeight(L"[") > H) H=Canvas->TextHeight(L"[");
   if(SL[Ind].h<H) SL[Ind].h=H;
   break; //доделать
 }
 return H;
}
//---------------------------------------------------------------------------
//void __fastcall TImageKDM::AreaSizes(TImage *Img, int &maxH,int &maxL){
void __fastcall TImageKDM::AreaSizes(TCanvas *Canvas, int &maxH,int
&maxL)  {
 int i,str,HM=Canvas->TextHeight(L"M");
 maxL = 0; maxH = 0;
//последнюю инструкцию РАСЧЕТ печатать не надо, поэтому уберем ее из списка:
 for(r=R; r->Sled->Sled; r=r->Sled); r->Sled=NULL;
//прочитаем номер последней строки:
 //for(r=R; r->Sled; r=r->Sled);
 str=r->Line;
//выделим память под массив информации о строках:
 SL=(TSizesLines *)calloc(str,sizeof(TSizesLines));
//дадим строкам номера и минимальную высоту, исходя из размера шрифта:
 for(i=0; i<str; i++){ SL[i].ind=i+1; SL[i].h=HM; }
//заполним для непустых строк требуемую высоту и ширину
 for(r=R; r; r=r->Sled){
  i=r->Line-1;
  r->Width(Canvas,SL);
  r->Height(Canvas,SL);
//  if(H>SL[i].h) SL[i].h=H; SL[i].l+=L;
 }
// определяем максимальную ширину и высоту
 maxL = SL[0].l;
 maxH = SL[str-1].h;
 for(i=1; i<str; i++){
  if(maxL < SL[i].l)
   maxL = SL[i].l;
  maxH += SL[i-1].h;
 }
 maxL+=MarginLeft;
}
//---------------------------------------------------------------------------
void TImageKDM::VaryHeight(TCanvas *Canvas, int &H){
 for(Vary *V=L->V; V; V=V->Sled){
  H+=max(FormHeight(V->Znach,Canvas),Canvas->TextHeight(V->Name));
}}
//---------------------------------------------------------------------------
void TImageKDM::Draws(TPaintBox *PaintBox){
 int a,X=MarginLeft,Line=R->Line;// d-ширина обьектов
 for(r=R; r; r=r->Sled){
  if(SL[r->Line-1].y>PaintBox->ClientRect.Height()) break;
  a=Round(0.5*(SL[r->Line-1].h-r->H)); if(a<0) a=0;
  if(r->Line!=Line){ Line=r->Line; X=MarginLeft; }
  X=r->Draw(PaintBox->Canvas,X,SL[r->Line-1].y+a,SL,MarginLeft);
}}
//---------------------------------------------------------------------------
int TImageUnit::Draw(TCanvas *Canvas,int X,int Ya,TSizesLines *SL,int 
MarginLeft){
// возвращаем ширину обьекта и рисуем в конце каждого точка с запятой и пробел
 int h,b=0,c,Line; // b,c - отступы по вертикали (имя,формула)
 switch(Atr){
  case VARY:
   h=FormHeight(UnitKDM.V->Znach,Canvas);
   b=Round(0.5*(h-Canvas->TextHeight(UnitKDM.V->Name)));
   DrawGrekText(Canvas,X,Ya+b,0,UnitKDM.V->Name,SimbInd);
   X+=LengthGrek(Canvas,UnitKDM.V->Name,SimbInd);
   Canvas->TextOut(X,Ya+b,L" = ");
   X+=Canvas->TextWidth(L" = ");
   X+=DrawForm(UnitKDM.V->Znach,Canvas,X,Ya);
   Canvas->TextOut(X,Ya+b,L";");
   X+=Canvas->TextWidth(L";");
   for(int i=0;i<Kblank;i++){
    Canvas->TextOut(X,Ya+b,L" "); X+=Canvas->TextWidth(L" ");
   }
   break;
  case ELEM:
   h=FormHeight(UnitKDM.E->Znach,Canvas);
   b=h-Canvas->TextHeight(UnitKDM.E->Name);
   Canvas->TextOut(X,Ya+b,UnitKDM.E->Name);
   X+=LengthGrek(Canvas,UnitKDM.E->Name,SimbInd);
   Canvas->TextOut(X,Ya+b,L".");
   X+=Canvas->TextWidth(L".");
   b=h-Canvas->TextHeight(UnitKDM.E->K->Name);
   DrawGrekText(Canvas,X,Ya+b,0,UnitKDM.E->K->Name,SimbInd);
   X+=LengthGrek(Canvas,UnitKDM.E->K->Name,SimbInd);
   Canvas->TextOut(X,Ya+b,L" = ");
   X+=Canvas->TextWidth(L" = ");
   X+=DrawForm(UnitKDM.E->Znach,Canvas,X,Ya);
   Canvas->TextOut(X,Ya+b,L";");
   X+=Canvas->TextWidth(L";");
   for(int i=0;i<Kblank;i++){
    Canvas->TextOut(X,Ya+b,L" "); X+=Canvas->TextWidth(L" ");
   }
   break;
  case ROUT:
   h=Canvas->TextHeight(UnitKDM.R->Name);
   DrawGrekText(Canvas,X,Ya,0,UnitKDM.R->Name,SimbInd);
   X+=LengthGrek(Canvas,UnitKDM.R->Name,SimbInd);
   Canvas->TextOut(X,Ya+b,L" := ");
   X+=Canvas->TextWidth(L" := ");
   Line=UnitKDM.R->First->Line;
   for(Parm *p=UnitKDM.R->First; p; p=p->Sled) {
    if(p->Line!=Line){
     for(int i=Line;i<p->Line;i++) Ya+=SL[i-1].h;
     Line=p->Line; X=2*MarginLeft;
    }
    if(p->Nam){
      DrawGrekText(Canvas,X,Ya+b,0,p->Nam,SimbInd);
      X+=LengthGrek(Canvas,p->Nam,SimbInd);
     if (p->R){
	  Canvas->TextOut(X,Ya+b,L"(");
      X+=Canvas->TextWidth(L"(");
      for(Parm *pp=p->R; pp; pp=pp->Sled) {
       if(pp->Nam){
        DrawGrekText(Canvas,X,Ya+b,0,pp->Nam,SimbInd);
        X+=LengthGrek(Canvas,pp->Nam,SimbInd);
       }
       else {
        Canvas->TextOut(X,Ya+b,pp->Val);
        X+=Canvas->TextWidth(pp->Val);
       }
       if (pp->Sled) {
        Canvas->TextOut(X,Ya+b,L", ");
        X+=Canvas->TextWidth(L", ");
      }}
      Canvas->TextOut(X,Ya+b,L")");
      X+=Canvas->TextWidth(L")");
    }}
    else {
     Canvas->TextOut(X,Ya+b,p->Val);
     X+=Canvas->TextWidth(p->Val);
    }
    if (p->Sled) {
     Canvas->TextOut(X,Ya+b,L", ");
     X+=Canvas->TextWidth(L", ");
   }}
   Canvas->TextOut(X,Ya+b,L";");
   X+=Canvas->TextWidth(L";");
   for(int i=0;i<Kblank;i++){
    Canvas->TextOut(X,Ya+b,L" "); X+=Canvas->TextWidth(L" ");
   }
   break;
  case TEXTCOMM:
   h=Canvas->TextHeight(UnitKDM.T->S);
   b=Canvas->TextHeight(UnitKDM.T->S);
   Canvas->TextOut(X,Ya-b,UnitKDM.T->S);
  break;
 }
 return X;
}

//---------------------------------------------------------------------------
void Zamostity(TImage *Im,TPaintBox *Img){
 int l,H,h,L,Ki,Kj;
 wchar_t FileFon[256],Ext[16],*s; TRect Rd,Rs;
// TImage *Im=new TImage(FormImage); Im->Parent=FormImage;
// TImage *Im=new TImage(FormEditKDM->TabControl); Im->Parent=FormEditKDM->TabControl;
 Im->AutoSize=true; Im->Stretch=true; Im->Align=alNone;
 swprintf(FileFon,L"%s\\fon.jpg",WORKDIR);
 if(!_waccess(FileFon,0)&&(s=wcsrchr(FileFon,'.'))){
  wcscpy(Ext,s+1); LowToUpp(Ext);
  if(!wcscmp(Ext,L"BMP"))
   Im->Picture->LoadFromFile(FileFon);
  else if(!wcscmp(Ext,L"JPG")||!wcscmp(Ext,L"JPEG")){
   TJPEGImage *jp = new TJPEGImage();
   jp->LoadFromFile(FileFon);
   Im->Picture->Bitmap->Assign(jp);
   delete jp;
 }}
 H=Img->ClientRect.Height();
 h=Im->ClientRect.Height();
 Ki=H/h; if(H%h) Ki++;
 L=Img->ClientRect.Width();
 l=Im->ClientRect.Width();
 Kj=L/l; if(L%l) Kj++;
 Rd.Top=0; Rd.Bottom=Im->ClientHeight;
 Rs.Left=0; Rs.Top=0; Rs.Right=Im->ClientWidth; Rs.Bottom=Im->ClientHeight;
 for(int i=0;i<Ki;i++){
  Rd.Left=0; Rd.Right=Im->ClientWidth;
  for(int j=0;j<Kj;j++){
   Img->Canvas->CopyRect(Rd,Im->Canvas,Rs);
   Rd.Left+=Im->ClientWidth; Rd.Right+=Im->ClientWidth;
  }
  Rd.Top+=Im->ClientHeight; Rd.Bottom+=Im->ClientHeight;
 }
 Img->Canvas->Brush->Style=bsClear;
}
//---------------------------------------------------------------------------
void SetFontColor(TFont *Font,wchar_t *S){
 int Hex; wchar_t Buf[256];
 swprintf(Inf,L"0x%s",S);
 swscanf(S,L"%x",&Hex);
 Font->Color=(TColor)Hex;
}
//---------------------------------------------------------------------------
Root *TImageKDM::FindLastInst(wchar_t *Name){
 Root *Rez=NULL;
 for(r=R;r;r=r->Sled){
  if(r->Atr==ROUT)
   if(!wcscmp(r->UnitKDM.R->Name,Name))
    Rez=r->UnitKDM.R;
 }
 return Rez;
}
//---------------------------------------------------------------------------
void TImageKDM::DelInstCalc(Root *I_Sled){
 for(r=R;r&&r->Sled&&r->Sled->UnitKDM.R!=I_Sled;r=r->Sled);
 if(r->Sled->UnitKDM.R==I_Sled)
  free(r->Sled);
 r->Sled=NULL;
}
//---------------------------------------------------------------------------
void TImageKDM::DrawShow(TPaintBox *pbReport,int &y){
 int K; Root *I; TRect Rd,Rs; wchar_t Dir[256];
 //RichEditInp->Align=alClient;
 if(I=FindInstLast(L"РИСУНОК")){
  wchar_t *s,Ext[4]; K=KolElem(I->First);
  if(K==1){//???? ???????
   if(!wcsrchr(I->First->Nam,'.'))//по умолчанию - BMP
    wcscat(I->First->Nam,L".BMP");
   if(!wcsrchr(I->First->Nam,'/')&&
	  !wcsrchr(I->First->Nam,'\\')){//файл в рабочей папке
	wcscpy(Dir,CurrDir);
    if((s=wcsrchr(Dir,'/'))||(s=wcsrchr(Dir,'\\')))
     wcscpy(s+1,I->First->Nam);
    else swprintf(Dir,L"%s\\%s",I->First->Nam);
   }
   else//полное имя файла
    wcscpy(Dir,I->First->Nam);
   if(_waccess(Dir,0)){
	swprintf(Inf,TextFromFile(WORKDIR,L"EditKiDyM",&SLK,70),I->First->Nam);
	Application->MessageBox(Inf,
	 TextFromFile(WORKDIR,L"EditKiDyM",&SLK,53),MB_OK);
    I->First->Nam[0]='\0';
   }
   if(s=wcsrchr(Dir,'.')){
    wcscpy(Ext,s+1); LowToUpp(Ext);
    if(!wcscmp(Ext,L"BMP")||!wcscmp(Ext,L"JPG")||!wcscmp(Ext,L"JPEG")){ int S1,S2;
	 if(ImageVvod) delete ImageVvod;
      ImageVvod=new TImage(NULL); ImageVvod->Parent=NULL;
     ImageVvod->Align=alNone; ImageVvod->Visible=false;
     if(!wcscmp(Ext,L"BMP")){
      ImageVvod->Picture->LoadFromFile(I->First->Nam);
      Rd=ImageVvod->Canvas->ClipRect;
	  Rs=ImageVvod->Canvas->ClipRect;
//Вычислим абсциссы
      Rd.Left=(pbReport->Canvas->ClipRect.Width()-Rd.Width())/2;
      Rd.Right+=Rd.Left; Rd.Top=y; Rd.Bottom+=y; y+=Rd.Height();
	  pbReport->Canvas->CopyRect(Rd,ImageVvod->Canvas,
	   ImageVvod->Canvas->ClipRect);// (0,0,ImageVvod);// >Canvas->Draw(0,0,ImageVvod);
    }}
    else if(!wcscmp(Ext,L"JPG")||!wcscmp(Ext,L"JPEG")){
     TJPEGImage *jp = new TJPEGImage();
     jp->LoadFromFile(I->First->Nam);
     ImageVvod->Picture->Bitmap->Assign(jp);
     pbReport->Canvas->CopyRect(Rd,ImageVvod->Canvas,Rs);
     delete jp;
   }}
   return;
 }}
 else{//несколько рисунков
}}
//---------------------------------------------------------------------------
void TImageKDM::ToImage(TImage *Imz,TPaintBox *Img){
 int l,h,H,hh,b,x,y=0,str,hm=Img->Canvas->TextHeight(L"M"); Root *I;Vary *V;
 wchar_t Dir[256]; 
// RichEdit->Visible=false;
 Zamostity(Imz,Img);

 l=Img->Canvas->TextWidth(L"О Т Ч Е Т");
 SetFontColor(Img->Canvas->Font,L"99");
 Img->Canvas->Font->Style = TFontStyles()<<fsBold;
 x=Round(0.5*(Img->Width-l));
 Img->Canvas->TextOut(x,y,L"О Т Ч Е Т");
 y+=2*hm;
 if(I=FindLastInst(L"РАБОТА")){
//Application->MessageBoxA(I->First->Nam,L"Внимание",MB_OK);
  swprintf(Inf,L"по работе \"%s\"",I->First->Nam);
  l=LengthGrek(Img->Canvas,Inf,SimbInd);
  x=Round(0.5*(Img->Width-l));
  Img->Canvas->TextOut(x,y,Inf);
  y+=2*hm;
 }
 if((I=FindInstLast(L"ВЫПОЛНИЛ"))||(I=FindInstLast(L"ВЫПОЛНИЛА"))||
     (I=FindInstLast(L"ВЫПОЛНИЛИ"))){
  swprintf(Inf,L"%s: \"%s\"",I->Name,I->First->Nam);
  l=Img->Canvas->TextWidth(Inf);
  x=Round(0.5*(Img->Width-l));
  Img->Canvas->TextOut(x,y,Inf);
  y+=2*hm;
 }
//вывод рисунка
// Img->Canvas->TextOut(80,120,L"H");
 if(I=FindInstLast(L"РИСУНОК")){
  DrawShow(Img,y);
 }
//пункт 1
 SetFontColor(Img->Canvas->Font,L"9900");
 wcscpy(Inf,L"1. Исходные данные");
//  l=Img->Canvas->TextWidth(L"1.");
 Img->Canvas->TextOut(MarginLeft,y,Inf);
 y+=2*hm;
 Img->Canvas->Font->Style = TFontStyles()>> fsBold;
 SetFontColor(Img->Canvas->Font,L"99");

 for(r=R; r->Sled; r=r->Sled);
 str=r->Line;

 for(int i=0; i<str; i++){
  SL[i].y=y; y+=SL[i].h;
 }

 Draws(Img);

 //пункт 2
 hh=Img->Height;
 hh=hh+H;
 //Img->Height=H;
 SetFontColor(Img->Canvas->Font,L"9900");
 Img->Canvas->Font->Style = TFontStyles()<<fsBold;
 wcscpy(Inf,L"2. Формулы для переменных, не равных нулю");
 l=Img->Canvas->TextWidth(L"2.");
 x=Round((Img->Width-l)/2)+MarginLeft; y+=Img->Canvas->TextHeight(L"M");
 Img->Canvas->TextOut(MarginLeft,y,Inf);
 y+=2*hm;
 Img->Canvas->Font->Style = TFontStyles()>> fsBold;
 SetFontColor(Img->Canvas->Font,L"99");
 for(V=L->V;V;V=V->Sled){
  x=MarginLeft;
  h=FormHeight(V->Znach,Img->Canvas);
  b=Round(0.5*(h-Img->Canvas->TextHeight(V->Name)));
  Img->Canvas->Font->Style = TFontStyles()<< fsBold;
  DrawGrekText(Img->Canvas,x,y+b,0,V->Name,SimbInd);
  x+=LengthGrek(Img->Canvas,V->Name,SimbInd);
  Img->Canvas->Font->Style = TFontStyles()>>fsBold;
  Img->Canvas->TextOut(x,y+b,L" = ");
  x+=Img->Canvas->TextWidth(L" = ");
  x+=DrawForm(V->Znach,Img->Canvas,x,y);
  Img->Canvas->TextOut(x,y+b,L";");
  x+=Img->Canvas->TextWidth(L";");
  y+=h;
 }
 y+=h;
//авторские права
 Img->Canvas->Pen->Color=(TColor)8421504; Img->Canvas->Pen->Width=2;
 Img->Canvas->MoveTo(MarginLeft,y);
 Img->Canvas->LineTo(Img->Width-MarginLeft,y);
 swprintf(Inf,L"© %s",Autor);
 l=Img->Canvas->TextWidth(Inf);
 x=(Img->Width-l)/2;
 y+=Img->Canvas->TextHeight(Inf);
 Img->Canvas->Font->Color=(TColor)8421504;
 Img->Canvas->TextOut(MarginLeft,y,Inf);
}
//---------------------------------------------------------------------------


