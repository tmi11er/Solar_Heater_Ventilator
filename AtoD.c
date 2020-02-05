#include <stdlib.h>
#include <math.h>
#include "definitions.h"


// if the Channel is 5 then I am sampling the maximum temperature setting
// The other channels are inside 7 and outside 6
//
double AtoD(unsigned int Channel)
{

//    double Mean = 0;
 //   double Deviation;
//    double E;
    double Rt;
    double T;
    double A = 0.6870561825E-3;  // Stein-hart coefficients for thermistors
    double B = 2.895424923E-4;
    double C = 0.002244232804E-7; 
    static int OverCount = 0;
//    int NumberofSamples =500;
    
    
 #ifdef pic688   
    ADCON0bits.CHS = Channel;   // Select Pin to Sample
#else
    ADPCH = Channel;
#endif
 
  // Auto matic accumulate and average in setupAtoD
//    for (i = 0; i < NumberofSamples; i++) {
    // Delay for sampling time.
//    ADCNT = 0;   
     ADCON0bits.GO = 1;         // Start Conversion
     while(ADCON0bits.GO_nDONE) {}; // Wait for the conversion to finish
     ADResult =  ADRES;
//     Mean += ADResult;
//    }
//    ADResult = Mean/NumberofSamples;
     
     
     // Door Position volts
     if(Channel == 0b000100){
         if(stv == - 100.0)
          ADPrevious_DoorVoltage = ADResult;    // first time through
         // I use 10 as the standard deviation in the AtoD return value
         stv = (double) ((int) ADResult - (int) ADPrevious_DoorVoltage) ;
         stv = stv/ sqrt((pow(30.0,2.0)/pow(47.0,2.0))+(pow(30.0,2.0)/pow(47.0,2.0))); // t = (x1 - x2) - (u1 - u2)/sqrt((s1^2/n^2)+s2^2/n^2) where u1         
         ADPrevious_DoorVoltage = ADResult;
         return (((double)(ADResult)/4095.0) * 5.0);        // voltage is the value

     }

//     Mean += Samples[i];
//    }
     if (Channel == 0b010011 || Channel == 0b010010) { // Inside or Outside Temperature
        Rt = (((double) (ADResult) / 4095.0)*10000) / (1 - (double) (ADResult) / 4095.0);
        T = 1 / (A + B * log(Rt) + C * pow((log(Rt)), 3));
        ADResult = T - 273.15;
        
        


        //    qsort(Samples,sizeof(Samples)/sizeof(Samples[0]),sizeof(Samples[0]),sortthem);

        // Test the normality of the distribution
        //    if(linearCorrelationCoefficient(NumberofSamples, 95) == 1) { // The distribution is normal
        //        i++;
        //    } else {  // The distribution is not normally distributed
        //        i++;
        //    }
//        Deviation = 0.8; // the deviation from the physical device 0.86 degrees Celsius
        //    for (i = 0; i < NumberofSamples; i++) {
        //        Mean /= NumberofSamples;
        //        Deviation += sqrt((Samples[i] - Mean)*(Samples[i] - Mean)) / (20 - 1);
        //    }
        //    Deviation = sqrt(Deviation);
        // t critical Values (2 tails)
        // 99% (19 DOF) 2.861
        // 98% 2.539   
        // 95% 2.093
        // 90% 1.729
        // 80% 1.328
        //    Rt = (((double)(ADPrevious)/8192.0)*10000)/(1-(double)(ADPrevious)/8192.0);
        //    T = (1/(A + B*log(Rt) + C*pow((log(Rt)),3))) - 273.15;

        // sqrt (0.8^2/47 + 0.8^2/47) = 0.165027399401
        //    t = 2.630330;      // 99% confidence level
        // If abs(t) is greater than 2.630330 then reject the null hypothesis that the
        // 2 true means are equal.
        //    E = t*Deviation/sqrt(NumberofSamples);  // Margin of Error
        //    THe sample I took has a statistical mean and a statistical deviation.  The
        //    value of the thing being measured has a true mean and a true deviation.  Statistically
        //    there is a level at which a mathematicial is 98% confident that the statistical
        //    mean and the standard deviation are equal to the true mean and the true deviation.
        //            The Margin of error of the mean, I would think, I would be 100% confident the
        //            true mean is within it, everything else being correct.  That establishes a range
        //            of the true mean.  However, below I had a standard deviation and I was 97.5% confident that
        //            it was equal to the true mean.  However, I used the margin of error to establish a range for
        //                    that standard deviation and instead of being 100% confident the true mean was in
        //                    that range, my confidence depended on the number of samples, and at a certain number
        //                    of samples I as 36% confident the true standard deviation was within that range.
        //                    Therefore, it is not that I am 100% confident that the true mean is within the
        //                    margin of error, I am 98% confident the true mean is within the margins of error and
        //                    I do not know how confident I am that the true mean is within the range.  
        //                            Working with ranges, the margin or margins of error are the maximum error of the
        //                            estimate.  When I have, therefore, a sample and I get a sample mean and a sample
        //                            standard deviation or variance, I still know there is a true mean and true standard 
        //                            deviation of the thing of which I am taking a sample.  The margin of error, made
        //                            by combining the sample mean and the sample standard deviation gives me the maximum
        //                            error of the estimate. The true value I am estimating, but I do not know what it is 
        //                            unless or without the true standard deviation.  If I do not know the true mean and I
        //                            do not know the true standard deviation and if the values are from a normal distribution
        //                            and there are enough of them then there is a realationship between the sample mean and
        //                            the true mean that will allow me, at some confidence level, to believe I know the true
        //                            mean.  With that true mean estimate I can then calculate the true standard deviation too or 
        //                            some value in percent of how close I am to it based on my sample size.  The difference then
        //                            here is that the mean of the sample is found, and believing the sample has a normal distribution
        //                            and there are enough samples so that the sample mean is the best estimate of the true mean (there
        //                                    went my calculation of how far I am away from the true mean in percent) I can find how
        //                                    close I am in percent to the true standard deviation.
        //                                    
        //                                    So, here, I begin with the sample mean, I insist that it is a normal distribution and so
        //                                    I am estimating the true mean because the distribution is normal, believing I have the
        //                                    true mean I can calculate how close I am to the true standard deviation.
        //                                    
        //                                    So, here, now, I want to use this data.  First, I will use the mean if I have a normal
        //                                    distribution, and for that I would need to know how much the distribution I have departs
        //                                    from the normal distribution.  To do this I should order the samples, divide the sample into
        //                                    quintiles, find the z-scores that correspond to thoes quintiles, graph the original data verses
        //                                    the z-scoles, then measure the linearity of the line and the straighter it is the more normal
        //                                    the distribution.
        //                                            
        //                                            If I were to do this with 20 data points, n = 20 and each is 1/20 = 1/4 * 1/5 is 1/4th of the
        //                                            sample.  1/2n, 3/2n, 5/2n, 7/2n, 9/2n = 1/40, 3/40, 5/40, 7/40, 9/40 = 1/40, 3/40, 1/8, 7/40, 9/40.
        //                                                    Now I would look those up as 0.025, .075, .125, .175, .225 and get z = -2.81, -2.43, -1.16, -0.93, -0.75
        //    if the new mean is within the range of the old mean then either
        //            
        //    i - 1/n-1 i = 1,...n  0/19, 1/19/ 2/19 3/19... or for 5 0/4, 1/4, 2/4 3/4 4/4 or 0, .25 .5 .75 1.0 or
        //            0.1 0.3 0.5 0.7 0.9 in the middles.  -1.28, -0.52, 0, 0.52, 1.28
        //            Now I take the twenty points and suppose they are  0 1 1 1 3 4 5 7 7 9 9 10 10 10 11 14 14 16 19 20
        //            so I need n = 20 I need z's for 1/40, 3/40,... til there are 20 of them.
        //            
        //            t

        //    meanR = Mean + E;
        //    meanL = Mean - E;
        //    if()

        //    if(mean + E) < (oldMean + Eold)
        // I have an old Mean and Standard Deviation
        // I test if the new mean is between the margins of error of the old mean.
        // If it is I keep the door where it is, if not I move it.
        // I do not know the true standard deviation:
        // X^2 at 97.5% confidence level for 20 samples is right 32.852 & left 8.907 values
        // sqrt(19*s^2/32.852) < sigma < sqrt(19*s^2)/8.907  - if is between these 2 values
        // I am 97.5% confident the true std.dev. is within 36% of this value.
        // 1 calculate old mean std. dev. at 97.5% conf. level to 36% of true value range:
        //    sl = sqrt((19*sold*sold)/32.852);
        //    sr = sqrt((19*sold*sold)/8.907);
        //    if((s > sl) && (s < sr)) {  // the std. dev.s are compatible - follow the mean
        //        if (mean > oldMean) {
        //            open doors a little
        //        } else if (Mean < oldMean) {
        //            close doors a little
        //        } // else do nothing, they are the same.
        //    } else if (s < sl) { // this sample has a smaller standard deviation than the last
        //        if (mean > oldMean) {
        //            open teh doors a little
        //        } else if (Mean < oldMean) {
        //            close the doors a little
        //        }
        //    } else if (s > sr) { // this sample has a larger standard dev. than the last
        //        do not change the door positions, this sample is uncertain
        //    }
        //    oldMean = Mean;
        //    sold = s;
        //        
        //        either s < sl or s > sr,  the new std. dev. is much smaller or larger than the last
        //        
        //        }
        //    }

        //#endif
        if (Channel == 0b010011) {
            if(sti == - 100.0)      // first time through
                ADPrevious_Inside = ADResult;
            sti = (ADResult - ADPrevious_Inside) / sqrt((0.64 / 47)*2); // t = (x1 - x2) - (u1 - u2)/sqrt((s1^2/n^2)+s2^2/n^2) where u1 = u2
            if (fabs(sti) > Zalpha)
                ADPrevious_Inside = ADResult;
            buffer = formatString(sti, 3, (char *) " ");           
//            buffer = ftoa(sti,&status);
//            INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266            
            TXout(studentTin);            
            TXout(buffer);
#else
            I2C_Out(ESP6288ModuleAddress,studentTin);            
            I2C_Out(ESP6288ModuleAddress,buffer);            
#endif
//            INTCONbits.GIE=0;           
        } else if (Channel == 0b010010) {       // Outside Temperature  RC2
            if(sto == -100.0)       // initialization
                ADPrevious_Outside = ADResult;                
            sto = (ADResult - ADPrevious_Outside) / sqrt((0.64 / 47)*2); // t = (x1 - x2) - (u1 - u2)/sqrt((s1^2/n^2)+s2^2/n^2) where u1 = u2 
            if(fabs(sto) > Zalpha)
                ADPrevious_Outside = ADResult;
            buffer = formatString(sto, 3, (char *) " ");           
//            buffer = ftoa(sto,&status);
//            INTCONbits.GIE=0;
#ifdef USE_TX_ESP8266             
            TXout(studentTout);           
            TXout(buffer);
                 TXout("\033[12;2H");           
            if((ADResult > (ADPrevious_Outside + 10.0)) && (OverCount < 1)) {
                OverCount++;
                TXout("T Outside Over Temperature ");
                buffer = formatString((double) (-4.40 * ((ADResult/4095) * 5.0) + 32.0), 3, (char *) " ");
                TXout(buffer);
                ADResult = ADPrevious_Outside;
            } else {
                TXout("                                               ");
                OverCount = 0;
            }
                
                
#else
            I2C_Out(ESP6288ModuleAddress,studentTout);           
            I2C_Out(ESP6288ModuleAddress,buffer);            
#endif
//            INTCONbits.GIE=0;            
        }

    } else { // The Maximum Temperature Knob value or Internal Temp in setupAtoD
         volts = (ADResult/4095) * 5.0;     // voltage measurement
         ADResult = -4.40 * volts + 32.0;       // 10 to 30 C, x = 30*y + -35

    }
     return (ADResult);
     
}
