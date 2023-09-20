#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define BITMAP_SIZE 1024

unsigned char bitmap[1024 >> 3] = {0}; // div by 3 (to get number of bytes required to represent bitmap)

// My first attempt
void read_from_first_bitmap() {
  bitmap[0] = 0b11010101;
  for (int i = 0; i < 8; i++) {
    printf("Bit #%d: %d\n", i, (bitmap[0] & (int)(pow(2, 7) / pow(2, i))) > 0 ? 1 : 0);
  }
}

// My second attempt (realizing law of exponents)
void first_improved_read_from_first_bitmap() {
  bitmap[0] = 0b11010101;
  for (int i = 0; i < 8; i++) {
    printf("Bit #%d: %d\n", i, (bitmap[0] & (int) pow(2, 7 - i)) > 0 ? 1 : 0);
  }
}

// My third attempt (remembering bit shifting and its relation to division)
void second_improved_read_from_first_bitmap() {
  bitmap[0] = 0b11010101;
  for (int i = 0; i < 8; i++) {
    printf("Bit #%d: %d\n", i, (bitmap[0] & 128 >> i) > 0 ? 1 : 0);
  }
}

// Now, putting it all together, I figured out generic bit map reading
void generic_bitmap_reading() {
  // Set a few bits in different bytes
  bitmap[0] = 0b11010101;
  bitmap[3] = 0b11000011;

  for (int i = 0; i < BITMAP_SIZE; i++) {
    printf("Bit #%d: %d\n", i, (bitmap[i >> 3] & 128 >> i % 8) > 0 ? 1 : 0);
  }
}
// And as a *challenge*, I also added bitmap logic for heap-based allocations.
// No real purpose since it's logic is pretty much the same as with the generic
// bitmap reading.
void heap_based_generic_bitmap_reading() {
  void *mem = malloc(1024 / BITMAP_SIZE);

  ((unsigned char*) mem)[0] = 0b11010101;
  ((unsigned char*) mem)[3] = 0b11000011;
  
  for (int i = 0; i < BITMAP_SIZE; i++) {
    printf("Bit #%d: %d\n", i, (*(unsigned char*) (mem + (i >> 3) * 8) & 128 >> i % 8) > 0 ? 1 : 0);
  }

  free(mem);
}

int main() {
  read_from_first_bitmap();
  printf("\n");
  first_improved_read_from_first_bitmap();
  printf("\n");
  second_improved_read_from_first_bitmap();
  printf("\n");
  generic_bitmap_reading();
  printf("\n");
  heap_based_generic_bitmap_reading();
}
