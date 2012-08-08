#include <stdio.h>

int main(int argc, char * argv[]) {
  // Create two arrays we care about
  int    ages[]  = { 23, 43, 12, 89, 2 };
  char * names[] = { "Alan", "Frank", "Mary", "John", "Lisa" };

  // Safely get the size of ages
  int count = sizeof(ages) / sizeof(int);
  int i     = 0;

  // First way, using indexing.
  for(i = 0; i < count; i++) {
    printf("%s had %d years alive.\n", names[i], ages[i]);
  }

  printf("---\n");

  // setup the pointers to the start of the arraty
  int  *  cur_age  = ages;
  char ** cur_name = names;

  // Second way, using pointers.
  for(i = 0; i < count; i++) {
    printf("%s had %d years alive.\n", *(cur_name + i), *(cur_age + i));
  }

  printf("---\n");

  // Third way, pointers are just arrays
  for(i = 0; i < count; i++) {
    printf("%s had %d years alive.\n", cur_name[i], cur_age[i]);
  }

  printf("---\n");

  // Fourth way with pointers in a stupid complex way
  for(cur_name = names, cur_age = ages; (cur_age - ages) < count; cur_age++, cur_name++) {
    printf("%s lived %d years so far\n", *cur_name, *cur_age);
  }

  return 0;
}
