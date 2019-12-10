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
#include "OtchetForm.h"
#include <stdio.h>

extern TFormOtchet *FormOtchet;
extern FILE *INFILE,*ERRFILE,*HTMFILE,*OPRFILE,*OUTFILE;
extern bool IsError,IsSyntError,ISRAND; extern int MAXZNAC,MAXFUNC;
extern char *NameFunc[],Inf[],ErrFile[],InpFile[],HtmFile[],OprFile[],OutFile[],
 WORKDIR[],CurrDir[],SimbInd;
extern Cnst *Pust,*Nul; extern List *L,*LBeg;
extern long DateInp;

char HeadCalc[256],HeadWork[256],HeadVersion[256],Autor[256],Blank[1024],*blank;
static long Lfile,PozFile;
int PUNKT=1,StrLong,NStrCalc;
bool RRR; TColor ColorModel=(TColor)255;
		   /*  Объявления объектов:  */
int Line=1,Com,NumbTask,KolErr,KolMes;  FILE *TMPFILE;
static unsigned char B; char ZNAC[]=" +-*/^",LOGZNAC[]=" <>/",
 WordErr[]="* ОШИБКА * ";
char Buf[1024],SP[1024],*sp=SP,*Head; static char S[256];

int KOper=14;
char *StrOp[] = {
 "УСРЕДНИТЬ","ПРИВЕСТИ СИЛЫ","ТЕНЗОР ИНЕРЦИИ","СТРУКТУРЫ",
 "РАЗМНОЖИТЬ","ЗАКРЫТЬ","ОТКРЫТЬ","ТВЕРДЫЕ ТЕЛА","ТВЕРДОЕ ТЕЛО",
 "ТВЁРДЫЕ ТЕЛА","ТВЁРДОЕ ТЕЛО","УПРУГИЕ СТЕРЖНИ","УПРУГИЙ СТЕРЖЕНЬ",
 "КВАТЕРНИОН"
//,"УПРУГИЙ ПАРАЛЛЕЛЕПИПЕД"
};

bool(*OP[])(Parm *,TRichEdit *)={
 MidlElem,LeadFors,Inertion,Structurs,MultiPly,ToClose,ToOpen,Solids,Solids,
 Solids,Solids,ElasticRods,ElasticRods,Quaternion
//,ElastBar
};

int FindOper(char *Name){ int i;
 for(i=0;i<KOper;i++){
  strcpy(Inf,Name); LowToUpp(Inf);
  if(!strcmp(Inf,StrOp[i])) return i+1;
 }
 return 0;
}

void Error(int Line,int Pos,bool Mess){
// Процедура выводит в файл ERRFILE информацию из строки Inf(глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом *  ОШИБКА  *.
// Кроме того, глобальному параметру IsError присвоится значение true,
// а KolErr++.
 char Buf[2048];
 IsError=true; KolErr++;
 if(Mess) Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
 if(ERRFILE=fopen(ErrFile,"a")){
  sprintf(Buf,"%3d%3d *  ОШИБКА  * %s\n",Line,Pos,Inf);
  Fputs(Buf,ERRFILE); fclose(ERRFILE);
}}

void ErrorSynt(int Line,int Pos){
// Процедура выводит в файл ERRFILE информацию из строки Inf(глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом *  ОШИБКА  *.
// Кроме того, глобальному параметру IsSyntError присвоится значение true,
// а KolErr++.
 char Buf[2048];
 IsSyntError=true; KolErr++;
 if(ERRFILE=fopen(ErrFile,"a")){
  sprintf(Buf,"%3d%3d *  ОШИБКА  * %s\n",Line,Pos,Inf);
  Fputs(Buf,ERRFILE); fclose(ERRFILE);
}}

void Atten(int Line,int Pos){
// Процедура выводит в файл ERRFILE информацию из строки Inf(глоба-
// льный параметр), предваряя ее номером строки файла исходных данных,
// номером позиции в ней, где локализована ошибка и словом * ВНИМАНИЕ *.
// Кроме того, счетчик предупреждений увеличится на 1( KolMes++ ).
 char Buf[2048]; KolMes++;
 Application->MessageBox(Inf,"Внимание!",MB_OK|MB_ICONWARNING);
 if(ERRFILE=fopen(ErrFile,"a")){
  sprintf(Buf,"%3d%3d * ВНИМАНИЕ * %s\n",Line,Pos,Inf);
  Fputs(Buf,ERRFILE); fclose(ERRFILE);
}}

char *Compress(char *S){ char *s=S;
 for(;*s;s++); --s; for(;(*s==' ')||(*s=='\t');s--); *(++s)='\0'; return S;
}

int Znac(char B){ int i; for(i=MAXZNAC;i>0&&B!=ZNAC[i];i--); return i; }

int Func(char *S){ int i=MAXFUNC;
 for(;i>=0&&strcmp(NameFunc[i],S)!=0;i--); return i==0 ? 2 : i ;
}

void PosLine(TRichEdit *RE){ char *b; blank=Blank;
 *sp='\0'; sp=SP; sprintf(Inf,"%4d.  %s",Line++,SP);
 if(b=strrchr(Inf,'\n')) *b='\0';
 RE->Lines->Add(Inf);
 fputs("<br>\n",TMPFILE);
}

bool ReadChar(void){
 if(PozFile<Lfile){
  B=Fgetc(INFILE); *sp++=B; PozFile++;
  if(B==' ') *blank++=' '; *blank='\0';
  return true;
 }
 return false;
}

POINT Pt;

void ErrorSynt(TRichEdit *RE,char *S,int Cod){
// Процедура работает внутри Vvod. Выводит на экран ошибочную строку,
// позицию ошибки в ней и текст ошибки. Выводит в файл ERRFILE номер
// ошибочной строки, номер позиции ошибки в ней и текст.
 int i,j,PosErr=(int)((long)sp-(long)SP),Kt=0,l;
 char *s=SP,Pusto[1024],Buf[256]; TColor TC;
 TTabControl *T=(TTabControl *)RE->Parent;
 *sp='\0';
 for(;*s!=*sp;) if(*s++=='\t') Kt++;
 IsSyntError=Cod;
 PosLine(RE);
 T->Canvas->Font->Assign(RE->Font);
 l=T->Canvas->TextWidth(SP);
 for(j=0;B!=';'&&!feof(INFILE);j++){
  B=Fgetc(INFILE); if(j<500) *sp++=B;
 }
 for(Pusto[0]='\0',i=0;T->Canvas->TextWidth(Pusto)<l;){
  Pusto[i++]=' '; Pusto[i]='\0';
 }
 Pusto[i-1]='^'; Pusto[i]='\0';
 sprintf(Buf,"       %s %s %s\n",Pusto,WordErr,S);
 TC=RE->SelAttributes->Color;
 RE->SelAttributes->Color=clRed;
 Pt=RE->CaretPos;
 l=strlen(Buf); if(Buf[l-1]=='\n') Buf[l-1]='\0';
 RE->Lines->Add(Buf);
 RE->SelAttributes->Color=TC;
 if(ERRFILE=fopen(ErrFile,"a")){
  sprintf(Inf,"%3d%3d *  ОШИБКА  * %s\n",Line-1,PosErr+Kt*7,S);
  Fputs(Inf,ERRFILE); fclose(ERRFILE);
}}

int LogZnac(TRichEdit *RE){ int i; char B1=B;
 if(!ReadChar()) return -1;
 switch(B1){
  case '<': if(B=='=') return 3; i=1; break;
  case '>': if(B=='=') return 4; i=2; break;
  case '/': if(B=='=') return 5; i=0; break;
  default : ErrorSynt(RE,"Ожидалось: '<', '>', '<=', '>=', '/='."); i=0;
 }
 fseek(INFILE,-1,SEEK_CUR); sp--; PozFile--; B=B1;
 return i;
}

