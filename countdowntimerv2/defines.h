#define matrixOePin 5         // VSPICS0 Not Output Enabled Matrix display, 0=enabled, 1=disabled
#define matrixClkPin 18       // VSPICLK Clock signal for Matrix display
#define matrixDataPin 23      // VSPID Data In Matrix display

#define row1Pin 22            // Row 1 of the matrix display
#define row2Pin 21            // Row 2 of the matrix display
#define row3Pin 32            // Row 3 of the matrix display
#define row4Pin 33            // Row 4 of the matrix display
#define row5Pin 25            // Row 5 of the matrix display
#define row6Pin 26            // Row 6 of the matrix display
#define row7Pin 27            // Row 7 of the matrix display

#define bigDataPin 13         // blue wire = Data for the large 7-segment display
#define bigClockPin 14        // green/white wire = Clock for the large 7-segment display
#define bigStrobePin 15       // green wire = Strobe for the large 7-segment display 


#define numChar 57            // 3 displays of 19 characters each
#define numRow 7              // 7 rows of pixels per characters
#define numReg 36             // 36 8-bit shift registers total

#if CONFIG_FREERTOS_UNICORE
#define TASK_RUNNING_CORE 0
#else
#define TASK_RUNNING_CORE 1
#endif
