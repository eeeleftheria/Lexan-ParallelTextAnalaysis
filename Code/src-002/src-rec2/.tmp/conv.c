#include <stdio.h>
#include <stdlib.h>

int main ()
{
  int i;
  char str[33];

  printf ("Enter a number: ");
  scanf ("%d",&i);
  
  itoa (i,str,10);
  printf ("decimal: %s\n", str);
  
  itoa (i, str, 16);
  printf ("hexadecimal: %s\n", str);
  
  itoa (i, str, 2);
  printf ("binary: %s\n", str);
  
  return 0;
}

