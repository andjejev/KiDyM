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
 "�����������",
 "����������",
 "�������",
 "������ ���������",
 "�������������",
 "��������� ���������",
 "����������� ���������",
 "���������� �������",
 "��������� ������",
 "��������� �������",
 "������� ���������������",
 "�������������",
 "����������� �������� �������"
};

BOOL (*Calcul[])(void) = {
 Diagnost,
 Kinemat,
 ForseOff, FreeVib,  ForseVib, DynamInt, ScltCurv, Correct1,
 LaUravn1, Vibroisl, PrintSol, Statika2, Statika3, Statika7,
 RungeKut, Favorin
};

void Ex (void) { fprintf(OUTFILE,"\n����� ������."); }

void SolTask (void) {
 Root *Instr; int Typ; char TypeCalc[51]="��� �������";
 while(VvodJob(TypeCalc)) {
  if(strcmp(TypeCalc,"�����")==0) { Ex(); return; }
  if(!(*TypeCalc))  /* B�� PAC�ETA �A�AH B TEKCTE */
   for(Typ=0;Typ<Types;Typ++) {
    if(FindInst(NameCalc[Typ],&Instr)) {
     fprintf(OUTFILE,"��� ������� = %s;\n",NameCalc[Typ]);
     if(Calcul[Typ]()) goto NaPovtor; else { Ex(); return; }
   }}
  else {
   for(Typ=0;Typ<Types;Typ++) {
    if(strcmp(TypeCalc,NameCalc[Typ])==0) {
     if(Calcul[Typ]()) goto NaPovtor; else { Ex(); return; }
  }}}
  strcpy(Inf,"������ �� ����������."); Error(0,0); return;
  NaPovtor: SbrosDif(); *TypeCalc='\0';
}}

void FreeConst (void) {
 free(DvaPi); free(Pi);    free(Pina2);     free(Pina3);    free(Pina4);
 free(Pina6); free(Exp);   free(  Tri);     free(Dva);      free(Odin);
 free(Halb);  free( Null); free(MinusOdin); free(MinusDva); free(MinusTri);
 free( Pust);
}

int main ( int ARGC, char *ARGV[] ) {
//������� ���������:
//1. ������� ����
//2. ��� ��������� ���������
//3. ����� ������� � c����� �� ����� <calculs.lst>, ������ �� ����
//�������� ���������:
//1. ������� ����
//2. ����� ������� � c����� �� ����� <calculs.lst>, ������ �� ����
 struct stat STAT; char *Tchk,Snc[8]="0"; clrscr();
 if(ARGC<2) { printf("\a�� ����� ������� ����.\n"); delay(500); exit(1); }
 if(ARGC>3) strcpy(Snc,ARGV[3]); strcpy(ARGV0,ARGV[0]);
 if(!(INFILE=fopen(strcpy(InFile,ARGV[1]),"r"))) {
  printf("\a������ ������� ������� ����!\n"); delay(500); fcloseall();
  if(ARGC>2) execl(ARGV[2],ARGV[2],ARGV[1],ARGV[3],NULL); exit(0);
 }
 if(Tchk=strchr(InFile,'.')) *Tchk='\0'; strcat(strcpy(OutFile,InFile),".out");
 if(!(OUTFILE=fopen(OutFile,"w"))) {
  printf("\a\n ������ ������� �������� ����! "); delay(500); fcloseall();
  if(ARGC>2) execl(ARGV[2],ARGV[2],ARGV[1],ARGV[3],NULL); exit(0);
 }
 strcat(strcpy(ErrFile,InFile),".err");
 if(!(ERRFILE=fopen(ErrFile,"w"))) {
  SignalT(40,12,"������ ������� ���� ������.");
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

 