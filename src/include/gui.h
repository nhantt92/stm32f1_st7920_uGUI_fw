#ifndef __GUI_H_
#define __GUI_H_

#include <stdio.h>
#include <stdbool.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>
#include "ascii_font.h"

/* Color structure */
typedef struct S_OBJECT UG_OBJECT;
typedef struct S_WINDOW UG_WINDOW;
typedef uint8_t      UG_COLOR;
typedef uint8_t 	UG_RESULT;
#define UG_RESULT_FAIL	(int8_t)-1
#define UG_RESULT_OK    (int8_t) 0

/* Feature enablers */
#define USE_PRERENDER_EVENT
#define USE_POSTRENDER_EVENT

#define UG_MaxPolyPointNum 20

#ifndef NULL
   #define NULL ((void*) 0)
#endif

/* Default IDs */
#define OBJ_ID_0                                      0
#define OBJ_ID_1                                      1
#define OBJ_ID_2                                      2
#define OBJ_ID_3                                      3
#define OBJ_ID_4                                      4
#define OBJ_ID_5                                      5
#define OBJ_ID_6                                      6
#define OBJ_ID_7                                      7
#define OBJ_ID_8                                      8
#define OBJ_ID_9                                      9
#define OBJ_ID_10                                     10
#define OBJ_ID_11                                     11
#define OBJ_ID_12                                     12
#define OBJ_ID_13                                     13
#define OBJ_ID_14                                     14
#define OBJ_ID_15                                     15
#define OBJ_ID_16                                     16
#define OBJ_ID_17                                     17
#define OBJ_ID_18                                     18
#define OBJ_ID_19                                     19

/* Area structure */
typedef struct
{
   UG_S16 xs;
   UG_S16 ys;
   UG_S16 xe;
   UG_S16 ye;
} UG_AREA;

/* Text structure */
typedef struct
{
   char* str;
   const UG_FONT* font;
   UG_AREA a;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_U8 align;
   UG_S16 h_space;
   UG_S16 v_space;
} UG_TEXT;

/* -------------------------------------------------------------------------------- */
/* -- BITMAP                                                                     -- */
/* -------------------------------------------------------------------------------- */
typedef struct
{
   void* p;
   UG_U16 width;
   UG_U16 height;
   UG_U8 bpp;
   UG_U8 colors;
} UG_BMP;

#define BMP_BPP_1                                     (1<<0)
#define BMP_BPP_2                                     (1<<1)
#define BMP_BPP_4                                     (1<<2)
#define BMP_BPP_8                                     (1<<3)
#define BMP_BPP_16                                    (1<<4)
#define BMP_BPP_32                                    (1<<5)

/* -------------------------------------------------------------------------------- */
/* -- MESSAGE                                                                    -- */
/* -------------------------------------------------------------------------------- */
/* Message structure */
typedef struct
{
   UG_U8 type;
   UG_U8 id;
   UG_U8 sub_id;
   UG_U8 event;
   void* src;
} UG_MESSAGE;

/* Message types */
#define MSG_TYPE_NONE                                 0
#define MSG_TYPE_WINDOW                               1
#define MSG_TYPE_OBJECT                               2

/* -------------------------------------------------------------------------------- */
/* -- OBJECTS                                                                    -- */
/* -------------------------------------------------------------------------------- */
/* Object structure */
struct S_OBJECT
{
   UG_U8 state;                              /* object state                               */
   UG_U8 touch_state;                        /* object touch state                         */
   void (*update) (UG_WINDOW*,UG_OBJECT*);   /* pointer to object-specific update function */
   UG_AREA a_abs;                            /* absolute area of the object                */
   UG_AREA a_rel;                            /* relative area of the object                */
   UG_U8 type;                               /* object type                                */
   UG_U8 id;                                 /* object ID                                  */
   UG_U8 event;                              /* object-specific events                     */
   void* data;                               /* pointer to object-specific data            */
};

