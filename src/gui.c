#include "gui.h"

/* Static functions */
static void _UG_PutChar( char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc, const UG_FONT* font);
static void _UG_PutText(UG_TEXT* txt);

static UG_RESULT _UG_WindowDrawTitle( UG_WINDOW* wnd );
static void _UG_WindowUpdate( UG_WINDOW* wnd );
static UG_RESULT _UG_WindowClear( UG_WINDOW* wnd );
static void _UG_TextboxUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
static void _UG_ButtonUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
static void _UG_CheckboxUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);
static void _UG_ImageUpdate(UG_WINDOW* wnd, UG_OBJECT* obj);

 /* Pointer to the gui */
static UG_GUI* gui;

UG_S8 UG_Init(UG_GUI* g, void (*p)(UG_S16, UG_S16, UG_COLOR), UG_S16 x, UG_S16 y)
{
	UG_U8 i;

	g->pset = (void(*)(UG_S16,UG_S16,UG_COLOR))p;
	g->x_dim = x;
	g->y_dim = y;

	g->console.x_start = 4;
	g->console.y_start = 4;
	g->console.x_end = g->x_dim - g->console.x_start-1;
	g->console.y_end = g->y_dim - g->console.x_start-1;
	g->console.x_pos = g->console.x_end;
	g->console.y_pos = g->console.y_end;
	g->console.fore_color = 0;
	g->console.back_color = 1;

	g->char_h_space = 1;
	g->char_v_space = 1;

	g->font.p = NULL;
	g->font.char_height = 0;
	g->font.char_width = 0;
	g->font.start_char = 0;
	g->font.end_char = 0;
	g->font.widths = NULL;
	
	g->desktop_color = 0;

	g->fore_color = 1;
	g->back_color = 0;
	// g->next_window = NULL;
	// g->active_window = NULL;
	// g->last_window = NULL;

	/* Clear drivers */
	for(i=0;i<NUMBER_OF_DRIVERS;i++)
	{
		g->driver[i].driver = NULL;
		g->driver[i].state = 0;
	}
	gui = g;
	return 1;
}

UG_S8 UG_SelectGUI(UG_GUI* g)
{
	gui = g;
   	return 1;
}

void UG_FontSelect(const UG_FONT* font)
{
   gui->font = *font;
}

void UG_FillScreen(UG_COLOR c)
{
   UG_FillFrame(0, 0, gui->x_dim-1, gui->y_dim-1, c);
}

void UG_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
   UG_S16 n,m;

   if (x2 < x1)
   {
      n = x2;
      x2 = x1;
      x1 = n;
   }
   if (y2 < y1)
   {
      n = y2;
      y2 = y1;
      y1 = n;
   }

   /* Is hardware acceleration available? */
   if ( gui->driver[DRIVER_FILL_FRAME].state & DRIVER_ENABLED )
   {
      if( ((UG_RESULT(*)(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c))gui->driver[DRIVER_FILL_FRAME].driver)(x1,y1,x2,y2,c) == UG_RESULT_OK ) return;
   }

   for(m = y1; m <= y2; m++ )
   {
      for(n = x1; n <= x2; n++)
      {
         gui->pset(n,m,c);
      }
   }
}

void UG_FillRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c)
{
   UG_S16  x,y,xd;

   if (x2 < x1)
   {
      x = x2;
      x2 = x1;
      x1 = x;
   }
   if (y2 < y1)
   {
      y = y2;
      y2 = y1;
      y1 = y;
   }

   if (r <= 0) return;

   xd = 3 - (r << 1);
   x = 0;
   y = r;

   UG_FillFrame(x1 + r, y1, x2 - r, y2, c);

   while (x <= y)
   {
     if(y > 0)
     {
        UG_DrawLine(x2 + x - r, y1 - y + r, x2+ x - r, y + y2 - r, c);
        UG_DrawLine(x1 - x + r, y1 - y + r, x1- x + r, y + y2 - r, c);
     }
     if(x > 0)
     {
        UG_DrawLine(x1 - y + r, y1 - x + r, x1 - y + r, x + y2 - r, c);
        UG_DrawLine(x2 + y - r, y1 - x + r, x2 + y - r, x + y2 - r, c);
     }
     if (xd < 0)
     {
        xd += (x << 2) + 6;
     }
     else
     {
        xd += ((x - y) << 2) + 10;
        y--;
     }
     x++;
   }
}

void UG_DrawMesh(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
   UG_S16 n,m;

   if (x2 < x1)
   {
      n = x2;
      x2 = x1;
      x1 = n;
   }
   if (y2 < y1)
   {
      n = y2;
      y2 = y1;
      y1 = n;
   }

   for(m = y1; m <= y2; m+=2)
   {
      for(n=x1; n<=x2; n+=2)
      {
         gui->pset(n,m,c);
      }
   }
}

void UG_DrawFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
   UG_DrawLine(x1,y1,x2,y1,c);
   UG_DrawLine(x1,y2,x2,y2,c);
   UG_DrawLine(x1,y1,x1,y2,c);
   UG_DrawLine(x2,y1,x2,y2,c);
}

void UG_DrawRoundFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_S16 r, UG_COLOR c)
{
   UG_S16 n;
   if (x2 < x1)
   {
      n = x2;
      x2 = x1;
      x1 = n;
   }
   if (y2 < y1)
   {
      n = y2;
      y2 = y1;
      y1 = n;
   }

   if (r > x2) return;
   if (r > y2) return;

   UG_DrawLine(x1+r, y1, x2-r, y1, c);
   UG_DrawLine(x1+r, y2, x2-r, y2, c);
   UG_DrawLine(x1, y1+r, x1, y2-r, c);
   UG_DrawLine(x2, y1+r, x2, y2-r, c);
   UG_DrawArc(x1+r, y1+r, r, 0x0C, c);
   UG_DrawArc(x2-r, y1+r, r, 0x03, c);
   UG_DrawArc(x1+r, y2-r, r, 0x30, c);
   UG_DrawArc(x2-r, y2-r, r, 0xC0, c);
}

void UG_DrawPixel(UG_S16 x0, UG_S16 y0, UG_COLOR c)
{
   gui->pset(x0,y0,c);
}

void UG_DrawCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c)
{
   UG_S16 x,y,xd,yd,e;

   if (x0 < 0) return;
   if (y0 < 0) return;
   if (r <= 0) return;

   xd = 1 - (r << 1);
   yd = 0;
   e = 0;
   x = r;
   y = 0;

   while (x >= y)
   {
      gui->pset(x0 - x, y0 + y, c);
      gui->pset(x0 - x, y0 - y, c);
      gui->pset(x0 + x, y0 + y, c);
      gui->pset(x0 + x, y0 - y, c);
      gui->pset(x0 - y, y0 + x, c);
      gui->pset(x0 - y, y0 - x, c);
      gui->pset(x0 + y, y0 + x, c);
      gui->pset(x0 + y, y0 - x, c);

      y++;
      e += yd;
      yd += 2;
      if ( ((e << 1) + xd) > 0 )
      {
         x--;
         e += xd;
         xd += 2;
      }
   }
}

void UG_FillCircle(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_COLOR c)
{
   UG_S16  x, y, xd;

   if (x0 < 0) return;
   if (y0 < 0) return;
   if (r <= 0) return;

   xd = 3 - (r << 1);
   x = 0;
   y = r;

   while (x <= y)
   {
     if(y > 0)
     {
        UG_DrawLine(x0 - x, y0 - y,x0 - x, y0 + y, c);
        UG_DrawLine(x0 + x, y0 - y,x0 + x, y0 + y, c);
     }
     if(x > 0)
     {
        UG_DrawLine(x0 - y, y0 - x,x0 - y, y0 + x, c);
        UG_DrawLine(x0 + y, y0 - x,x0 + y, y0 + x, c);
     }
     if (xd < 0)
     {
        xd += (x << 2) + 6;
     }
     else
     {
        xd += ((x - y) << 2) + 10;
        y--;
     }
     x++;
   }
   UG_DrawCircle(x0, y0, r,c);
}

