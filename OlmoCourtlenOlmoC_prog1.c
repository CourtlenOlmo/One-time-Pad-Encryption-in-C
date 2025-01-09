#include <stdio.h>
#include <math.h>

float pi = 3.14159265359;

int main(void) {
    int segments;
    float r;
    float ha;
    float hb;

    printf("How many spherical segments you want to evaluate [2-10]?\n");
    scanf("%d", &segments);
    
    while (segments < 2 || segments > 10) {
        printf("How many spherical segments you want to evaluate [2-10]?\n");
        scanf("%d", &segments);
    }
    for (int i = 1; i <= segments; i++) {
        printf("Obtaining data for spherical segment number %d\n", i);
        printf("What is the radius of the sphere (R)\n");
        scanf("%f", &r);
        printf("What is the height of the top area of the spherical segment (ha)\n");
        scanf("%f", &ha);
        printf("What is the height of the bottom are aof the spherical segment (hb)\n");
        scanf("%f", &hb);
    }

    printf("Entered data: R = %.2f, ha = %.2f, hb = %.2f\n", r, ha, hb);
}