/*
 * main.c 
 * Creates a Window with a custom color. 
 * This hosts a Layer which draws a GPath in custom fill color.
 *
 * Copyright 2015 Matt Thompson All Rights Reserved
 */

#include <pebble.h>
#define CUR_PLAID 0
#define CUR_PLAID_COLOR 1
#define CUR_HAND_COLOR 2
#define WATCH_MODE 3
#define CAL_DATE 4
#define BL_DETECT 5
#define COLOR1 6
#define COLOR2 7
#define COLOR3 8

static Window *s_main_window;
static Layer *s_path_layer, *s_hands_layer;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GPath *s_minute_arrow, *s_hour_arrow;


// GPath describes the shape
static GPath *s_path;
static GPathInfo PATH_INFO = {
  .num_points = 5,
  .points = (GPoint[]) { {30, 60}, {70, 30}, {110, 60}, {110, 120}, {30, 120} }
};

int curPlaid = 0;
int curPlaidColor = 0;
int curHandColor = 0xAAFFAA;
int color1 = 0;
int color2 = 0;
int color3 = 0;
bool digitalWatch = true;
int watchMode = 0;
int calDate = 1;
bool bluetoothOn = true;
int blDetect = 0;
bool customPlaid = false;

static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    { -4, 5 },
    { 4, 5 },
    { 4, -60 },
    { -4, -60 }  
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4, (GPoint []){
    {-4, 5},
    {4, 5},
    {4, -40},
    {-4, -40}, 
  }
};

