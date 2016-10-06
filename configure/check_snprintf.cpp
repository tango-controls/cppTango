//
// Created by ingvord on 9/14/16.
//
#include <stdio.h>
int main () { int l = snprintf(NULL,0,"%d",100); return (!((3 <= l) || (-1 == l))); }

