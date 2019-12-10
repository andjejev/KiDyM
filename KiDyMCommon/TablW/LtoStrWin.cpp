#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

char *Mlrd (unsigned long Val, char *S) { strcat(S," ��������");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"�"); break; }
  default: strcat(S,"��");
 } return S;
}

char *Mln (unsigned long Val, char *S) { strcat(S," �������");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"�"); break; }
  default: strcat(S,"��");
 } return S;
}

char *Tys (unsigned long Val, char *S) { strcat(S," �����");
 if(Val%10>4||!(Val%10)) return S;
 if(Val%100>10&&Val%100<20) return S;
 if(Val%10==1) { strcat(S,"�"); return S; }
 strcat(S,"�"); return S;
}

char *LtoStr (int Pol, unsigned long Val, char *S) {
 if(Val>=1000000000l) {
  LtoStr(2,Val/1000000000l,S); Mlrd(Val/1000000000l,S);
  return Val%1000000000l?LtoStr(Pol,Val%1000000000l,S):S;
 }
 if(Val>=1000000l) {
  LtoStr(2,Val/1000000l,S); Mln(Val/1000000l,S);
  return Val%1000000l?LtoStr(Pol,Val%1000000l,S):S;
 }
 if(Val>=1000) {
  LtoStr(0,Val/1000,S); Tys(Val/1000,S);
  return Val%1000?LtoStr(Pol,Val%1000,S):S;
 }
 if(Val>=100) {
  switch((int)(Val/100)) {
   case 1: strcat(S," ���");       break;
   case 2: strcat(S," ������");    break;
   case 3: strcat(S," ������");    break;
   case 4: strcat(S," ���������"); break;
   case 5: strcat(S," �������");   break;
   case 6: strcat(S," ��������");  break;
   case 7: strcat(S," �������");   break;
   case 8: strcat(S," ���������"); break;
   case 9: strcat(S," ���������");
  } return Val%100?LtoStr(Pol,Val%100,S):S;
 }
 if(Val>=20) {
  switch((int)(Val/10)) {
   case 2: strcat(S," ��������");    break;
   case 3: strcat(S," ��������");    break;
   case 4: strcat(S," �����");       break;
   case 5: strcat(S," ���������");   break;
   case 6: strcat(S," ����������");  break;
   case 7: strcat(S," ���������");   break;
   case 8: strcat(S," �����������"); break;
   case 9: strcat(S," ���������");   break;
  }
  return Val%10?LtoStr(Pol,Val%10,S):S;
 }
 switch((int)Val) {
  case  0: strcat(S," ����"); break;
  case  1: strcat(S,Pol==2 ? " ����" : Pol==1 ? " ����" : " ����"); break;
  case  2: strcat(S,Pol ? " ���" : " ���");                         break;
  case  3: strcat(S," ���");                                        break;
  case  4: strcat(S," ������");                                     break;
  case  5: strcat(S," ����");                                       break;
  case  6: strcat(S," �����");                                      break;
  case  7: strcat(S," ����");                                       break;
  case  8: strcat(S," ������");                                     break;
  case  9: strcat(S," ������");                                     break;
  case 10: strcat(S," ������");                                     break;
  case 11: strcat(S," �����������");                                break;
  case 12: strcat(S," ����������");                                 break;
  case 13: strcat(S," ����������");                                 break;
  case 14: strcat(S," ������������");                               break;
  case 15: strcat(S," ����������");                                 break;
  case 16: strcat(S," �����������");                                break;
  case 17: strcat(S," ����������");                                 break;
  case 18: strcat(S," ������������");                               break;
  case 19: strcat(S," ������������");
 } return S;
}
char *MlrdU (unsigned long Val, char *S) { strcat(S," �i�����");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"�"); break; }
  default: strcat(S,"i�");
 } return S;
}

char *MlnU (unsigned long Val, char *S) { strcat(S," �i�����");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"�"); break; }
  default: strcat(S,"i�");
 } return S;
}

char *TysU (unsigned long Val, char *S) { strcat(S," �����");
 if(Val%10>4||!(Val%10)) return S;
 if(Val%100>10&&Val%100<20) return S;
 if(Val%10==1) { strcat(S,"�"); return S; }
 strcat(S,"i"); return S;
}

char *LtoStrU (int Pol, unsigned long Val, char *S) { char s[256];
// if(Val==0L) { strcat(S," ����"); return S; }
 if(Val>=1000000000l) {
  LtoStrU(2,Val/1000000000l,S); MlrdU(Val/1000000000l,S);
  return Val%1000000000l?LtoStrU(Pol,Val%1000000000l,S):S;
 }
 if(Val>=1000000l) {
  LtoStrU(2,Val/1000000l,S); MlnU(Val/1000000l,S);
  return Val%1000000l?LtoStrU(Pol,Val%1000000l,S):S;
 }
 if(Val>=1000) {
  LtoStrU(0,Val/1000,S); TysU(Val/1000,S);
  return Val%1000?LtoStrU(Pol,Val%1000,S):S;
 }
 if(Val>=100) {
  switch((int)(Val/100)) {
   case 1: strcat(S," ���");       break;
   case 2: strcat(S," ��i��i");    break;
   case 3: strcat(S," ������");    break;
   case 4: strcat(S," ���������"); break;
   case 5: strcat(S," �'�����");   break;
   case 6: strcat(S," �i�����");  break;
   case 7: strcat(S," �i����");   break;
   case 8: strcat(S," �i�i����"); break;
   case 9: strcat(S," ���'�����");
  }
  return Val%100?LtoStrU(Pol,Val%100,S):S;
 }
 if(Val>=20) {
  switch((int)(Val/10)) {
   case 2: strcat(S," ��������");    break;
   case 3: strcat(S," ��������");    break;
   case 4: strcat(S," �����");       break;
   case 5: strcat(S," �'�������");   break;
   case 6: strcat(S," �i�������");   break;
   case 7: strcat(S," �i������");    break;
   case 8: strcat(S," �i�i������");  break;
   case 9: strcat(S," ���'������");
  }
  return Val%10?LtoStrU(Pol,Val%10,S):S;
 }
 switch((int)Val) {
  case  0: strcat(S," ����"); break;
  case  1: strcat(S,Pol==2 ? " ����" : Pol==1 ? " ����" : " ����"); break;
  case  2: strcat(S,Pol ? " ���" : " ��i");                         break;
  case  3: strcat(S," ���");                                        break;
  case  4: strcat(S," ������");                                     break;
  case  5: strcat(S," �'���");                                      break;
  case  6: strcat(S," �i���");                                      break;
  case  7: strcat(S," �i�");                                        break;
  case  8: strcat(S," �i�i�");                                      break;
  case  9: strcat(S," ���'���");                                    break;
  case 10: strcat(S," ������");                                     break;
  case 11: strcat(S," ����������");                                 break;
  case 12: strcat(S," ����������");                                 break;
  case 13: strcat(S," ����������");                                 break;
  case 14: strcat(S," ������������");                               break;
  case 15: strcat(S," ����������");                                 break;
  case 16: strcat(S," �i���������");                                break;
  case 17: strcat(S," �i��������");                                 break;
  case 18: strcat(S," �i�i��������");                               break;
  case 19: strcat(S," ���'���������");
 }
 return S;
}

