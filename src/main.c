#include <pebble.h>
#include "main.h"
#define KEY_TEMPERATURE 1
#define KEY_CONDITIONS 1

static Window *s_window;
static TextLayer *s_time_layer;
static TextLayer  *m_time_layer, *s_date_layer, *s_weather_layer;
static int s_battery_level;
static Layer *s_battery_layer;
static TextLayer *s_label_secondtick;
static GFont s_time_font,s_time_font1;



// A struct for our specific settings (see main.h)
ClaySettings settings;


// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.ForegroundColor = GColorWhite;
  settings.btbColor = GColorWhite;
  settings.btfColor = GColorChromeYellow;
  settings.DateColor = GColorWhite;
  settings.HourColor = GColorWhite;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  
  // Background color
  window_set_background_color(s_window, settings.BackgroundColor);
  text_layer_set_text_color(m_time_layer, settings.ForegroundColor);
  text_layer_set_text_color(s_time_layer, settings.HourColor);
    text_layer_set_text_color(s_date_layer, settings.DateColor);

  
   // Seconds
  //if (settings.Cel) {
  //  text_layer_set_text(s_label_secondtick, "Farenheit : enabled");
 // } else {
  //  text_layer_set_text(s_label_secondtick, "Farenheit : disabled");
//  }

}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Foreground Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  if (fg_color_t) {
    settings.ForegroundColor = GColorFromHEX(fg_color_t->value->int32);
  }
  
   // Foreground Color
  Tuple *hr_color_t = dict_find(iter, MESSAGE_KEY_HourColor);
  if (hr_color_t) {
    settings.HourColor = GColorFromHEX(hr_color_t->value->int32);
  }
   // Foreground Color
  Tuple *btf_color_t = dict_find(iter, MESSAGE_KEY_btfColor);
  if (btf_color_t) {
    settings.btfColor = GColorFromHEX(btf_color_t->value->int32);
  } // Foreground Color
  
  Tuple *btb_color_t = dict_find(iter, MESSAGE_KEY_btbColor);
  if (btb_color_t) {
    settings.btbColor = GColorFromHEX(btb_color_t->value->int32);
  }
 

//Date Color  
  Tuple *date_color_t = dict_find(iter, MESSAGE_KEY_DateColor);
  if (date_color_t) {
    settings.DateColor = GColorFromHEX(date_color_t->value->int32);
  }


  // Save the new settings to persistent storage
  prv_save_settings();
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H" : "%I", tick_time);
  static char m_buffer[8];
  strftime(m_buffer, sizeof(m_buffer), clock_is_24h_style() ?
                                          "%M" : "%M", tick_time);
  
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
    text_layer_set_text(s_date_layer, date_buffer);

   text_layer_set_text(m_time_layer, m_buffer);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
  update_time();
  
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
  
  
  
   if(tick_time->tm_min > 0 && tick_time->tm_min < 16) {
       //Top Time
       layer_set_frame(text_layer_get_layer(m_time_layer), GRect(17, 01, 110, 48));
       text_layer_set_text_alignment(m_time_layer, GTextAlignmentCenter); 
     
  }else if(tick_time->tm_min > 16 && tick_time->tm_min < 31) {
       //Right Time
       layer_set_frame(text_layer_get_layer(m_time_layer), GRect(67, 40, 110, 48));
       text_layer_set_text_alignment(m_time_layer, GTextAlignmentCenter); 
     
  }else if(tick_time->tm_min > 31 && tick_time->tm_min < 46) {
       //Left Time
       layer_set_frame(text_layer_get_layer(m_time_layer), GRect(-32, 40, 110, 48));
       text_layer_set_text_alignment(m_time_layer, GTextAlignmentCenter); 
  }else if(tick_time->tm_min > 46 && tick_time->tm_min < 59) {
       //Bottom Time
       layer_set_frame(text_layer_get_layer(m_time_layer), GRect(17, 73, 110, 48));
       text_layer_set_text_alignment(m_time_layer, GTextAlignmentCenter); 
  }
  


}


static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  
  // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * 114.0F);

  // Draw the background
  graphics_context_set_fill_color(ctx, settings.btbColor);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, settings.btfColor);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

// Window Load event
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create GFont
s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SKY_48));
s_time_font1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SKY_36));


  s_label_secondtick = text_layer_create(GRect(0, 10, bounds.size.w, 30));
  text_layer_set_text_alignment(s_label_secondtick, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_secondtick, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_secondtick));
  
    // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(GRect(0, 30, 144, 100));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);

  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
   // Improve the layout to be more like a watchface
  PBL_IF_ROUND_ELSE(m_time_layer = text_layer_create(GRect(-5, 55, 144, 84)), m_time_layer = text_layer_create(GRect(67, 40, 110, 48)));
  
  text_layer_set_text_color(m_time_layer, GColorBlack);
  text_layer_set_font(m_time_layer, s_time_font1);
  text_layer_set_background_color(m_time_layer, GColorClear);
  text_layer_set_text_alignment(m_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(m_time_layer));
  
  // Settting up battery layer ----- BATTERY 
  layer_add_child(window_layer, text_layer_get_layer(m_time_layer));
  // Create battery meter Layer
  PBL_IF_ROUND_ELSE(s_battery_layer = layer_create(GRect(40, 25, 100, 3)), s_battery_layer = layer_create(GRect(15, 120, 115, 3)));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), s_battery_layer);

// Setting up date layer ---- DATE DISPLAY 
  layer_add_child(window_layer, s_battery_layer);
  PBL_IF_ROUND_ELSE(s_date_layer = text_layer_create(GRect(0, 40, 180, 75)), s_date_layer = text_layer_create(GRect(0, 145, 144, 36)));
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  

  
  //--------------------------------
  prv_update_display();
}

// Window Unload event
static void prv_window_unload(Window *window) {
fonts_unload_custom_font(s_time_font);

  // Destroy TextLayer
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_layer);
  // Destroy TextLayer
  text_layer_destroy(m_time_layer);
}

static void prv_init(void) {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  window_stack_push(s_window, true);
   // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
    // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());

}

static void prv_deinit(void) {
    window_destroy(s_window);

}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