/* Currently supported objects */
#define OBJ_TYPE_NONE                                 0
#define OBJ_TYPE_BUTTON                               1
#define OBJ_TYPE_TEXTBOX                              2
#define OBJ_TYPE_IMAGE                                3
#define OBJ_TYPE_CHECKBOX                             4

/* Standard object events */
#define OBJ_EVENT_NONE                                0
#define OBJ_EVENT_CLICKED                             1
#ifdef USE_PRERENDER_EVENT
#define OBJ_EVENT_PRERENDER                           2
#endif
#ifdef USE_POSTRENDER_EVENT
#define OBJ_EVENT_POSTRENDER                          3
#endif
#define OBJ_EVENT_PRESSED                             4
#define OBJ_EVENT_RELEASED                            5


/* Object states */
#define OBJ_STATE_FREE                                (1<<0)
#define OBJ_STATE_VALID                               (1<<1)
#define OBJ_STATE_BUSY                                (1<<2)
#define OBJ_STATE_VISIBLE                             (1<<3)
#define OBJ_STATE_ENABLE                              (1<<4)
#define OBJ_STATE_UPDATE                              (1<<5)
#define OBJ_STATE_REDRAW                              (1<<6)
#define OBJ_STATE_TOUCH_ENABLE                        (1<<7)
#define OBJ_STATE_INIT                                (OBJ_STATE_FREE | OBJ_STATE_VALID)

/* Object touch states */
#define OBJ_TOUCH_STATE_CHANGED                       (1<<0)
#define OBJ_TOUCH_STATE_PRESSED_ON_OBJECT             (1<<1)
#define OBJ_TOUCH_STATE_PRESSED_OUTSIDE_OBJECT        (1<<2)
#define OBJ_TOUCH_STATE_RELEASED_ON_OBJECT            (1<<3)
#define OBJ_TOUCH_STATE_RELEASED_OUTSIDE_OBJECT       (1<<4)
#define OBJ_TOUCH_STATE_IS_PRESSED_ON_OBJECT          (1<<5)
#define OBJ_TOUCH_STATE_IS_PRESSED                    (1<<6)
#define OBJ_TOUCH_STATE_CLICK_ON_OBJECT               (1<<7)
#define OBJ_TOUCH_STATE_INIT                          0

/* -------------------------------------------------------------------------------- */
/* -- WINDOW                                                                     -- */
/* -------------------------------------------------------------------------------- */
/* Title structure */
typedef struct
{
   char* str;
   const UG_FONT* font;
   UG_S8 h_space;
   UG_S8 v_space;
   UG_U8 align;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_COLOR ifc;
   UG_COLOR ibc;
   UG_U8 height;
} UG_TITLE;

/* Window structure */
struct S_WINDOW
{
   UG_U8 objcnt;
   UG_OBJECT* objlst;
   UG_U8 state;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_S16 xs;
   UG_S16 ys;
   UG_S16 xe;
   UG_S16 ye;
   UG_U8 style;
   UG_TITLE title;
   void (*cb)( UG_MESSAGE* );
};

/* Window states */
#define WND_STATE_FREE                                (1<<0)
#define WND_STATE_VALID                               (1<<1)
#define WND_STATE_BUSY                                (1<<2)
#define WND_STATE_VISIBLE                             (1<<3)
#define WND_STATE_ENABLE                              (1<<4)
#define WND_STATE_UPDATE                              (1<<5)
#define WND_STATE_REDRAW_TITLE                        (1<<6)

/* Window styles */
#define WND_STYLE_2D                                  (0<<0)
#define WND_STYLE_3D                                  (1<<0)
#define WND_STYLE_HIDE_TITLE                          (0<<1)
#define WND_STYLE_SHOW_TITLE                          (1<<1)

/* -------------------------------------------------------------------------------- */
/* -- BUTTON OBJECT                                                              -- */
/* -------------------------------------------------------------------------------- */
/* Button structure */
typedef struct
{
   UG_OBJECT* obj;  // to get quick accsess to self system Object from user defined components
   UG_U8 state;
   UG_U8 style;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_COLOR afc;
   UG_COLOR abc;
   const UG_FONT* font;
   UG_U8 align;
   UG_S8 h_space;
   UG_S8 v_space;
   char* str;
}UG_BUTTON;

