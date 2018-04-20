
#include <stdio.h>

int f(int a, double b, float c)
{
    int v = a;
    v = b;
    v = c;
}

void m()
{
    int a = 9;
    double b = 19;
    float c = 23;

    f(a, b, c);
}

int f2(int a, int b, int c, int d, int g, int h, int i, int j)
{
    int v = a;
    int x = i;
    int y = j;
}

void mm()
{
    f2(1, 2, 3, 4, 5, 6, 7, 8);
}

int f3(float a, float b, float c, float d, float e, float f, float g,
        float h, float i, float j, float k, float l, float m)
{
    float z = i;
    float y = j;
}

void mmm()
{
    f3(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
}
