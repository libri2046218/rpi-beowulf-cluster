#ifndef ARRAY_H
#define ARRAY_H

void array_random(int* arr, int n);
void array_print(int* arr, int n);
void array_split(int* in, int l, int r, int* out);
void array_copy(int* in, int* out, int n);
int array_check_sorting(int* arr, int n);

#endif