#include "CA.h"
#include <string.h>
#include "ConvDW.h"

		    /*  Œ·˙ˇ‚ÎÂÌËˇ Ó·˙ÂÍÚÓ‚:  */
extern char Inf[],ZNAC[],LOGZNAC[],SimbInd,*NameFunc[]; extern List *L;
extern int Line,Pos,StrLong; extern Cnst *Pust;
extern TRichEdit *RichEdit;
static int k,l,Ksk;
char Sk[11]; 

int GrekToHTM(char *Buf,char *Kod,char *s){
 char *b; int L,l;
 L=sprintf(b=Buf,
    "<font face=\"Times New Roman, Times, serif\">%s</font>",Kod); b+=L;
 if(*s){
  l=sprintf(b,"<sub><font size=\"-1\">%s</font></sub>",s); L+=l;
 }
 return L;
}

int LatToHTM(char *Buf,char *S){ char C='\0',*s,*b=Buf; int L=0,l;
 if((S[0]=='x'||S[0]=='y'||S[0]=='z'||S[0]=='X'||S[0]=='Y'||S[0]=='Z')){
  L+=(l=sprintf(b,"<i>%c</i><sub><font size=\"-1\">",S[0])); b+=l;
  l=LatToHTM(b,S+1); L+=l; b+=l;
  L+=sprintf(b,"</font></sub>"); 
  return L;
 }
 if(!isdigit(S[0])){
  for(s=S;*s;s++)
   if(isdigit(*s)&&(s[-1]!='[')&&!isdigit(s[-1])){ C=*s; *s='\0'; break; }
  for(s=S;*s;){
   if(isalpha(*s)){
    b+=sprintf(b,"<i>"); while(*s&&isalpha(*s)){ *b++=*s++; L++; }
    b+=sprintf(b,"</i>"); L+=7;
   }
   else while(*s&&!isalpha(*s)){ *b++=*s++; L++; }
  }
  *b='\0';
  if(C){ *s=C; L+=sprintf(b,"<sub><font size=\"-1\">%s</font></sub>",s); }
  return L;
 }
 return sprintf(b,"%s",S);
}

