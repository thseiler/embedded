
#include <EEPROM.h>

void setup() {
  int i = 0;
  
  /* debug */
  Serial.begin(9600);

  /* this is the name you give to the board */
  char name[] = "BOX";
  
  /* copy name to eeprom in the inverse        */
  /* order that is expected by the bootloader  */
  while (name[i] && i<8) {
    EEPROM.write(E2END - i, name[i]);
    i++;
  }
  
  /* mark end */
  EEPROM.write(E2END - i, 0xFF);
 
  delay(2000);

  Serial.print("The board was given the name '");
  Serial.print(name);
  Serial.print("'.");
  Serial.println();
}

void loop() {
  /* do nothing */
}