static uint8_t plaidColor[][4][8][12] = {
    //Gingham
    {{{GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8,GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8, GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8},
    {GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8, GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorCelesteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8, GColorLibertyARGB8, GColorCelesteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8},
    {GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8, GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8, GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8}},

    {{GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8},
    {GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8},
    {GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8}},
    
    {{GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8},
    {GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorMintGreenARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8, GColorKellyGreenARGB8, GColorMintGreenARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8},
    {GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8}},
      
     {{GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8},
    {GColorFollyARGB8, GColorWhiteARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8, GColorFollyARGB8, GColorWhiteARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8},
    {GColorFollyARGB8, GColorMelonARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8, GColorFollyARGB8, GColorMelonARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8},
    {GColorFollyARGB8, GColorWhiteARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8, GColorFollyARGB8, GColorWhiteARGB8, GColorFollyARGB8, GColorSunsetOrangeARGB8}},
      
    
   
    
    },
      
    //Grid
    {{{GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8},
    {GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorCelesteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8},
    {GColorOxfordBlueARGB8, GColorDarkGrayARGB8, GColorOxfordBlueARGB8, GColorCobaltBlueARGB8},
    {GColorLibertyARGB8, GColorWhiteARGB8, GColorLibertyARGB8, GColorPictonBlueARGB8}},

    {{GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8},
    {GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8},
    {GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8},
    {GColorLightGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8}},
    
    {{GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8},
    {GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorMintGreenARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8},
    {GColorDarkGreenARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8},
    {GColorKellyGreenARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorInchwormARGB8}},
    
    {{GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorFollyARGB8},
    {GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorFollyARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorFollyARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorFollyARGB8},
    {GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorFollyARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorDarkGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorFollyARGB8}}
    
    },
      
    //White Stripes  
    {{{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorWhiteARGB8, GColorVividCeruleanARGB8, GColorWhiteARGB8}},
    {{ GColorBlackARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8}},
    {{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGreenARGB8, GColorWhiteARGB8}},
    {{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkCandyAppleRedARGB8, GColorWhiteARGB8}},
    },
      
    //Black Stripes
    {{{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorBlackARGB8, GColorVividCeruleanARGB8, GColorBlackARGB8}},
    {{ GColorBlackARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8}},
    {{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGreenARGB8, GColorBlackARGB8}},
    {{ GColorDarkGrayARGB8, GColorBlackARGB8,  GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkCandyAppleRedARGB8, GColorBlackARGB8}},
    },
    
    
    //Graph
    {
    {{GColorOxfordBlueARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorBlueMoonARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8,
      GColorDarkGrayARGB8,GColorDarkGrayARGB8,GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8},
    {GColorOxfordBlueARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorBlueMoonARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8,
      GColorDarkGrayARGB8,GColorDarkGrayARGB8,GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8},
    {GColorOxfordBlueARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorBlueMoonARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorOxfordBlueARGB8, GColorCelesteARGB8, GColorDarkGrayARGB8, GColorCelesteARGB8, GColorDarkGrayARGB8,
      GColorCelesteARGB8,GColorBlueMoonARGB8,GColorCelesteARGB8, GColorDarkGrayARGB8, GColorCelesteARGB8, GColorDarkGrayARGB8, GColorCelesteARGB8}},
      
    {{GColorBlackARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorDarkGrayARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8,
      GColorDarkGrayARGB8,GColorDarkGrayARGB8,GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8},
    {GColorBlackARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorDarkGrayARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8,
      GColorDarkGrayARGB8,GColorDarkGrayARGB8,GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8, GColorDarkGrayARGB8},
    {GColorBlackARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8,
      GColorWhiteARGB8,GColorDarkGrayARGB8,GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8},
    {GColorBlackARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8,
      GColorLightGrayARGB8,GColorDarkGrayARGB8,GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8}},
      
    {{GColorDarkGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8,
      GColorWhiteARGB8,GColorJaegerGreenARGB8,GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8},
    {GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8,
      GColorJaegerGreenARGB8,GColorJaegerGreenARGB8,GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8},
    {GColorDarkGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8,
      GColorWhiteARGB8,GColorJaegerGreenARGB8,GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8},
    {GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8,
      GColorJaegerGreenARGB8,GColorJaegerGreenARGB8,GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8, GColorJaegerGreenARGB8},
    {GColorDarkGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8,
      GColorWhiteARGB8,GColorJaegerGreenARGB8,GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8, GColorJaegerGreenARGB8, GColorWhiteARGB8},
    {GColorDarkGreenARGB8, GColorMintGreenARGB8, GColorJaegerGreenARGB8, GColorMintGreenARGB8, GColorJaegerGreenARGB8,
      GColorMintGreenARGB8,GColorJaegerGreenARGB8,GColorMintGreenARGB8, GColorJaegerGreenARGB8, GColorMintGreenARGB8, GColorJaegerGreenARGB8, GColorMintGreenARGB8}},
      
    {{GColorDarkCandyAppleRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8,
      GColorWhiteARGB8,GColorRedARGB8,GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8},
    {GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8,
      GColorRedARGB8,GColorRedARGB8,GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8,
      GColorWhiteARGB8,GColorRedARGB8,GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8},
    {GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8,
      GColorRedARGB8,GColorRedARGB8,GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8, GColorRedARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8,
      GColorWhiteARGB8,GColorRedARGB8,GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8, GColorRedARGB8, GColorWhiteARGB8},
    {GColorDarkCandyAppleRedARGB8, GColorMelonARGB8, GColorRedARGB8, GColorMelonARGB8, GColorRedARGB8,
      GColorMelonARGB8,GColorRedARGB8,GColorMelonARGB8, GColorRedARGB8, GColorMelonARGB8, GColorRedARGB8, GColorMelonARGB8}}
    
    },
      
    //Blue boxes
    {{{GColorBlueMoonARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8},
      {GColorBlueMoonARGB8, GColorPictonBlueARGB8, GColorBlackARGB8, GColorPictonBlueARGB8, GColorBlackARGB8, GColorPictonBlueARGB8 }
    },
       {{GColorBlackARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8},
      {GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8 }
    },
    {{GColorDarkGreenARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8},
      {GColorDarkGreenARGB8, GColorMayGreenARGB8, GColorBlackARGB8, GColorMayGreenARGB8, GColorBlackARGB8, GColorMayGreenARGB8 }
    },
    {{GColorDarkCandyAppleRedARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorWhiteARGB8},
      {GColorDarkCandyAppleRedARGB8, GColorRichBrilliantLavenderARGB8, GColorBlackARGB8, GColorRichBrilliantLavenderARGB8, GColorBlackARGB8, GColorRichBrilliantLavenderARGB8 }
    }
      
    },
    
      //Blue and red
     {{
      {GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorBlueARGB8, GColorOxfordBlueARGB8, GColorBlueARGB8, GColorBlackARGB8,GColorBlueARGB8, GColorOxfordBlueARGB8, GColorBlueARGB8, GColorOxfordBlueARGB8},
       {GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorOxfordBlueARGB8, GColorLightGrayARGB8,  GColorBlackARGB8, GColorLightGrayARGB8, GColorOxfordBlueARGB8, GColorLightGrayARGB8,GColorBlackARGB8},
       {GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorBlueARGB8, GColorOxfordBlueARGB8, GColorBlueARGB8, GColorBlackARGB8,GColorBlueARGB8, GColorOxfordBlueARGB8, GColorBlueARGB8, GColorOxfordBlueARGB8},
       {GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlueARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorOxfordBlueARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorOxfordBlueARGB8, GColorLightGrayARGB8, GColorOxfordBlueARGB8 }},
       
       {
      {GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8,GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8},
       {GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8,  GColorBlackARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8,GColorBlackARGB8},
       {GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8,GColorDarkGrayARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorBlackARGB8},
       {GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorDarkGrayARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorBlackARGB8 }},
       
       {
      {GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorKellyGreenARGB8, GColorDarkGreenARGB8, GColorKellyGreenARGB8, GColorBlackARGB8,GColorKellyGreenARGB8, GColorDarkGreenARGB8, GColorKellyGreenARGB8, GColorDarkGreenARGB8},
       {GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorDarkGreenARGB8, GColorLightGrayARGB8,  GColorBlackARGB8, GColorLightGrayARGB8, GColorDarkGreenARGB8, GColorLightGrayARGB8,GColorBlackARGB8},
       {GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorKellyGreenARGB8, GColorDarkGreenARGB8, GColorKellyGreenARGB8, GColorBlackARGB8,GColorKellyGreenARGB8, GColorDarkGreenARGB8, GColorKellyGreenARGB8, GColorDarkGreenARGB8},
       {GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorKellyGreenARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorDarkGreenARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorDarkGreenARGB8, GColorLightGrayARGB8, GColorDarkGreenARGB8 }},

       {
      {GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8, GColorJazzberryJamARGB8, GColorBlackARGB8,GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8, GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8},
       {GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorLightGrayARGB8,  GColorBlackARGB8, GColorLightGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorLightGrayARGB8,GColorBlackARGB8},
       {GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8, GColorJazzberryJamARGB8, GColorBlackARGB8,GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8, GColorJazzberryJamARGB8, GColorDarkCandyAppleRedARGB8},
       {GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorJazzberryJamARGB8, GColorWhiteARGB8, GColorLightGrayARGB8},
       {GColorLightGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorDarkCandyAppleRedARGB8, GColorLightGrayARGB8, GColorDarkCandyAppleRedARGB8 }}

     
     },
    //Wide Veritcal Stripes
    {{{ GColorBlackARGB8,  GColorCobaltBlueARGB8, GColorBlackARGB8, GColorCobaltBlueARGB8, GColorBlackARGB8}},
    {{ GColorBlackARGB8,  GColorLightGrayARGB8, GColorBlackARGB8, GColorLightGrayARGB8, GColorBlackARGB8}},
    {{ GColorBlackARGB8,  GColorMayGreenARGB8, GColorBlackARGB8, GColorMayGreenARGB8, GColorBlackARGB8}},
    {{ GColorBlackARGB8, GColorDarkCandyAppleRedARGB8, GColorBlackARGB8, GColorDarkCandyAppleRedARGB8, GColorBlackARGB8}},
    },
};  

 int plaidWidth[8][2][12] = {
    //Gingham
    {{20,20,20,20,20,20,20,20},
    {20,20,20,20,20,20,20,20}},
    
    //Random
    {{20,2,20,2},
    {20,2,20,2}},
      
    //White Stripes  
    {{1,1,1,4,4,4},
    {168,168,168,168}},
      
    //Black Stripes  
    {{1,1,1,4,4,4},
    {168,168,168,168}},
    
    //Aqua grid
    {{6,6,1,6,1,6,6,6,1,6,1,6},
    {6,1,6,1,6,6,6,1,6,1,6,6}},
   
    //Blue boxes
    {{14,2,2,14,2,2},
    {14,14}},
    
      //Blue and red
    {{6,6,6,6,6,6,6,80},
    {6,6,6,6,6,6,6,80}},

    //Wide vertical grid
    {{30,28,28,28,30},
    {168,168,168,168}}
  };  
  