/* Default button IDs */
#define BTN_ID_0                                      OBJ_ID_0
#define BTN_ID_1                                      OBJ_ID_1
#define BTN_ID_2                                      OBJ_ID_2
#define BTN_ID_3                                      OBJ_ID_3
#define BTN_ID_4                                      OBJ_ID_4
#define BTN_ID_5                                      OBJ_ID_5
#define BTN_ID_6                                      OBJ_ID_6
#define BTN_ID_7                                      OBJ_ID_7
#define BTN_ID_8                                      OBJ_ID_8
#define BTN_ID_9                                      OBJ_ID_9
#define BTN_ID_10                                     OBJ_ID_10
#define BTN_ID_11                                     OBJ_ID_11
#define BTN_ID_12                                     OBJ_ID_12
#define BTN_ID_13                                     OBJ_ID_13
#define BTN_ID_14                                     OBJ_ID_14
#define BTN_ID_15                                     OBJ_ID_15
#define BTN_ID_16                                     OBJ_ID_16
#define BTN_ID_17                                     OBJ_ID_17
#define BTN_ID_18                                     OBJ_ID_18
#define BTN_ID_19                                     OBJ_ID_19

/* Button states */
#define BTN_STATE_RELEASED                            (0<<0)
#define BTN_STATE_PRESSED                             (1<<0)
#define BTN_STATE_ALWAYS_REDRAW                       (1<<1)

/* Button style */
#define BTN_STYLE_2D                                  (0<<0)
#define BTN_STYLE_3D                                  (1<<0)
#define BTN_STYLE_TOGGLE_COLORS                       (1<<1)
#define BTN_STYLE_USE_ALTERNATE_COLORS                (1<<2)
#define BTN_STYLE_NO_BORDERS                          (1<<3)
#define BTN_STYLE_NO_FILL                             (1<<4)

/* Button events */
#define BTN_EVENT_CLICKED                             OBJ_EVENT_CLICKED

/* -------------------------------------------------------------------------------- */
/* -- CHECKBOX OBJECT                                                            -- */
/* -------------------------------------------------------------------------------- */
/* Checkbox structure */
typedef struct
{
   UG_OBJECT* obj; 
   UG_U8 state;
   UG_U8 style;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_COLOR afc;
   UG_COLOR abc;
   const UG_FONT* font;
   UG_U8 align;
   UG_S8 h_space;
   UG_S8 v_space;
   char* str;
   UG_U8 checked;
}UG_CHECKBOX;

/* Default checkbox IDs */
#define CHB_ID_0                                      OBJ_ID_0
#define CHB_ID_1                                      OBJ_ID_1
#define CHB_ID_2                                      OBJ_ID_2
#define CHB_ID_3                                      OBJ_ID_3
#define CHB_ID_4                                      OBJ_ID_4
#define CHB_ID_5                                      OBJ_ID_5
#define CHB_ID_6                                      OBJ_ID_6
#define CHB_ID_7                                      OBJ_ID_7
#define CHB_ID_8                                      OBJ_ID_8
#define CHB_ID_9                                      OBJ_ID_9
#define CHB_ID_10                                     OBJ_ID_10
#define CHB_ID_11                                     OBJ_ID_11
#define CHB_ID_12                                     OBJ_ID_12
#define CHB_ID_13                                     OBJ_ID_13
#define CHB_ID_14                                     OBJ_ID_14
#define CHB_ID_15                                     OBJ_ID_15
#define CHB_ID_16                                     OBJ_ID_16
#define CHB_ID_17                                     OBJ_ID_17
#define CHB_ID_18                                     OBJ_ID_18
#define CHB_ID_19                                     OBJ_ID_19

/* Checkbox states */
#define CHB_STATE_RELEASED                            (0<<0)
#define CHB_STATE_PRESSED                             (1<<0)
#define CHB_STATE_ALWAYS_REDRAW                       (1<<1)

