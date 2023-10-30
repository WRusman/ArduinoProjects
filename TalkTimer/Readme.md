Timer based on ESP32-wroom and WS2812 led strips to create a timer that can be used on symposia to let speakers know how much time they have left, or to show how much they are over time.
The web-interface also shows the remaining time on the webpage and allows adjustment of timezone and wether the clock counts down to zero, or keeps counting after zero.

The 7-segment displays are printed from a design from thingyverse

The led strip inside is connected like this :

```
`    3  4  5            26 27 28      47 48 49          70 71 72      91 92 93
  2          6       25        29  46        50      69        73  90        94
  1          7  21   24        30  45        51  65  68        74  89        95
  0          8       23        31  44        52      67        75  88        96
    11 10  9           34 33 32      55 54 53          78 77 76      99 98 97
  12        20       35        43  56        64      79        87  100       108
  13        19  22   36        42  57        63  66  80        86  101       107
  14        18       37        41  58        62      81        85  102       106
    15 16 17           38 39 40      59 60 61          82 83 84     103 104 105

```