void UG_DrawArc(UG_S16 x0, UG_S16 y0, UG_S16 r, UG_U8 s, UG_COLOR c)
{
   UG_S16 x, y, xd, yd, e;

   if (x0 < 0) return;
   if (y0 < 0) return;
   if (r <= 0) return;

   xd = 1 - (r << 1);
   yd = 0;
   e = 0;
   x = r;
   y = 0;

   while (x >= y)
   {
      // Q1
      if (s & 0x01) gui->pset(x0 + x, y0 - y, c);
      if (s & 0x02) gui->pset(x0 + y, y0 - x, c);

      // Q2
      if (s & 0x04) gui->pset(x0 - y, y0 - x, c);
      if (s & 0x08) gui->pset(x0 - x, y0 - y, c);

      // Q3
      if (s & 0x10) gui->pset(x0 - x, y0 + y, c);
      if (s & 0x20) gui->pset(x0 - y, y0 + x, c);

      // Q4
      if (s & 0x40) gui->pset(x0 + y, y0 + x, c);
      if (s & 0x80) gui->pset(x0 + x, y0 + y, c);

      y++;
      e += yd;
      yd += 2;
      if (((e << 1) + xd) > 0)
      {
        x--;
        e += xd;
        xd += 2;
      }
   }
}

void UG_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
   UG_S16 n, dx, dy, sgndx, sgndy, dxabs, dyabs, x, y, drawx, drawy;

   /* Is hardware acceleration available? */
   if (gui->driver[DRIVER_DRAW_LINE].state & DRIVER_ENABLED)
   {
      if( ((UG_RESULT(*)(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c))gui->driver[DRIVER_DRAW_LINE].driver)(x1,y1,x2,y2,c) == UG_RESULT_OK ) return;
   }

   dx = x2 - x1;
   dy = y2 - y1;
   dxabs = (dx>0)?dx:-dx;
   dyabs = (dy>0)?dy:-dy;
   sgndx = (dx>0)?1:-1;
   sgndy = (dy>0)?1:-1;
   x = dyabs >> 1;
   y = dxabs >> 1;
   drawx = x1;
   drawy = y1;

   gui->pset(drawx, drawy, c);

   if(dxabs >= dyabs)
   {
      for(n=0; n<dxabs; n++)
      {
         y += dyabs;
         if(y >= dxabs)
         {
            y -= dxabs;
            drawy += sgndy;
         }
         drawx += sgndx;
         gui->pset(drawx, drawy, c);
      }
   }
   else
   {
      for(n=0; n<dyabs; n++)
      {
         x += dxabs;
         if(x >= dyabs)
         {
            x -= dyabs;
            drawx += sgndx;
         }
         drawy += sgndy;
         gui->pset(drawx, drawy, c);
      }
   }
}

void UG_FillPoly(UG_PointPtr p, UG_U8 n, UG_U8 filled, UG_COLOR color)
{
	UG_S16 i;
	UG_S16 j;
	UG_S16 index;
	UG_S16 y;
	UG_S16 miny, maxy, pmaxy;
	UG_S16 x1, y1;
	UG_S16 x2, y2;
	UG_S16 ind1, ind2;
	UG_S16 ints;
  	UG_S16 polyInts[UG_MaxPolyPointNum];

	if ((n <= 0)||(n>UG_MaxPolyPointNum))
		return;

    if (filled)
    {
        miny = p[0].y;
        maxy = p[0].y;
        for (i = 1; (i < n); i++)
        {
            if (p[i].y < miny)
                miny = p[i].y;

            if (p[i].y > maxy)
                maxy = p[i].y;
        }
        pmaxy = maxy;

        for (y = miny; (y <= maxy); y++)
        {
            ints = 0;
            for (i = 0; (i < n); i++)
            {
                if (i)
                {
                    ind1 = i - 1;
                    ind2 = i;
                }
                else
                {
                    ind1 = n - 1;
                    ind2 = 0;
                }

                y1 = p[ind1].y;
                y2 = p[ind2].y;
                if (y1 < y2)
                {
                    x1 = p[ind1].x;
                    x2 = p[ind2].x;
                }
                else if (y1 > y2)
                     {
                         y2 = p[ind1].y;
                         y1 = p[ind2].y;
                         x2 = p[ind1].x;
                         x1 = p[ind2].x;
                     }
                     else
                     {
                         continue;
                     }

                if ((y >= y1) && (y < y2))
                {
                  polyInts[ints++] = (UG_S16) ((float)((y - y1) * (x2 - x1))/(float)(y2 - y1) + (float)0.5 + x1);
                }
                else
                    if ((y == pmaxy) && (y == y2))
                    {
                        polyInts[ints++] = x2;
                    }
            }

            for (i=1; (i<ints); i++)
            {
                index = polyInts[i];
                j = i;
                while ((j > 0) && (polyInts[j-1] > index))
                {
                    polyInts[j] = polyInts[j-1];
                    j--;
                }
                polyInts[j] = index;
            }

            for (i=0; (i < (ints-1)); i += 2)
            {
                UG_DrawLine(polyInts[i], y, polyInts[i+1], y, color);
            }
        }
    }
	else
    {
        for (i = 0; (i < n); i++)
        {
            if (i)
            {
                ind1 = i - 1;
                ind2 = i;
            }
            else
            {
                ind1 = n - 1;
                ind2 = 0;
            }

            UG_DrawLine(p[ind1].x, p[ind1].y, p[ind2].x, p[ind2].y, color);
        }
	}
}

void UG_PutString(UG_S16 x, UG_S16 y, const char* str)
{
   UG_S16 xp,yp;
   UG_U8 cw;
   char chr;

   xp=x;
   yp=y;

   while (*str != 0)
   {
      	chr = *str++;
    	if (chr < gui->font.start_char || chr > gui->font.end_char) continue;
      	if ( chr == '\n' )
      	{
        	xp = gui->x_dim;
         	continue;
      	}
    	cw = gui->font.widths ? gui->font.widths[chr - gui->font.start_char] : gui->font.char_width;

      	if ( xp + cw > gui->x_dim - 1 )
      	{
        	xp = x;
         	yp += gui->font.char_height+gui->char_v_space;
      	}

      	UG_PutChar(chr, xp, yp, gui->fore_color, gui->back_color);

     	xp += cw + gui->char_h_space;
   }
}

void UG_PutChar(char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc)
{
	_UG_PutChar(chr,x,y,fc,bc,&gui->font);
}

void UG_ConsolePutString(const char* str)
{
   char chr;
   UG_U8 cw;

   while (*str != 0)
   {
      	chr = *str;
      	if(chr == '\n')
      	{
        	gui->console.x_pos = gui->x_dim;
         	str++;
         	continue;
      	}

      	cw = gui->font.widths ? gui->font.widths[chr - gui->font.start_char] : gui->font.char_width;
      	gui->console.x_pos += cw+gui->char_h_space;

      	if(gui->console.x_pos+cw > gui->console.x_end)
      	{
        	gui->console.x_pos = gui->console.x_start;
        	gui->console.y_pos += gui->font.char_height+gui->char_v_space;
      	}
      	if(gui->console.y_pos+gui->font.char_height > gui->console.y_end)
      	{
         	gui->console.x_pos = gui->console.x_start;
         	gui->console.y_pos = gui->console.y_start;
         	UG_FillFrame(gui->console.x_start,gui->console.y_start,gui->console.x_end,gui->console.y_end,gui->console.back_color);
    	}

      	UG_PutChar(chr, gui->console.x_pos, gui->console.y_pos, gui->console.fore_color, gui->console.back_color);
      	str++;
   	}
}

void UG_ConsoleSetArea(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
   gui->console.x_start = xs;
   gui->console.y_start = ys;
   gui->console.x_end = xe;
   gui->console.y_end = ye;
}

void UG_ConsoleSetForecolor(UG_COLOR c)
{
   gui->console.fore_color = c;
}

void UG_ConsoleSetBackcolor(UG_COLOR c)
{
   gui->console.back_color = c;
}

void UG_SetForecolor(UG_COLOR c)
{
   gui->fore_color = c;
}