/* Checkbox style */
#define CHB_STYLE_2D                                  (0<<0)
#define CHB_STYLE_3D                                  (1<<0)
#define CHB_STYLE_TOGGLE_COLORS                       (1<<1)
#define CHB_STYLE_USE_ALTERNATE_COLORS                (1<<2)
#define CHB_STYLE_NO_BORDERS                          (1<<3)
#define CHB_STYLE_NO_FILL                             (1<<4)

/* Checkbox events */
#define CHB_EVENT_CLICKED                             OBJ_EVENT_CLICKED


/* -------------------------------------------------------------------------------- */
/* -- TEXTBOX OBJECT                                                             -- */
/* -------------------------------------------------------------------------------- */
/* Textbox structure */
typedef struct
{
   UG_OBJECT* obj; 
   char* str;
   const UG_FONT* font;
   UG_U8 style;
   UG_COLOR fc;
   UG_COLOR bc;
   UG_U8 align;
   UG_S8 h_space;
   UG_S8 v_space;
} UG_TEXTBOX;

/* Default textbox IDs */
#define TXB_ID_0                                      OBJ_ID_0
#define TXB_ID_1                                      OBJ_ID_1
#define TXB_ID_2                                      OBJ_ID_2
#define TXB_ID_3                                      OBJ_ID_3
#define TXB_ID_4                                      OBJ_ID_4
#define TXB_ID_5                                      OBJ_ID_5
#define TXB_ID_6                                      OBJ_ID_6
#define TXB_ID_7                                      OBJ_ID_7
#define TXB_ID_8                                      OBJ_ID_8
#define TXB_ID_9                                      OBJ_ID_9
#define TXB_ID_10                                     OBJ_ID_10
#define TXB_ID_11                                     OBJ_ID_11
#define TXB_ID_12                                     OBJ_ID_12
#define TXB_ID_13                                     OBJ_ID_13
#define TXB_ID_14                                     OBJ_ID_14
#define TXB_ID_15                                     OBJ_ID_15
#define TXB_ID_16                                     OBJ_ID_16
#define TXB_ID_17                                     OBJ_ID_17
#define TXB_ID_18                                     OBJ_ID_18
#define TXB_ID_19                                     OBJ_ID_19

/* -------------------------------------------------------------------------------- */
/* -- IMAGE OBJECT                                                               -- */
/* -------------------------------------------------------------------------------- */
/* Image structure */
typedef struct
{
   UG_OBJECT* obj; 
   void* img;
   UG_U8 type;
} UG_IMAGE;

/* Default image IDs */
#define IMG_ID_0                                      OBJ_ID_0
#define IMG_ID_1                                      OBJ_ID_1
#define IMG_ID_2                                      OBJ_ID_2
#define IMG_ID_3                                      OBJ_ID_3
#define IMG_ID_4                                      OBJ_ID_4
#define IMG_ID_5                                      OBJ_ID_5
#define IMG_ID_6                                      OBJ_ID_6
#define IMG_ID_7                                      OBJ_ID_7
#define IMG_ID_8                                      OBJ_ID_8
#define IMG_ID_9                                      OBJ_ID_9
#define IMG_ID_10                                     OBJ_ID_10
#define IMG_ID_11                                     OBJ_ID_11
#define IMG_ID_12                                     OBJ_ID_12
#define IMG_ID_13                                     OBJ_ID_13
#define IMG_ID_14                                     OBJ_ID_14
#define IMG_ID_15                                     OBJ_ID_15
#define IMG_ID_16                                     OBJ_ID_16
#define IMG_ID_17                                     OBJ_ID_17
#define IMG_ID_18                                     OBJ_ID_18
#define IMG_ID_19                                     OBJ_ID_19

/* Image types */
#define IMG_TYPE_BMP                                  (1<<0)

/* -------------------------------------------------------------------------------- */
/* -- µGUI DRIVER                                                                -- */
/* -------------------------------------------------------------------------------- */
typedef struct
{
  void* driver;
  UG_U8 state;
} UG_DRIVER;

