#ifndef CREDITS_H
#define CREDITS_H

/*
    h1: 1 Text, Huge Font, Center
    h2: 1 Text, Large Font, Center
    h3: 1 Text, Medium Font, Center
    c:  1 Text, Normal Font, Center
    l:  1 Text, Normal Font, Left
    r:  1 Text, Normal Font, Right
    x:  2 Text, Normal Font, (Left,Right)
    f:  2 Text, Normal Font, (Left,'-',Right)
    i:  1 File, Autosize, Center
    s:  1 Number, Spacing in num * normal font size
*/

void credits_init();
void credits_open();
void credits_close();
void credits_update();
var credits_isdone();

#endif