void UG_SetBackcolor(UG_COLOR c)
{
   gui->back_color = c;
}

UG_S16 UG_GetXDim(void)
{
   return gui->x_dim;
}

UG_S16 UG_GetYDim(void)
{
   return gui->y_dim;
}

void UG_FontSetHSpace(UG_U16 s)
{
   gui->char_h_space = s;
}

void UG_FontSetVSpace(UG_U16 s)
{
   gui->char_v_space = s;
}

#ifdef USE_FONT_VECTOR_CYRILLIC
void UG_GetVectFont(UG_FONT *font, UG_U16 w, UG_U16 h)
{
    font->char_width = w;
    font->char_height = h;
    font->start_char = 0;
    font->end_char = 255;
    font->font_type = FONT_TYPE_VECTOR_1BPP;
    font->widths = NULL;
    font->p = (unsigned char*)&VECT_FONT_CYRILLIC;
}
#endif

/* -------------------------------------------------------------------------------- */
/* -- INTERNAL FUNCTIONS                                                         -- */
/* -------------------------------------------------------------------------------- */
static void _UG_PutChar(char chr, UG_S16 x, UG_S16 y, UG_COLOR fc, UG_COLOR bc, const UG_FONT* font)
{
   UG_U16 i, j, k, xo, yo, c, bn, actual_char_width;
   UG_U8 b, bt;
   UG_U32 index;
   UG_COLOR color;
   void(*push_pixel)(UG_COLOR);

   bt = (UG_U8)chr;

   #ifndef USE_FONT_8X12_CYRILLIC 
     #ifndef USE_FONT_VECTOR_CYRILLIC
     switch (bt)
     {
        case 0xF6: bt = 0x94; break; // �
        case 0xD6: bt = 0x99; break; // �
        case 0xFC: bt = 0x81; break; // �
        case 0xDC: bt = 0x9A; break; // �
        case 0xE4: bt = 0x84; break; // �
        case 0xC4: bt = 0x8E; break; // �
        case 0xB5: bt = 0xE6; break; // �
        case 0xB0: bt = 0xF8; break; // �
     }
     #endif
   #endif

   if (bt < font->start_char || bt > font->end_char) return;

   yo = y;
   bn = font->char_width;
   if (!bn) return;
   bn >>= 3;
   if (font->char_width % 8) bn++;
   actual_char_width = (font->widths ? font->widths[bt - font->start_char] : font->char_width);

   #ifdef USE_FONT_VECTOR_CYRILLIC
   if (font->font_type == FONT_TYPE_VECTOR_1BPP) // vector font could not use acceleration,
   {
      UG_VECT_FONT *fnt;
      fnt = (UG_VECT_FONT*)font->p;
      if ( gui->driver[DRIVER_DRAW_LINE].state & DRIVER_ENABLED )
      i = 0;
      while(fnt->p[bt][i]!=0)
      {
         UG_DrawLine(x+((((UG_S16)fnt->p[bt][i])-48)*actual_char_width/fnt->char_code_width),
                     y+((((UG_S16)fnt->p[bt][i+1])-48)*font->char_height/fnt->char_code_height),
                     x+((((UG_S16)fnt->p[bt][i+2])-48)*actual_char_width/fnt->char_code_width),
                     y+((((UG_S16)fnt->p[bt][i+3])-48)*font->char_height/fnt->char_code_height), fc);
         i += 4;
      }
   }
   else
   #endif
   /* Is hardware acceleration available? */
   if ( gui->driver[DRIVER_FILL_AREA].state & DRIVER_ENABLED )
   {
     //(void(*)(UG_COLOR))
     push_pixel = (void(*)(UG_COLOR)) ((void*(*)(UG_S16, UG_S16, UG_S16, UG_S16))gui->driver[DRIVER_FILL_AREA].driver)(x,y,x+actual_char_width-1,y+font->char_height-1);

     if (font->font_type == FONT_TYPE_1BPP)
		 {
        index = (bt - font->start_char)* font->char_height * bn;
      for( j=0;j<font->char_height;j++ )
      {
       c=actual_char_width;
       for( i=0;i<bn;i++ )
       {
        b = font->p[index++];
        for( k=0;(k<8) && c;k++ )
        {
           if( b & 0x01 )
           {
            push_pixel(fc);
           }
           else
           {
            push_pixel(bc);
           }
           b >>= 1;
           c--;
        }
       }
     }
    }
    else if (font->font_type == FONT_TYPE_8BPP)
    {
       index = (bt - font->start_char)* font->char_height * font->char_width;
       for( j=0;j<font->char_height;j++ )
       {
        for( i=0;i<actual_char_width;i++ )
        {
         b = font->p[index++];
				 color = ((((fc & 0xFF) * b + (bc & 0xFF) * (256 - b)) >> 8) & 0xFF) | //Blue component
				         ((((fc & 0xFF00) * b + (bc & 0xFF00) * (256 - b)) >> 8)  & 0xFF00) | //Green component
				         ((((fc & 0xFF0000) * b + (bc & 0xFF0000) * (256 - b)) >> 8) & 0xFF0000); //Red component
         push_pixel(color);
        }
        index += font->char_width - actual_char_width;
      }
    }
   }
   else
   {
     /*Not accelerated output*/
     if (font->font_type == FONT_TYPE_1BPP)
     {
         index = (bt - font->start_char)* font->char_height * bn;
         for( j=0;j<font->char_height;j++ )
         {
           xo = x;
           c=actual_char_width;
           for( i=0;i<bn;i++ )
           {
             b = font->p[index++];
             for( k=0;(k<8) && c;k++ )
             {
               if( b & 0x01 )
               {
                  gui->pset(xo,yo,fc);
               }
               else
               {
                  gui->pset(xo,yo,bc);
               }
               b >>= 1;
               xo++;
               c--;
             }
           }
           yo++;
         }
      }
      else if (font->font_type == FONT_TYPE_8BPP)
      {
         index = (bt - font->start_char)* font->char_height * font->char_width;
         for( j=0;j<font->char_height;j++ )
         {
            xo = x;
            for( i=0;i<actual_char_width;i++ )
            {
               b = font->p[index++];
               color = ((((fc & 0xFF) * b + (bc & 0xFF) * (256 - b)) >> 8) & 0xFF) |//Blue component
                       ((((fc & 0xFF00) * b + (bc & 0xFF00) * (256 - b)) >> 8)  & 0xFF00)|//Green component
                       ((((fc & 0xFF0000) * b + (bc & 0xFF0000) * (256 - b)) >> 8) & 0xFF0000); //Red component
               gui->pset(xo,yo,color);
               xo++;
            }
            index += font->char_width - actual_char_width;
            yo++;
         }
      }
   }
}

static void _UG_PutText(UG_TEXT* txt)
{
   UG_U16 sl,rc,wl;
   UG_S16 xp,yp;
   UG_S16 xs=txt->a.xs;
   UG_S16 ys=txt->a.ys;
   UG_S16 xe=txt->a.xe;
   UG_S16 ye=txt->a.ye;
   UG_U8  align=txt->align;
   UG_S16 char_width=txt->font->char_width;
   UG_S16 char_height=txt->font->char_height;
   UG_S16 char_h_space=txt->h_space;
   UG_S16 char_v_space=txt->v_space;

   char chr;

   char* str = txt->str;
   char* c = str;

   if ( txt->font->p == NULL ) return;
   if ( str == NULL ) return;
// if ( (ye - ys) < txt->font->char_height ) return;
   if ( (ye - ys + 1) < txt->font->char_height ) return;

   rc=1;
   c=str;
   while ( *c != 0 )
   {
      if ( *c == '\n' ) rc++;
      c++;
   }

   yp = 0;
   if ( align & (ALIGN_V_CENTER | ALIGN_V_BOTTOM) )
   {
      yp = ye - ys + 1;
      yp -= char_height*rc;
      yp -= char_v_space*(rc-1);
      if ( yp < 0 ) return;
   }
   if ( align & ALIGN_V_CENTER ) yp >>= 1;
   yp += ys;

   while( 1 )
   {
      sl=0;
      c=str;
      wl = 0;
      while( (*c != 0) && (*c != '\n') )
      {
         if (*c < txt->font->start_char || *c > txt->font->end_char) {c++; continue;}
         sl++;
         wl += (txt->font->widths ? txt->font->widths[*c - txt->font->start_char] : char_width) + char_h_space;
         c++;
      }
      wl -= char_h_space;

      xp = xe - xs + 1;
      xp -= wl;
      if ( xp < 0 ) return;

      if ( align & ALIGN_H_LEFT ) xp = 0;
      else if ( align & ALIGN_H_CENTER ) xp >>= 1;
      xp += xs;

      while( (*str != '\n') )
      {
         chr = *str++;
         if ( chr == 0 ) return;
         _UG_PutChar(chr,xp,yp,txt->fc,txt->bc,txt->font);
         xp += (txt->font->widths ? txt->font->widths[chr - txt->font->start_char] : char_width) + char_h_space;
      }
      str++;
      yp += char_height + char_v_space;
   }
}