bool ReadComm(TRichEdit *RE){
 fputs("<font color=\"#008080\">",TMPFILE); fputc('#',TMPFILE);
 while(ReadChar()) switch(B){
  case '#':
   fputc('#',TMPFILE); fputs("</font>",TMPFILE); return true;
  case ' ': fputs("&nbsp;",TMPFILE); continue;
  case '\t': for(int i=0;i<8;i++) fputs("&nbsp;",TMPFILE); continue;
  case '\n': PosLine(RE); fputs("&nbsp;",TMPFILE); continue;
  default: fputc(B,TMPFILE);
 }
 ErrorSynt(RE,"Не завершен комментарий!"); return false;
}

bool EatBlock(TRichEdit *RE){
 fputs("<font color=\"#008080\">",TMPFILE);
 while(ReadChar())
  switch(B){
   case '}' :
    Com--;
    if(!Com){ fputs("</font>}",TMPFILE); /*fclose(TMPFILE);*/ return true; }
    else{ fputc('}',TMPFILE); } break;
   case '{' : fputc('{',TMPFILE); Com++; break;
   case '\n': PosLine(RE); fputs("&nbsp;",TMPFILE); continue;
   case ' ': fputc(' ',TMPFILE); continue;
   default: fputc(B,TMPFILE);// fclose(TMPFILE); TMPFILE=fopen("tmp.htm","a");
  }
 fputs("</font>",TMPFILE);
 return false;
}
/*
bool EatName(TRichEdit *RE){ fputc('\\',TMPFILE);
 while(ReadChar()) switch(B){
  case '\n':
   PosLine(RE); ErrorSynt(RE,"Комментарий \\...\\ используется для имен.");
   return 0;
  case '\\': fputc('\\',TMPFILE); return true;
  default  :
   if(isdigit(B)||isalpha(B)||isciril(B)){ Fputc(B,TMPFILE); break; }
   else ErrorSynt(RE,"Ожидалось: '\\'."); return false;
 }
 return false;
}
*/
int ReadStr(int Chto,TRichEdit *RE){
 bool Numb,Quot=false; int KT=0,KE=0,i=0; char C; blank=Blank;
M:if(ReadChar()){
  switch(B){
   case  '#': if(ReadComm(RE)) goto M; else return -1;
//   case '\\': if(EatName(RE))  goto M; else return -1;
   case  '{':
    fputc('{',TMPFILE); Com++;
    if(EatBlock(RE)) goto M;
    else return -1;
   case '\n': PosLine(RE); goto M;
   case  '+': goto M;
   case '\t': for(int i=0;i<8;i++) fputs("&nbsp;",TMPFILE); goto M;
   case  ' ': /*fputs("&nbsp;",TMPFILE);*/ goto M;
   case  ';': S[i++]=B; S[i]='\0'; fputc(B,TMPFILE); return PUST;
   case  '-': S[i++]=B; S[i]='\0'; return '-';
   case  '(': S[i++]=B; S[i]='\0'; return '(';
   case  ')': S[i++]=B; S[i]='\0'; return ')';
   case  '=': S[i++]=B; S[i]='\0'; fputc(B,TMPFILE); return INST;
   case  '.': Chto=Double; Numb=true; S[i++]=B; /*fputc(B,TMPFILE);*/ break;
   case  '"':
    Chto=NAME; Numb=false; Quot=!Quot;
    break;
   default  :
    if(isdigit(B)){
     if(Chto!=Double) Chto=Long; Numb=true; S[i++]=B; break;
    }
    if(isalpha(B)||isciril(B)||B=='_'||B=='\x14'){
     if(!Chto) Chto=NAME; Numb=false; S[i++]=B; break;
    }
    else { ErrorSynt(RE,"Запрещенный символ."); return -1; }
 }}
 else return 0;
L:if(ReadChar()){
  switch(B){
   case  '#': if(!Quot){ if(ReadComm(RE)) goto M; else return -1; } break;
   case  ']':
    if(!Quot&&(Chto==ARR)) Chto=NAME;
    break;
   case  '[':
    if(!Quot){
     if(Numb){ ErrorSynt(RE,"Ожидалось имя."); return -1; } Chto=ARR;
    }
    break;
//   case '\\': Chto=STRINGER; break;
//    if(Quot||(Chto==ARR)){ S[i++]=B; goto L; }
//    else{ if(EatName(RE)) goto M; else return -1; }
   case '\n': Quot=false; PosLine(RE); goto L;
   case  ' ':
    S[i++]=B; goto L;
   case  '\t': S[i++]=B; for(int i=0;i<8;i++) fputs("&nbsp;",TMPFILE); goto L;
   case  '"': Quot=!Quot; goto L;
   case  ':':
    if(Quot||(Chto==ARR)){ S[i++]=B; goto L; }
    else{
     if(Numb){ ErrorSynt(RE,"Ожидалось имя."); return -1; }
     if(ReadChar()&&B=='=') Chto=INST; else{ Chto=ROUT; fseek(INFILE,-1,1); }
     Numb=false;
    }
    break;
   case  '(':
    if(!Quot){ if(Numb){ ErrorSynt(RE,"Ожидалось имя."); return -1; } }
    if(S[i-1]==SimbInd){
     for(S[i++]=B;ReadChar()&&(B!=')');) S[i++]=B; S[i++]=B; goto L; //!!!
    }
    break;
   case  ')': case  ',': case  '*': case  '/': case  '^': case  '>':
   case  '<': case  '!': case  '?': case  ';':
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
     if(!strchr(S,'.')) Chto=VARY; else Chto=ELEM; Numb=false;
    }
    break;
   case  'E': case  'e':
    if(!Quot){
     if(Numb){ Chto=Double;
      if(++KE>1){ ErrorSynt(RE,"В числе 2 буквы 'E'."); return -1; }
    }}
    S[i++]=B; goto L;
   case  '.':
    if(!Quot&&(Chto!=ARR)){
     if(Numb){ Chto=Double;
      if(++KT>1){
       ErrorSynt(RE,"В числе 2 точки."); return -1;
      }
     }
     else Chto=ELEM;
    }
    S[i++]=B; goto L;
   case '|': Chto=BODY; break;
   default  :
    if(!Quot){
     if((isalpha(B)||isciril(B))&&Numb){
      ErrorSynt(RE,
       "Если это число, то ожидалась цифра или символы: '.', 'E', 'e', ';'.");
      return -1;
    }}
    S[i++]=B; goto L;
  }
  S[i]='\0';
 }
 else return 0;
 return Chto;
}