#define DRIVER_REGISTERED                             (1<<0)
#define DRIVER_ENABLED                                (1<<1)

/* Supported drivers */
#define NUMBER_OF_DRIVERS                             3
#define DRIVER_DRAW_LINE                              0
#define DRIVER_FILL_FRAME                             1
#define DRIVER_FILL_AREA                              2

/* -------------------------------------------------------------------------------- */
/* -- µGUI CORE STRUCTURE                                                        -- */
/* -------------------------------------------------------------------------------- */
typedef struct
{
   void (*pset)(UG_S16,UG_S16,UG_COLOR);
   UG_S16 x_dim;
   UG_S16 y_dim;
   // UG_TOUCH touch;
   UG_WINDOW* next_window;
   UG_WINDOW* active_window;
   UG_WINDOW* last_window;
   struct
   {
      UG_S16 x_pos;
      UG_S16 y_pos;
      UG_S16 x_start;
      UG_S16 y_start;
      UG_S16 x_end;
      UG_S16 y_end;
      UG_COLOR fore_color;
      UG_COLOR back_color;
   } console;
   UG_FONT font;
   UG_S8 char_h_space;
   UG_S8 char_v_space;
   UG_COLOR fore_color;
   UG_COLOR back_color;
   UG_COLOR desktop_color;
   UG_U8 state;
   UG_DRIVER driver[NUMBER_OF_DRIVERS];
} UG_GUI;

#define UG_SATUS_WAIT_FOR_UPDATE                      (1<<0)

UG_S8 UG_Init(UG_GUI* g, void (*p)(UG_S16, UG_S16, UG_COLOR), UG_S16 x, UG_S16 y);
UG_S8 UG_SelectGUI(UG_GUI* g);
void UG_FontSelect(const UG_FONT* font);
void UG_FillScreen(UG_COLOR c);
void UG_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c);
void UG_FillRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c);
void UG_DrawMesh(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c);
void UG_DrawFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c);
void UG_DrawRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c);
void UG_DrawPixel(UG_S16 x0, UG_S16 y0, UG_COLOR c);
void UG_DrawCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c);
void UG_FillCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c);
void UG_DrawArc(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c);
void UG_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c);
void UG_FillPoly(UG_PointPtr p, UG_U8 n, UG_U8 filled, UG_COLOR color);
void UG_PutString(UG_S16 x, UG_S16 y, const char* str);
void UG_PutChar(char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc);
void UG_ConsolePutString(const char* str);
void UG_ConsoleSetArea(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye);
void UG_ConsoleSetForecolor(UG_COLOR c);
void UG_ConsoleSetBackcolor(UG_COLOR c);
void UG_SetForecolor(UG_COLOR c);
void UG_SetBackcolor(UG_COLOR c);
UG_S16 UG_GetXDim(void);
UG_S16 UG_GetYDim(void);
void UG_FontSetHSpace(UG_U16 s);
void UG_FontSetVSpace(UG_U16 s);
void UG_GetVectFont(UG_FONT *font, UG_U16 w, UG_U16 h);

/* Miscellaneous functions */
void UG_WaitForUpdate(void);
void UG_Update(void);
void UG_DrawBMP(UG_S16 xp, UG_S16 yp, UG_BMP* bmp);
void UG_TouchUpdate(UG_S16 xp, UG_S16 yp, UG_U8 state);

/* Driver functions */
void UG_DriverRegister(UG_U8 type, void* driver);
void UG_DriverEnable(UG_U8 type);
void UG_DriverDisable(UG_U8 type);

