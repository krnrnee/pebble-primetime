#include <pebble.h>
  
static const GPathInfo MINUTE_HAND_POINTS =
{
  3,
  (GPoint []) {
    { -5, 15 },
    { 5, 15 },
    { 0, -70 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  3, (GPoint []){
    {-5, 15},
    {5, 15},
    {0, -50}
  }
};