int Grek(char *S){
 char *s=S,*s0=s,Buf[1024]="\0",*b=Buf,B,Name[1024],Txt[256]; int k=0,Ind=0;
 while(B=*s0){
  if((B==SimbInd)||(B=='\'')||(B=='.')||(B=='+')||(B=='-')||(B=='*')||
     (B=='/')||(B==' ')||(B=='@')||(B=='π')||(B=='∞')||(B=='(')||(B==')')){
   *s0++='\0'; strcpy(Name,s); LowToUpp(Name);
        if(strstr(Name,   "ALFA")==Name){ b+=GrekToHTM(b,"&#945;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,  "ALPHA")==Name){ b+=GrekToHTM(b,"&#945;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "¿À‹‘¿")==Name){ b+=GrekToHTM(b,"&#945;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,   "BETA")==Name){ b+=GrekToHTM(b,"&#946;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "¡≈“¿")==Name){ b+=GrekToHTM(b,"&#946;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,  "BETTA")==Name){ b+=GrekToHTM(b,"&#946;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "¡≈““¿")==Name){ b+=GrekToHTM(b,"&#946;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "GAMMA")==Name){ b+=GrekToHTM(b,"&#947;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "√¿ÃÃ¿")==Name){ b+=GrekToHTM(b,"&#947;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "DELTA")==Name){ b+=GrekToHTM(b,"&#948;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name, "ƒ≈À‹“¿")==Name){ b+=GrekToHTM(b,"&#948;",s+6); k+=strlen(s+6)+1; }
   else if(strstr(Name,"EPSILON")==Name){ b+=GrekToHTM(b,"&#949;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,"≈œ—»ÀŒÕ")==Name){ b+=GrekToHTM(b,"&#949;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,  "DZETA")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "DSETA")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "ƒ«≈“¿")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,    "ETA")==Name){ b+=GrekToHTM(b,"&#951;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,    "›“¿")==Name){ b+=GrekToHTM(b,"&#951;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,   "“≈“¿")==Name){ b+=GrekToHTM(b,"&#952;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "TETA")==Name){ b+=GrekToHTM(b,"&#952;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "…Œ“¿")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "»Œ“¿")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "JOTA")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,   "IOTA")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
   else if(strstr(Name,  " ¿œœ¿")==Name){ b+=GrekToHTM(b,"&#954;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "KAPPA")==Name){ b+=GrekToHTM(b,"&#954;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "ÀﬂÃƒ¿")==Name){ b+=GrekToHTM(b,"&#955;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "LAMDA")==Name){ b+=GrekToHTM(b,"&#955;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name, "ÀﬂÃ¡ƒ¿")==Name){ b+=GrekToHTM(b,"&#955;",s+6); k+=strlen(s+6)+1; }
   else if(strstr(Name, "LAMBDA")==Name){ b+=GrekToHTM(b,"&#955;",s+6); k+=strlen(s+6)+1; }
   else if(strstr(Name,     "MU")==Name){ b+=GrekToHTM(b,"&#956;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "Ãﬁ")==Name){ b+=GrekToHTM(b,"&#956;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "NU")==Name){ b+=GrekToHTM(b,"&#957;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "Õﬁ")==Name){ b+=GrekToHTM(b,"&#957;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,    "KSI")==Name){ b+=GrekToHTM(b,"&#958;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,    " —»")==Name){ b+=GrekToHTM(b,"&#958;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,"OMICRON")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,"OMIKRON")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,"ŒÃ» –ŒÕ")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,     "PI")==Name){ b+=GrekToHTM(b,"&#960;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "œ»")==Name){ b+=GrekToHTM(b,"&#960;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "RO")==Name){ b+=GrekToHTM(b,"&#961;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "–Œ")==Name){ b+=GrekToHTM(b,"&#961;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,  "SIGMA")==Name){ b+=GrekToHTM(b,"&#963;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "—»√Ã¿")==Name){ b+=GrekToHTM(b,"&#963;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,    "TAU")==Name){ b+=GrekToHTM(b,"&#964;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,    "“¿”")==Name){ b+=GrekToHTM(b,"&#964;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,"IPSILON")==Name){ b+=GrekToHTM(b,"&#965;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,"»œ—»ÀŒÕ")==Name){ b+=GrekToHTM(b,"&#965;",s+7); k+=strlen(s+7)+1; }
   else if(strstr(Name,     "FI")==Name){ b+=GrekToHTM(b,"&#966;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "‘»")==Name){ b+=GrekToHTM(b,"&#966;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "HI")==Name){ b+=GrekToHTM(b,"&#967;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,     "’»")==Name){ b+=GrekToHTM(b,"&#967;",s+2); k+=strlen(s+2)+1; }
   else if(strstr(Name,    "PSI")==Name){ b+=GrekToHTM(b,"&#968;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,    "œ—»")==Name){ b+=GrekToHTM(b,"&#968;",s+3); k+=strlen(s+3)+1; }
   else if(strstr(Name,  "OMEGA")==Name){ b+=GrekToHTM(b,"&#969;",s+5); k+=strlen(s+5)+1; }
   else if(strstr(Name,  "ŒÃ≈√¿")==Name){ b+=GrekToHTM(b,"&#969;",s+5); k+=strlen(s+5)+1; }
   else{ b+=LatToHTM(b,s); k+=strlen(s); }
   if(B==SimbInd){
    if(!Ind){
     if(*s0=='('){ s0++; Ind++; }
     b+=sprintf(b,"<sub><font size=\"-1\">"); Ind++;
    }
    else if(Ind>1){
     sprintf(Txt,"ÕÂ Á‡Í˚Ú Ó‰ËÌ ËÌ‰ÂÍÒ, ÓÚÍ˚‚‡ÂÚÒˇ ‰Û„ÓÈ.\n"
      "ÕÂ ı‚‡Ú‡ÂÚ Á‡Í˚ÚÓÈ ÍÛ„ÎÓÈ ÒÍÓ·ÍË!");
     ErrorSynt(RichEdit,Txt,2);
   }}
   else{
    if((Ind==1)/*||(B==')')*/){
     b+=sprintf(b,"</font></sub>");
     if(Ind==1) *b++=B;
     Ind=0;
    }
    else *b++=B;
   }
   s=s0; k++;
  }
  else s0++;
 }
M:strcpy(Name,s); LowToUpp(Name);
      if(strstr(Name,   "ALFA")==Name){ b+=GrekToHTM(b,"&#945;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,  "ALPHA")==Name){ b+=GrekToHTM(b,"&#945;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "¿À‹‘¿")==Name){ b+=GrekToHTM(b,"&#945;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,   "BETA")==Name){ b+=GrekToHTM(b,"&#946;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "¡≈“¿")==Name){ b+=GrekToHTM(b,"&#946;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,  "BETTA")==Name){ b+=GrekToHTM(b,"&#946;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "¡≈““¿")==Name){ b+=GrekToHTM(b,"&#946;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "GAMMA")==Name){ b+=GrekToHTM(b,"&#947;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "√¿ÃÃ¿")==Name){ b+=GrekToHTM(b,"&#947;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "DELTA")==Name){ b+=GrekToHTM(b,"&#948;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name, "ƒ≈À‹“¿")==Name){ b+=GrekToHTM(b,"&#948;",s+6); k+=strlen(s+6)+1; }
 else if(strstr(Name,"EPSILON")==Name){ b+=GrekToHTM(b,"&#949;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,"≈œ—»ÀŒÕ")==Name){ b+=GrekToHTM(b,"&#949;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,  "DZETA")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "DSETA")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "ƒ«≈“¿")==Name){ b+=GrekToHTM(b,"&#950;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,    "ETA")==Name){ b+=GrekToHTM(b,"&#951;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,    "›“¿")==Name){ b+=GrekToHTM(b,"&#951;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,   "“≈“¿")==Name){ b+=GrekToHTM(b,"&#952;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "TETA")==Name){ b+=GrekToHTM(b,"&#952;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "…Œ“¿")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "»Œ“¿")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "JOTA")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,   "IOTA")==Name){ b+=GrekToHTM(b,"&#953;",s+4); k+=strlen(s+4)+1; }
 else if(strstr(Name,  " ¿œœ¿")==Name){ b+=GrekToHTM(b,"&#954;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "KAPPA")==Name){ b+=GrekToHTM(b,"&#954;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "ÀﬂÃƒ¿")==Name){ b+=GrekToHTM(b,"&#955;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "LAMDA")==Name){ b+=GrekToHTM(b,"&#955;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name, "ÀﬂÃ¡ƒ¿")==Name){ b+=GrekToHTM(b,"&#955;",s+6); k+=strlen(s+6)+1; }
 else if(strstr(Name, "LAMBDA")==Name){ b+=GrekToHTM(b,"&#955;",s+6); k+=strlen(s+6)+1; }
 else if(strstr(Name,     "MU")==Name){ b+=GrekToHTM(b,"&#956;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "Ãﬁ")==Name){ b+=GrekToHTM(b,"&#956;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "NU")==Name){ b+=GrekToHTM(b,"&#957;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "Õﬁ")==Name){ b+=GrekToHTM(b,"&#957;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,    "KSI")==Name){ b+=GrekToHTM(b,"&#958;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,    " —»")==Name){ b+=GrekToHTM(b,"&#958;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,"OMICRON")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,"OMIKRON")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,"ŒÃ» –ŒÕ")==Name){ b+=GrekToHTM(b,"&#959;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,     "PI")==Name){ b+=GrekToHTM(b,"&#960;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "œ»")==Name){ b+=GrekToHTM(b,"&#960;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "RO")==Name){ b+=GrekToHTM(b,"&#961;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "–Œ")==Name){ b+=GrekToHTM(b,"&#961;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,  "SIGMA")==Name){ b+=GrekToHTM(b,"&#963;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "—»√Ã¿")==Name){ b+=GrekToHTM(b,"&#963;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,    "TAU")==Name){ b+=GrekToHTM(b,"&#964;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,    "“¿”")==Name){ b+=GrekToHTM(b,"&#964;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,"IPSILON")==Name){ b+=GrekToHTM(b,"&#965;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,"»œ—»ÀŒÕ")==Name){ b+=GrekToHTM(b,"&#965;",s+7); k+=strlen(s+7)+1; }
 else if(strstr(Name,     "FI")==Name){ b+=GrekToHTM(b,"&#966;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "‘»")==Name){ b+=GrekToHTM(b,"&#966;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "HI")==Name){ b+=GrekToHTM(b,"&#967;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,     "’»")==Name){ b+=GrekToHTM(b,"&#967;",s+2); k+=strlen(s+2)+1; }
 else if(strstr(Name,    "PSI")==Name){ b+=GrekToHTM(b,"&#968;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,    "œ—»")==Name){ b+=GrekToHTM(b,"&#968;",s+3); k+=strlen(s+3)+1; }
 else if(strstr(Name,  "OMEGA")==Name){ b+=GrekToHTM(b,"&#969;",s+5); k+=strlen(s+5)+1; }
 else if(strstr(Name,  "ŒÃ≈√¿")==Name){ b+=GrekToHTM(b,"&#969;",s+5); k+=strlen(s+5)+1; }
 else{ b+=LatToHTM(b,s); k+=strlen(s); }
 if(Ind){ b+=sprintf(b,"</font></sub>"); Ind=0; }
 strcpy(S,Buf);
 return k;
}

bool IsGrek(char *S){
 char Name[80]; strcpy(Name,S); LowToUpp(Name);
      if(strstr(Name,   "ALFA")==Name) return true;
 else if(strstr(Name,  "ALPHA")==Name) return true;
 else if(strstr(Name,  "¿À‹‘¿")==Name) return true;
 else if(strstr(Name,   "BETA")==Name) return true;
 else if(strstr(Name,   "¡≈“¿")==Name) return true;
 else if(strstr(Name,  "BETTA")==Name) return true;
 else if(strstr(Name,  "¡≈““¿")==Name) return true;
 else if(strstr(Name,  "GAMMA")==Name) return true;
 else if(strstr(Name,  "√¿ÃÃ¿")==Name) return true;
 else if(strstr(Name,  "DELTA")==Name) return true;
 else if(strstr(Name, "ƒ≈À‹“¿")==Name) return true;
 else if(strstr(Name,"EPSILON")==Name) return true;
 else if(strstr(Name,"≈œ—»ÀŒÕ")==Name) return true;
 else if(strstr(Name,  "DZETA")==Name) return true;
 else if(strstr(Name,  "DSETA")==Name) return true;
 else if(strstr(Name,  "ƒ«≈“¿")==Name) return true;
 else if(strstr(Name,    "ETA")==Name) return true;
 else if(strstr(Name,    "›“¿")==Name) return true;
 else if(strstr(Name,   "“≈“¿")==Name) return true;
 else if(strstr(Name,   "TETA")==Name) return true;
 else if(strstr(Name,   "…Œ“¿")==Name) return true;
 else if(strstr(Name,   "»Œ“¿")==Name) return true;
 else if(strstr(Name,   "JOTA")==Name) return true;
 else if(strstr(Name,   "IOTA")==Name) return true;
 else if(strstr(Name,  " ¿œœ¿")==Name) return true;
 else if(strstr(Name,  "KAPPA")==Name) return true;
 else if(strstr(Name,  "ÀﬂÃƒ¿")==Name) return true;
 else if(strstr(Name,  "LAMDA")==Name) return true;
 else if(strstr(Name, "ÀﬂÃ¡ƒ¿")==Name) return true;
 else if(strstr(Name, "LAMBDA")==Name) return true;
 else if(strstr(Name,     "MU")==Name) return true;
 else if(strstr(Name,     "Ãﬁ")==Name) return true;
 else if(strstr(Name,     "NU")==Name) return true;
 else if(strstr(Name,     "Õﬁ")==Name) return true;
 else if(strstr(Name,    "KSI")==Name) return true;
 else if(strstr(Name,    " —»")==Name) return true;
 else if(strstr(Name,"OMICRON")==Name) return true;
 else if(strstr(Name,"OMIKRON")==Name) return true;
 else if(strstr(Name,"ŒÃ» –ŒÕ")==Name) return true;
 else if(strstr(Name,     "PI")==Name) return true;
 else if(strstr(Name,     "œ»")==Name) return true;
 else if(strstr(Name,     "RO")==Name) return true;
 else if(strstr(Name,     "–Œ")==Name) return true;
 else if(strstr(Name,  "SIGMA")==Name) return true;
 else if(strstr(Name,  "—»√Ã¿")==Name) return true;
 else if(strstr(Name,    "TAU")==Name) return true;
 else if(strstr(Name,    "“¿”")==Name) return true;
 else if(strstr(Name,"IPSILON")==Name) return true;
 else if(strstr(Name,"»œ—»ÀŒÕ")==Name) return true;
 else if(strstr(Name,     "FI")==Name) return true;
 else if(strstr(Name,     "‘»")==Name) return true;
 else if(strstr(Name,     "HI")==Name) return true;
 else if(strstr(Name,     "’»")==Name) return true;
 else if(strstr(Name,    "PSI")==Name) return true;
 else if(strstr(Name,    "œ—»")==Name) return true;
 else if(strstr(Name,  "OMEGA")==Name) return true;
 else if(strstr(Name,  "ŒÃ≈√¿")==Name) return true;
 return false;
}

void PF(bool DOS,FILE *OUTFILE,int a,int Prio,Form F) {
 char *s,Beg,End,Buf[1024],NameV[256]; int i; bool DRAWSK;
 switch(F.C->Atr){
  case PUST: putc('$',OUTFILE); k++; break;
  case CNST:
   if(F.C->Name){
    if(DOS){
     strcpy(Buf,F.C->Name); for(s=Buf;*s&&*s==' ';s++); StrWINtoDOS(s);
    }
    else{
     if(!strcmp(F.C->Name,"2PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">2&#960;</font>");
      k+=2;
     }
     else if(!strcmp(F.C->Name,"3PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">3&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"4PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">4&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"5PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">5&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"6PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">6&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"7PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">7&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"PI")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"pi")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;</font>");
      k++;
     }
     else if(!strcmp(F.C->Name,"PIna2")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;/2</font>");
      k+=3;
     }
     else if(!strcmp(F.C->Name,"PIna3")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;/3</font>");
      k+=3;
     }
     else if(!strcmp(F.C->Name,"PIna4")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;/4</font>");
      k+=3;
     }
     else if(!strcmp(F.C->Name,"PIna6")){
      s=strcpy(Buf,"<font face=\"Times New Roman, Times, serif\">&#960;/6</font>");
      k+=3;
     }
     else{ s=F.C->Name; k+=strlen(s); }
    }
    fprintf(OUTFILE,s);
   }
   else k+=fprintf(OUTFILE,"%g",F.C->Val); break;
  case VARY:
   strcpy(Buf,F.V->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   break;
  case UNOP:
   if((F.U->Func==2)&&!DOS){ fprintf(OUTFILE,"&#8730;"); k++; }
   else{
    if(DOS||F.U->Func!=1)
     k+=fprintf(OUTFILE,NameFunc[F.U->Func]);
    else
     k+=fprintf(OUTFILE,"&#9472;"); // 8722 8211
   }
   if((F.U->Func==1)&&
      ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
       ((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
       ((F.U->Arg.C->Atr==BNOP)&&(F.U->Arg.B->Oper>2))||
       (F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
    PF(DOS,OUTFILE,a,0,F.U->Arg);
   }
   else
    if(!DOS&&(F.U->Func==2)&&
       ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||(F.U->Arg.C->Atr==UNOP)))
     PF(DOS,OUTFILE,a,0,F.U->Arg);
   else
    if(!DOS&&(F.U->Func>2)&&(F.U->Func<6)&&
       (F.U->Arg.C->Atr==VARY)&&IsGrek(F.U->Arg.V->Name)){
     fputc(' ',OUTFILE); PF(DOS,OUTFILE,a,0,F.U->Arg);
   }
   else{ //putc('(',OUTFILE);
    switch((++Ksk)%3){
     case 1: putc('(',OUTFILE); break;
     case 2: putc('[',OUTFILE); break;
     case 0: putc('{',OUTFILE); break;
    }
    k++; PF(DOS,OUTFILE,a,0,F.U->Arg);
//    putc(')',OUTFILE);
    switch((Ksk--)%3){
     case 1: putc(')',OUTFILE); break;
     case 2: putc(']',OUTFILE); break;
     case 0: putc('}',OUTFILE); break;
    }
    k++;
   }
   break;
  case BNOP : i=F.B->Oper;
   if((i<Prio)||((i==Prio)&&(i==2))){
    switch((++Ksk)%3){
     case 1: putc('(',OUTFILE); break;
     case 2: putc('[',OUTFILE); break;
     case 0: putc('{',OUTFILE); break;
    }
    k++;
   }
   if(DOS||i<5){
//    PF(DOS,OUTFILE,a,(i+1)/2,F.B->L); putc(ZNAC[i],OUTFILE); k++;
    PF(DOS,OUTFILE,a,i,F.B->L);
    if(DOS||i!=2) putc(ZNAC[i],OUTFILE);
    else fputs("&#9472;",OUTFILE); // 8722 8211
    k++;
   }
   else if((F.B->L.C->Atr==4)&&!strchr(F.B->L.V->Name,'\'')){
//    PF(DOS,OUTFILE,a,(i+1)/2,F.B->L); fputs("<sup><font size=\"-1\">",OUTFILE);
    PF(DOS,OUTFILE,a,i,F.B->L); fputs("<sup><font size=\"-1\">",OUTFILE);
   }
   else{
    if(F.B->L.C->Atr==INTG){
     switch((++Ksk)%3){
      case 1: putc('(',OUTFILE); break;
      case 2: putc('[',OUTFILE); break;
      case 0: putc('{',OUTFILE); break;
     }
     k++;
    }
    PF(DOS,OUTFILE,a,i,F.B->L);
    if(F.B->L.C->Atr==INTG){
     switch((Ksk--)%3){
      case 1: putc(')',OUTFILE); break;
      case 2: putc(']',OUTFILE); break;
      case 0: putc('}',OUTFILE); break;
     }
     k++;
    }
/*    switch((++Ksk)%3){
     case 1: putc('(',OUTFILE); break;
     case 2: putc('[',OUTFILE); break;
     case 0: putc('{',OUTFILE); break;
    }
    k++;*/
//    PF(DOS,OUTFILE,a,(i+1)/2,F.B->L);
//    PF(DOS,OUTFILE,a,i,F.B->L);
/*    switch((Ksk--)%3){
     case 1: putc(')',OUTFILE); break;
     case 2: putc(']',OUTFILE); break;
     case 0: putc('}',OUTFILE); break;
    }
    k++;*/
    fputs("<sup><font size=\"-1\">",OUTFILE);
   }
   if((k>l)&&(i<3)) {
    if(DOS){
     fputc('\n',OUTFILE); for(int j=0;j<=a;j++) fputc(' ',OUTFILE);
    }
    else{
     fputs("<br>",OUTFILE); for(int j=0;j<=2*a;j++) fputs("&nbsp;",OUTFILE);
    }
    k=a+1;
   }
//   PF(DOS,OUTFILE,a,(i+1)/2,F.B->R);
   if(F.B->R.C->Atr==INTG){
    switch((++Ksk)%3){
     case 1: putc('(',OUTFILE); break;
     case 2: putc('[',OUTFILE); break;
     case 0: putc('{',OUTFILE); break;
    }
    k++;
   }
   PF(DOS,OUTFILE,a,i,F.B->R);
   if(F.B->R.C->Atr==INTG){
    switch((Ksk--)%3){
     case 1: putc(')',OUTFILE); break;
     case 2: putc(']',OUTFILE); break;
     case 0: putc('}',OUTFILE); break;
    }
    k++;
   }
   if((!DOS)&&(i==5)/*&&(F.B->L.C->Atr==4)&&!strchr(F.B->L.V->Name,'\'')*/)
    fputs("</sup></font>",OUTFILE);
//   if((i<Prio)){//||((i==Prio)&&((i==2)||(i==4)))){
//   if(i<Prio){ //putc(')',OUTFILE);
   if((i<Prio)||((i==Prio)&&(i==2))){
    switch((Ksk--)%3){
     case 1: putc(')',OUTFILE); break;
     case 2: putc(']',OUTFILE); break;
     case 0: putc('}',OUTFILE); break;
    }
    k++;
   }
   break;
  case IFOP: //putc('(',OUTFILE);
   switch((++Ksk)%3){
    case 1: putc('(',OUTFILE); break;
    case 2: putc('[',OUTFILE); break;
    case 0: putc('{',OUTFILE); break;
   }
   k++; PF(DOS,OUTFILE,a,0,F.L->U);
   switch(F.L->Oper){
    case 1:// <
     if(DOS) putc(LOGZNAC[F.L->Oper],OUTFILE);
     else fputs("&#60;",OUTFILE);
     break;
    case 2:// >
     if(DOS) putc(LOGZNAC[F.L->Oper],OUTFILE);
     else fputs("&#62;",OUTFILE);
     break;
    case 3:// <=
     if(DOS) fprintf(OUTFILE,"%c=",LOGZNAC[F.L->Oper-2]);
     else fputs("&#8804;",OUTFILE);
     break;
    case 4:// >=
     if(DOS) fprintf(OUTFILE,"%c=",LOGZNAC[F.L->Oper-2]);
     else fputs("&#8805;",OUTFILE);
     break;
    case 5:// /=
     if(DOS) fprintf(OUTFILE,"%c=",LOGZNAC[F.L->Oper-2]);
     else fputs("&#8800;",OUTFILE);
     break;
    case 6:// /=
     if(DOS) fprintf(OUTFILE,"%c=",LOGZNAC[F.L->Oper-2]);
     else fputs("&#8801;",OUTFILE);
     break;
   }
/*   if(F.L->Oper<5){
    if(DOS) putc(LOGZNAC[F.L->Oper],OUTFILE);
    else{
     switch(LOGZNAC[F.L->Oper]){
      case '<': fputs("&#60;",OUTFILE); break;
      case '>': fputs("&#62;",OUTFILE); break;
      case '!': putc('!',OUTFILE);
    }}
    k++;
   }
   if(F.L->Oper>2){ putc('=',OUTFILE); k++;}*/
   PF(DOS,OUTFILE,a,0,F.L->V);  fputs(" ? ",OUTFILE); k+=3;
   PF(DOS,OUTFILE,a,0,F.L->FT); fputs(" ! ",OUTFILE); k+=3;
   PF(DOS,OUTFILE,a,0,F.L->FF);
//   putc(')',OUTFILE);
   switch((Ksk--)%3){
    case 1: putc(')',OUTFILE); break;
    case 2: putc(']',OUTFILE); break;
    case 0: putc('}',OUTFILE); break;
   }
   k++; break;
  case IPLL:
   switch(F.I->Type) {
    case 1: fputs("ipl(",OUTFILE); Beg=',';  End=')'; break;
    case 2: Beg='['; End=']';
   }
   strcpy(Buf,F.I->A->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   putc(Beg,OUTFILE); PF(DOS,OUTFILE,a,0,F.I->F); putc(End,OUTFILE); break;
  case SPLN:
   switch(F.P->Type) {
    case 1: fputs("spln(",OUTFILE); Beg=',';  End=')'; break;
    default: Beg='['; End=']';
   }
   strcpy(Buf,F.P->A->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   putc(Beg,OUTFILE); PF(DOS,OUTFILE,a,0,F.P->F); putc(End,OUTFILE); break;
  case SUMM:
   fputs("summ(",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.S->F); putc(',',OUTFILE);
   strcpy(Buf,F.S->i->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   putc(',',OUTFILE); PF(DOS,OUTFILE,a,0,F.S->a);
   putc(',',OUTFILE); PF(DOS,OUTFILE,a,0,F.S->b);
   putc(')',OUTFILE); break;
  case INTG:
   fputs("intg( ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.G->f); fputs(", ",OUTFILE);
   strcpy(Buf,F.G->x->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   if(F.G->a!=Pust){
    fputs(" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->a);
    fputs(" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->b);
   }
   fputs(" ) ",OUTFILE);
   if(F.G->F!=Pust){
    fputs(" = ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->F);
    if(F.G->Rez!=Pust){
     fputs(" = ",OUTFILE); PF(DOS,OUTFILE,a,0,F.G->Rez);
   }}
   break;
  case RUUT:
   fputs("root( ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.R->f); fputs(", ",OUTFILE);
   strcpy(Buf,F.R->x->Name);
   if(DOS){ StrWINtoDOS(Buf); k+=fprintf(OUTFILE,Buf); }
   else{ k+=Grek(Buf); fprintf(OUTFILE,Buf); }
   if(F.R->a!=Pust){
    fputs(" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.R->a);
    fputs(" , ",OUTFILE); PF(DOS,OUTFILE,a,0,F.R->b);
   }
   fputs(" ) ",OUTFILE);
   break;
  case REST:
   fputs("rest( ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.O->a); fputs(", ",OUTFILE);
   PF(DOS,OUTFILE,a,0,F.O->b);
   fputs(" ) ",OUTFILE);
   break;
}}

void UtilVars(Form F,Magazine **VarsInForm,bool ViewVal){
 Form FF; char *s,NameV[256]; Vary *V; int Ind;
//œÓˆÂ‰Û‡ Á‡ÔÓÎÌÂÌËˇ Ï‡„‡ÁËÌ‡ VarsInForm ËÏÂÌ‡ÏË ÔÂÂÏÂÌÌ˚ı, ‚ıÓ‰ˇ˘Ëı
//‚ ÙÓÏÛÎÛ F Ë ‚ ÔÂÂÏÂÌÌ˚Â Ë ÓÔÂ‡ˆËË, ‚ıÓ‰ˇ˘ËÂ ‚ ˝ÚÛ ÙÓÏÛÎÛ
 switch(F.C->Atr) {
  case PUST:
  case CNST: break;
  case VARY:
   if(s=strchr(F.V->Name,SimbInd)){
    *s='\0';
    if(FindVary(s+1,&V)){
     Ind=Round(V->P>=0?V->Val:Val(V->Znach));
     sprintf(NameV,"%s%d",F.V->Name,Ind);
     if(FindVary(NameV,&V)){
      if(ViewVal) sprintf(Inf,"%s=%s=%g",F.V->Name,V->Name,V->Val);
      else sprintf(Inf,"%s=%s",F.V->Name,V->Name);
     }
     else sprintf(Inf,"%s=%s???",F.V->Name,NameV);
    }
    else sprintf(Inf,"%s???",F.V->Name);
    *s=SimbInd;
   }
   else{
    if(ViewVal) sprintf(Inf,"%s=%g",F.V->Name,F.V->Val);
    else sprintf(Inf,"%s",F.V->Name);
   }
   TakeMagazine(VarsInForm,Inf);
   UtilVars(F.V->Znach,VarsInForm,ViewVal); break;
  case UNOP: UtilVars(F.U->Arg,VarsInForm,ViewVal); break;
  case BNOP:
   UtilVars(F.B->L,VarsInForm,ViewVal); UtilVars(F.B->R,VarsInForm,ViewVal);
   break;
  case IFOP:
   UtilVars(F.L->U,VarsInForm,ViewVal);  UtilVars(F.L->V,VarsInForm,ViewVal);
   UtilVars(F.L->FT,VarsInForm,ViewVal); UtilVars(F.L->FF,VarsInForm,ViewVal);
   break;
  case IPLL:
   UtilVars(F.I->F,VarsInForm,ViewVal); break;
  case SPLN:
   UtilVars(F.P->F,VarsInForm,ViewVal); break;
  case SUMM:
   FF.V=F.S->i; UtilVars(FF,VarsInForm,ViewVal);
   UtilVars(F.S->a,VarsInForm,ViewVal); UtilVars(F.S->b,VarsInForm,ViewVal);
   UtilVars(F.S->F,VarsInForm,ViewVal);
   break;
  case INTG:
   FF.V=F.G->x; UtilVars(FF,VarsInForm,ViewVal);
   UtilVars(F.G->a,VarsInForm,ViewVal); UtilVars(F.G->b,VarsInForm,ViewVal);
   UtilVars(F.G->f,VarsInForm,ViewVal); break;
}}

int PrintForm(bool DOS,FILE *OUTFILE,wchar_t *NameF,Form F) {
 char Buf[1024]; k=0; Ksk=0; l=DOS?72:StrLong;
 if(NameF){
//  fputc('\n',OUTFILE);
  if(DOS){ Fputs(NameF,OUTFILE); k=strlen(NameF); }
  else{
   strcpy(Buf,NameF); k=Grek(Buf); fprintf(OUTFILE,"<b>%s</b>",Buf);
  }
  fputs(" = ",OUTFILE); k+=3;
 }
 PF(DOS,OUTFILE,k,0,F);
 if(NameF){ putc(';',OUTFILE); k++; }
 return k;
}

void KOp(Form F,int *KolOp){
 switch(F.C->Atr){
  case PUST: case CNST: break;
  case VARY:
   if(!F.V->O){ KOp(F.V->Znach,KolOp); F.V->O=1; } break;
  case UNOP:
   if(!F.U->O){
    if(F.U->Func!=1) (KolOp[5])++;
    KOp(F.U->Arg,KolOp); F.U->O=1;
   }
   break;
  case BNOP :
   switch(F.B->Oper){
    case 1: (KolOp[0])++; break;
    case 2: (KolOp[1])++; break;
    case 3: (KolOp[2])++; break;
    case 4: (KolOp[3])++; break;
    case 5: (KolOp[4])++; break;
   }
   KOp(F.B->L,KolOp);
   KOp(F.B->R,KolOp);
  case IFOP: case IPLL: case SPLN: case SUMM: case INTG: break;
}}

void ClearOper(void){
 L->Time->O=L->Freq->O=0;
 for(Vary *V=L->V;V;V=V->Sled) V->O=0;
 for(Unop *U=L->U;U;U=U->Sled) U->O=0;
}

void PrintOper(bool DOS,FILE *OUTFILE,Form F,int *KolOp){
 char Buf[256]; int *KO=(int *)calloc(6,SzI);
 KOp(F,KO);
 sprintf(Buf,
  "\nŒÔÂ‡ˆËÈ: \"+\"=%d, \"-\"=%d, \"*\"=%d, \":\"=%d, \"^\"=%d, \"ÙÛÌÍˆËÈ\"=%d",
  KO[0],KO[1],KO[2],KO[3],KO[4],KO[5]);
 if(DOS){ Fputs(Buf,OUTFILE); fputc('\n',OUTFILE); }
 else{ fputs(Buf,OUTFILE); fputs("<br>",OUTFILE); }
 if(KolOp){ for(int i=0;i<6;i++) KolOp[i]+=KO[i]; free(KO); }
}

void FormToStr(Form F,int Prio,AnsiString &S){
 int i; char Beg,End; char Buf[256];
 switch(F.C->Atr){
  case PUST: S+="$"; break;
  case CNST:
   if(F.C->Name) S+=F.C->Name; else{ sprintf(Buf,"%g",F.C->Val); S+=Buf; }
   break;
  case VARY: S+=F.V->Name; break;
  case UNOP:
   S+=NameFunc[F.U->Func];
   if((F.U->Func==1)&&
      ((F.U->Arg.C->Atr==CNST)||(F.U->Arg.C->Atr==VARY)||
       ((F.U->Arg.C->Atr==UNOP)&&(F.U->Arg.U->Func!=1))||
       (F.U->Arg.C->Atr==IPLL)||(F.U->Arg.C->Atr==SPLN))){
    FormToStr(F.U->Arg,0,S);
   }
   else{
    switch((++Ksk)%3){
     case 1: S+="("; break; case 2: S+="["; break; case 0: S+="{"; break;
    }
    FormToStr(F.U->Arg,0,S);
    switch((Ksk--)%3){
     case 1: S+=")"; break; case 2: S+="]"; break; case 0: S+="}"; break;
   }}
   break;
  case BNOP : i=F.B->Oper;
   if((i<Prio)||((i==Prio)&&((i==2)||(i==4)))){
    switch((++Ksk)%3){
     case 1: S+="("; break; case 2: S+="["; break; case 0: S+="{"; break;
   }}
   FormToStr(F.B->L,i,S); sprintf(Buf,"%c",ZNAC[i]); S+=Buf;
   FormToStr(F.B->R,i,S);
   if((i<Prio)||((i==Prio)&&((i==2)||(i==4)))){
    switch((Ksk--)%3){
     case 1: S+=")"; break; case 2: S+="]"; break; case 0: S+="}"; break;
   }}
   break;
  case IFOP:
   switch((++Ksk)%3){
    case 1: S+="("; break; case 2: S+="["; break; case 0: S+="{"; break;
   }
   FormToStr(F.L->U,0,S);
   if(F.L->Oper<3){ sprintf(Buf,"%c",LOGZNAC[F.L->Oper]); S+=Buf; }
   else{ sprintf(Buf,"%c",LOGZNAC[F.L->Oper-2]); S+=Buf; }
   if(F.L->Oper>2) S+="=";
   FormToStr(F.L->V,0,S); S+=" ? ";
   FormToStr(F.L->FT,0,S); S+=" ! ";
   FormToStr(F.L->FF,0,S);
   switch((Ksk--)%3){
    case 1: S+=")"; break; case 2: S+="]"; break; case 0: S+="}"; break;
   }
   break;
  case IPLL:
   switch(F.I->Type){
    case 1: S+="ipl("; Beg=',';  End=')'; break;
    case 2: Beg='['; End=']';
   }
   S+=F.I->A->Name; sprintf(Buf,"%c",Beg); S+=Buf;
   FormToStr(F.I->F,0,S); sprintf(Buf,"%c",End); S+=Buf;
   break;
  case SPLN:
   switch(F.P->Type){
    case 1: S+="spln("; Beg=',';  End=')'; break;
    case 2: Beg='['; End=']';
   }
   S+=F.P->A->Name; sprintf(Buf,"%c",Beg); S+=Buf;
   FormToStr(F.P->F,0,S); sprintf(Buf,"%c",End); S+=Buf;
   break;
  case SUMM:
   S+="summ("; S+=F.S->i->Name;
   S+=","; FormToStr(F.S->a,0,S);
   S+=","; FormToStr(F.S->b,0,S);
   S+=","; FormToStr(F.S->F,0,S);
   S+=")";
   break;
  case INTG:
   S+="intg("; FormToStr(F.G->f,0,S);
   S+=","; S+=F.G->x->Name;
   S+=","; FormToStr(F.G->a,0,S);
   S+=","; FormToStr(F.G->b,0,S);
   S+=")";
   break;
  case RUUT:
   S+="root("; FormToStr(F.R->f,0,S);
   S+=","; S+=F.R->x->Name;
   S+=","; FormToStr(F.R->a,0,S);
   S+=","; FormToStr(F.R->b,0,S);
   S+=")";
   break;
  case REST:
   S+="rest("; FormToStr(F.O->a,0,S);
   S+=","; FormToStr(F.O->b,0,S);
   S+=")";
   break;
}}

AnsiString *FormToStr(Form F,AnsiString *S){
 AnsiString aS; Ksk=0;
 FormToStr(F,0,aS);
 *S=aS;
 return S;
}

