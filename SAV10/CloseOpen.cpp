#include "CA.h"
#include <io.h>

//Оператор ЗАКРЫТЬ служит для исключения из списка элементов или переменных
// перечисленных в нем, соответстенно, элементов или переменных.
//Оператор ОТКРЫТЬ служит для восстановления в списке элементов или переменных
// перечисленных в нем, ранее исключенных оператором ЗАКРЫТЬ, соответстенно,
// элементов или переменных.
//
// Синтаксис операторов одинаковый
//      ЗАКРЫТЬ : <список_имен_элементов_или_переменных_для_исключения>;
//      ОТКРЫТЬ : <список_имен_элементов_или_переменных_для_восстановления>;
//Оператор ЗАКРЫТЬ прячет элементы и переменные, перечисленные в нем
// в отдельные списки, оператор ОТКРЫТЬ вовращает их оттуда

extern List *L; extern char SimbInd;
extern char ErrFile[],OprFile[],Inf[],WORKDIR[],FILESHAP[];
extern Cnst *Pust; extern FILE *OPRFILE;

bool ToClose(Parm *Par,TRichEdit *R){
// char *inf=Inf,*s,NameInstr[256]; Vary *V,*W=NULL; Elem *e,*E=NULL;
/* char *inf=Inf,*s; Vary *V; Elem *E;
// FILE *F; Magazine *Mi=NULL,*m; Root *I; bool Mode;
//Диагностика
 if(OPRFILE=fopen(OprFile,"w")){
  fprintf(OPRFILE,
   "<html>\n<head>\n<title>ПРОТОКОЛ ОПЕРАТОРА \"РАЗМНОЖИТЬ\"</title>\n"
   "<meta http-equiv=\"Content-Type\" content=\"text/html;"
   "charset=windows-1251\">\n</head>\n"
   "<body bgcolor=\"#FFFFFF\" text=\"#800000\""
   " background=\"%s/fon2.jpg\""
   " font face=\"Times New Roman, Times, serif\">\n",
    WORKDIR);
 }
 Inds=new TInds;
 for(Parm *p=Par;p;p=p->Sled){
  if(!p->Nam||!p->R){ Inds->~TInds(); goto Ex; }
  Inds->AddInd(p);
 }
 for(Vary *V=L->V;V;V=V->Sled)
  if((s=strchr(V->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s);
//Соберем индексированные выражения из элементов
 for(Elem *E=L->E;E;E=E->Sled){
  if((s=strchr(E->Name,SimbInd))&&(s[1]=='(')) IndExp->AddInd(s); //*t='.';
 }
 if(W||E){
  fprintf(OPRFILE,
   "Оператор \"РАЗМНОЖИТЬ\" сгенерировал из индексированных выражений<hr>\n");
  if(W){
   fprintf(OPRFILE,"<b>Переменные:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Переменные</b></div></td>\n</tr>\n",OPRFILE);
   for(V=W;V;V=V->Sled){
    if(V->Znach.C!=Pust){
     ListVE.AddProt(V);
     s=strchr(V->Name,SimbInd);
     if(s[1]=='('){
      if(!IndExp->IndexForm(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
     }}
     else{
      if(!Inds->Index(V,ListVE)){
       fclose(OPRFILE); goto Ex3;
   }}}}
   ListVE.VtoFile();
   fputs("</table>\n",OPRFILE);
  }
  if(E){
   fprintf(OPRFILE,"<b>Элементы:</b><br>\n");
   fputs("<table width=\"100%\" border=\"1\">\n",OPRFILE);
   fputs("<tr>",OPRFILE);
   fputs("<td><div align=\"center\"><b>Исходные выражения</b></div></td>\n"
    "<td><div align=\"center\"><b>Элементы</b></div></td>\n</tr>",OPRFILE);
   for(e=E;e;e=e->Sled){
    ListVE.AddProt(e);
    s=strchr(E->Name,SimbInd);
    if(s[1]=='('){
     if(!IndExp->IndexForm(e,ListVE)){
      fclose(OPRFILE); goto Ex3;
    }}
    else{
     if(!Inds->Index(e,ListVE)){ fclose(OPRFILE); goto Ex3; }
   }}
   ListVE.EtoFile();
   fputs("</table\n",OPRFILE);
  }
  fputs("</font></body>\n</html>\n",OPRFILE);
 }
 fclose(OPRFILE);
 delete Inds; delete IndExp;
 return true;
Ex:sprintf(inf,
 "Оператор \"РАЗМНОЖИТЬ\" должен быть записан в виде:<br>\n"
 "РАЗМНОЖИТЬ : <индекс-символ1>(<ОТ1>,<ДО1>[,<ШАГ1>]),<br>\n"
 "             <индекс-символ2>(<ОТ2>,<ДО2>[,<ШАГ2>]),...;,<br>\n"
 "например,    РАЗМНОЖИТЬ : i(1,5),k(2,5,1);<br>\n");
 if(F=fopen(OprFile,"a")){ Fputs(Inf,F); fclose(F); }
 Application->MessageBox(Inf,"Ошибка в операторе \*РАЗМНОЖИТЬ\*:",MB_OK);
 return false;
Ex3: delete Inds; delete IndExp;
*/
 return false;
}

bool ToOpen(Parm *Par,TRichEdit *R){
 return false;
}

