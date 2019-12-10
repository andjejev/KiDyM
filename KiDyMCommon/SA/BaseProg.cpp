#include "ca.h"
//#include <string.h>
//#include <alloc.h>
//#include <conio.h>
//#include <process.h>
//#include <dos.h>
//#include <sys/stat.h>

char Inf[1024];
Cnst
 *DvaPi, *Pi, *Pina2, *Pina3, *Pina4, *Pina6, *Exp, *Tri, *Dva, *Odin,
 *Halb, *Null, *MinusOdin, *MinusDva, *MinusTri, *Pust;
List *L;

char InFile[256],OutFile[256],ErrFile[256];
FILE *OUTFILE,*INFILE,*ERRFILE;
int Types=15;

char *NameCalc[]={
 "ДИАГНОСТИКА",
 "КИНЕМАТИКА",
 "СТАТИКА",
 "ПЕЧАТЬ УРАВНЕНИЙ",
 "КИНЕТОСТАТИКА",
 "СВОБОДНЫЕ КОЛЕБАНИЯ",
 "ВЫНУЖДЕННЫЕ КОЛЕБАНИЯ",
 "ПЕРЕХОДНОЙ ПРОЦЕСС",
 "СКЕЛЕТНЫЕ КРИВЫЕ",
 "ОТСТРОЙКА СИСТЕМЫ",
 "ВНЕШНЕЕ УРАВНОВЕШИВАНИЕ",
 "ВИБРОИЗОЛЯЦИЯ",
 "ОПРЕДЕЛЕНИЕ МОМЕНТОВ ИНЕРЦИИ"
};

BOOL (*Calcul[])(void) = {
 Diagnost,
 Kinemat,
 ForseOff, FreeVib,  ForseVib, DynamInt, ScltCurv, Correct1,
 LaUravn1, Vibroisl, PrintSol, Statika2, Statika3, Statika7,
 RungeKut, Favorin
};

void Ex (void) { fprintf(OUTFILE,"\nКОНЕЦ ЗАДАЧИ."); }

void SolTask (void) {
 Root *Instr; int Typ; char TypeCalc[51]="НЕТ РАСЧЕТА";
 while(VvodJob(TypeCalc)) {
  if(strcmp(TypeCalc,"КОНЕЦ")==0) { Ex(); return; }
  if(!(*TypeCalc))  /* BИД PACЧETA ЗAДAH B TEKCTE */
   for(Typ=0;Typ<Types;Typ++) {
    if(FindInst(NameCalc[Typ],&Instr)) {
     fprintf(OUTFILE,"ВИД РАСЧЕТА = %s;\n",NameCalc[Typ]);
     if(Calcul[Typ]()) goto NaPovtor; else { Ex(); return; }
   }}
  else {
   for(Typ=0;Typ<Types;Typ++) {
    if(strcmp(TypeCalc,NameCalc[Typ])==0) {
     if(Calcul[Typ]()) goto NaPovtor; else { Ex(); return; }
  }}}
  strcpy(Inf,"Расчет не проводится."); Error(0,0); return;
  NaPovtor: SbrosDif(); *TypeCalc='\0';
}}

void FreeConst (void) {
 free(DvaPi); free(Pi);    free(Pina2);     free(Pina3);    free(Pina4);
 free(Pina6); free(Exp);   free(  Tri);     free(Dva);      free(Odin);
 free(Halb);  free( Null); free(MinusOdin); free(MinusDva); free(MinusTri);
 free( Pust);
}

int main ( int ARGC, char *ARGV[] ) {
//входные параметры:
//1. входной файл
//2. имя вызвавшей программы
//3. номер расчета в cписке из файла <calculs.lst>, считая от нуля
//выходные параметры:
//1. входной файл
//2. номер расчета в cписке из файла <calculs.lst>, считая от нуля
 struct stat STAT; char *Tchk,Snc[8]="0"; clrscr();
 if(ARGC<2) { printf("\aНе задан входной файл.\n"); delay(500); exit(1); }
 if(ARGC>3) strcpy(Snc,ARGV[3]); strcpy(ARGV0,ARGV[0]);
 if(!(INFILE=fopen(strcpy(InFile,ARGV[1]),"r"))) {
  printf("\aНельзя открыть входной файл!\n"); delay(500); fcloseall();
  if(ARGC>2) execl(ARGV[2],ARGV[2],ARGV[1],ARGV[3],NULL); exit(0);
 }
 if(Tchk=strchr(InFile,'.')) *Tchk='\0'; strcat(strcpy(OutFile,InFile),".out");
 if(!(OUTFILE=fopen(OutFile,"w"))) {
  printf("\a\n Нельзя открыть выходной файл! "); delay(500); fcloseall();
  if(ARGC>2) execl(ARGV[2],ARGV[2],ARGV[1],ARGV[3],NULL); exit(0);
 }
 strcat(strcpy(ErrFile,InFile),".err");
 if(!(ERRFILE=fopen(ErrFile,"w"))) {
  SignalT(40,12,"Нельзя открыть файл ошибок.");
 }
 Fclose(ERRFILE);
 strcpy(DirEGAVGABGI,ARGV[0]); if(Tchk=strrchr(DirEGAVGABGI,'\\')) *Tchk='\0';
 InitSav();
 SolTask();
 FreeConst();
 fcloseall();
 stat(ErrFile,&STAT);
 if(!STAT.st_size) unlink(ErrFile);
 if(ARGC>2) execl(ARGV[2],ARGV[2],ARGV[1],ARGV[3],NULL);
 return 1;
}

 