static UG_OBJECT* _UG_GetFreeObject( UG_WINDOW* wnd )
{
   UG_U8 i;
   UG_OBJECT* obj=(UG_OBJECT*)wnd->objlst;

   for(i=0;i<wnd->objcnt;i++)
   {
      obj = (UG_OBJECT*)(&wnd->objlst[i]);
      if ( (obj->state & OBJ_STATE_FREE) && (obj->state & OBJ_STATE_VALID) )
      {
         /* Free object found! */
         return obj;
      }
   }
   return NULL;
}

/* OBJECT WINDOWS*/

static UG_OBJECT* _UG_SearchObject( UG_WINDOW* wnd, UG_U8 type, UG_U8 id )
{
   UG_U8 i;
   UG_OBJECT* obj=(UG_OBJECT*)wnd->objlst;

   for(i=0;i<wnd->objcnt;i++)
   {
      obj = (UG_OBJECT*)(&wnd->objlst[i]);
      if ( !(obj->state & OBJ_STATE_FREE) && (obj->state & OBJ_STATE_VALID) )
      {
         if ( (obj->type == type) && (obj->id == id) )
         {
            /* Requested object found! */
            return obj;
         }
      }
   }
   return NULL;
}

static UG_RESULT _UG_DeleteObject( UG_WINDOW* wnd, UG_U8 type, UG_U8 id )
{
   UG_OBJECT* obj=NULL;

   obj = _UG_SearchObject( wnd, type, id );

   /* Object found? */
   if ( obj != NULL )
   {
      /* We dont't want to delete a visible or busy object! */
      if ( (obj->state & OBJ_STATE_VISIBLE) || (obj->state & OBJ_STATE_UPDATE) ) return UG_RESULT_FAIL;
      obj->state = OBJ_STATE_INIT;
      obj->data = NULL;
      obj->event = 0;
      obj->id = 0;
      obj->touch_state = 0;
      obj->type = 0;
      obj->update = NULL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

static void _UG_UpdateObjects( UG_WINDOW* wnd )
{
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_U8 objstate;
   UG_U8 objtouch;

   /* Check each object, if it needs to be updated? */
   objcnt = wnd->objcnt;
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&wnd->objlst[i];
      objstate = obj->state;
      objtouch = obj->touch_state;
      if ( !(objstate & OBJ_STATE_FREE) && (objstate & OBJ_STATE_VALID) )
      {
         if ( objstate & OBJ_STATE_UPDATE )
         {
            obj->update(wnd,obj);
         }
         if ( (objstate & OBJ_STATE_VISIBLE) && (objstate & OBJ_STATE_TOUCH_ENABLE) )
         {
            if ( (objtouch & (OBJ_TOUCH_STATE_CHANGED | OBJ_TOUCH_STATE_IS_PRESSED)) )
            {
               obj->update(wnd,obj);
            }
         }
      }
   }
}

static void _UG_HandleEvents( UG_WINDOW* wnd )
{
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_U8 objstate;
   static UG_MESSAGE msg;
   msg.src = NULL;

   /* Handle window-related events */
   //ToDo


   /* Handle object-related events */
   msg.type = MSG_TYPE_OBJECT;
   objcnt = wnd->objcnt;
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&wnd->objlst[i];
      objstate = obj->state;
      if ( !(objstate & OBJ_STATE_FREE) && (objstate & OBJ_STATE_VALID) )
      {
         if ( obj->event != OBJ_EVENT_NONE )
         {
            msg.src = &obj;
            msg.id = obj->type;
            msg.sub_id = obj->id;
            msg.event = obj->event;

            wnd->cb( &msg );

            obj->event = OBJ_EVENT_NONE;
         }
      }
   }
}

static void _UG_DrawObjectFrame(UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye, UG_COLOR p)
{
   // Frame 0
   UG_DrawLine(xs, ys  , xe-1, ys  , p);
   UG_DrawLine(xs, ys+1, xs  , ye-1, p);
   UG_DrawLine(xs, ye  , xe  , ye  , p);
   UG_DrawLine(xe, ys  , xe  , ye-1, p);
   // Frame 1
   UG_DrawLine(xs+1, ys+1, xe-2, ys+1, p);
   UG_DrawLine(xs+1, ys+2, xs+1, ye-2, p);
   UG_DrawLine(xs+1, ye-1, xe-1, ye-1, p);
   UG_DrawLine(xe-1, ys+1, xe-1, ye-2, p);
   // Frame 2
   UG_DrawLine(xs+2, ys+2, xe-3, ys+2, p);
   UG_DrawLine(xs+2, ys+3, xs+2, ye-3, p);
   UG_DrawLine(xs+2, ye-2, xe-2, ye-2, p);
   UG_DrawLine(xe-2, ys+2, xe-2, ye-3, p);
}

#ifdef USE_PRERENDER_EVENT
static void _UG_SendObjectPrerenderEvent(UG_WINDOW *wnd,UG_OBJECT *obj)
{
  UG_MESSAGE msg;
  msg.event = OBJ_EVENT_PRERENDER;
  msg.type = MSG_TYPE_OBJECT;
  msg.id = obj->type;
  msg.sub_id = obj->id;
  msg.src = obj;

  wnd->cb(&msg);
}
#endif

#ifdef USE_POSTRENDER_EVENT
static void _UG_SendObjectPostrenderEvent(UG_WINDOW *wnd,UG_OBJECT *obj)
{
  UG_MESSAGE msg;
  msg.event = OBJ_EVENT_POSTRENDER;
  msg.type = MSG_TYPE_OBJECT;
  msg.id = obj->type;
  msg.sub_id = obj->id;
  msg.src = obj;

  wnd->cb(&msg);
}
#endif

/* -------------------------------------------------------------------------------- */
/* -- DRIVER FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */
void UG_DriverRegister( UG_U8 type, void* driver )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;

   gui->driver[type].driver = driver;
   gui->driver[type].state = DRIVER_REGISTERED | DRIVER_ENABLED;
}

void UG_DriverEnable( UG_U8 type )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;
   if ( gui->driver[type].state & DRIVER_REGISTERED )
   {
      gui->driver[type].state |= DRIVER_ENABLED;
   }
}

void UG_DriverDisable( UG_U8 type )
{
   if ( type >= NUMBER_OF_DRIVERS ) return;
   if ( gui->driver[type].state & DRIVER_REGISTERED )
   {
      gui->driver[type].state &= ~DRIVER_ENABLED;
   }
}