Form ReadForm(int Prio,TRichEdit *RE){
 Cnst *C; Form F,U,V,FT,FF; signed int Oper; Vary *VV; int KS=0;
 char *Shtr,tst[16];
 sprintf(tst,"%s'%s",L->Time->Name,L->Time->Name);
 switch(ReadStr(0,RE)){
  case '-': U=ReadForm(6,RE); if(U.C==Pust) return U; F=-U; break;
  case '(': KS++; F=ReadForm(0,RE); if(F.C==Pust) return F; break;
  case  NAME  :
   if(B!='('){
    if(FindCnst(S,&C)) F.C=TakeCnst(S);
    else {
     for(Shtr=S;Shtr=strchr(Shtr+1,'\'');*Shtr='\''){
      *Shtr='\0';
      if((strstr(Shtr+1,tst)==(Shtr+1))||
         (strstr(Shtr+1,L->Time->Name)==(Shtr+1))){
       VV=TakeVary(S); if(!(VV->Line)) VV->Line=Line;
     }}
     F.V=TakeVary(S); if(!(F.V->Line)) F.V->Line=Line;
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
      U=ReadForm(0,RE);
      if(U.C==Pust) return U; F=rand(U); ISRAND=true; break;
     case 15:
      U=ReadForm(0,RE); if(U.C==Pust) return U; F=ceil(U); break;
     case 16:
      U=ReadForm(0,RE); if(U.C==Pust) return U; F=floor(U); break;
     case 17:
      U=ReadForm(0,RE); if(U.C==Pust) return U; F=arccos(U); break;
     default:
      if(!strcmp(S,"ipl")){ Arra *A;
       if(!ReadStr(0,RE)) return F;
       if(B==',') A=TakeArra(S);
       U=ReadForm(0,RE);
       if(U.C==Pust) return U;
       F.I=TakeIpll(1,A,U);
      }
      else if(!strcmp(S,"spln")){ Arra *A;
       if(!ReadStr(0,RE)) return F;
       if(B==',') A=TakeArra(S);
       U=ReadForm(0,RE); if(U.C==Pust) return U;
       F.P=TakeSpln(1,A,U);
      }
      else if(!strcmp(S,"summ")){ int Rez;
       FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0,RE))) return F;
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
      else if(!strcmp(S,"intg")){ int Rez;
       FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0,RE))) return F;
       if(Rez==NAME){
        VV=TakeVary(S); if(VV&&!(VV->Line)) VV->Line=Line;
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
      else if(!strcmp(S,"root")){ int Rez;
       FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0,RE))) return F;
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
      else if(!strcmp(S,"rest")){
       U=ReadForm(0,RE); if(U.C==Pust) return U;
	     if(B==','){
        V=ReadForm(0,RE); if(V.C==Pust) return V;
        F.O=TakeRest(U,V); break;
       }
       return F;
      }
      else {
       ErrorSynt(RE,"Неопознанная унарная операция."); F.C=Pust; return F;
   }} }
   break;
  case Long: case Double: F.C=TakeCnst(S); break;
  case PUST: case -1: F.C=Pust; return F;
 }
 for(;;) switch(B){
  case '\n': PosLine(RE);
  case '\t': case ' ': if(!ReadChar()) return F.C=Pust,F; else break;
  case ')':
   if(KS){ KS--; if(!ReadChar()) F.C=Pust; break;} return F;
  case  ';':
  case  ',':
   if(KS>0){
    ErrorSynt(RE,"Ожидалось: ')'."); F.C=Pust; break;
   }
  case  '?': case '!': return F;
  case  '=': //снова читаем переменную с формулой
    Compress(S);
    for(Shtr=S;Shtr=strchr(Shtr+1,'\'');*Shtr='\''){
     *Shtr='\0';
     if((strstr(S,tst)==(Shtr+1))||(strstr(Shtr+1,L->Time->Name)==(Shtr+1))){
      VV=TakeVary(S); if(!VV->Line) VV->Line=Line;
    }}
    FF.V=VV=TakeVary(S);
    if((VV->Znach.C!=Pust)&&(VV->Line>NStrCalc)){
     sprintf(Inf,"Повторный ввод формулы для переменной \"%s\" в строке %d",S,Line);
     Atten(Line,0);
    }
    VV->Line=Line;
    VV->Znach=ReadForm(0,RE); return FF;
  case  '/':
   if((Oper=LogZnac(RE))>0){
    V=ReadForm(0,RE); if(V.C==Pust) return V;
    if((B=='!')||(B=='?')){
     FT=ReadForm(0,RE); if(FT.C==Pust) return FT;
    }
    else { ErrorSynt(RE,"Ожидалось: '?'."); F.C=Pust; return F; }
    if(B=='!'){
     FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
    }
    else { ErrorSynt(RE,"Ожидалось: '!'."); F.C=Pust; return F; }
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
    case  3:
     U=ReadForm(Oper,RE); if(U.C==Pust) return U;
     F=F*U; break;
    case  4:
     U=ReadForm(Oper,RE); if(U.C==Pust) return U;
     F=F/U; break;
    case  5:
     U=ReadForm(Oper,RE); if(U.C==Pust) return U;
     F=F^U; break;
    default:
     if((Oper=LogZnac(RE))<0) return F;
     if(Oper){
      V=ReadForm(0,RE); if(V.C==Pust) return V;
      if((B=='!')||(B=='?')){
       FT=ReadForm(0,RE); if(FT.C==Pust) return FT;
      }
      else { ErrorSynt(RE,"Ожидалось: '?'."); F.C=Pust; return F; }
      if(B=='!'){
       FF=ReadForm(0,RE); if(FF.C==Pust) return FF;
      }
      else { ErrorSynt(RE,"Ожидалось: '!'."); F.C=Pust; return F; }
      F.L=TakeIfop(Oper,F,V,FT,FF); break;
     }
     else { ErrorSynt(RE,"Неопознанный символ."); F.C=Pust; return F; }
}}}

bool SptoArr(Arra *Arr,int &N,TRichEdit *RE){
 int i=0; Form *F; 
 for(ReadChar();B!='=';ReadChar()) if(B==' ') fputs("&nbsp;",TMPFILE);
 for(ReadChar();B==' '||B=='\n'||B=='\v'||B=='\t';ReadChar()){
  switch(B){
   case ' ': fputs("&nbsp;",TMPFILE); break;
   case '\n': case '\v': fputs("\n<br>",TMPFILE); break;
   default: for(int i=0;i<8;i++) fputs("&nbsp;",TMPFILE); break;
 }}
 F=(Form *)calloc(N+1,sizeof(Form));
 fseek(INFILE,-1L,1);
 do{
  if(i<N){
   F[i]=ReadForm(0,RE);
   PrintForm(false,TMPFILE,NULL,F[i]); fputc(B,TMPFILE);
   i++;
  }
  else{
   ErrorSynt(RE,"Массив больше объявленной размерности.");
   free(F); F=NULL; return false;
  }
 }while(B!=';');
 if(i<N){
  ErrorSynt(RE,"Массив меньше объявленной размерности.");
  free(F); F=NULL; return false;
 }
 else{
  Arr->N=N;
  for(i=0;i<N;i++) if(F[i].C->Atr!=3){ Arr->F=F; return true; }
  Arr->A=(double *)calloc(N,sizeof(double));
  for(i=0;i<N;i++){ Arr->A[i]=F[i].C->Val; } free(F);
  return true;
}}