static void plaid(GContext *ctx){
  //144 × 168 dimensions

  if (curPlaidColor == 4){
      customPlaid = true;
      curPlaidColor = 1;
    } else {
      customPlaid = false;
    }
  
  int curW = 0;
  int curH = 0;
  int curColorCol = 0;
  int curColorRow = 0;
  int curHeight = 0;
  int curWidth = 0;
  
  while (curH < 168){
    if (customPlaid){
      switch (plaidColor[curPlaid][curPlaidColor][curColorRow][curColorCol]){
        case GColorBlackARGB8:
          graphics_context_set_fill_color(ctx, GColorFromHEX(color1));
        break;
        case GColorDarkGrayARGB8:
          graphics_context_set_fill_color(ctx, GColorFromHEX(color2));
        break;
        case GColorLightGrayARGB8:
          graphics_context_set_fill_color(ctx, GColorFromHEX(color3));
        break;
        default:
         graphics_context_set_fill_color(ctx,  (GColor8) plaidColor[curPlaid][curPlaidColor][curColorRow][curColorCol]);
        break;
      }  
    }
    else {
      graphics_context_set_fill_color(ctx,  (GColor8) plaidColor[curPlaid][curPlaidColor][curColorRow][curColorCol]);   
    }
      graphics_fill_rect(ctx, GRect(curW, curH, plaidWidth[curPlaid][0][curWidth], plaidWidth[curPlaid][1][curHeight]),0 ,0);
      
      //Increment colors
      curColorCol++;
      if (curColorCol >= (int) (sizeof(plaidColor[curPlaid][curPlaidColor][curColorRow])/sizeof(plaidColor[curPlaid][curPlaidColor][curColorRow][0])) ){
        curColorCol = 0;
      }
 
    
      //Increment widths
      curW+=plaidWidth[curPlaid][0][curWidth];
      curWidth++;
      if (curWidth >= (int) (sizeof(plaidWidth[curPlaid][0])/sizeof(plaidWidth[curPlaid][0][0])) ){
        curWidth = 0;
      }
      
      //Increment location
      
      if (curW > 144){
        curH+=plaidWidth[curPlaid][1][curHeight];
        curW = 0;
        curColorRow++;
        curColorCol = 0;
        if (curColorRow >= (int) (sizeof(plaidColor[curPlaid][curPlaidColor]) / sizeof(plaidColor[curPlaid][curPlaidColor][0] ) ) ){
          curColorRow = 0;  
        } else 
        //C arrays fill with zeros. Need to check for array both ways until switched to use pointers
          if ( plaidColor[curPlaid][curPlaidColor][curColorRow][0] == 0){
          curColorRow = 0;  
        }
        
        curHeight++;
        curWidth = 0;
        if (curHeight >= (int) (sizeof(plaidWidth[curPlaid][1]) / sizeof(plaidWidth[curPlaid][1][0] ) ) ){
          curHeight = 0;  
        }
      }      
    
  }
  
  //Draw date circle, if needed
  if (calDate == 1){
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, GPoint(72, 166), 18);
  }
  
  //Reset customPlaid
  if (customPlaid)
    curPlaidColor = 4;
    
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now); 
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 10);

  // minute/hour hand
  graphics_context_set_fill_color(ctx,  GColorFromHEX(curHandColor));
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 2);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  // dot in the middle
  //graphics_context_set_fill_color(ctx, GColorBlack);
  //graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, bounds.size.h / 2 - 1, 3, 3), 0, GCornerNone);
  graphics_fill_circle(ctx, center, 7);
}


