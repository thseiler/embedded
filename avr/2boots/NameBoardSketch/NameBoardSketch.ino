#include <EEPROM.h>

  /* IMPORTANT: Please be aware that after you have given a name to your Board,
   *            the bootloader will start to use the SPI lines after reset, in
   *            order to look for an SD Card, and it does this on every boot.
   *            Some SPI chips might not like this. So if you run into problems,
   *            with a SPI chip, you might consider to remove the name from the
   *            board, and see if this helps. To do this, simply use an empty 
   *            string, i.e. "" as name.
   */

void setup() {
  int i = 0;
  
  /* debug */
  Serial.begin(9600);
  Serial.println("Giving board name...");

  /* this is the name you give to the board
   * IMPORTANT: Must comply to MS DOS naming scheme!
   *            - Maximum 8 characters
   *            - Must be UPPER CASE or numbers, (no lower case letters allowed).
   *            - No UTF-8 special characters, like umlauts, accents, etc...
   */
  char name[] = "ARDUINO";
  
  /* copy name to eeprom in the inverse
   * order, as the bootloader is expecting it in reverse order (to save a few bytes)
   */
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
