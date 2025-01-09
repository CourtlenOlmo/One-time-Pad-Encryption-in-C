#include <stdio.h>
#include <math.h>

double pi = 3.14159265359;

float surfacearea(float r, float a){
    double result = sqrt((r*r)-(a*a));
    result = pi * (result * result);
    return result;
}

float lateralsurfacearea(float r,float a,float b){
    float height = a - b;
    float result = 2 * pi * r * height;
    return result;
}

float calcvolume(float r, float ha, float hb){
    float height = ha - hb;
    double a = sqrt((r*r)-(a*a));
    double b = sqrt((r*r)-(b*b));
    float volume = (1.0/6.0) * pi * height * ((3 * (a*a))+ (3 * (b * b)) + (height * height));
    return volume;
}

/*Asks the user for the number of slices, then loops through, requesting radius, ha, and hb for each slice.
Then calculates the colume and total surface area of a given slice. Lastly, prints out the average of all slices

Variables:
segments is the number of slices
r is the radius
checkflag tracks whether or not an input is valid, 1 for yes and 0 for no
tsa - top surface area, bsa - bottom surface area, lsa - lateral surface area
total is the total calculated surface area, volume is the calculated volume*/

int main(void) {
    int segments;
    float r;
    float ha;
    float hb;
    int checkflag = 1;
    float tsa, bsa, lsa, total, volume;

    printf("How many spherical segments you want to evaluate [2-10]?\n");
    scanf("%d", &segments);
    
    while (segments < 2 || segments > 10) {
        printf("How many spherical segments you want to evaluate [2-10]?\n");
        scanf("%d", &segments);
    }
    for (int i = 1; i <= segments; i++) {
        do {
            /*Gather data from user*/
            printf("Obtaining data for spherical segment number %d\n", i);
            printf("What is the radius of the sphere (R)\n");
            scanf("%f", &r);
            printf("What is the height of the top area of the spherical segment (ha)\n");
            scanf("%f", &ha);
            printf("What is the height of the bottom area of the spherical segment (hb)\n");
            scanf("%f", &hb);
            checkflag = 1;

            /*Validate inputs*/
            if (r <= 0 || ha <= 0 || hb <= 0){
                printf("Invalid Input: all numbers must be positive real values.\n");
                checkflag = 0;
            } else if (ha >= r){
                printf("Invalid Input: R = %2.f, ha = %2.f. R must be greater than or equal to ha.\n", r, ha);
                checkflag = 0;
            } else if (hb >= r){
                printf("Invalid Input: R = %2.f, hb = %2.f. R must be greater than or equal to hb.\n", r, hb);
                checkflag = 0;
            } else if (ha <= hb){
                printf("Invalid Input: ha = %2.f, hb = %2.f. ha must be greater than or equal to hb.\n", r, hb);
                checkflag = 0;
            }
        } while (checkflag == 0);

        /* run the functions for calculating surface area and volume, then present to user. */
        tsa = surfacearea(r, ha);
        bsa = surfacearea(r, hb);
        lsa = lateralsurfacearea(r, ha, hb);
        printf("%.2f, %.2f, %.2f\n", tsa, bsa, lsa);
        total = tsa + bsa + lsa;
        volume = calcvolume(r, ha, hb);
        printf("Total Surface Area = %.2f, Volume %.2f.\n", total, volume);
    }

    printf("Entered data: R = %.2f, ha = %.2f, hb = %.2f\n", r, ha, hb);
}