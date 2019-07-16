#ifndef __ASCII_FONT_H
#define __ASCII_FONT_H

/* Enable needed fonts here */
#define  USE_FONT_4X6
//#define  USE_FONT_5X8
#define  USE_FONT_5X12
#define  USE_FONT_6X8
#define  USE_FONT_6X10
#define  USE_FONT_7X12
#define  USE_FONT_8X8
//#define  USE_FONT_8X12
//#define  USE_FONT_8X12_CYRILLIC
//#define  USE_FONT_8X14
//#define  USE_FONT_10X16
//#define  USE_FONT_12X16
#define  USE_FONT_12X20
#define  USE_FONT_16X26
//#define  USE_FONT_22X36
//#define  USE_FONT_24X40
//#define  USE_FONT_32X53
#define  USE_FONT_VECTOR_CYRILLIC

/* Specify platform-dependent integer types here */

#define __UG_FONT_DATA const
typedef uint8_t      UG_U8;
typedef int8_t       UG_S8;
typedef uint16_t     UG_U16;
typedef int16_t      UG_S16;
typedef uint32_t     UG_U32;
typedef int32_t      UG_S32;

typedef struct {
	UG_S16 x, y;
} UG_Point, *UG_PointPtr;

typedef enum
{
	FONT_TYPE_1BPP,
	FONT_TYPE_8BPP,
	FONT_TYPE_VECTOR_1BPP
} FONT_TYPE;

#ifdef USE_FONT_VECTOR_CYRILLIC
typedef struct
{
   const char *p[256];
   UG_S16 char_code_width;
   UG_S16 char_code_height;
} UG_VECT_FONT;
#endif

typedef struct
{
   unsigned char* p;
   FONT_TYPE font_type;
   UG_S16 char_width;
   UG_S16 char_height;
   UG_U16 start_char;
   UG_U16 end_char;
   UG_U8  *widths;
} UG_FONT;

#ifdef USE_FONT_4X6
   extern const UG_FONT FONT_4X6;
#endif
#ifdef USE_FONT_5X8
   extern const UG_FONT FONT_5X8;
#endif
#ifdef USE_FONT_5X12
   extern const UG_FONT FONT_5X12;
#endif
#ifdef USE_FONT_6X8
   extern const UG_FONT FONT_6X8;
#endif
#ifdef USE_FONT_6X10
   extern const UG_FONT FONT_6X10;
#endif
#ifdef USE_FONT_7X12
   extern const UG_FONT FONT_7X12;
#endif
#ifdef USE_FONT_8X8
   extern const UG_FONT FONT_8X8;
#endif
#ifdef USE_FONT_8X12
   extern const UG_FONT FONT_8X12;
#endif
#ifdef USE_FONT_8X12_CYRILLIC
   extern const UG_FONT FONT_8X12;
#endif
#ifdef USE_FONT_8X14
   extern const UG_FONT FONT_8X14;
#endif
#ifdef USE_FONT_10X16
   extern const UG_FONT FONT_10X16;
#endif
#ifdef USE_FONT_12X16
   extern const UG_FONT FONT_12X16;
#endif
#ifdef USE_FONT_12X20
   extern const UG_FONT FONT_12X20;
#endif
#ifdef USE_FONT_16X26
   extern const UG_FONT FONT_16X26;
#endif
#ifdef USE_FONT_22X36
   extern const UG_FONT FONT_22X36;
#endif
#ifdef USE_FONT_24X40
   extern const UG_FONT FONT_24X40;
#endif
#ifdef USE_FONT_32X53
   extern const UG_FONT FONT_32X53;
#endif

#ifdef USE_FONT_VECTOR_CYRILLIC
   extern const UG_VECT_FONT VECT_FONT_CYRILLIC;
#endif

#ifndef NULL
   #define NULL ((void*) 0)
#endif

/* Alignments */
#define ALIGN_H_LEFT                                  (1<<0)
#define ALIGN_H_CENTER                                (1<<1)
#define ALIGN_H_RIGHT                                 (1<<2)
#define ALIGN_V_TOP                                   (1<<3)
#define ALIGN_V_CENTER                                (1<<4)
#define ALIGN_V_BOTTOM                                (1<<5)
#define ALIGN_BOTTOM_RIGHT                            (ALIGN_V_BOTTOM|ALIGN_H_RIGHT)
#define ALIGN_BOTTOM_CENTER                           (ALIGN_V_BOTTOM|ALIGN_H_CENTER)
#define ALIGN_BOTTOM_LEFT                             (ALIGN_V_BOTTOM|ALIGN_H_LEFT)
#define ALIGN_CENTER_RIGHT                            (ALIGN_V_CENTER|ALIGN_H_RIGHT)
#define ALIGN_CENTER                                  (ALIGN_V_CENTER|ALIGN_H_CENTER)
#define ALIGN_CENTER_LEFT                             (ALIGN_V_CENTER|ALIGN_H_LEFT)
#define ALIGN_TOP_RIGHT                               (ALIGN_V_TOP|ALIGN_H_RIGHT)
#define ALIGN_TOP_CENTER                              (ALIGN_V_TOP|ALIGN_H_CENTER)
#define ALIGN_TOP_LEFT                                (ALIGN_V_TOP|ALIGN_H_LEFT)

#endif