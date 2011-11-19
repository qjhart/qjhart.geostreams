#include <stdlib.h>
#include <time.h>
#include "RandomHeight.h"

RandomHeight::RandomHeight
    (int maxLvl, float prob)
{
  // randomize();
  srandom (time(NULL)) ;
  maxLevel = maxLvl;
  probability = prob;
}

int RandomHeight::newLevel(void)
{
int tmpLvl = 1;
  // Develop a random number between 1 and
  // maxLvl (node height).
  while (((((float)random())/((float)RAND_MAX)) < probability) &&
         (tmpLvl < maxLevel))
    tmpLvl++;

  return tmpLvl;
}

