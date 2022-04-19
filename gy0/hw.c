#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
  char nev1[20], *nev2 = malloc(20), c; //(char*)malloc(20 * sizeof(char))
  long long int i;
  double d;
  scanf("%19[^\n]s", nev1);
  do
  {
    scanf("%c", &c);
  } while (c != '\n');
  scanf("%Ld%lf %19[^\n]s", &i, &d, nev2);
  printf("\n%s\n%lld\n%f\n%s\n", nev1, i, d, nev2);
  free(nev2);
}