/* -------------------------------------------------------------------------------- */
/* -- MISCELLANEOUS FUNCTIONS                                                    -- */
/* -------------------------------------------------------------------------------- */
void UG_Update( void )
{
   UG_WINDOW* wnd;

   /*  prevent noinit access, hardware exception */
   if (gui == NULL) return;

   /* Is somebody waiting for this update? */
   if ( gui->state & UG_SATUS_WAIT_FOR_UPDATE ) gui->state &= ~UG_SATUS_WAIT_FOR_UPDATE;

   /* Keep track of the windows */
   if ( gui->next_window != gui->active_window )
   {
      if ( gui->next_window != NULL )
      {
         gui->last_window = gui->active_window;
         gui->active_window = gui->next_window;

         /* Do we need to draw an inactive title? */
         if ((gui->last_window != NULL) && (gui->last_window->style & WND_STYLE_SHOW_TITLE) && (gui->last_window->state & WND_STATE_VISIBLE) )
         {
            /* Do both windows differ in size */
            if ( (gui->last_window->xs != gui->active_window->xs) || (gui->last_window->xe != gui->active_window->xe) || (gui->last_window->ys != gui->active_window->ys) || (gui->last_window->ye != gui->active_window->ye) )
            {
               /* Redraw title of the last window */
               _UG_WindowDrawTitle( gui->last_window );
            }
         }
         gui->active_window->state &= ~WND_STATE_REDRAW_TITLE;
         gui->active_window->state |= WND_STATE_UPDATE | WND_STATE_VISIBLE;
      }
   }

   /* Is there an active window */
   if ( gui->active_window != NULL )
   {
      wnd = gui->active_window;

      /* Does the window need to be updated? */
      if ( wnd->state & WND_STATE_UPDATE )
      {
         /* Do it! */
         _UG_WindowUpdate( wnd );
      }

      /* Is the window visible? */
      if ( wnd->state & WND_STATE_VISIBLE )
      {
         //_UG_ProcessTouchData( wnd );
         _UG_UpdateObjects( wnd );
         _UG_HandleEvents( wnd );
      }
   }
}

void UG_WaitForUpdate( void )
{
   gui->state |= UG_SATUS_WAIT_FOR_UPDATE;
   #ifdef USE_MULTITASKING
   while ( (volatile UG_U8)gui->state & UG_SATUS_WAIT_FOR_UPDATE ){};
   #endif
   #ifndef USE_MULTITASKING
   while ( (UG_U8)gui->state & UG_SATUS_WAIT_FOR_UPDATE ){};
   #endif
}

void UG_DrawBMP( UG_S16 xp, UG_S16 yp, UG_BMP* bmp )
{
   UG_S16 x,y,xs;
   #ifndef USE_COLOR_RGB565
   UG_U8 r,g,b;
   UG_COLOR c;
   #endif
   UG_U16* p;
   UG_U16 tmp;

   if ( bmp->p == NULL ) return;

   /* Only support 16 BPP so far */
   if ( bmp->bpp == BMP_BPP_16 )
   {
      p = (UG_U16*)bmp->p;
   }
   else
   {
      return;
   }

   xs = xp;
   for(y=0;y<bmp->height;y++)
   {
      xp = xs;
      for(x=0;x<bmp->width;x++)
      {
         tmp = *p++;
         #ifdef USE_COLOR_RGB565
            UG_DrawPixel( xp++ , yp , tmp );
         #else
            /* Convert RGB565 to RGB888 */
            r = (tmp>>11)&0x1F;
            r<<=3;
            g = (tmp>>5)&0x3F;
            g<<=2;
            b = (tmp)&0x1F;
            b<<=3;
            c = ((UG_COLOR)r<<16) | ((UG_COLOR)g<<8) | (UG_COLOR)b;
            UG_DrawPixel( xp++ , yp , c );
         #endif
      }
      yp++;
   }
}

const UG_COLOR pal_window = 0;

/* -------------------------------------------------------------------------------- */
/* -- WINDOW FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */
UG_RESULT UG_WindowCreate( UG_WINDOW* wnd, UG_OBJECT* objlst, UG_U8 objcnt, void (*cb)( UG_MESSAGE* ) )
{
   UG_U8 i;
   UG_OBJECT* obj=NULL;

   if ( (wnd == NULL) || (objlst == NULL) || (objcnt == 0) ) return UG_RESULT_FAIL;

   /* Initialize all objects of the window */
   for(i=0; i<objcnt; i++)
   {
      obj = (UG_OBJECT*)&objlst[i];
      obj->state = OBJ_STATE_INIT;
      obj->data = NULL;
   }

   /* Initialize window */
   wnd->objcnt = objcnt;
   wnd->objlst = objlst;
   wnd->state = WND_STATE_VALID;

   wnd->fc = 0;
   wnd->bc = 1;

   wnd->xs = 0;
   wnd->ys = 0;
   wnd->xe = UG_GetXDim()-1;
   wnd->ye = UG_GetYDim()-1;
   wnd->cb = cb;
   wnd->style = WND_STYLE_3D | WND_STYLE_SHOW_TITLE;

   /* Initialize window title-bar */
   wnd->title.str = NULL;
   if (gui != NULL) wnd->title.font = &gui->font;
   else wnd->title.font = NULL;
   wnd->title.h_space = 2;
   wnd->title.v_space = 2;
   wnd->title.align = ALIGN_CENTER_LEFT;
   wnd->title.fc = 0;
   wnd->title.bc = 1;
   wnd->title.ifc = 0;
   wnd->title.ibc = 1;
   wnd->title.height = 15;

   return UG_RESULT_OK;
}

