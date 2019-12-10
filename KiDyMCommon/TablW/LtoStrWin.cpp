#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

char *Mlrd (unsigned long Val, char *S) { strcat(S," миллиард");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"ов");
 } return S;
}

char *Mln (unsigned long Val, char *S) { strcat(S," миллион");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"ов");
 } return S;
}

char *Tys (unsigned long Val, char *S) { strcat(S," тыс€ч");
 if(Val%10>4||!(Val%10)) return S;
 if(Val%100>10&&Val%100<20) return S;
 if(Val%10==1) { strcat(S,"а"); return S; }
 strcat(S,"и"); return S;
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
   case 1: strcat(S," сто");       break;
   case 2: strcat(S," двести");    break;
   case 3: strcat(S," триста");    break;
   case 4: strcat(S," четыреста"); break;
   case 5: strcat(S," п€тьсот");   break;
   case 6: strcat(S," шестьсот");  break;
   case 7: strcat(S," семьсот");   break;
   case 8: strcat(S," восемьсот"); break;
   case 9: strcat(S," дев€тьсот");
  } return Val%100?LtoStr(Pol,Val%100,S):S;
 }
 if(Val>=20) {
  switch((int)(Val/10)) {
   case 2: strcat(S," двадцать");    break;
   case 3: strcat(S," тридцать");    break;
   case 4: strcat(S," сорок");       break;
   case 5: strcat(S," п€тьдес€т");   break;
   case 6: strcat(S," шестьдес€т");  break;
   case 7: strcat(S," семьдес€т");   break;
   case 8: strcat(S," восемьдес€т"); break;
   case 9: strcat(S," дев€носто");   break;
  }
  return Val%10?LtoStr(Pol,Val%10,S):S;
 }
 switch((int)Val) {
  case  0: strcat(S," нуль"); break;
  case  1: strcat(S,Pol==2 ? " один" : Pol==1 ? " одно" : " одна"); break;
  case  2: strcat(S,Pol ? " два" : " две");                         break;
  case  3: strcat(S," три");                                        break;
  case  4: strcat(S," четыре");                                     break;
  case  5: strcat(S," п€ть");                                       break;
  case  6: strcat(S," шесть");                                      break;
  case  7: strcat(S," семь");                                       break;
  case  8: strcat(S," восемь");                                     break;
  case  9: strcat(S," дев€ть");                                     break;
  case 10: strcat(S," дес€ть");                                     break;
  case 11: strcat(S," одиннадцать");                                break;
  case 12: strcat(S," двенадцать");                                 break;
  case 13: strcat(S," тринадцать");                                 break;
  case 14: strcat(S," четырнадцать");                               break;
  case 15: strcat(S," п€тнадцать");                                 break;
  case 16: strcat(S," шестнадцать");                                break;
  case 17: strcat(S," семнадцать");                                 break;
  case 18: strcat(S," восемнадцать");                               break;
  case 19: strcat(S," дев€тнадцать");
 } return S;
}
char *MlrdU (unsigned long Val, char *S) { strcat(S," мiль€рд");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"iв");
 } return S;
}

char *MlnU (unsigned long Val, char *S) { strcat(S," мiльйон");
 if(Val%10==1&&Val%100!=11) return S;
 switch((int)(Val%10)) {
  case 2: case 3: case 4:
   if(Val%100<12||Val%100>14) { strcat(S,"а"); break; }
  default: strcat(S,"iв");
 } return S;
}

char *TysU (unsigned long Val, char *S) { strcat(S," тис€ч");
 if(Val%10>4||!(Val%10)) return S;
 if(Val%100>10&&Val%100<20) return S;
 if(Val%10==1) { strcat(S,"а"); return S; }
 strcat(S,"i"); return S;
}

char *LtoStrU (int Pol, unsigned long Val, char *S) { char s[256];
// if(Val==0L) { strcat(S," нуль"); return S; }
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
   case 1: strcat(S," сто");       break;
   case 2: strcat(S," двiстi");    break;
   case 3: strcat(S," триста");    break;
   case 4: strcat(S," чотириста"); break;
   case 5: strcat(S," п'€тсот");   break;
   case 6: strcat(S," шiстсот");  break;
   case 7: strcat(S," сiмсот");   break;
   case 8: strcat(S," вiсiмсот"); break;
   case 9: strcat(S," дев'€тсот");
  }
  return Val%100?LtoStrU(Pol,Val%100,S):S;
 }
 if(Val>=20) {
  switch((int)(Val/10)) {
   case 2: strcat(S," двадц€ть");    break;
   case 3: strcat(S," тридц€ть");    break;
   case 4: strcat(S," сорок");       break;
   case 5: strcat(S," п'€тдес€т");   break;
   case 6: strcat(S," шiстдес€т");   break;
   case 7: strcat(S," сiмдес€т");    break;
   case 8: strcat(S," вiсiмдес€т");  break;
   case 9: strcat(S," дев'€носто");
  }
  return Val%10?LtoStrU(Pol,Val%10,S):S;
 }
 switch((int)Val) {
  case  0: strcat(S," нуль"); break;
  case  1: strcat(S,Pol==2 ? " один" : Pol==1 ? " одно" : " одна"); break;
  case  2: strcat(S,Pol ? " два" : " двi");                         break;
  case  3: strcat(S," три");                                        break;
  case  4: strcat(S," чотири");                                     break;
  case  5: strcat(S," п'€ть");                                      break;
  case  6: strcat(S," шiсть");                                      break;
  case  7: strcat(S," сiм");                                        break;
  case  8: strcat(S," вiсiм");                                      break;
  case  9: strcat(S," дев'€ть");                                    break;
  case 10: strcat(S," дес€ть");                                     break;
  case 11: strcat(S," одинадц€ть");                                 break;
  case 12: strcat(S," дванадц€ть");                                 break;
  case 13: strcat(S," тринадц€ть");                                 break;
  case 14: strcat(S," чотирнадц€ть");                               break;
  case 15: strcat(S," п€тнадц€ть");                                 break;
  case 16: strcat(S," шiстнадц€ть");                                break;
  case 17: strcat(S," сiмнадц€ть");                                 break;
  case 18: strcat(S," вiсiмнадц€ть");                               break;
  case 19: strcat(S," дев'€тнадц€ть");
 }
 return S;
}

