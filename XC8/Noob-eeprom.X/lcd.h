#include <stdbool.h>

#define CLOCK GPIObits.GP0
#define ENLCD GPIObits.GP1
#define DATA  GPIObits.GP5

#define CMD 0
#define DISP 1

// Pub
extern void init_Lcd(void);
extern void pos_Lcd(unsigned char, unsigned char);
extern void send_Text(const unsigned char*, unsigned char);
extern void reset_Lcd(void);

// Priv
void write_Lcden(void);
void do_Shiftr(unsigned char);
void send_Nibbles(unsigned char, bool);
void do_Lcden(void);