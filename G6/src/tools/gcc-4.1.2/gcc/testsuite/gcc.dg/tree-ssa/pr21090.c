/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-vrp" } */

int g, h;

int
foo (int a)
{
  int *p;

  if (a)
    p = &g;
  else
    p = &h;

  if (p != 0)
    return 1;
  else
    return 0;
}

/* { dg-final { scan-tree-dump-times "Folding predicate.*to 1" 1 "vrp" } } */
/* { dg-final { cleanup-tree-dump "vrp" } } */
