#ifndef TRANSITION_H
#define TRANSITION_H

#include <stdint.h>

/* Eight display frames per shade, about 0.4 seconds for one fade. */
#define TRANSITION_FRAMES_PER_STEP 8u

/* Initialize the normal DMG palettes and start with a black image. */
void transition_init(void);
/* Change target palettes while the image is black, before fading in. */
void transition_set_palettes(uint8_t background, uint8_t objects0, uint8_t objects1);
/* Blocking screen transitions. LCD must be on, interrupts enabled.
 * Gameplay does not advance. Call only between screens, not inside an ISR.
 * Keep LCD and background enabled while preparing the next black screen.
 */
void transition_fade_out(void);
void transition_fade_in(void);

#endif
