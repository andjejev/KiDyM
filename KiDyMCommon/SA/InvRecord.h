#ifndef InvRecordH
#define InvRecordH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <stdio.h>

#define CNST 1
#define VARY 2
#define ZNAC 3

//---------------------------------------------------------------------------
class TFormInvRec : public TForm {
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TEdit *EditFormuls;
        TButton *Button1;
        TEdit *Edit2;
        void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TFormInvRec(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormInvRec *FormInvRec;
//---------------------------------------------------------------------------
class Znac{
 public:
  Znac *Sled;
 private:
  int Atr; char Z;
 public:
  Znac(){ Atr=ZNAC; Z='\0'; };
  Znac(char z){ Atr=ZNAC; Z=z; };
  char *ToStr(char *Str){
   sprintf(Str,"%c",Z);
   return Str;
  };
  int Atrib(){ return Atr; };
};
//---------------------------------------------------------------------------
class Cnst{
 public:
  Cnst *Sled;
 private:
  int Atr; double Val;
 public:
  Cnst(){ Atr=CNST; Val=0.0; };
  Cnst(double D){ Atr=CNST; Val=D; };
  char *ToStr(char *Str){
   sprintf(Str,"%g",Val);
   return Str;
  };
  int Atrib(){ return Atr; };
};
//---------------------------------------------------------------------------
class Vary{
 public:
  Vary *Sled;
 private:
  int Atr; char *Name; double Val;
 public:
  Vary(){ Atr=VARY; Name=NULL; Val=0; };
  Vary(char *name){
   int L=strlen(name);
   Atr=VARY;
   Name=new char[L+1];
   strcpy(Name,name);
   Val=0;
  };
  char *ToStr(char *Str){
   return strcpy(Str,Name);
  };
  int Atrib(){ return Atr; };
 ~Vary(){
   if(Name) delete Name;
 }
};
//---------------------------------------------------------------------------
class Vagon{
 public: Vagon *Sled;
 private:
  union { Cnst *C; Vary *V; Znac *Z; } CVZ;
 public:
  void Fill(Cnst *c){ CVZ.C=c; };
  void Fill(Vary *v){ CVZ.V=v; };
  void Fill(Znac *z){ CVZ.Z=z; };
  int Atr(){
   return CVZ.C->Atrib();
  };
  char *ToStr(char *Str){
   switch(Atr()){
    case CNST: return CVZ.C->ToStr(Str);
    case VARY: return CVZ.V->ToStr(Str);
    default: /*ZNAC!*/ return CVZ.Z->ToStr(Str);
  }};
};
//---------------------------------------------------------------------------
class Stack{
 public:
  Vagon *R;
  Stack(){ R=NULL; };
  void push(Vagon *v){
   v->Sled=R;
   R=v;
  };
  void push(Cnst *c){
   Vagon *v=new Vagon;
   v->Sled=R; R=v; v->Fill(c);
  };
  void push(Vary *V){
   Vagon *v=new Vagon;
   v->Sled=R; R=v; v->Fill(V);
  };
  void push(Znac *z){
   Vagon *v=new Vagon;
   v->Sled=R; R=v; v->Fill(z);
  };
  Vagon *pop(void){
   Vagon *v=R;
   if(v) R=v->Sled;
   return v;
  };
  Vagon *wo(void){
   return R;
  };
  void print(TEdit *Rez){
   for(Vagon *v=R;v;v=v->Sled){ char Str[1024];
    v->ToStr(Str);
    Rez->Text=Rez->Text+Str;
    if(v->Sled) Rez->Text=Rez->Text+" ";
    else Rez->Text=Rez->Text+';';
  }};
  void reverse(){
   Vagon *Pv=NULL;
   for(Vagon *v=R;v;v=R){
    R=v->Sled;
    v->Sled=Pv;
    Pv=v;
   }
   R=Pv;
  };
} NewYork,Texas,California;
#endif
