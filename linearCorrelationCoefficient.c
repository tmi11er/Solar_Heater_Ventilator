#include <stdlib.h>
#include <math.h>
#include "definitions.h"

extern int Samples[NumberofSamples];

double normsinv( double p);

int linearCorrelationCoefficient(int NumberOfSamples, int ConfidenceLevel)
{
    double *Quantiles;
    if ((Quantiles = (double *) malloc (NumberOfSamples * sizeof(double))) == NULL)
        return -1;      // Cannot allocate enough memory
    
    // The Quantile from Samples is made and the decision of whether the
    // distribution is normal is determined, returning 1 (yes) or 0 (no)
    int i;
    double Sum_xy = 0;
    double Sum_x = 0;
    double Sum_y = 0;
    double Sum_x2 = 0;
    double Sum_y2 = 0;
    double r;
    for (i = 0; i < NumberOfSamples; i++) {
        // The Quantiles are constructed 1/2n, 3/2n, etc.
        Quantiles[i] = (double)(i+(1+i))/(2*NumberOfSamples);
        // The probabilities of e quantile are calculate and replace them in quantiles[]]
        Quantiles[i] = normsinv(Quantiles[i]);
        // The samples are sorted during AotD conversion and Samples[] become x, Quantiles[] y.
        Sum_xy += Samples[i] * Quantiles[i];
        Sum_x += Samples[i];
        Sum_y += Quantiles[i];
        Sum_x2 += Samples[i] * Samples[i];
        Sum_y2 += Quantiles[i] * Quantiles[i];
    }
    free(Quantiles);
    // The linear Coorelation coefficient is calculated
    r = (NumberOfSamples*Sum_xy - Sum_x * Sum_y)/(sqrt(NumberOfSamples*Sum_x2 - Sum_x*Sum_x) * sqrt(NumberOfSamples*Sum_y2 - Sum_y*Sum_y));
    // r is always between -1 and +1
    // DOF=19 95% >=.456, 99% >=.575
    if(r < 0)
        r = r * -1;
    if (((r > 0.456) && (ConfidenceLevel == 95)) || ((r > 0.575) && (ConfidenceLevel == 99))) {
        return 1;       // there is a significant correlation - the distribution is normally distributed
    } else {
        return 0;
    }
    
}
