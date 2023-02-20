#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "program_state.h"
#include "IO.hpp"
#include "config.h"


OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);
extern State state;


uint32_t ticks_50[] = {2838,2783,2739,2701,2668,2639,2613,2589,2566,2545,2526,2507,2487,2471,2455,2440,2425,2411,2397,2384,2371,2358,2346,2333,2322,2310,2299,2288,2277,2266,2255,2245,2235,2225,2215,2206,2196,2187,2177,2168,2159,2150,2141,2133,2124,2115,2107,2099,2090,2082,2074,2066,2058,2050,2042,2034,2027,2019,2011,2004,1996,1989,1981,1974,1967,1960,1952,1945,1938,1931,1924,1917,1910,1903,1896,1890,1883,1876,1869,1862,1856,1849,1842,1836,1829,1823,1816,1810,1803,1797,1790,1784,1777,1771,1764,1758,1752,1745,1739,1733,1726,1720,1714,1708,1701,1695,1689,1683,1676,1670,1664,1657,1651,1645,1639,1633,1627,1621,1615,1608,1602,1596,1590,1584,1578,1572,1566,1559,1553,1547,1541,1535,1529,1523,1517,1510,1504,1498,1492,1486,1480,1474,1468,1461,1455,1449,1442,1436,1430,1424,1417,1411,1405,1399,1392,1386,1380,1373,1367,1361,1354,1348,1341,1335,1328,1322,1315,1309,1302,1296,1289,1283,1276,1269,1263,1256,1249,1242,1235,1229,1222,1215,1208,1201,1194,1187,1180,1173,1165,1158,1151,1144,1136,1129,1121,1114,1106,1098,1091,1083,1075,1067,1059,1051,1043,1035,1026,1018,1010,1001,992,984,975,966,957,948,938,929,919,910,900,890,880,870,859,848,837,826,815,803,792,779,767,754,741,728,714,700,685,670,654,638,618,599,580,559,536,512,486,457,424,386,342,287,216};

void IRAM_ATTR zeroCrossingISR()
{
  if(state.power == 255)
  {
    digitalWrite(TRIAC_PIN,LOW);
    return;
  }
  digitalWrite(TRIAC_PIN,HIGH);
  if(state.power == 0) return;

  uint32_t ticks = ticks_50[state.power-1] + TRIAC_DELAY; // (10ms/255)/3.2us ~= 12 
  timer1_write(ticks);
}

void IRAM_ATTR timerISR()
{
  digitalWrite(TRIAC_PIN,LOW);
}

void IO_init()
{
  //===IO===
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN,HIGH);
  pinMode(ZERO_DETECTION_PIN, FUNCTION_3);
  attachInterrupt(digitalPinToInterrupt(ZERO_DETECTION_PIN),zeroCrossingISR,FALLING);
  //===DS18B20
  sensors.begin();
  sensors.setResolution(11); //0.125°C, 375ms
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  //===TIMER===
  timer1_isr_init();
  timer1_attachInterrupt(timerISR);        
  timer1_enable(TIM_DIV256, TIM_EDGE, TIM_SINGLE);
}


void measure()
{
  state.actualTemp = sensors.getTempCByIndex(0);
  sensors.requestTemperatures();
  if(state.actualTemp == DEVICE_DISCONNECTED_C) 
  {
    state.status = ProgramStatus::ERROR_TEMP_SENSOR;
    state.actualTemp = 0.0f;
  }
  else if(state.actualTemp > OVERHEAT_PROTECTION)
  {
    state.status = ProgramStatus::ERROR_OVERHEAT;
  }
  else
  {
    state.status = ProgramStatus::WORKING;
  }
}