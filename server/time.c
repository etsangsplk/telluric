#include "time.h"

#include <stdlib.h>
#include <time.h>

timestamp get_timestamp() {
  struct timespec c;
  struct tm* d;
  timestamp out;

  clock_gettime(CLOCK_REALTIME, &c);
  // let glibc handle leap seconds and days
  d = gmtime(&c.tv_sec);

  uint32_t secs = d->tm_yday * 86400;
  secs += d->tm_hour * 60 * 60;
  secs += d->tm_min * 60;
  secs += d->tm_sec;

  // todo defloat
  uint32_t subsec = (float) c.tv_nsec / 1000000000.0f * 64.0f;

  out.i &= (d->tm_year % 2) << 31;
  out.i &= (secs << 6) & 0b01111111111111111111111111000000;
  out.i &= subsec      & 0b00000000000000000000000000111111;

  free(d);
  return out;
}

// check timestamp is within delta, in either direction
bool range_timestamp(timestamp t, interval delta) {

  // todo this can obviously be minimized

  timestamp c = get_timestamp();
  timestamp max = {0};
  timestamp min = {0};

  max.subseconds = c.subseconds + delta.subseconds;
  max.seconds = c.seconds + delta.seconds
    + (max.subseconds < c.subseconds ? 1 : 0);
  max.yearity = max.seconds < c.seconds ? !c.yearity : c.yearity;

  min.subseconds = c.subseconds - delta.subseconds;
  min.seconds = c.seconds - delta.seconds
    - (min.subseconds > c.subseconds ? 1 : 0);
  min.yearity = min.seconds > c.seconds ? !c.yearity : c.yearity;

  if (t.yearity == c.yearity) {
    // usual case
    if (t.i > c.i) {
      // max check
      if (max.yearity != t.yearity) return true;
      if (max.seconds > t.seconds) return true;
      if (max.seconds == t.seconds && max.subseconds > t.subseconds) return true;
      return false;
    } else if (t.i < c.i) {
      // min check
      if (min.yearity != t.yearity) return true;
      if (min.seconds < t.seconds) return true;
      if (min.seconds == t.seconds && min.subseconds < t.subseconds) return true;
    } else {
      // equal
      return true;
    }
  } else {
    // year mismatch
    if (min.yearity != c.yearity && min.yearity == t.yearity) {
      // past year
      if (min.seconds < t.seconds) return true;
      if (min.seconds == t.seconds && min.subseconds < t.subseconds) return true;
    } else if (max.yearity != c.yearity && max.yearity == t.yearity) {
      // next year
      if (max.seconds > t.seconds) return true;
      if (max.seconds == t.seconds && max.subseconds > t.subseconds) return true;
    } else {
      return false;
    }
  }

  return false;
}
