#include <stdio.h>
#include <math.h>

int main() {

double phi=-0.00610298;
double phi_1=1.570786;

double sinphi = sin(phi);

printf("sinphi = %f \n", sinphi);
printf("sinhalfpi = %f\n", sin(phi_1));

double phi_2=tan(phi) *  1.0067396607958714;

double rlat = atan(phi_2);

printf("rlat = %f\n", rlat);

return 0;
}
