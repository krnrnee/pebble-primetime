/* 
 * PrimeTime Watchface v1.0
 * 
 * main.c
 *
 * Copyright (c) 2014 Brain Dance Designs LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "main.h"
#include <pebble.h>
  
#include "string.h"
#include "stdlib.h"
 
static GPath *minute_arrow;
static GPath *hour_arrow;
static Layer *battery_layer;
static BitmapLayer *bluetooth_layer;
static BitmapLayer *watchface_layer;

static GBitmap *icon_battery;
static GBitmap *icon_battery_charge;
static uint8_t battery_level;
static bool battery_plugged;

static GBitmap *bluetooth_bitmap;

Window *window;
Layer *hands_layer;
TextLayer *text_layer;
GBitmap *watchface_bitmap;

Layer *date_layer;
TextLayer *day_label;
char day_buffer[6];
TextLayer *month_label;
char month_buffer[6];
TextLayer *num_label;
char num_buffer[4];

char buffer[] = "00:00";
  
 //Battery icon callback handler
static void battery_layer_update_callback(Layer *layer, GContext *ctx) {

	graphics_context_set_compositing_mode(ctx, GCompOpAssign);
	if (!battery_plugged) {
		graphics_draw_bitmap_in_rect(ctx, icon_battery, GRect(0, 0, 24, 12));
		graphics_context_set_stroke_color(ctx, GColorBlack);
		graphics_context_set_fill_color(ctx, GColorWhite);
		graphics_fill_rect(ctx, GRect(7, 4, battery_level / 9, 4), 0, GCornerNone);
	} else {
		graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(0, 0, 24, 12));
	}
}

//Battery state change
static void battery_state_handler(BatteryChargeState charge) {
	battery_level = charge.charge_percent;
	battery_plugged = charge.is_plugged;
	layer_mark_dirty(battery_layer);
}

//Bluetooth connection status
static void bluetooth_state_handler(bool connected) {
	layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), !connected);
}

//update hands
static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint tempcenter = grect_center_point(&bounds);
  tempcenter.y = tempcenter.y + 5;
  const GPoint center = tempcenter;
  const int16_t secondHandLength = center.x - 10;

  GPoint secondHand;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
  secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;

  // second hand
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, secondHand, center);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, minute_arrow);
  gpath_draw_outline(ctx, minute_arrow);

  gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, hour_arrow);
  gpath_draw_outline(ctx, hour_arrow);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(bounds.size.w / 2 - 1, (bounds.size.h / 2 - 1 + 5), 3, 3), 0, GCornerNone);
}

static void date_update_proc(Layer *layer, GContext *ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(day_buffer, sizeof(day_buffer), "%a", t);
  text_layer_set_text(day_label, day_buffer);

  strftime(month_buffer, sizeof(month_buffer), "%b", t);
  text_layer_set_text(month_label, month_buffer);

  strftime(num_buffer, sizeof(num_buffer), "%d", t);
  text_layer_set_text(num_label, num_buffer);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    //Here we will update the watchface display
    //Format the buffer string using tick_time as the time source
  if (clock_is_24h_style())
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  else
    strftime(buffer, sizeof("00:00"), "%l:%M", tick_time); 
 
    //Change the TextLayer text to show the new time!
    text_layer_set_text(text_layer, buffer);
  
    //Update the hands layer to move the hands
    layer_mark_dirty(window_get_root_layer(window));    
}

void window_load (Window *window)
{
  //window code here
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Load font
  ResHandle font_handle = resource_get_handle(RESOURCE_ID_FONT_PRIMER_PRINT_BOLD_12);
  
  text_layer = text_layer_create(bounds);
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_load_custom_font(font_handle));
  
  layer_add_child(window_layer, (Layer*) text_layer);
  
  // init date layer -> a plain parent layer to create a date update proc
  date_layer = layer_create(bounds);
  layer_set_update_proc(date_layer, date_update_proc);
  layer_add_child(window_layer, date_layer);

  // init day
  day_label = text_layer_create(GRect(4, 0, 27, 20));
  text_layer_set_text(day_label, day_buffer);
  text_layer_set_background_color(day_label, GColorBlack);
  text_layer_set_text_color(day_label, GColorWhite);
  text_layer_set_font(day_label, fonts_load_custom_font(font_handle));

  layer_add_child(date_layer, text_layer_get_layer(day_label));
  
  // init month
  month_label = text_layer_create(GRect(50, 96, 27, 20));
  text_layer_set_text(month_label, day_buffer);
  text_layer_set_background_color(month_label, GColorBlack);
  text_layer_set_text_color(month_label, GColorWhite);
  text_layer_set_font(month_label, fonts_load_custom_font(font_handle));
  
  layer_add_child(date_layer, text_layer_get_layer(month_label));

// init num
  num_label = text_layer_create(GRect(86, 96, 18, 20));

  text_layer_set_text(num_label, num_buffer);
  text_layer_set_background_color(num_label, GColorBlack);
  text_layer_set_text_color(num_label, GColorWhite);
  text_layer_set_font(num_label, fonts_load_custom_font(font_handle));

  layer_add_child(date_layer, text_layer_get_layer(num_label));

// init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);
  
  //Get a time structure so that the face doesn't start blank
  struct tm *t;
  time_t temp;
  temp = time(NULL);
  t = localtime(&temp);
 
  //Manually call the tick handler when the window is loading
  tick_handler(t, MINUTE_UNIT);
}

void window_unload (Window *window)
{
  //window unload code here
}

void init ()
{
  //initialize app elements here 
  tick_timer_service_subscribe(SECOND_UNIT, (TickHandler) tick_handler);
  battery_state_service_subscribe(&battery_state_handler);
	bluetooth_connection_service_subscribe(bluetooth_state_handler);

  //create window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
  .load = window_load,
  .unload = window_unload,
  });
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
    
  //init buffers
  day_buffer[0] = '\0';
  month_buffer[0] = '\0';
  num_buffer[0] = '\0';
  
  //init watchface
  watchface_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WATCHFACE_BLACK);
  watchface_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  //watchface_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(watchface_layer, watchface_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(watchface_layer));
  
  //init battery indicator
	icon_battery = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_ICON);
	icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGE);

	BatteryChargeState initial = battery_state_service_peek();
	battery_level = initial.charge_percent;
	battery_plugged = initial.is_plugged;
	battery_layer = layer_create(GRect(144-26,2,24,12)); //24*12
	layer_set_update_proc(battery_layer, &battery_layer_update_callback);
	layer_add_child(window_layer, battery_layer);

	//init bluetooth indicator
  bluetooth_layer = bitmap_layer_create(GRect(144-26-10, 2, 9, 12));
	layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer));
	bluetooth_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);
	bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_bitmap);
	layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), !bluetooth_connection_service_peek());
  
  // init hand paths
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  GPoint tempcenter = grect_center_point(&bounds);
  tempcenter.y = tempcenter.y + 13;
  const GPoint center = tempcenter;
  gpath_move_to(minute_arrow, center);
  gpath_move_to(hour_arrow, center);

  //push the window onto the stack
  window_stack_push(window, true);
}

void deinit ()
{
  //deinitialize app elements here
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();

  
  //destroy hands
  gpath_destroy(minute_arrow);
  gpath_destroy(hour_arrow);
 
  //destroy GBitmaps
  gbitmap_destroy(watchface_bitmap);
  
  //destroy layers
  bitmap_layer_destroy(watchface_layer);  
  text_layer_destroy(text_layer);
  layer_destroy(battery_layer);
  bitmap_layer_destroy(bluetooth_layer);
  
  window_destroy(window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}