UG_RESULT UG_WindowDelete( UG_WINDOW* wnd )
{
   if ( wnd == gui->active_window ) return UG_RESULT_FAIL;

   /* Only delete valid windows */
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->state = 0;
      wnd->cb = NULL;
      wnd->objcnt = 0;
      wnd->objlst = NULL;
      wnd->xs = 0;
      wnd->ys = 0;
      wnd->xe = 0;
      wnd->ye = 0;
      wnd->style = 0;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowShow( UG_WINDOW* wnd )
{
   if ( wnd != NULL )
   {
      /* Force an update, even if this is the active window! */
      wnd->state |= WND_STATE_VISIBLE | WND_STATE_UPDATE;
      wnd->state &= ~WND_STATE_REDRAW_TITLE;
      gui->next_window = wnd;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowHide( UG_WINDOW* wnd )
{
   if ( wnd != NULL )
   {
      if ( wnd == gui->active_window )
      {
         /* Is there an old window which just lost the focus? */
         if ( (gui->last_window != NULL) && (gui->last_window->state & WND_STATE_VISIBLE) )
         {
            if ( (gui->last_window->xs > wnd->xs) || (gui->last_window->ys > wnd->ys) || (gui->last_window->xe < wnd->xe) || (gui->last_window->ye < wnd->ye) )
            {
               _UG_WindowClear( wnd );
            }
            gui->next_window = gui->last_window;
         }
         else
         {
            gui->active_window->state &= ~WND_STATE_VISIBLE;
            gui->active_window->state |= WND_STATE_UPDATE;
         }
      }
      else
      {
         /* If the old window is visible, clear it! */
         _UG_WindowClear( wnd );
      }
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowResize( UG_WINDOW* wnd, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye )
{
   UG_S16 pos;
   UG_S16 xmax,ymax;

   xmax = UG_GetXDim()-1;
   ymax = UG_GetYDim()-1;

   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      /* Do some checks... */
      if ( (xs < 0) || (ys < 0) ) return UG_RESULT_FAIL;
      if ( (xe > xmax) || (ye > ymax) ) return UG_RESULT_FAIL;
      pos = xe-xs;
      if ( pos < 10 ) return UG_RESULT_FAIL;
      pos = ye-ys;
      if ( pos < 10 ) return UG_RESULT_FAIL;

      /* ... and if everything is OK move the window! */
      wnd->xs = xs;
      wnd->ys = ys;
      wnd->xe = xe;
      wnd->ye = ye;

      if ( (wnd->state & WND_STATE_VISIBLE) && (gui->active_window == wnd) )
      {
         if ( wnd->ys ) UG_FillFrame(0, 0, xmax,wnd->ys-1,gui->desktop_color);
         pos = wnd->ye+1;
         if ( !(pos > ymax) ) UG_FillFrame(0, pos, xmax,ymax,gui->desktop_color);
         if ( wnd->xs ) UG_FillFrame(0, wnd->ys, wnd->xs-1,wnd->ye,gui->desktop_color);
         pos = wnd->xe+1;
         if ( !(pos > xmax) ) UG_FillFrame(pos, wnd->ys,xmax,wnd->ye,gui->desktop_color);

         wnd->state &= ~WND_STATE_REDRAW_TITLE;
         wnd->state |= WND_STATE_UPDATE;
      }
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowAlert( UG_WINDOW* wnd )
{
   UG_COLOR c;
   c = UG_WindowGetTitleTextColor( wnd );
   if ( UG_WindowSetTitleTextColor( wnd, UG_WindowGetTitleColor( wnd ) ) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
   if ( UG_WindowSetTitleColor( wnd, c ) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
   return UG_RESULT_OK;
}

UG_RESULT UG_WindowSetForeColor( UG_WINDOW* wnd, UG_COLOR fc )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->fc = fc;
      wnd->state |= WND_STATE_UPDATE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetBackColor( UG_WINDOW* wnd, UG_COLOR bc )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->bc = bc;
      wnd->state |= WND_STATE_UPDATE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleTextColor(UG_WINDOW* wnd, UG_COLOR c)
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.fc = c;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleColor( UG_WINDOW* wnd, UG_COLOR c)
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.bc = c;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleInactiveTextColor( UG_WINDOW* wnd, UG_COLOR c)
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.ifc = c;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleInactiveColor( UG_WINDOW* wnd, UG_COLOR c )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.ibc = c;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleText( UG_WINDOW* wnd, char* str )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.str = str;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleTextFont( UG_WINDOW* wnd, const UG_FONT* font )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      wnd->title.font = font;
      if ( wnd->title.height <= (font->char_height + 1) )
      {
         wnd->title.height = font->char_height + 2;
         wnd->state &= ~WND_STATE_REDRAW_TITLE;
      }
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleTextHSpace( UG_WINDOW* wnd, UG_S8 hs )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.h_space = hs;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleTextVSpace( UG_WINDOW* wnd, UG_S8 vs )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.v_space = vs;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleTextAlignment( UG_WINDOW* wnd, UG_U8 align )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.align = align;
      wnd->state |= WND_STATE_UPDATE | WND_STATE_REDRAW_TITLE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetTitleHeight( UG_WINDOW* wnd, UG_U8 height )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->title.height = height;
      wnd->state &= ~WND_STATE_REDRAW_TITLE;
      wnd->state |= WND_STATE_UPDATE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetXStart( UG_WINDOW* wnd, UG_S16 xs )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->xs = xs;
      if ( UG_WindowResize( wnd, wnd->xs, wnd->ys, wnd->xe, wnd->ye) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetYStart( UG_WINDOW* wnd, UG_S16 ys )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->ys = ys;
      if ( UG_WindowResize( wnd, wnd->xs, wnd->ys, wnd->xe, wnd->ye) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetXEnd( UG_WINDOW* wnd, UG_S16 xe )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->xe = xe;
      if ( UG_WindowResize( wnd, wnd->xs, wnd->ys, wnd->xe, wnd->ye) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetYEnd( UG_WINDOW* wnd, UG_S16 ye )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      wnd->ye = ye;
      if ( UG_WindowResize( wnd, wnd->xs, wnd->ys, wnd->xe, wnd->ye) == UG_RESULT_FAIL ) return UG_RESULT_FAIL;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_RESULT UG_WindowSetStyle( UG_WINDOW* wnd, UG_U8 style )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      /* 3D or 2D? */
      if ( style & WND_STYLE_3D )
      {
         wnd->style |= WND_STYLE_3D;
      }
      else
      {
         wnd->style &= ~WND_STYLE_3D;
      }
      /* Show title-bar? */
      if ( style & WND_STYLE_SHOW_TITLE )
      {
         wnd->style |= WND_STYLE_SHOW_TITLE;
      }
      else
      {
         wnd->style &= ~WND_STYLE_SHOW_TITLE;
      }
      wnd->state |= WND_STATE_UPDATE;
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_COLOR UG_WindowGetForeColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->fc;
   }
   return c;
}

UG_COLOR UG_WindowGetBackColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->bc;
   }
   return c;
}

UG_COLOR UG_WindowGetTitleTextColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->title.fc;
   }
   return c;
}

UG_COLOR UG_WindowGetTitleColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->title.bc;
   }
   return c;
}

UG_COLOR UG_WindowGetTitleInactiveTextColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->title.ifc;
   }
   return c;
}

UG_COLOR UG_WindowGetTitleInactiveColor( UG_WINDOW* wnd )
{
   UG_COLOR c = 1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      c = wnd->title.ibc;
   }
   return c;
}

char* UG_WindowGetTitleText( UG_WINDOW* wnd )
{
   char* str = NULL;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      str = wnd->title.str;
   }
   return str;
}

UG_FONT* UG_WindowGetTitleTextFont( UG_WINDOW* wnd )
{
   UG_FONT* f = NULL;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      f = (UG_FONT*)wnd->title.font;
   }
   return f;
}

UG_S8 UG_WindowGetTitleTextHSpace( UG_WINDOW* wnd )
{
   UG_S8 hs = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      hs = wnd->title.h_space;
   }
   return hs;
}

UG_S8 UG_WindowGetTitleTextVSpace( UG_WINDOW* wnd )
{
   UG_S8 vs = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      vs = wnd->title.v_space;
   }
   return vs;
}

UG_U8 UG_WindowGetTitleTextAlignment( UG_WINDOW* wnd )
{
   UG_U8 align = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      align = wnd->title.align;
   }
   return align;
}

UG_U8 UG_WindowGetTitleHeight( UG_WINDOW* wnd )
{
   UG_U8 h = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      h = wnd->title.height;
   }
   return h;
}

UG_S16 UG_WindowGetXStart( UG_WINDOW* wnd )
{
   UG_S16 xs = -1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      xs = wnd->xs;
   }
   return xs;
}

UG_S16 UG_WindowGetYStart( UG_WINDOW* wnd )
{
   UG_S16 ys = -1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      ys = wnd->ys;
   }
   return ys;
}

UG_S16 UG_WindowGetXEnd( UG_WINDOW* wnd )
{
   UG_S16 xe = -1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      xe = wnd->xe;
   }
   return xe;
}

UG_S16 UG_WindowGetYEnd( UG_WINDOW* wnd )
{
   UG_S16 ye = -1;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      ye = wnd->ye;
   }
   return ye;
}

UG_U8 UG_WindowGetStyle( UG_WINDOW* wnd )
{
   UG_U8 style = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      style = wnd->style;
   }
   return style;
}

UG_RESULT UG_WindowGetArea( UG_WINDOW* wnd, UG_AREA* a )
{
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      a->xs = wnd->xs;
      a->ys = wnd->ys;
      a->xe = wnd->xe;
      a->ye = wnd->ye;
      if ( wnd->style & WND_STYLE_3D )
      {
         a->xs+=3;
         a->ys+=3;
         a->xe-=3;
         a->ye-=3;
      }
      if ( wnd->style & WND_STYLE_SHOW_TITLE )
      {
         a->ys+= wnd->title.height+1;
      }
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

UG_S16 UG_WindowGetInnerWidth( UG_WINDOW* wnd )
{
   UG_S16 w = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      w = wnd->xe-wnd->xs;

      /* 3D style? */
      if ( wnd->style & WND_STYLE_3D ) w-=6;

      if ( w < 0 ) w = 0;
   }
   return w;
}

UG_S16 UG_WindowGetOuterWidth( UG_WINDOW* wnd )
{
   UG_S16 w = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      w = wnd->xe-wnd->xs;

      if ( w < 0 ) w = 0;
   }
   return w;
}

UG_S16 UG_WindowGetInnerHeight( UG_WINDOW* wnd )
{
   UG_S16 h = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      h = wnd->ye-wnd->ys;

      /* 3D style? */
      if ( wnd->style & WND_STYLE_3D ) h-=6;

      /* Is the title active */
      if ( wnd->style & WND_STYLE_SHOW_TITLE ) h-=wnd->title.height;

      if ( h < 0 ) h = 0;
   }
   return h;
}

UG_S16 UG_WindowGetOuterHeight( UG_WINDOW* wnd )
{
   UG_S16 h = 0;
   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      h = wnd->ye-wnd->ys;

      if ( h < 0 ) h = 0;
   }
   return h;
}

