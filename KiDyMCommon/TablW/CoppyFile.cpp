#include <dir.h>
#include <vcl.h>
#include <io.h>
#include <time.h>
#include <sys\stat.h>
#include <process.h>
#include <stdio.h>
#include <conio.h>
//Работа с файлами при копировании и разархивировании

bool Copy (char *Fi,char *Fo) { char Info[256]; int Rez;
// sprintf(Info,"copy %s %s",Fi,Fo);
 Rez=CopyFile(Fi,Fo,false);
// if(Rez=system(Info)) {
 if(!Rez){
//  sprintf(Info,"Копирование файла <%s>\nв файл <%s> прошло с ошибкой\n"
//   "Код ошибки - %d",Fi,Fo,Rez);
  sprintf(Info,"Копирование файла <%s>\nв файл <%s> прошло с ошибкой\n"
   "Код ошибки - %d",Fi,Fo,GetLastError());
  Application->MessageBox(Info,"Ошибка!",MB_OK|MB_ICONWARNING);
  return false;
 }
 return true;
}

int CopyFromFloppy(char *Dir,char *File){
 char Sf[80],Info[256]; long Tv=0,Tn=0,Lv,Ln; struct stat Sv,Sn;
 sprintf(Sf,"%s\\%s",Dir,File);
 if(!access(  Sf,0)){ stat( Sf,&Sv); Tv=Sv.st_mtime; Lv=Sv.st_size; }
 if(!access(File,0)){ stat(File,&Sn); Tn=Sn.st_mtime; Ln=Sn.st_size; }
 if(Tv>Tn){
  Copy(Sf,File);
  if(!access(File,0)){ stat(File,&Sn); Tn=Sn.st_mtime; Ln=Sn.st_size; }
  if((Tv>Tn)||(Lv!=Ln)){
   sprintf(Info,"Длина или время создания скопированного файла <%s>\n"
    "не совпадает с параметрами исходного файла <%s>",File,Sf);
   Application->MessageBox(Info,"Ошибка!",MB_OK|MB_ICONWARNING);
 }}
 return Tv||Tn;
}

int CopyToFloppy(char *Dir,char *File){
 char Fd[256],Info[256]; long Tv=0,Tn=0,Lv,Ld; struct stat Sv,Sd;
 sprintf(Fd,"%s\\%s",Dir,File);
 if(!access(Fd,0)){ stat(Fd,&Sd); Tn=Sd.st_mtime; Ld=Sd.st_size; }
 if(!access(File,0)){ stat(File,&Sv); Tv=Sv.st_mtime; Lv=Sv.st_size; }
 if(Tv>Tn) {
  Copy(File,Fd);
  if(!access(Fd,0)){ stat(Fd,&Sd); Tn=Sd.st_mtime; Ld=Sd.st_size; }
  if((Tv>Tn)||(Sv.st_size!=Sd.st_size)){
   sprintf(Info,"Длина или время создания скопированного файла <%s>\n"
    "не совпадает с параметрами исходного файла <%s>.\n"
    "Tv=%lu, Tn=%lu, Lv=%lu, Ld=%lu",Sd,File,Tv,Tn,Lv,Ld);
   Application->MessageBox(Info,"Ошибка!",MB_OK|MB_ICONWARNING);
 }}
 return Tv||Tn;
}

int CopyToArhWin(char *Dir,char *File){
// FILE *Fin,*Fout;
 char Fd[256],Info[256],S[1024]; long Tv=0,Tn=0,Lv,Ld; struct stat Sv,Sd;
 sprintf(Fd,"%s\\%s",Dir,File);
 if(!access(Fd,0)){ stat(Fd,&Sd); Tn=Sd.st_mtime; Ld=Sd.st_size; }
 if(!access(File,0)){ stat(File,&Sv); Tv=Sv.st_mtime;  Lv=Sv.st_size; }
 if(Tv>Tn){
//  if(!(Fin=fopen(File,"r"))||feof(Fin)) return 0;
//  if(!(Fout=fopen(Fd,"w"))||feof(Fout)) return 0;
//  while(fgets(S,1023,Fin)) fputs(S,Fout);
//  fclose(Fout); fclose(Fin);
  Copy(File,Fd);
  if(!access(Fd,0)) { stat(Fd,&Sd); Tn=Sd.st_mtime; Ld=Sd.st_size; }
  if((Tv>Tn)||(Sv.st_size!=Sd.st_size)){
   sprintf(Info,"Длина или время создания скопированного файла <%s>\n"
    "не совпадает с параметрами исходного файла <%s>.\n"
    "Tv=%lu, Tn=%lu, Lv=%lu, Ld=%lu",Sd,File,Tv,Tn,Lv,Ld);
   Application->MessageBox(Info,"Ошибка!",MB_OK|MB_ICONWARNING);
 }}
 return Tv||Tn;
}
