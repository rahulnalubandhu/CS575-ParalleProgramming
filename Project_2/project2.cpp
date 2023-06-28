
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Declaration of variables.
int NowYear = 2023; // 2023 - 2028
int NowMonth = 0;   // 0 - 11

float NowPrecip; // inches of rain per month
float NowTemp;   // temperature this month

float NowHeight = 8.;  // rye grass height in inches
int NowNumRabbits = 15; // number of rabbits in the current population

const float RYEGRASS_GROWS_PER_MONTH = 20.0;
const float ONE_RABBITS_EATS_PER_MONTH = 1.0;

const float AVG_PRECIP_PER_MONTH = 12.0; // average
const float AMP_PRECIP_PER_MONTH = 4.0;  // plus or minuss
const float RANDOM_PRECIP = 2.0;         // plus or minus noise

const float AVG_TEMP = 60.0;    // average
const float AMP_TEMP = 20.0;    // plus or minus
const float RANDOM_TEMP = 10.0; // plus or minus noise

const float MIDTEMP = 60.0;
const float MIDPRECIP = 14.0;

int NowNumVampires = 5;

unsigned int seed = 0;

omp_lock_t Lock;
volatile int NumInThreadTeam;
volatile int NumAtBarrier;
volatile int NumGone;

// specify how many threads will be in the barrier:
//	(also init's the Lock)
void InitBarrier(int n)
{
    NumInThreadTeam = n;
    NumAtBarrier = 0;
    omp_init_lock(&Lock);
}

// have the calling thread wait here until all the other threads catch up:
void WaitBarrier()
{
    omp_set_lock(&Lock);
    {
        NumAtBarrier++;
        if (NumAtBarrier == NumInThreadTeam)
        {
            NumGone = 0;
            NumAtBarrier = 0;
            // let all other threads get back to what they were doing
            // before this one unlocks, knowing that they might immediately
            // call WaitBarrier( ) again:
            while (NumGone != NumInThreadTeam - 1)
                ;
            omp_unset_lock(&Lock);
            return;
        }
    }
    omp_unset_lock(&Lock);

    while (NumAtBarrier != 0)
        ; // this waits for the nth thread to arrive

#pragma omp atomic
    NumGone++; // this flags how many threads have returned
}

// squaring function.
float Sqr(float x)
{
    return x * x;
}

float Ranf(unsigned int *seedp, float low, float high)
{
    float r = (float)rand_r(seedp); // 0 - RAND_MAX

    return (low + r * (high - low) / (float)RAND_MAX);
}

void Rabbits()
{
    while (NowYear < 2029)
    {
        int nextNumRabbits = NowNumRabbits;
        int carryingCapacity = (int)(NowHeight * 2);

        if (carryingCapacity > nextNumRabbits)
            nextNumRabbits += 2;
        else
            nextNumRabbits -= 1;

        if (nextNumRabbits < 0)
            nextNumRabbits = 0;

        // done computing
        WaitBarrier();
        NowNumRabbits = nextNumRabbits;
        if (NowNumRabbits < 0){
            NowNumRabbits = 0;
        }

        // done assigning
        WaitBarrier();
        // done printing
        WaitBarrier();
    }
}


void RyeGrass()
{
    while (NowYear < 2029)
    {
        float nextHeight = NowHeight;

        float tempFactor = exp(-Sqr((NowTemp - MIDTEMP) / 10.));
        float precipFactor = exp(-Sqr((NowPrecip - MIDPRECIP) / 10.));

        nextHeight += tempFactor * precipFactor * RYEGRASS_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumRabbits * ONE_RABBITS_EATS_PER_MONTH;

        if (nextHeight < 0.)
            nextHeight = 0.;
        // done computing
        WaitBarrier();
        NowHeight = nextHeight;
        // done assigning
        WaitBarrier();
        // done printing
        WaitBarrier();
    }
}

void Watcher()
{
    while (NowYear < 2029)
    {
        // Done Computing barrier.
        WaitBarrier();

        // Done Assigning barrier.
        WaitBarrier();

        fprintf(stderr, "%d,%f,%d,%f,%f,%d\n", NowMonth + 1, NowHeight * 2.54, NowNumRabbits, (NowTemp - 32) * (5. / 9.), NowPrecip, NowNumVampires);

        NowMonth++;

        if (NowMonth > 11)
        {
            NowMonth = 0;
            NowYear++;
        }

        float ang = (30. * (float)NowMonth + 15.) * (M_PI / 180.);

        float temp = AVG_TEMP - AMP_TEMP * cos(ang);
        NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
        NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);

        if (NowPrecip < 0.)
            NowPrecip = 0.;

        // Done Printing barrier.
        WaitBarrier();
    }
}

void Vampire()
{
    while (NowYear < 2029)
    {   
        
        int nextNumVampires = NowNumVampires;


        // If vampires are less than rabbits, vampires eat rabbits
        if (NowNumRabbits >= 10)
        {
            nextNumVampires += 2;
        }else if( NowNumRabbits <= 8 && NowNumRabbits >= 5){

            nextNumVampires -=1 ;
        }
        else{
            nextNumVampires -=2;
        }
        // Done computing
        WaitBarrier();
        NowNumVampires = nextNumVampires;
        if(NowNumVampires <= 0){
            NowNumVampires = 1;
        }

        // Done assigning
        WaitBarrier();

        // Done printing
        WaitBarrier();
    }
}
// Main program
int main(int argc, char *argv[])
{
    fprintf(stderr, "Month,Height,NowNumRabbits,Temp,Precip,NowNumVampires\n ");
    // omp_set_num_threads( 4 );	// same as # of sections
    omp_set_num_threads(4); // or 4
    InitBarrier(4);         // or 4

#pragma omp parallel sections
    {
#pragma omp section
        {
            Rabbits();
        }

#pragma omp section
        {
            RyeGrass();
        }

#pragma omp section
        {
            Watcher();
        }

#pragma omp section
        {
            Vampire(); // your own
        }
    } // implied barrier -- all functions must return in order
      // to allow any
    return 0;
}
