/*Revisions
  rev     date        author      description
  1       12-24-2015  kasprzak    initial creation
  2       09-29-2019  ilan        graphingfunctions

  This pin setting will also operate the SD card

  Pin settings

  Arduino   device
  5V        Vin
  GND       GND
  A0
  A1
  A2         Y+ (for touch screen use)
  A3         X- (for touch screen use)
  A4
  A5
  1
  2
  3
  4         CCS (42 for mega)
  5
  6
  7         Y- (44 for mega)
  8         X+ (46 for mega)
  9         DC (48 on mega * change define)
  10        CS (53 for mega * change define)
  11        MOSI (51 for mega)
  12        MISO  (50 for mega)
  13        CLK (SCK) (52 for mega)
  44        Y- (for touch screen only)
  46        X+ (for touch screen only)
  48        DC
  SDA
  SLC

*/

//


//void setup(){
//}
//
//void loop(){
//}

//for (InputX = xMIN; InputX <= xMAX; InputX += plotfreq) {
//  process_infix_begin_calculation(infxstr, input);
//  // Graph(tft, InputX, InputY, 45, 290, 420, 260, xMIN, xMAX, 5, yMIN, yMAX, 5, infxstr, "X", "Y", DKBLUE, RED, LTMAGENTA, WHITE, BLACK, display1);
///


//void Graph(
//  BigNumber xMIN = -30;
//  BigNumber xMAX = 30;
//  BigNumber yMIN = -30;
//  BigNumber yMAX = 30;
//  BigNumber plotfreq = 1;
//  BigNumber ox, oy;
//
//for (InputX = xMIN; InputX <= xMAX; InputX += plotfreq) {
//  calculate


void plot(TFT_HX8357 & disp, BigNumber x, BigNumber y, BigNumber gx, BigNumber gy, BigNumber w, BigNumber h, BigNumber xlo, BigNumber xhi, BigNumber xinc, BigNumber ylo, BigNumber yhi, BigNumber yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, boolean & redraw) {
    /* a 1
       b 10
       c 20
       d 30
       e 40 */
    BigNumber ydiv, xdiv;
    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    //static double ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    //static double oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    BigNumber i;
    BigNumber temp;
    BigNumber rot, newrot;

    if (redraw == true) {

      redraw = false;
      ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
      oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
      // draw y scale
      for ( i = ylo; i <= yhi; i += yinc) {
        // compute the transform
        temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        if (i == 0) {
          disp.drawLine(gx, temp, gx + w, temp, acolor);
        }
        else {
          disp.drawLine(gx, temp, gx + w, temp, gcolor);
        }

        disp.setTextSize(1);
        disp.setTextColor(tcolor, bcolor);
        disp.setCursor(gx - BigNumber(40), temp);
        // precision is default Arduino--this could really use some format control
        disp.println(int(i));
      }
      // draw x scale
      for (i = xlo; i <= xhi; i += xinc) {

        // compute the transform

        temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
        if (i == 0) {
          disp.drawLine(temp, gy, temp, gy - h, acolor);
        }
        else {
          disp.drawLine(temp, gy, temp, gy - h, gcolor);
        }

        disp.setTextSize(1);
        disp.setTextColor(tcolor, bcolor);
        disp.setCursor(temp, gy + BigNumber(10));
        // precision is default Arduino--this could really use some format control
        disp.println(int(i));
      }

      //now draw the labels
      disp.setTextSize(2);
      disp.setTextColor(tcolor, bcolor);
      disp.setCursor(gx, gy - h - BigNumber(30));
      disp.println(title);

      disp.setTextSize(1);
      disp.setTextColor(acolor, bcolor);
      disp.setCursor(gx, gy + BigNumber(20));
      disp.println(xlabel);

      disp.setTextSize(1);
      disp.setTextColor(acolor, bcolor);
      disp.setCursor(gx - BigNumber(30), gy - h - BigNumber(10));
      disp.println(ylabel);


    }

    //graph drawn now plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy are initialized as static above
    // BigNumber a = 1;
    x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
    y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    disp.drawLine(ox, oy, x, y, pcolor);
    disp.drawLine(ox, oy + BigNumber(1), x, y + BigNumber(1), pcolor);
    disp.drawLine(ox, oy - BigNumber(1), x, y - BigNumber(1), pcolor);
    ox = x;
    oy = y;

  }