bool NoName(char *S){
 bool T=false,T2=false,E=false,C=false; char *s;
 if(!S) return true;
 for(s=S;*s&&((*s==' ')||(*s=='\t')||(*s=='\n'));s++);
 if(!*s) return true;
 switch(*s){
  case '.': T=true;
  case '+':case '-':
  case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':
  case '8':case '9':
   C=true;
   for(s++;*s;s++)
    switch(*s){
     case 'E':case 'e':
      if(!C){
       if(!T) return false;
       Application->MessageBox(
        "Неправильно записано экспоненциальное представление числа",
        "ОШИБКА!",MB_OK|MB_ICONERROR);
       return false;
      }
      if(E){
       Application->MessageBox(
        "Неправильно записано экспоненциальное представление числа",
        "ОШИБКА!",MB_OK|MB_ICONERROR);
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
     case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':
     case '8':case '9':
      break;
     default: return false;
    }
   break;
  default: return false;
 }
 if(T2){
  Application->MessageBox("В числе 2 точки!","ОШИБКА!",MB_OK|MB_ICONERROR);
  return false;
 }
 return true;
}

Parm *ReadSp(TRichEdit *RE,int Ln){
 Parm *R=NULL,**Pr=&R,*P,*PR=NULL; char Buf[1024],*b=Buf; int Index=0;
M:while(ReadChar()){
  switch(B){
   case '#': ReadComm(RE); break;
   case '"':
    for(*b++=B;ReadChar()&&B!='"'&&B!=';';*b++=B);
    if(B=='\"') *b++='\"';
    if(B==';'){ ErrorSynt(RE,"Нет закрывающей двойной кавычки"); return NULL; }
    if(!B){ ErrorSynt(RE,"Преждевременное окончание файла"); return NULL; }
    break;
   case '(':
    *b='\0'; CleanStr(Buf,Inf); strcpy(Buf,Inf);
    if(NoName(Buf)){
     ErrorSynt(RE,"Должно быть сначала имя, а потом список в скобках");
     return NULL;
    }
    else PR=ReadSp(RE,Line);
    break;
   case ',': case ')': case ';': case '|': case '\\':
    P=*Pr=(Parm *)calloc(1,sizeof(Parm)); Pr=(Parm **)P; P->Line=Ln; *b='\0';
    CleanStr(Buf,Inf); strcpy(Buf,Inf);
    if(NoName(Buf)){
     P->Val=atof(Buf);
     if(b=strchr(Buf,'=')){
      ErrorSynt(RE,"Слева от '=' должно быть имя"); return NULL;
    }}
    else{
     if(b=strchr(Buf,'=')) *b++='\0';
     CleanStr(Buf,Inf);
     P->Nam=strcpy((char *)calloc(strlen(Inf)+1,SzC),Inf);
     if(b){
      P->R=(Parm *)calloc(1,sizeof(Parm));
      CleanStr(b,Inf); strcpy(Buf,Inf);
      if(NoName(Buf)) P->R->Val=atof(Buf);
      else P->R->Nam=strcpy((char *)calloc(strlen(Buf)+1,SzC),Buf);
     }
     else{ P->R=PR; PR=NULL; }
    }
    *Buf='\0'; b=Buf;
    if((B==')')||(B==';')||(B=='|')||(B=='\\')) return R;
    break;
   case '$':
    *b++=B; ReadChar(); Index=1;
    if(B=='('){
     Index++;
     for(;Index;ReadChar()){
      if(B==')'){
       Index--;
       if(Index==1)
        Index=0; *b++=B; break;
      }
      *b++=B;
    }}
    else
     *b++=B;
    break;
   default: *b++=B;
    if(B=='\n'){ char *b;
     Ln++; /*PosLine(RE);*/
     *sp='\0'; sp=SP; sprintf(Inf,"%4d.  %s",Line++,SP);
     if(b=strrchr(Inf,'\n')) *b='\0';
     RE->Lines->Add(Inf);
    }
 }}
 return R;
}

char Name[256],NameElem[256],*NameCord,NameRoot[50];

void DelVary(Vary **R,Vary *V){ Vary *v=*R;
 if(V==*R){ *R=v->Sled; free(V); return; }
 for(;v;v=v->Sled)
  if(v->Sled==V){ v->Sled=V->Sled; free(V); return; }
}

bool ToWithdraw(Root *I){
 int N=1; Elem *E; Vary *V;
 for(Parm *P=I->First;P;P=P->Sled,N++){
  if(!P->Nam){
   sprintf(Inf,"%d-й параметр инструкции %s %d-й строки\n"
    "должен содержать имя удаляемой инструкции, переменной, элемента!",
    N,I->Name,I->Line);
   Error(I->Line,0,Inf); IsSyntError=true;
  }
  if(P->R){
   sprintf(Inf,"%d-й параметр инструкции %s %d-й строки\n"
    "не должен содержать внутренний список!",N,I->Name,I->Line);
   Error(I->Line,0,Inf); IsSyntError=true;
  }
  if(strchr(P->Nam,'.')){
   if(!FindElem(P->Nam,&E)){
    sprintf(Inf,"%d-й параметр инструкции %s %d-й строки\n"
     "содержит имя, отсутствующее среди элементов, или уже удаленное!",
     N,I->Name,I->Line);
    Error(I->Line,0,Inf); IsSyntError=true;
   }
   else
    DelStruct((void **)&(L->E),E);
  }
  else{ Root *i;
   if(FindVary(P->Nam,&V)) DelVary(&(L->V),V);
   else if(i=FindInstLast(P->Nam)) DelStruct((void **)&(L->Inst),i);
   else{
    sprintf(Inf,"%d-й параметр инструкции %s %d-й строки\n"
     "содержит имя, отсутствующее среди переменных и инструкций, или уже удаленное!",
     N,I->Name,I->Line);
    Error(I->Line,0,Inf); IsSyntError=true;
 }}}
 return IsSyntError;
}

void Vvod(TRichEdit *RE,TCGauge *CGauge,TStatusBar *StatusBar){
 char *s,*Shtr,tst[256],KONEC[]="КОНЕЦ"; bool END,MAKE; struct stat SB;
 Elem *E; Root *I,*O,**PI=&L->Inst; Stringer *r;
 int Flag,N,Ln,Rez; Arra *A; Vary *V; Form F; Body *b; Parm *P; Cord *K;
 stat(InpFile,&SB); DateInp=SB.st_mtime;
 NumbTask++; NameRoot[0]='\0';
 sprintf(tst,"%s'%s",L->Time->Name,L->Time->Name);
 if(CGauge){
  CGauge->ForeColor=ColorModel; CGauge->Progress=0; CGauge->MaxValue=Lfile;
 }
 StatusBar->SimpleText="Чтение файла исходных данных";
M:Flag=ReadStr(0,RE);
 if(CGauge) CGauge->Progress=PozFile;
 StatusBar->SimpleText=S;
 switch(Flag){
  case Double: case   Long:
   ErrorSynt(RE,"Объект ввода не может быть отдельным числом."); goto M;
  case   PUST:
   if(PozFile<Lfile){
    ErrorSynt(RE,"Пустое выражение."); goto M;
   }
   return;
  case   ELEM: END=false;
   Compress(S);
   strcpy(Name,S); s=Name; while(*s++!='.'); *(s-1)='\0'; NameCord=s++;
   strcpy(NameElem,Name); strcat(NameElem,".");
   while(!END){ MAKE=true;
    while(MAKE) switch(*s){
      case  '.':  *s='\0'; MAKE=false; break;
      case '\0': END=true; MAKE=false; break;
      default:   s++;
    }
    sprintf(NameElem,"%s.%s",Name,NameCord);
    if(FindElem(NameElem,&E)&&(E->Line>NStrCalc)){
     sprintf(Inf,"Повторный ввод элемента \"%s\" в строке %d",NameElem,Line);
     Atten(Line,0);
    }
    else E=TakeElem(NameElem);
    E->K=TakeVary(NameCord);
     E->K->Line=Line;
    if(B!=';') F=ReadForm(0,RE);
    E->Znach=F; NameCord=++s;
    sprintf(Inf,"%s",E->Name); fputs(Blank,TMPFILE);
    PrintForm(false,TMPFILE,Inf,E->Znach); fputs("&nbsp;",TMPFILE);
   }
   goto L;
  case   VARY: Compress(S);
   for(Shtr=S;Shtr=strchr(Shtr+1,'\'');*Shtr='\''){
    *Shtr='\0';
    if((strstr(S,tst)==(Shtr+1))||(strstr(Shtr+1,L->Time->Name)==(Shtr+1))){
     V=TakeVary(S); if(!V->Line) V->Line=Line;
   }}
   V=TakeVary(S);
   if((V->Znach.C!=Pust)&&(V->Line>NStrCalc)){
    sprintf(Inf,"Повторный ввод формулы для переменной \"%s\" в строке %d",S,Line);
    Atten(Line,0);
   }
   V->Line=Line;
   V->Znach=ReadForm(0,RE);
   fputs(Blank,TMPFILE);
   PrintForm(false,TMPFILE,V->Name,V->Znach);
   fputs("&nbsp;",TMPFILE);
   goto L;
  case    ARR:
   for(int j=strlen(Blank);j--;) fputs("&nbsp;",TMPFILE); blank=Blank; *Blank='\0';
   Compress(S);
   A=TakeArra(S); A->Line=Line; Grek(S);
   fprintf(TMPFILE,"<font face=\"Courier New\">%s[",S);
   Rez=ReadStr(ARR,RE);
   if(Rez==Long){
    N=atoi(S);
    if(A->A) free(A->A);
    fprintf(TMPFILE,"%d]=",N);
    if(SptoArr(A,N,RE))
     TakeSplns(1,A);
//    else{
//     sprintf(Inf,"Массив \"%s\" не прочитан",A->Name); ErrorSynt(RE,Inf);
//    }
    fputs("</font>",TMPFILE);
    goto L;
   }
   else if(Rez==NAME){
    FILE *F; long DateFile; struct stat SB;
    for(ReadChar();B!=';';ReadChar());
    if(access(S,0)){
     ErrorSynt(RE,"Файл не найден!"); return;
    }
    stat(S,&SB); DateFile=SB.st_mtime;
    if(DateFile>DateInp) DateInp=DateFile;
    if(F=fopen(S,"rb")){
     fread(&N,SzI,1,F); A->N=N;
     A->A=(double *)realloc(A->A,N*SzD);
     fread(A->A,SzD,N,F);
     fprintf(TMPFILE,"%s];",S);
     TakeSplns(1,A);
     fputs("</font>",TMPFILE);
     goto L;
    }
    else{
     ErrorSynt(RE,"Файл не открывается!"); return;
   }}
   else{
    fputs("</font>",TMPFILE);
    if(!Rez) return;
    goto M;
   }
  case NAME:
   strcpy(Inf,S); LowToUpp(Inf);
   if(!strcmp(Inf,"КОНЕЦ")){
    if(CGauge) CGauge->Progress=CGauge->MaxValue; return;
   }
   I=*PI=(Root *)calloc(1,sizeof(Root)); PI=(Root **)I; I->Line=Ln;
   I->Name=strcpy((char *)malloc(strlen(Inf)+1),Inf);
   fprintf(TMPFILE,"%s;",S);
   break;
  case   INST:
   strcpy(NameRoot,S);
   Compress(NameRoot);
   LowToUpp(NameRoot); Ln=Line;
   if(!strcmp(NameRoot,"РАСЧЕТ")||!strcmp(NameRoot,"РАСЧЁТ")) NStrCalc=Line;
   if(!strcmp(NameRoot,"ЛИСТ")){
    P=B=='='?ReadSp(RE,Line):NULL;
    if(!P||(KolElem(P)>1)||P->R){
     sprintf(Inf,"Инструкция \"ЛИСТ:=\" должна содержать\n"
      "справа только наименование листа.");
     ErrorSynt(RE,Inf); goto L;
    }
    L=TakeList(P->Nam); free(P->Nam); free(P);
    fprintf(TMPFILE,"%s:=",NameRoot);
   }
   else {
    I=*PI=(Root *)calloc(1,sizeof(Root)); PI=(Root **)I; I->Line=Ln;
    I->Name=strcpy((char *)malloc(strlen(NameRoot)+1),NameRoot);
    if(!strcmp(NameRoot,"РАБОТА")||strstr(NameRoot,"ВЫПОЛНИЛ")){
     char *s=Inf; while(ReadChar()&&(B!=';')) *s++=B; *s='\0';
     if(I->First=P=(Parm *)calloc(1,sizeof(Parm))){
      P->Nam=strcpy((char *)calloc(strlen(Inf)+1,SzC),Inf);
      fprintf(TMPFILE,"%s := ",S);
      ReadAllRight(I->First,TMPFILE,false,false);
    }}
    else{
     I->First=P=B=='='?ReadSp(RE,Line):NULL;
     if(I->First){
      fprintf(TMPFILE,"%s := ",S);
      if(!strcmp(NameRoot,"ПЕЧАТАТЬ")||!strcmp(NameRoot,"ПОКАЗАТЬ"))
       ReadAllRight(I->First,TMPFILE,true,false);
      else if(strstr(NameRoot,"РИСУН"))
       ReadAllRight(I->First,TMPFILE,false,true);
      else
       ReadAllRight(I->First,TMPFILE,true,true);
     }
     else fprintf(TMPFILE,"%s",S);
    }
    fputc(';',TMPFILE);
   }
   if(!strcmp(NameRoot,"ИЗЬЯТЬ")){
    strcpy(Inf,"Правильное название инструкции - \"ИЗЪЯТЬ\"!"); Atten(Line,0);
    if(ToWithdraw(I)) goto L;
   }
   if(!strcmp(NameRoot,"ИЗЪЯТЬ"))
    if(ToWithdraw(I)) goto L;
   break;
  case ROUT:
   Compress(S);
   if(!(P=ReadSp(RE,Ln=Line))) goto M;
   O=TakeOper(S); O->Line=Ln; O->First=P;
   fprintf(TMPFILE,"%s : ",S);
   strcpy(Inf,O->Name); LowToUpp(Inf);
   if(!strcmp(Inf,"ТЕНЗОР ИНЕРЦИИ"))
    ReadAllRight(O->First,TMPFILE,true,true);
   else
    ReadAllRight(O->First,TMPFILE,true,false);
   fputc(';',TMPFILE);
   goto L;
  case BODY: fprintf(TMPFILE,"%s | ",S);
   CleanStr(S,Inf);
   b=TakeBody(Inf,strchr(Inf,'~'));
   if(b&&(b->KoordSys=ReadSp(RE,Line)))
    b->Inert=ReadSp(RE,Line);
   for(Parm *P=b->KoordSys;P;P=P->Sled){
    if(P->R){
     for(Parm *p=P->R;p;p=p->Sled){ Cnst *C;
      if(p->Nam&&!FindCnst(*p->Nam=='-'?p->Nam+1:p->Nam,&C)){
       V=TakeVary(*p->Nam=='-'?p->Nam+1:p->Nam);
       if(!(V->Line)) V->Line=Line;
   }}}}
   if(b->KoordSys){
    ReadAllRight(b->KoordSys,TMPFILE,true,false); fputs(" | ",TMPFILE);
   }
   if(b->Inert) ReadAllRight(b->Inert,TMPFILE,true,false);
   fputc(';',TMPFILE);
   goto L;
  case STRINGER: fprintf(TMPFILE,"%s \\ ",S);
   CleanStr(S,Inf);
   r=TakeStringer(Inf);
   if(r)
    r->Parms=ReadSp(RE,Line);
   for(Parm *P=r->Parms;P;P=P->Sled){
    if(P->R){ Cnst *C;
     for(Parm *p=P->R;p;p=p->Sled){
      if(!FindCnst(*p->Nam=='-'?p->Nam+1:p->Nam,&C)){
      V=TakeVary(*p->Nam=='-'?p->Nam+1:p->Nam);
      if(!(V->Line)) V->Line=Line;
   }}}}
   if(r->Parms) ReadAllRight(r->Parms,TMPFILE,true,false);
   fputc(';',TMPFILE);
   goto L;
 }
 switch(B){
  case   ';':
   LowToUpp(S);
   if(!strcmp(S,KONEC)) return;
   goto M;
  case 255: return;
 }
L:goto M;
}

void AllToCollect(char *S,Parm *R){
 char *h=S;
 for(Parm *p=R;p;p=p->Sled){
  char *n=p->Nam;
  if(n){
   while(*n){
    if((*n=='\n')||(*n==' ')){ while(*n&&(*n=='\n')||(*n==' ')) n++; *h++=' '; }
    else *h++=*n++;
  }}
  if(p->R){
   *h+='(';
   for(Parm *r=p->R;r;r=r->Sled){
    if(r->Nam) h+=sprintf(h,r->Nam); else h+=sprintf(h,"%g",r->Val);
    if(r->Sled){ *h++=','; *h++=' '; } else *h++=')';
  }}
  if(p->Sled){ *h++=','; *h++=' '; } else *h++='\0';
}}

bool VvodJob(TRichEdit *Ri,TRichEdit *Ro,TCGauge *CGauge,TStatusBar *StatusBar){
 int i,l; struct stat St; Root *O,*I; Vary *V; char NameRoot[256],Buf[256];
 Magazine *Mi=NULL,*m;
 if(stat(InpFile,&St)) return false;
 Lfile=St.st_size;
 rewind(INFILE);
 while(fgets(Inf,1023,INFILE)&&strrchr(Inf,'\n')) Lfile--; rewind(INFILE);
Beg:
 IsSyntError=false; PozFile=NumbTask=0; S[0]=SP[0]='\0'; Line=1; sp=SP;
 rewind(INFILE);
 while(Fgets(Inf,255,INFILE))
  if((l=strlen(Inf))>StrLong)
   StrLong=l;
 rewind(INFILE);    
 TMPFILE=fopen("tmp.htm","w");
 Vvod(Ri,CGauge,StatusBar);
 if(CGauge) CGauge->Progress=0;
 fclose(TMPFILE);
 if(IsSyntError) goto AvExit;
/*
 for(I=L->Inst;I;I=I->Sled){
  if(!strcmp(I->Name,"ИЗЬЯТЬ")){
   strcpy(Inf,"Правильное название инструкции - \"ИЗЪЯТЬ\"!");
   Atten(I->Line,0);
   if(ToWithdraw(I)) goto AvExit;
  }
  if(!strcmp(I->Name,"ИЗЪЯТЬ")){
   if(ToWithdraw(I)) goto AvExit;
 }}*/
 if(ISRAND) randomize();
 StatusBar->SimpleText="Поиск колец в формулах";
 if(CGauge){
  CGauge->MaxValue=0;
  for(L=LBeg;L;L=L->Sled) for(Vary *V=L->V;V;V=V->Sled) CGauge->MaxValue++;
  CGauge->Progress=0;
 }
 for(L=LBeg;L;L=L->Sled){
  for(Vary *V=L->V;V;V=V->Sled){
   Magazine *Mv=NULL; TakeMagazine(&Mv,V->Name);
   if(CGauge) CGauge->Progress++;
   if(IsCykl(Mv,V->Znach)){
    sprintf(Inf,
    "Переменная \"%s\" или входящие в ее формулу\n"
    "другие переменные зависят сами от себя.",V->Name);
    Application->MessageBox(Inf,"ОШИБКА!",MB_OK|MB_ICONERROR);
    ErrorSynt(0,0); goto AvExit;
 }}}
 if(CGauge) CGauge->Progress=0;
 L=LBeg;
//Уберем последнюю инструкцию РАСЧЕТ
 for(I=L->Inst;I&&I->Sled&&I->Sled->Sled;I=I->Sled);
 if(I&&I->Sled){
  strcpy(Inf,I->Sled->Name); LowToUpp(Inf);
  if(!strcmp(Inf,"РАСЧЕТ")||!strcmp(Inf,"РАСЧЁТ")){
   if(I->Sled->First){
    if(I->Sled->First->Nam) free(I->Sled->First->Nam);
    free(I->Sled->First);
   }
   if(I->Sled->Name) free(I->Sled->Name);
   free(I->Sled); I->Sled=NULL;
 }}
//Зафиксируем № строки с предыдущей инструкцией РАСЧЕТ
 for(NStrCalc=0,I=L->Inst;I;I=I->Sled){
  strcpy(Inf,I->Name); LowToUpp(Inf);
  if(!strcmp(Inf,"РАСЧЕТ")||!strcmp(Inf,"РАСЧЁТ")) NStrCalc=I->Line;
 }
//Проверим инструкции на повторение
 for(I=L->Inst;I;I=I->Sled){
  if(I->Line>NStrCalc){
   strcpy(NameRoot,I->Name); LowToUpp(NameRoot);
   if(strcmp(NameRoot,"АНИМАЦИЯ")&&strcmp(NameRoot,"ФОН")&&FindMagazine(Mi,NameRoot,&m)){
    sprintf(Inf,"Инструкция \"%s\" повторно записана в строке %d",NameRoot,I->Line);
    Application->MessageBox(Inf,"ВНИМАНИЕ!",MB_OK|MB_ICONWARNING);
   }
   else TakeMagazine(&Mi,NameRoot);
 }}
 if(I=FindInstLast("КОДИРОВКА")){
  if(I->First->Nam&&(strlen(I->First->Nam)==6)) CodElem(I->First->Nam);
 }
 HTMFILE=fopen(HtmFile,access(HtmFile,0)?"w":"a");
 if(!HTMFILE){
  sprintf(Inf,"Нельзя открыть файл для формирования отчета!");
  Application->MessageBox(Inf,"Системная ошибка!",MB_OK|MB_ICONERROR);
 }
 else{
  fprintf(HTMFILE,"<html>\n<head>\n<title>О Т Ч Е Т   К И Д И М</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"file:///%s\\fon.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
  fprintf(HTMFILE,"<h3 align=\"center\"><font color=\"#990000\">"
   "О Т Ч Е Т</font></h3>\n");
 }
 if(!(I=FindInstLast("РАБОТА"))){
  sprintf(Inf,"В исходных данных нет инструкции \"РАБОТА:=<Название работы>;\"");
  Atten(0,0); strcpy(HeadWork,"Нет названия работы");
 }
 else if(!I->First||!I->First->Nam){
  sprintf(Inf,
   "В инструкции \"%s\" в правой части надо указать название работы",I->Name);
  Atten(I->Line,0);
 }
 else{
  AllToCollect(HeadWork,I->First);
  if(HTMFILE)
   fprintf(HTMFILE,"<h3 align=\"center\"><font color=\"#990000\">"
    "по работе \"%s\"</font></h3>\n",HeadWork);
 }
 if(I=FindInstLast("ВАРИАНТ")){
  if(!I->First||!I->First->Nam){
   sprintf(Inf,
    "В инструкции \"%s\" в правой части надо указать название варианта работы",I->Name);
   Atten(I->Line,0);
  }
  else{
   AllToCollect(HeadVersion,I->First);
   if(HTMFILE)
    fprintf(HTMFILE,"<h4 align=\"center\"><font color=\"#000099\">"
    "Вариант: \"%s\"</font></h4>\n",HeadVersion);
 }}
 if(!(I=FindInstLast("ВЫПОЛНИЛ"))&&!(I=FindInstLast("ВЫПОЛНИЛА"))&&
    !(I=FindInstLast("ВЫПОЛНИЛИ"))){
  sprintf(Inf,"В исходных данных нет инструкции \"ВЫПОЛНИЛ\"\n"
   "(\"ВЫПОЛНИЛА\", \"ВЫПОЛНИЛИ\")");
  Atten(0,0); strcpy(Autor,"Нет автора");
 }
 else{
  if(I->First&&I->First->Nam){
   AllToCollect(Autor,I->First);
   if(HTMFILE)
    fprintf(HTMFILE,
     "<h3 align=\"center\"><font color=\"#990000\">%s: \"%s\"</font></h3>\n",
     !strcmp(I->Name,"ВЫПОЛНИЛ")?"Выполнил":
     !strcmp(I->Name,"ВЫПОЛНИЛА")?"Выполнила":"Выполнили",Autor);
  }
  else{
   sprintf(Inf,"В инструкции \"%s\" в правой части надо указать авторство",
    I->Name);
   Atten(I->Line,0);
 }}
 if((I=FindInstLast("РИСУНОК"))||(I=FindInstLast("РИСУНКИ"))&&HTMFILE){
  char DrawsFile[256]="\0";
  if(!I->First||!I->First->Nam){
   sprintf(Inf,
    "В инструкции \"%s\" в правой части надо указать имя файла с рисунком",I->Name);
   Atten(I->Line,0);
  }
  else{ char *s,Ris[256],*ss; int KolRis=0;
   for(Parm *P=I->First;P;P=P->Sled){
    if(strrchr(P->Nam,'\\'))
     strcpy(DrawsFile,*P->Nam=='"'?P->Nam+1:P->Nam);
    else if(s=strstr(P->Nam,"../")){
     if(ss=strrchr(CurrDir,'\\')){
      *ss='\0'; sprintf(DrawsFile,"%s\\%s",CurrDir,s+3); *ss='\\';
    }}
    else
     sprintf(DrawsFile,"%s\\%s",CurrDir,*P->Nam=='"'?P->Nam+1:P->Nam);
    if(s=strrchr(DrawsFile,'"')) *s='\0';
    if(!(DrawsFile[0])){
     sprintf(Inf,"В инструкции \"РИСУНОК\" не указано имени файла с рисунком.");
     Atten(I->Line,0);
    }
    else if(access(DrawsFile,0)){
     if(s=strrchr(InpFile,'\\')){
      strcpy(Ris,InpFile); s=strrchr(Ris,'\\'); *s='\0';
      sprintf(Inf,"%s\\%s",Ris,DrawsFile);
      if(!access(Inf,0)){
       strcpy(DrawsFile,Inf); KolRis++;
       if(strcmp(P->Nam,DrawsFile)){
        free(P->Nam);
        P->Nam=strcpy((char *)calloc(strlen(DrawsFile)+1,SzC),DrawsFile);
      }}
      else{
       sprintf(Inf,"В инструкции \"РИСУНОК\" ссылка на файл с рисунком недействительна.\n"
        "Файл %s не найден",P->Nam);
       Atten(I->Line,0);
     }}
     else{
      sprintf(Inf,"В инструкции \"РИСУНОК\" ссылка на файл с рисунком недействительна.\n"
       "Файл %s не найден",P->Nam);
      Atten(I->Line,0);
    }}
    else{
     KolRis++;
     if(strcmp(P->Nam,DrawsFile)){
      free(P->Nam);
      P->Nam=strcpy((char *)calloc(strlen(DrawsFile)+1,SzC),DrawsFile);
   }}}
   if(KolRis>1){
    fputs("<p align=\"center\">\n<table width=\"100%\" border=\"0\">\n <tr>\n",HTMFILE);
   }
   for(Parm *P=I->First;P;P=P->Sled){
    if(strrchr(P->Nam,'\\')) strcpy(DrawsFile,*P->Nam=='"'?P->Nam+1:P->Nam);
    else sprintf(DrawsFile,"%s\\%s",CurrDir,*P->Nam=='"'?P->Nam+1:P->Nam);
    if(s=strrchr(DrawsFile,'"')) *s='\0';
    if(DrawsFile[0]){
     if(!access(DrawsFile,0)){
      if(KolRis>1)
       fprintf(HTMFILE,"  <th><img src=\"%s\"></th>\n",DrawsFile);
      else
       fprintf(HTMFILE,"<p align=\"center\"><img src=\"%s\"></p>",DrawsFile);
     }
     else{
      if(s=strrchr(InpFile,'\\')){
       strcpy(Ris,InpFile); s=strrchr(Ris,'\\'); *s='\0';
       sprintf(Inf,"%s\\%s",Ris,DrawsFile);
       if(!access(Inf,0)){
        if(KolRis>1)
         fprintf(HTMFILE,"  <th><img src=\"%s\"></th>\n",Inf);
        else
         fprintf(HTMFILE,"<p align=\"center\"><img src=\"%s\"></p>",Inf);
   }}}}}
   if(KolRis>1) fputs(" </tr>\n</table>\n</p>\n",HTMFILE);
 }}
 if(HTMFILE){
  fprintf(HTMFILE,"<h4 align=\"left\"><font color=\"#009900\">"
   "%d. Исходные данные</font></h4>\n\n",PUNKT++);
  fprintf(HTMFILE,"<p align=\"left\"><font face=\"%s\">",Ri->Font->Name);
  if(StrLong>75){ fprintf(HTMFILE,"<font size=-%d>",1); StrLong=120; }
  if(TMPFILE=fopen("tmp.htm","r")){ struct stat St; char B;
   stat("tmp.htm",&St);
   for(int i=0;i<St.st_size;i++){
    B=fgetc(TMPFILE); if(B=='\n') i++; fputc(B,HTMFILE);
   }
   fclose(TMPFILE);
  }
  if(StrLong>75) fputs("</font>\n",HTMFILE);
  fputs("</font></p>\n",HTMFILE);
 }
 for(L=LBeg;L;L=L->Sled){ Root *Op=NULL; int Kr=0;
  for(O=L->Oper;O;O=O->Sled){
   strcpy(NameRoot,O->Name); LowToUpp(NameRoot);
   if(!strcmp(NameRoot,"РАЗМНОЖИТЬ")) Kr++;
   else
    for(int i=7;i<13;i++)
     if(!strcmp(NameRoot,StrOp[i])){
      if(!OP[i](O->First,Ro))
       IsSyntError=true;
      strcpy(Inf,O->Name);
      O->Name=(char *)realloc(O->Name,strlen(O->Name)+2);
      sprintf(O->Name,"@%s",Inf); break;
     }
  }
  if(Kr>1){
   sprintf(Inf,"Оператор \"РАЗМНОЖИТЬ\" должен быть в одном экземпляре,\n"
    "объедините содержимое таких операторов в один!");
   Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
   ErrorSynt(O->Line,0); fclose(HTMFILE); goto AvExit;
  }
//переставим на первое место оператор "РАЗМНОЖИТЬ"
  if(L->Oper){
   strcpy(NameRoot,L->Oper->Name); LowToUpp(NameRoot);
   if(Kr&&strcmp(NameRoot,"РАЗМНОЖИТЬ")&&L->Oper->Sled){
    for(O=L->Oper;O->Sled;O=O->Sled){
     strcpy(NameRoot,O->Sled->Name); LowToUpp(NameRoot);
     if(!strcmp(NameRoot,"РАЗМНОЖИТЬ")){
      Op=O->Sled; O->Sled=O->Sled->Sled; Op->Sled=L->Oper; L->Oper=Op; break;
   }}}
   for(O=L->Oper;O;O=O->Sled){
    if(*O->Name=='@') continue;
    if(i=FindOper(O->Name)){ i--;
     if(!OP[i](O->First,Ro))
      IsSyntError=true;
    }
    else{
     sprintf(Inf,"Обработчик оператора \"%s\", записанного в строке №%d, не найден.",
      O->Name,O->Line);
     Application->MessageBox(Inf,"Ошибка!",MB_OK|MB_ICONERROR);
     ErrorSynt(O->Line,0); fclose(HTMFILE); goto AvExit;
  }}}
  if(L->B) Bodys(L->B);
  if(L->T) Stringers(L->T);
 }
AvExit: L=LBeg;
 return !IsSyntError;
}

int ReadStr(int Chto){
 bool Numb; int KT=0,KE=0,i=0,j,Ks; char C;
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
     sprintf(Inf,"Запрещенный символ - %c",B);
     Application->MessageBox(Inf,"Ошибка!",MB_OK); return -1;
 }}}
 else return 0;
