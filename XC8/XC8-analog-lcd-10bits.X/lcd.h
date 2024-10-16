#include <stdbool.h>

#define DATA  GPIObits.GP0
#define CLOCK GPIObits.GP1
#define ENLCD GPIObits.GP5

#define CMD  0
#define DISP 1

// Pub
extern void init_Lcd(void);
extern void write_Nibbles(unsigned char, bool);
extern void pos_Lcd(unsigned char, unsigned char);
extern void reset_Lcd(void);

// Priv
void write_Lcden(void);
void do_Shiftr(unsigned char);