static void layer_update_proc(Layer *layer, GContext *ctx) {
  plaid(ctx);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
 // if(clock_is_24h_style() == true) {
 //   //Use 2h hour format
//    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
//  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  if (buffer[0] == '0')
    buffer[0] = ' ';
 // }
  
  //Set date
  text_layer_set_text_color(s_date_layer,  GColorFromHEX(curHandColor));
  static char date_str[] = "  ";
  snprintf(date_str, sizeof(date_str), "%d", tick_time->tm_mday);
  text_layer_set_text(s_date_layer, date_str );

  // Display this time on the TextLayer
  if (digitalWatch)
    text_layer_set_text(s_time_layer, buffer);
  else
    layer_mark_dirty(s_hands_layer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  // Get the first pair
  Tuple *t = dict_read_first(iterator);

  // Process all pairs present
  while(t != NULL) {
    // Process this pair's key
    switch (t->key) {
      case CUR_PLAID:
        APP_LOG(APP_LOG_LEVEL_INFO, "CUR_PLAID received with value %d", (int)t->value->int32);
        curPlaid = (int)t->value->int32;
        break;
      case CUR_PLAID_COLOR:
        APP_LOG(APP_LOG_LEVEL_INFO, "CUR_PLAID_COLOR received with value %d", (int)t->value->int32);
        curPlaidColor = (int)t->value->int32;
        break;
      case CUR_HAND_COLOR:
        APP_LOG(APP_LOG_LEVEL_INFO, "CUR_HAND_COLOR received with value %d", (int)t->value->int32);
        curHandColor = (int)t->value->int32;
        break;
      case WATCH_MODE:
        APP_LOG(APP_LOG_LEVEL_INFO, "WATCH_MODE received with value %d", (int)t->value->int32);
        watchMode = (int)t->value->int32;
        break;
       case CAL_DATE:
        APP_LOG(APP_LOG_LEVEL_INFO, "CAL_DATE received with value %d", (int)t->value->int32);
        calDate = (int)t->value->int32;
        break;
      case BL_DETECT:
        APP_LOG(APP_LOG_LEVEL_INFO, "BL_DETECT received with value %d", (int)t->value->int32);
        blDetect = (int)t->value->int32;
        break;
      case COLOR1:
        APP_LOG(APP_LOG_LEVEL_INFO, "COLOR1 received with value %d", (int)t->value->int32);
        color1 = (int)t->value->int32;
        break;
      case COLOR2:
        APP_LOG(APP_LOG_LEVEL_INFO, "COLOR2 received with value %d", (int)t->value->int32);
        color2 = (int)t->value->int32;
        break;
      case COLOR3:
        APP_LOG(APP_LOG_LEVEL_INFO, "COLOR3 received with value %d", (int)t->value->int32);
        color3 = (int)t->value->int32;
        break;
    }
    
    //ANALOG =0, DIGITAL = 1
    if (watchMode == 0){
          layer_set_hidden(s_hands_layer, false);
          layer_set_hidden(text_layer_get_layer(s_time_layer), true);
    } else {
          layer_set_hidden(s_hands_layer, true);
          layer_set_hidden(text_layer_get_layer(s_time_layer), false);
    }
    
    //Show date layer?
    
  if (calDate == 0){
    layer_set_hidden(text_layer_get_layer(s_date_layer), true);
  } else {
    layer_set_hidden(text_layer_get_layer(s_date_layer), false);
  }
    
    
    text_layer_set_text_color(s_time_layer, GColorFromHEX(curHandColor));
    text_layer_set_text_color(s_date_layer, GColorFromHEX(curHandColor));
    layer_mark_dirty(s_path_layer);


    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GPath object
  s_path = gpath_create(&PATH_INFO);

  // Create Layer that the path will be drawn on
  s_path_layer = layer_create(bounds);
  layer_set_update_proc(s_path_layer, layer_update_proc);
  layer_add_child(window_layer, s_path_layer);
  
   // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 60, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorFromHEX(curHandColor));
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(0, 149, 144, 19));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorFromHEX(curHandColor));
  text_layer_set_text(s_date_layer, "28");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  // Add it as a child layer to the Window's root layer
  //if (digitalWatch){
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
    // Make sure the time is displayed from the start
    update_time();
 // } else {
    s_hands_layer = layer_create(bounds);
    layer_set_update_proc(s_hands_layer, hands_update_proc);
    layer_add_child(window_layer, s_hands_layer);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
//  }
  
  if (watchMode == 1)
    layer_set_hidden(s_hands_layer, true);
  else
    layer_set_hidden(text_layer_get_layer(s_time_layer), true);

  if (calDate == 0){
    layer_set_hidden(text_layer_get_layer(s_date_layer), true);
  } else {
    layer_set_hidden(text_layer_get_layer(s_date_layer), false);
  }
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void bluetooth_callback(bool connected) {
  if(!connected && bluetoothOn) {
      // Issue a vibrating alert
      if (blDetect)
        vibes_double_pulse();
      bluetoothOn = false;
    }
  if (connected && !bluetoothOn){
      if (blDetect)
        vibes_short_pulse();
      bluetoothOn = true;
  }
}

static void window_unload(Window *window) {
  // Destroy layer and path
  layer_destroy(s_path_layer);
  gpath_destroy(s_path);
}

static void init(void) {
  //Pull values from saved data
  curPlaid = persist_exists(CUR_PLAID) ? persist_read_int(CUR_PLAID) : 0;
  curPlaidColor = persist_exists(CUR_PLAID_COLOR) ? persist_read_int(CUR_PLAID_COLOR) : 0;
  curHandColor = persist_exists(CUR_HAND_COLOR) ? persist_read_int(CUR_HAND_COLOR) : 0xAAFFAA;
  watchMode = persist_exists(WATCH_MODE) ? persist_read_int(WATCH_MODE) : 0;
  calDate = persist_exists(CAL_DATE) ? persist_read_int(CAL_DATE) : 1;
  blDetect = persist_exists(BL_DETECT) ? persist_read_int(BL_DETECT) : 1;
  color1 = persist_exists(COLOR1) ? persist_read_int(COLOR1) : 0;
  color2 = persist_exists(COLOR2) ? persist_read_int(COLOR2) : 0;
  color3 = persist_exists(COLOR3) ? persist_read_int(COLOR3) : 0;

  // Create main Window
  s_main_window = window_create();

  // Specify custom Window color using byte
  window_set_background_color(s_main_window, (GColor8){ .argb = 0b11010101 });

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);

  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);
  //Add message callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  connection_service_subscribe((ConnectionHandlers) {
  .pebble_app_connection_handler = bluetooth_callback
});
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
  persist_write_int(CUR_PLAID, curPlaid);
  persist_write_int(CUR_PLAID_COLOR, curPlaidColor);
  persist_write_int(CUR_HAND_COLOR, curHandColor);
  persist_write_int(WATCH_MODE, watchMode);
  persist_write_int(CAL_DATE, calDate);
  persist_write_int(BL_DETECT, blDetect);
  persist_write_int(COLOR1, color1);
  persist_write_int(COLOR2, color2);
  persist_write_int(COLOR3, color3);

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}