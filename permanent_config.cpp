#include <EEPROM.h>

#include "permanent_config.h"

unsigned int uiConfigHolder, uiConfigSize;

struct
{
  unsigned int uiConfigHolder;
  unsigned int uiConfigSize;
} oBaseConfigInfo;

void vInitPermanentConfig(unsigned int uiCurrentConfigHolder, unsigned int uiCurrentConfigSize)
{
  uiConfigHolder = uiCurrentConfigHolder;
  uiConfigSize = uiCurrentConfigSize;
  EEPROM.begin(uiConfigSize + 8);
}

extern void vLoadConfig(void *oConfigStruct)
{
  EEPROM.get(0, oBaseConfigInfo);
  if(oBaseConfigInfo.uiConfigHolder != uiConfigHolder)
  {
    oBaseConfigInfo.uiConfigHolder = uiConfigHolder;
    oBaseConfigInfo.uiConfigSize = uiConfigSize;
    EEPROM.put(0, oBaseConfigInfo);
    for(int i = 0; i < uiConfigSize; i++)
    {
      EEPROM.write(8+i, *((unsigned char *)(oConfigStruct + i)));
    }
    EEPROM.commit();
    return;
  }
  for(int i = 0; i < uiConfigSize; i++)
  {
    *((unsigned char*)(oConfigStruct + i)) = EEPROM.read(8+i);
  }
}
