/*
 * main.c 
 * Creates a Window with a custom color. 
 * This hosts a Layer which draws a GPath in custom fill color.
 */

#include <pebble.h>

static Window *s_main_window;
static Layer *s_path_layer;
static TextLayer *s_time_layer;


// GPath describes the shape
static GPath *s_path;
static GPathInfo PATH_INFO = {
  .num_points = 5,
  .points = (GPoint[]) { {30, 60}, {70, 30}, {110, 60}, {110, 120}, {30, 120} }
};

int boxW = 24;
int curPlaid = 0;

static GColor8 getRandomColor(){
 
  GColor8 color = GColorFromRGB((rand() % 255), (rand() % 255), (rand() % 255));

  return color;
} 

static void makePlaid(GColor8 colorz[4][4]){
  
    
  
   GColor8 plaidz[3][4][4] = {
    {{GColorOxfordBlue, GColorDarkGray, GColorOxfordBlue, GColorCobaltBlue},
    {GColorLiberty, GColorWhite, GColorLiberty, GColorPictonBlue},
    {GColorOxfordBlue, GColorDarkGray, GColorOxfordBlue, GColorCobaltBlue},
    {GColorLiberty, GColorCeleste, GColorLiberty, GColorPictonBlue}},

    {{GColorBlack, GColorDarkGray, GColorBlack, GColorDarkGray},
    {GColorLightGray, GColorWhite, GColorLightGray, GColorWhite},
    {GColorBlack, GColorDarkGray, GColorBlack, GColorDarkGray},
    {GColorLightGray, GColorWhite, GColorLightGray, GColorWhite}},
    
    {{GColorDarkGreen, GColorDarkGray, GColorDarkGreen, GColorIslamicGreen},
    {GColorKellyGreen, GColorWhite, GColorKellyGreen, GColorInchworm},
    {GColorDarkGreen, GColorDarkGray, GColorDarkGreen, GColorIslamicGreen},
    {GColorKellyGreen, GColorMintGreen, GColorKellyGreen, GColorInchworm}}
  };
  

   
  memcpy(colorz, plaidz[curPlaid], sizeof(plaidz[curPlaid]));
}


static void plaid(GContext *ctx){
  //144 × 168 dimensions

  
  int curW = 0;
  int curH = 0;
  int curColorCol = 0;
  int curColorRow = 0;
  
  
 /* GColor8 colorz[4][4] = {
    {GColorOxfordBlue, GColorDarkGray, GColorOxfordBlue, GColorCobaltBlue},
    {GColorLiberty, GColorWhite, GColorLiberty, GColorPictonBlue},
    {GColorOxfordBlue, GColorDarkGray, GColorOxfordBlue, GColorCobaltBlue},
    {GColorLiberty, GColorCeleste, GColorLiberty, GColorPictonBlue}
  };*/
  
  GColor8 colorz[4][4];
  makePlaid(colorz);
  
  while (curH < 168){
      graphics_context_set_fill_color(ctx,  colorz[curColorRow][curColorCol]);   
      graphics_fill_rect(ctx, GRect(curW, curH, boxW, boxW),0 ,0);
      
      //Increment colors
      curColorCol++;
      if (curColorCol >= (int) (sizeof(colorz[curColorRow])/sizeof(colorz[curColorRow][0])) ){
        curColorCol = 0;
      }
      
      
      //Increment location
      curW+=boxW;
      if (curW > 144){
        curH+=boxW;
        curW = 0;
        curColorRow++;
        curColorCol = 0;
        if (curColorRow >= (int) (sizeof(colorz) / sizeof(colorz[0] ) ) ){
          curColorRow = 0;  
      }
      }      
    
  }
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
 // }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_mark_dirty(s_path_layer);
  boxW+=2;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_mark_dirty(s_path_layer);
  boxW-=2;
  if (boxW < 2)
    boxW = 2;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  curPlaid++;
  if (curPlaid > 2)
    curPlaid = 0;
  layer_mark_dirty(s_path_layer);
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
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
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  // Make sure the time is displayed from the start
  update_time();
  
  //Add click handler
  window_set_click_config_provider(s_main_window, click_config_provider);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void window_unload(Window *window) {
  // Destroy layer and path
  layer_destroy(s_path_layer);
  gpath_destroy(s_path);
}

static void init(void) {
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
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}