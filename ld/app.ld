/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Linker script for ST STM32 (STM32F103VET6, 512K flash, 64K RAM). */

/* Define memory regions. */
MEMORY
{
	rom (rx) : ORIGIN = 0x08000000, LENGTH = 384K
	ram (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
  //rom (rx) : ORIGIN = 0x08000000, LENGTH = 128K
  //ram (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

/* define stack size and heap size here */
stack_size = 0;
heap_size = 0;

/* define beginning and ending of stack */
/* _stack_start = ORIGIN(ram)+LENGTH(ram); */
/* _stack_end = _stack_start - stack_size; */

/* Include the common ld script. */
INCLUDE cortex-m-generic.ld