static UG_RESULT _UG_WindowDrawTitle( UG_WINDOW* wnd )
{
   UG_TEXT txt;
   UG_S16 xs,ys,xe,ye;

   if ( (wnd != NULL) && (wnd->state & WND_STATE_VALID) )
   {
      xs = wnd->xs;
      ys = wnd->ys;
      xe = wnd->xe;
      ye = wnd->ye;

      /* 3D style? */
      if ( wnd->style & WND_STYLE_3D )
      {
         xs+=3;
         ys+=3;
         xe-=3;
         ye-=3;
      }

      /* Is the window active or inactive? */
      if ( wnd == gui->active_window )
      {
         txt.bc = wnd->title.bc;
         txt.fc = wnd->title.fc;
      }
      else
      {
         txt.bc = wnd->title.ibc;
         txt.fc = wnd->title.ifc;
      }

      /* Draw title */
      UG_FillFrame(xs,ys,xe,ys+wnd->title.height-1,txt.bc);

      /* Draw title text */
      txt.str = wnd->title.str;
      txt.font = wnd->title.font;
      txt.a.xs = xs+3;
      txt.a.ys = ys;
      txt.a.xe = xe;
      txt.a.ye = ys+wnd->title.height-1;
      txt.align = wnd->title.align;
      txt.h_space = wnd->title.h_space;
      txt.v_space = wnd->title.v_space;
      _UG_PutText( &txt );
      //UG_PutString(10, 0, &txt);

      /* Draw line */
      UG_DrawLine(xs,ys+wnd->title.height,xe,ys+wnd->title.height,pal_window);
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

static void _UG_WindowUpdate( UG_WINDOW* wnd )
{
   UG_U16 i,objcnt;
   UG_OBJECT* obj;
   UG_S16 xs,ys,xe,ye;

   xs = wnd->xs;
   ys = wnd->ys;
   xe = wnd->xe;
   ye = wnd->ye;

   wnd->state &= ~WND_STATE_UPDATE;
   /* Is the window visible? */
   if ( wnd->state & WND_STATE_VISIBLE )
   {
      /* 3D style? */
      if ( (wnd->style & WND_STYLE_3D) && !(wnd->state & WND_STATE_REDRAW_TITLE) )
      {
         _UG_DrawObjectFrame(xs,ys,xe,ye,(UG_COLOR)pal_window);
         xs+=3;
         ys+=3;
         xe-=3;
         ye-=3;
      }
      /* Show title bar? */
      if ( wnd->style & WND_STYLE_SHOW_TITLE )
      {
         _UG_WindowDrawTitle( wnd );
         ys += wnd->title.height+1;
         if ( wnd->state & WND_STATE_REDRAW_TITLE )
         {
            wnd->state &= ~WND_STATE_REDRAW_TITLE;
            return;
         }
      }
      /* Draw window area? */
      UG_FillFrame(xs,ys,xe,ye,wnd->bc);

      /* Force each object to be updated! */
      objcnt = wnd->objcnt;
      for(i=0; i<objcnt; i++)
      {
         obj = (UG_OBJECT*)&wnd->objlst[i];
         if ( !(obj->state & OBJ_STATE_FREE) && (obj->state & OBJ_STATE_VALID) && (obj->state & OBJ_STATE_VISIBLE) ) obj->state |= (OBJ_STATE_UPDATE | OBJ_STATE_REDRAW);
      }
   }
   else
   {
      UG_FillFrame(wnd->xs,wnd->xs,wnd->xe,wnd->ye,gui->desktop_color);
   }
}

static UG_RESULT _UG_WindowClear( UG_WINDOW* wnd )
{
   if ( wnd != NULL )
   {
      if (wnd->state & WND_STATE_VISIBLE)
      {
         wnd->state &= ~WND_STATE_VISIBLE;
         UG_FillFrame( wnd->xs, wnd->ys, wnd->xe, wnd->ye, gui->desktop_color );

         if ( wnd != gui->active_window )
         {
            /* If the current window is visible, update it! */
            if ( gui->active_window->state & WND_STATE_VISIBLE )
            {
               gui->active_window->state &= ~WND_STATE_REDRAW_TITLE;
               gui->active_window->state |= WND_STATE_UPDATE;
            }
         }
      }
      return UG_RESULT_OK;
   }
   return UG_RESULT_FAIL;
}

/* -------------------------------------------------------------------------------- */
/* -- BUTTON FUNCTIONS                                                           -- */
/* -------------------------------------------------------------------------------- */

const UG_COLOR pal_button_released = 1;

UG_RESULT UG_ButtonCreate(UG_WINDOW* wnd, UG_BUTTON* btn, UG_U8 id, UG_S16 xs, UG_S16 ys, UG_S16 xe, UG_S16 ye)
{
   UG_OBJECT* obj;

   obj = _UG_GetFreeObject( wnd );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   /* Initialize object-specific parameters */
   btn->obj = obj;
   btn->state = BTN_STATE_RELEASED;
   btn->bc = wnd->bc;
   btn->fc = wnd->fc;
   btn->abc = wnd->bc;
   btn->afc = wnd->fc;
   btn->style = BTN_STYLE_3D;
   btn->align = ALIGN_CENTER;
   if (gui != NULL) btn->font = &gui->font;
   else btn->font = NULL;
   btn->str = "-";

   /* Initialize standard object parameters */
   obj->update = _UG_ButtonUpdate;
   obj->touch_state = OBJ_TOUCH_STATE_INIT;
   obj->type = OBJ_TYPE_BUTTON;
   obj->event = OBJ_EVENT_NONE;
   obj->a_rel.xs = xs;
   obj->a_rel.ys = ys;
   obj->a_rel.xe = xe;
   obj->a_rel.ye = ye;
   obj->a_abs.xs = -1;
   obj->a_abs.ys = -1;
   obj->a_abs.xe = -1;
   obj->a_abs.ye = -1;
   obj->id = id;
   obj->state |= OBJ_STATE_VISIBLE | OBJ_STATE_REDRAW | OBJ_STATE_VALID | OBJ_STATE_TOUCH_ENABLE;
   obj->data = (void*)btn;

   /* Update function: Do your thing! */
   obj->state &= ~OBJ_STATE_FREE;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonDelete( UG_WINDOW* wnd, UG_U8 id )
{
   return _UG_DeleteObject( wnd, OBJ_TYPE_BUTTON, id );
}

UG_RESULT UG_ButtonShow( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   obj->state |= OBJ_STATE_VISIBLE;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonHide( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);

   btn->state &= ~BTN_STATE_PRESSED;
   obj->touch_state = OBJ_TOUCH_STATE_INIT;
   obj->event = OBJ_EVENT_NONE;
   obj->state &= ~OBJ_STATE_VISIBLE;
   obj->state |= OBJ_STATE_UPDATE;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetForeColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR fc )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->fc = fc;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetBackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR bc )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->bc = bc;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetAlternateForeColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR afc )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->afc = afc;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetAlternateBackColor( UG_WINDOW* wnd, UG_U8 id, UG_COLOR abc )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->abc = abc;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetText( UG_WINDOW* wnd, UG_U8 id, char* str )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->str = str;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetFont( UG_WINDOW* wnd, UG_U8 id, const UG_FONT* font )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->font = font;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetStyle( UG_WINDOW* wnd, UG_U8 id, UG_U8 style )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);

   /* Select color scheme */
   btn->style &= ~(BTN_STYLE_USE_ALTERNATE_COLORS | BTN_STYLE_TOGGLE_COLORS | BTN_STYLE_NO_BORDERS | BTN_STYLE_NO_FILL);
   btn->state |= BTN_STATE_ALWAYS_REDRAW;
   if ( style & BTN_STYLE_NO_BORDERS )
   {
      btn->style |= BTN_STYLE_NO_BORDERS;
   }
   if ( style & BTN_STYLE_NO_FILL )
   {
      btn->style |= BTN_STYLE_NO_FILL;
   }
   if ( style & BTN_STYLE_TOGGLE_COLORS )
   {
      btn->style |= BTN_STYLE_TOGGLE_COLORS;
   }
   else if ( style & BTN_STYLE_USE_ALTERNATE_COLORS )
   {
      btn->style |= BTN_STYLE_USE_ALTERNATE_COLORS;
   }
   else
   {
      btn->state &= ~BTN_STATE_ALWAYS_REDRAW;
   }

   /* 3D or 2D */
   if ( style & BTN_STYLE_3D )
   {
      btn->style |= BTN_STYLE_3D;
   }
   else
   {
      btn->style &= ~BTN_STYLE_3D;
   }
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetHSpace( UG_WINDOW* wnd, UG_U8 id, UG_S8 hs )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->h_space = hs;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetVSpace( UG_WINDOW* wnd, UG_U8 id, UG_S8 vs )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->v_space = vs;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_RESULT UG_ButtonSetAlignment( UG_WINDOW* wnd, UG_U8 id, UG_U8 align )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj == NULL ) return UG_RESULT_FAIL;

   btn = (UG_BUTTON*)(obj->data);
   btn->align = align;
   obj->state |= OBJ_STATE_UPDATE | OBJ_STATE_REDRAW;

   return UG_RESULT_OK;
}

