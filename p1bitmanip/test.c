//#include <stdio.h>

int byteswap(int x, int n, int m){
  int nn = 0xFF<<(8*n)&x;
  printf("%x\n",nn);
  x = x ^ nn;
  printf("%x\n",x);
  int mm =  0xFF<<(8*m)&x;
  printf("%x\n",mm);
  x = x ^ mm;
  printf("%x\n",x);
  x = x + ((nn >>(8*n-m)));
  printf("%x\n",x);
  x = x + ((mm >>(8*m)) << (8*n));
  printf("%x\n",x);
  return x;
}

int main() {
  int x = 0x80000000;
  byteswap(x,0,3);
}