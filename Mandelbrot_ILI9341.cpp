
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.

#define TFT_CS 15
#define TFT_DC 2
#define TFT_MOSI 23
#define TFT_CLK 18
#define TFT_RST 4
#define TFT_MISO 19
#define TFT_LED 32

#define double float

typedef struct
{
  int r, g, b;
} COLOUR;

uint16_t
rgbTo565 (uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

COLOUR
GetColour (double v, double vmin, double vmax)
{
  COLOUR c =
    { 255, 255, 255 }; // white
  double dv;
  if (v < vmin)
    v = vmin;
  if (v > vmax)
    v = vmax;
  dv = vmax - vmin;
  if (v < (vmin + 0.25 * dv))
    {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv * 255;
    }
  else if (v < (vmin + 0.5 * dv))
    {
      c.r = 0;
      c.b = (1 + 4 * (vmin + 0.25 * dv - v) / dv) * 255;
    }
  else if (v < (vmin + 0.75 * dv))
    {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv * 255;
      c.b = 0;
    }
  else
    {
      c.g = (1 + 4 * (vmin + 0.75 * dv - v) / dv) * 255;
      c.b = 0;
    }
//  c.r = floor (c.r * 255);
//  c.g = floor (c.g * 255);
//  c.b = floor (c.b * 255);
  return (c);
}

COLOUR c;

Adafruit_ILI9341 tft = Adafruit_ILI9341 (TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK,
TFT_RST,
           TFT_MISO);

int width;
int height;

double x0 = -0.21503361460851339;
double y0c = 0.67999116792639069;

double x1, x2, y1c, y2, x, y;
int i, j;

double cR, cI, R, I, iteration, Rtemp;

void
setup ()
{

  tft.begin ();

  tft.setRotation (1);
  width = tft.width ();  //320
  height = tft.height ();  //240

//  ledcSetup(0, 5000, 8);
//  ledcAttachPin(32, 0);
//  ledcWrite(0, 80);
  Serial.begin (9600);
  Serial.println (F ("TFT LCD test"));
  Serial.print ("TFT size is ");
  Serial.print (width);
  Serial.print ("x");
  Serial.println (height);
  tft.setTextSize (2);
  tft.setTextColor (ILI9341_BLACK);
  randomSeed (analogRead (0));
  ledcAttachPin (TFT_LED, 0);
  ledcSetup (0, 5000, 8);
  ledcWrite (0, 70);
}
double xStore[] =
  { -0.658266142810369, -0.21503361460851339, 0.362563027602331,
      0.296308640491387, 0.376889448556568, 0.288375478361361,
      0.410452789284661, 0.393104502900071, -0.525791949630041,
      0.288117250106003, -0.129777809356499, -0.044920999538265,
      -0.467460633288236, 0.375075932756520, 0.407084260991775,
      0.272113401976109, 0.074422641386633, -1.482308337854703,
      0.380331133286605, -0.038337216755400, -0.473135527559957,
      0.085199004711218, -0.605347743063962, -0.538433560374785,
      -0.445254125674094, -1.766739539051968, -0.526171662677524,
      0.289201709169692, 0.148466855399092, -0.547019172627289,
      -0.561667297134825, -0.525452900107244, -0.250029874541825,
      -0.655311899899588, 0.404286784342259, 0.325681596513175,
      0.279901658059328, 0.399790560175320

  };
double yStore[] =
  { 0.437622059571268, 0.67999116792639069, -0.642474639761669,
      0.587113362465481, -0.259148866240585, -0.578641984311939,
      0.209973442866357, -0.359831201627307, 0.503151420744168,
      0.579716261545153, 0.876592735083569, 0.802928701113609,
      0.584112398387079, -0.282790215137745, 0.325949168796276,
      -0.006137498805174, -0.622489335967747, -0.000826467982459,
      -0.159560987674884, 0.801861658062407, 0.592900036036071,
      0.613215213937243, 0.444961870391686, 0.528632798843020,
      -0.561551822182768, 0.008781835987431, 0.611369047133524,
      0.580177194662057, -0.589686568945376, 0.547582866831690,
      -0.486231369714657, -0.612084552852080, 0.650789521904262,
      -0.420229050239812, 0.346278346566997, 0.551860237273477,
      0.009138453579311, -0.202112672343847

  };

char start[] =
    "This program prints Mandelbrot set: z(0)=0; \n z(n+1)=z(n)^2+c, where "
  " z and c are defined in the complex space. The program will zoom in "
  "a random point until it exceeds the precision of floating number.\n\n"
  "Made by Chen Shen\n2/15/2019\nchenshen@rams.colostate.edu";
char second[] =
    "This board uses 3V input. Using 2 AA battery can last for about 48 hours or use the USB port, but not use both at the same time.\n "
  "If the program hangs, there is a reset button on the back, Press it will restart the program.\n";

int pickN = 38;

void
printStart (char* start)
{
  tft.fillScreen (ILI9341_WHITE);
  tft.setCursor (0, 0);
  tft.setTextSize (2);
  tft.setTextColor (ILI9341_BLACK);
  int i = 0;
  while (start[i] != '\0')
    {
      tft.print (start[i]);
      i++;
      delay (30);
    }
  delay (800);
}

void
loop (void)
{

  printStart (start);

  printStart (second);
// random pick x0 and y0
  int pick = random (pickN);
  x0 = xStore[pick];
  y0c = yStore[pick];
  double dx, dy;
  int maxIte = 300;
  dx = 1;

  char str[50];
  bool FIRST = true;
  int iterationI = 0;
  double dx0;
  double zm = 1;
  tft.fillScreen (ILI9341_BLACK);

  while (dx > 1e-8)
    {
      x1 = x0 - 2. * exp (-zm / 20.);
      x2 = x0 + 2. * exp (-zm / 20.);
      y1c = y0c - 1.5 * exp (-zm / 20.);
      y2 = y0c + 1.5 * exp (-zm / 20.);

      dx = (x2 - x1) / (width - 1);
      dy = (y2 - y1c) / (height - 1);
      if (FIRST)
  {
    dx0 = dx;
  }

      for (i = 0; i < height; i++)
  for (j = 0; j < width; j++)
    {
      if (i == 40 && j == 10)
        {
    tft.setCursor (0, 0);
    sprintf (str, "%5.1f", dx0 / dx);
    tft.print ("Magnification= ");
    tft.print (str);

//    tft.print (" ");
//    tft.print (zm);
    if (FIRST)
      {
        tft.print ("\nPick=");
        tft.print (pick);
        FIRST = false;
      }

        }
      else if (i == 131 && j == 0)
        {
    tft.drawLine (width / 2 - 10, height / 2, width / 2 + 10,
            height / 2, ILI9341_BLACK);
    tft.drawLine (width / 2, height / 2 - 10, width / 2,
            height / 2 + 10, ILI9341_BLACK);

        }
      cR = (j - 1) * dx + x1;
      cI = (i - 1) * dy + y1c;
      R = 0;
      I = 0;
      iteration = 0;
      while (I * I + R * R <= 4. && iteration < maxIte)
        {
    Rtemp = R * R - I * I + cR;
    I = 2 * R * I + cI;
    R = Rtemp;
    iteration = iteration + 1;
        }

      c = GetColour (iteration, maxIte / 8, maxIte);
      tft.drawPixel (j, i, rgbTo565 (c.r, c.g, c.b));
    }

      if (iterationI < 5)
  zm = zm + 20;
      else
  zm = zm + 5;

      iterationI++;
    }
  tft.setTextSize (3);
  tft.setCursor (0, 50);
  tft.println ("Exceed floating number precision!\n\n");
  tft.println ("Restarting ......");
  delay (5000);

}