L:if(B=*sp++){
  switch(B){
   case  ' ':
   case  '\t': S[i++]=B; goto L;
   case  '(':
    if(Numb){
     sprintf(Inf,"Ожидалось имя");
     Application->MessageBox(Inf,"Ошибка!",MB_OK); return -1;
    }
    else
     if(S[i-1]==SimbInd){
      for(S[i++]=B;(B=*sp++)&&(B!=')');) S[i++]=B;
      S[i++]=B;
      goto L; //!!!
     }
    break;
   case  ')': case  ',': case  '*': case  '/': case  '^': case  '>':
   case  '<': case  '!': case  '?': case  ';':
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
      sprintf(Inf,"В числе 2 буквы 'E'");
      Application->MessageBox(Inf,"Ошибка!",MB_OK); return -1;
    }}
    S[i++]=B; goto L;
   case  '.':
    if(Numb){ Chto=Double;
     if(++KT>1){
      sprintf(Inf,"В числе 2 точки");
      Application->MessageBox(Inf,"Ошибка!",MB_OK); return -1;
    }}
    S[i++]=B; goto L;
   default  :
//    if((isalpha(B)||isciril(B))&&Numb){
//     sprintf(Inf,"Если это число, то ожидалась цифра\n"
//      "или символы: '.', 'E', 'e', ';'");
//     Application->MessageBox(Inf,"Ошибка!",MB_OK); return -1;
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