/* Window functions */
UG_RESULT UG_WindowCreate( UG_WINDOW* wnd, UG_OBJECT* objlst, UG_U8 objcnt, void (*cb)( UG_MESSAGE* ) );
UG_RESULT UG_WindowDelete( UG_WINDOW* wnd );
UG_RESULT UG_WindowShow( UG_WINDOW* wnd );
UG_RESULT UG_WindowHide( UG_WINDOW* wnd );
UG_RESULT UG_WindowResize( UG_WINDOW* wnd, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye );
UG_RESULT UG_WindowAlert( UG_WINDOW* wnd );
UG_RESULT UG_WindowSetForeColor( UG_WINDOW* wnd, UG_COLOR fc );
UG_RESULT UG_WindowSetBackColor( UG_WINDOW* wnd, UG_COLOR bc );
UG_RESULT UG_WindowSetTitleTextColor( UG_WINDOW* wnd, UG_COLOR c );
UG_RESULT UG_WindowSetTitleColor( UG_WINDOW* wnd, UG_COLOR c );
UG_RESULT UG_WindowSetTitleInactiveTextColor( UG_WINDOW* wnd, UG_COLOR c );
UG_RESULT UG_WindowSetTitleInactiveColor( UG_WINDOW* wnd, UG_COLOR c );
UG_RESULT UG_WindowSetTitleText( UG_WINDOW* wnd, char* str );
UG_RESULT UG_WindowSetTitleTextFont( UG_WINDOW* wnd, const UG_FONT* font );
UG_RESULT UG_WindowSetTitleTextHSpace( UG_WINDOW* wnd, UG_S8 hs );
UG_RESULT UG_WindowSetTitleTextVSpace( UG_WINDOW* wnd, UG_S8 vs );
UG_RESULT UG_WindowSetTitleTextAlignment( UG_WINDOW* wnd, UG_U8 align );
UG_RESULT UG_WindowSetTitleHeight( UG_WINDOW* wnd, UG_U8 height );
UG_RESULT UG_WindowSetXStart( UG_WINDOW* wnd, UG_S16 xs );
UG_RESULT UG_WindowSetYStart( UG_WINDOW* wnd, UG_S16 ys );
UG_RESULT UG_WindowSetXEnd( UG_WINDOW* wnd, UG_S16 xe );
UG_RESULT UG_WindowSetYEnd( UG_WINDOW* wnd, UG_S16 ye );
UG_RESULT UG_WindowSetStyle( UG_WINDOW* wnd, UG_U8 style );
UG_COLOR UG_WindowGetForeColor( UG_WINDOW* wnd );
UG_COLOR UG_WindowGetBackColor( UG_WINDOW* wnd );
UG_COLOR UG_WindowGetTitleTextColor( UG_WINDOW* wnd );
UG_COLOR UG_WindowGetTitleColor( UG_WINDOW* wnd );
UG_COLOR UG_WindowGetTitleInactiveTextColor( UG_WINDOW* wnd );
UG_COLOR UG_WindowGetTitleInactiveColor( UG_WINDOW* wnd );
char* UG_WindowGetTitleText( UG_WINDOW* wnd );
UG_FONT* UG_WindowGetTitleTextFont( UG_WINDOW* wnd );
UG_S8 UG_WindowGetTitleTextHSpace( UG_WINDOW* wnd );
UG_S8 UG_WindowGetTitleTextVSpace( UG_WINDOW* wnd );
UG_U8 UG_WindowGetTitleTextAlignment( UG_WINDOW* wnd );
UG_U8 UG_WindowGetTitleHeight( UG_WINDOW* wnd );
UG_S16 UG_WindowGetXStart( UG_WINDOW* wnd );
UG_S16 UG_WindowGetYStart( UG_WINDOW* wnd );
UG_S16 UG_WindowGetXEnd( UG_WINDOW* wnd );
UG_S16 UG_WindowGetYEnd( UG_WINDOW* wnd );
UG_U8 UG_WindowGetStyle( UG_WINDOW* wnd );
UG_RESULT UG_WindowGetArea( UG_WINDOW* wnd, UG_AREA* a );
UG_S16 UG_WindowGetInnerWidth( UG_WINDOW* wnd );
UG_S16 UG_WindowGetOuterWidth( UG_WINDOW* wnd );
UG_S16 UG_WindowGetInnerHeight( UG_WINDOW* wnd );
UG_S16 UG_WindowGetOuterHeight( UG_WINDOW* wnd );

#endif

