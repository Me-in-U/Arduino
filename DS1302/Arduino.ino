#include <DS1302.h>
const int CEPin = 5;
const int IOPin = 6;
const int CLKPin = 7;

DS1302 rtc(CEPin, IOPin, CLKPin);

String dayAsString(const Time::Day day)
{
  switch (day)
  {
    {
    case Time::kSunday:
      return "Sunday";
    case Time::kMonday:
      return "Monday";
    case Time::kTuesday:
      return "Tuesday";
    case Time::kWednesday:
      return "Wednesday";
    case Time::kThursday:
      return "Thursday";
    case Time::kFriday:
      return "Friday";
    case Time::kSaturday:
      return "Saturday";
    }
    return "(unknown day)";
  }
}

