#ifndef THREADS_FIXED_POINT_H
#define THREADS_FIXED_POINT_H
#define H (1 << 14)

#define INT_MAX (1*(1 << 31) - 1)
#define INT_MIN (-1*(1 << 31))

// according to stanford site, define functions in fixed_point.h and call them in 
// thread.c



int multFixInt(int i, int k);
int divFixInt(int i, int k);
int subFix(int i, int j);
int subFixInt(int i, int k);
int intToFix(int k);
int fixRound(int i);
int fixInt(int i);
//such that i+j are fixed point and k is an integer value


int fixRound(int i)
{
  if (i >= 0)
    {
      return (i + H / 2) / H;
    }
  else
    {
      return (i - H / 2) / H;
    }
}

int multFixInt(int i, int k)   {return i*k;} 
int subFixInt(int i, int k) {return i-intToFix(k);}

int intToFix(int k)        {return k*H;}
int fixInt(int i)        {return i/H;}

int subFix(int i, int j)    {return i-j;}
int divFixInt(int i, int k) {return i/k;}

#endif