int LogZnac(){ int i; char B1=B;
 if(!(B=*sp++)) return -1;
 switch(B1){
  case '<': if(B=='=') return 3; i=1; break;
  case '>': if(B=='=') return 4; i=2; break;
  case '/': if(B=='=') return 5; i=0; break;
  default :
   sprintf(Inf,"Ожидалось: '<', '>', '<=', '>=', '/='");
   Application->MessageBox(Inf,"Ошибка!",MB_OK);
   i=0;
 }
 B=B1; sp--; return i;
}

Form ReadForm(int Prio){
 Cnst *C; Form F,U,V,FT,FF; signed int Oper; Vary *VV; int KS=0/*,Flag*/; char *Shtr;
 switch(ReadStr(0)){
  case '-': U=ReadForm(6); if(U.C==Pust) return U; F=-U; break;
  case '(':
   KS++;
   F=ReadForm(0);
   if(F.C==Pust)
    return F;
   break;
  case NAME:
   if(B!='('){
    if(FindCnst(S,&C)) F.C=C;
    else{
     for(Shtr=S;Shtr=strchr(Shtr+1,'\'');*Shtr='\''){
      *Shtr='\0'; VV=TakeVary(S); if(!(VV->Line)) VV->Line=Line;
     }
     F.V=TakeVary(S); if(!(F.V->Line)) F.V->Line=Line;
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
      if(!strcmp(S,"ipl")){ Arra *A;
       if(!ReadStr(0)) return F;
       if(B==',') A=TakeArra(S);
       U=ReadForm(0); if(U.C==Pust) return U;
       F.I=TakeIpll(1,A,U);
      }
      else if(!strcmp(S,"spln")){ Arra *A;
       if(!ReadStr(0)) return F;
       if(B==',')/*{*/ A=TakeArra(S);
       U=ReadForm(0); if(U.C==Pust) return U;
       F.P=TakeSpln(1,A,U);
      }
      else if(!strcmp(S,"summ")){ int Rez;
       FF=ReadForm(0); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0))) return F;
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
      else if(!strcmp(S,"intg")){ int Rez;
       FF=ReadForm(0); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0))) return F;
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
      else if(!strcmp(S,"root")){ int Rez;
       FF=ReadForm(0); if(FF.C==Pust) return FF;
       if(!(Rez=ReadStr(0))) return F;
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
      else if(!strcmp(S,"rest")){
       U=ReadForm(0); if(U.C==Pust) return U;
	     if(B==','){
        V=ReadForm(0); if(V.C==Pust) return V;
        F.O=TakeRest(U,V); break;
       }
       return F;
      }
      else{
       sprintf(Inf,"Неопознанная унарная операция");
       Application->MessageBox(Inf,"Ошибка!",MB_OK); F.C=Pust; return F;
   }} }
   break;
  case Long: case Double:
   if(IsLiters(S)){
    if(!FindCnst(S,&(F.C))){
     sprintf(Inf,"Если это число, то ожидалась цифра\n"
      "или символы: '.', 'E', 'e', ';'");
     Application->MessageBox(Inf,"Ошибка!",MB_OK);
     F.C=Pust;
   }}
   else F.C=TakeCnst(S);
   break;
  case PUST: case -1:
   F.C=Pust;
   return F;
 }
 for(;;) switch(B){
  case ')':
   if(KS){ KS--; B=*sp++; while(B==' ') B=*sp++; break; }
   return F;
  case  '?': case '!': return F;
  case  '=':
   sprintf(Inf,"Ожидалось: '+', '-', '*', '/', '^', '<', '>', '<=', '>=', '/='");
   Application->MessageBox(Inf,"Ошибка!",MB_OK);
   F.C=Pust; return F;
  case  '/': case  '<': case  '>':
   if((Oper=LogZnac())>0){
    V=ReadForm(0);
    if(V.C==Pust) return V;
    if(B=='?'){
     FT=ReadForm(0);
     if(FT.C==Pust) return FT;
    }
    else{
     sprintf(Inf,"Ожидалось: '?'");
     Application->MessageBox(Inf,"Ошибка!",MB_OK);
     F.C=Pust; return F;
    }
    if(B=='!'){
     FF=ReadForm(0);
     if(FF.C==Pust) return FF;
    }
    else{
     sprintf(Inf,"Ожидалось: '!'");
     Application->MessageBox(Inf,"Ошибка!",MB_OK); F.C=Pust; return F;
    }
    F.L=TakeIfop(Oper,F,V,FT,FF); return F;
   }
  default:
   if(Oper=Znac(B)){
    if(Prio>=Oper) return F;
    switch(Oper){
     case  1:
      U=ReadForm(Oper);
      if(U.C==Pust)
       return U;
      F=F+U;
      break;
     case  2: U=ReadForm(Oper); if(U.C==Pust) return U; F=F-U; break;
     case  3: U=ReadForm(Oper); if(U.C==Pust) return U; F=F*U; break;
     case  4:
      U=ReadForm(Oper);
      if(U.C==Pust)
       return U;
      F=F/U;
      break;
     case  5: U=ReadForm(Oper); if(U.C==Pust) return U; F=F^U; break;
     default:
      sprintf(Inf,"Неопознанный символ - '%c'",B);
      Application->MessageBox(Inf,"Ошибка!",MB_OK); F.C=Pust; return F;
   }}
   else return F;
}}

Form StrToForm(char *Sp){
 Form F; int Kl,Kp; char Buf[80]="Нарушен баланс скобок"; sp=Sp;
 if(Sp&&*Sp){
  Kl=KSimbStr(Sp,'('); Kp=KSimbStr(Sp,')');
  if(Kl!=Kp){
   if(Kl>Kp) sprintf(Inf,"Левых скобок больше, чем правых:\n%s",Sp);
   else      sprintf(Inf,"Правых скобок больше, чем левых:\n%s",Sp);
   Application->MessageBox(Inf,Buf,MB_OK); IsError=true;
  }
  else return ReadForm(0);
 }
 F.C=Pust; return F;
}