UG_COLOR UG_ButtonGetForeColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_COLOR c = 1;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      c = btn->fc;
   }
   return c;
}

UG_COLOR UG_ButtonGetBackColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_COLOR c = 1;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      c = btn->bc;
   }
   return c;
}

UG_COLOR UG_ButtonGetAlternateForeColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_COLOR c = 1;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      c = btn->afc;
   }
   return c;
}

UG_COLOR UG_ButtonGetAlternateBackColor( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_COLOR c = 1;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      c = btn->abc;
   }
   return c;
}

char* UG_ButtonGetText( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   char* str = NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      str = btn->str;
   }
   return str;
}

UG_FONT* UG_ButtonGetFont( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_FONT* font = NULL;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      font = (UG_FONT*)btn->font;
   }
   return font;
}

UG_U8 UG_ButtonGetStyle( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_U8 style = 0;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      style = btn->style;
   }
   return style;
}

UG_S8 UG_ButtonGetHSpace( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_S8 hs = 0;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      hs = btn->h_space;
   }
   return hs;
}

UG_S8 UG_ButtonGetVSpace( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_S8 vs = 0;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      vs = btn->v_space;
   }
   return vs;
}

UG_U8 UG_ButtonGetAlignment( UG_WINDOW* wnd, UG_U8 id )
{
   UG_OBJECT* obj=NULL;
   UG_BUTTON* btn=NULL;
   UG_U8 align = 0;

   obj = _UG_SearchObject( wnd, OBJ_TYPE_BUTTON, id );
   if ( obj != NULL )
   {
      btn = (UG_BUTTON*)(obj->data);
      align = btn->align;
   }
   return align;
}


static void _UG_ButtonUpdate(UG_WINDOW* wnd, UG_OBJECT* obj)
{
   UG_BUTTON* btn;
   UG_AREA a;
   UG_TEXT txt;
   UG_U8 d;

   /* Get object-specific data */
   btn = (UG_BUTTON*)(obj->data);

   /* -------------------------------------------------- */
   /* Object touch section                               */
   /* -------------------------------------------------- */
   if ( (obj->touch_state & OBJ_TOUCH_STATE_CHANGED) )
   {
      /* Handle 'click' event */
      if ( obj->touch_state & OBJ_TOUCH_STATE_CLICK_ON_OBJECT )
      {
         obj->event = BTN_EVENT_CLICKED;
         obj->state |= OBJ_STATE_UPDATE;
      }
      /* Is the button pressed down? */
      if ( obj->touch_state & OBJ_TOUCH_STATE_PRESSED_ON_OBJECT )
      {
         btn->state |= BTN_STATE_PRESSED;
         obj->state |= OBJ_STATE_UPDATE;
         obj->event = OBJ_EVENT_PRESSED;
      }
      /* Can we release the button? */
      else if ( btn->state & BTN_STATE_PRESSED )
      {
         btn->state &= ~BTN_STATE_PRESSED;
         obj->state |= OBJ_STATE_UPDATE;
         obj->event = OBJ_EVENT_RELEASED;
      }
      obj->touch_state &= ~OBJ_TOUCH_STATE_CHANGED;
   }

   /* -------------------------------------------------- */
   /* Object update section                              */
   /* -------------------------------------------------- */
   if ( obj->state & OBJ_STATE_UPDATE )
   {
      if ( obj->state & OBJ_STATE_VISIBLE )
      {
         /* Full redraw necessary? */
         if ( (obj->state & OBJ_STATE_REDRAW) || (btn->state & BTN_STATE_ALWAYS_REDRAW) )
         {
            UG_WindowGetArea(wnd,&a);
            obj->a_abs.xs = obj->a_rel.xs + a.xs;
            obj->a_abs.ys = obj->a_rel.ys + a.ys;
            obj->a_abs.xe = obj->a_rel.xe + a.xs;
            obj->a_abs.ye = obj->a_rel.ye + a.ys;
            if ( obj->a_abs.ye > wnd->ye ) return;
            if ( obj->a_abs.xe > wnd->xe ) return;
#ifdef USE_PRERENDER_EVENT
            _UG_SendObjectPrerenderEvent(wnd, obj);
#endif

            /* 3D or 2D style? */
            d = ( btn->style & BTN_STYLE_3D )? 3:1;

            txt.bc = btn->bc;
            txt.fc = btn->fc;

            if( btn->state & BTN_STATE_PRESSED )
            {
               /* "toggle" style? */
               if( btn->style & BTN_STYLE_TOGGLE_COLORS )
               {
                  /* Swap colors */
                  txt.bc = btn->fc;
                  txt.fc = btn->bc;
               }
               /* Use alternate colors? */
               else if ( btn->style & BTN_STYLE_USE_ALTERNATE_COLORS )
               {
                  txt.bc = btn->abc;
                  txt.fc = btn->afc;
               }
            }
            if ( !(btn->style & BTN_STYLE_NO_FILL) )
               UG_FillFrame(obj->a_abs.xs+d, obj->a_abs.ys+d, obj->a_abs.xe-d, obj->a_abs.ye-d, txt.bc);

            /* Draw button text */
            txt.a.xs = obj->a_abs.xs+d;
            txt.a.ys = obj->a_abs.ys+d;
            txt.a.xe = obj->a_abs.xe-d;
            txt.a.ye = obj->a_abs.ye-d;
            txt.align = btn->align;
            txt.font = btn->font;
            txt.h_space = btn->h_space; //txt.h_space = 2;
            txt.v_space = btn->v_space; //txt.v_space = 2;
            txt.str = btn->str;
            _UG_PutText( &txt );
            obj->state &= ~OBJ_STATE_REDRAW;
#ifdef USE_POSTRENDER_EVENT
            _UG_SendObjectPostrenderEvent(wnd, obj);
#endif
         }
         /* Draw button frame */
         if ( !(btn->style & BTN_STYLE_NO_BORDERS) )
         {
             if ( btn->style & BTN_STYLE_3D )
             {  /* 3D */
                _UG_DrawObjectFrame(obj->a_abs.xs,obj->a_abs.ys,obj->a_abs.xe,obj->a_abs.ye, (btn->state&BTN_STATE_PRESSED)?(UG_COLOR)pal_button_released:(UG_COLOR)pal_button_released);
             }
             else
             {  /* 2D */
                 UG_DrawFrame(obj->a_abs.xs,obj->a_abs.ys,obj->a_abs.xe,obj->a_abs.ye,(btn->state&BTN_STATE_PRESSED)?btn->abc:btn->afc);
             }
         }
      }
      else
      {
          if ( !(btn->style & BTN_STYLE_NO_FILL) )
            UG_FillFrame(obj->a_abs.xs, obj->a_abs.ys, obj->a_abs.xe, obj->a_abs.ye, wnd->bc);
      }
      obj->state &= ~OBJ_STATE_UPDATE;
   }
}

