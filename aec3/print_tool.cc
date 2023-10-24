#include <stdio.h>
#include <stdint.h>
#include "print_tool.h"

ProgressBar::ProgressBar() {
    bar[0] = '\0';
}
void ProgressBar::print_bar(float pro) {
    int16_t i = static_cast<int16_t>(pro * 100);
    printf("[%-101s][%d%%][%c]\r",bar,i,lable[i%4]);
    fflush(stdout);
    bar[i++] = '=';
    bar[i] = '\0';
}
