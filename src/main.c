#include <gb/gb.h>
#include <stdio.h>
#include <gbdk/console.h>

void main(void)
{
    gotoxy(3, 8);
    printf("VERTICAL SHMUP");

    gotoxy(7, 9);
    printf("ENGINE");

    while (1)
    {
        vsync();
    }
}
