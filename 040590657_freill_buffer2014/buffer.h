/*************************************************************************************************************************************************************************************
File name: buffer.h
Author: James Freille, ID# 040590657
Course: CST8152 - Compilers Lab Section:010
Assignment: Assignment 1
Date: Semptember 1st 2014
Professor: Sv. Ranev
Purpose: Complete a "buffer utitity", which includes the buffer data structure and the following functions, following strictly given specififications. Using the given data
declarations and function prototypes, without any modification to them, or their names. Along with actively asserting a "excessively defensive programming" ability to
help maintain the bonderies of the program within the allocated memory, with no creation of memory leaks.
Function list:
*************************************************************************************************************************************************************************************/
#ifndef BUFFER_H_
#define BUFFER_H_
/*#pragma warning(1:4001) *//*to enforce C89 type comments  - to make //comments an warning */
/*#pragma warning(error:4001)*//* to enforce C89 comments - to make // comments an error */
/**************************************************************************************************
**********************************     STANDARD HEADER FILES	 **********************************
**************************************************************************************************/
#include <stdio.h>			/* standard input/output */
#include <malloc.h>			/* for dynamic memory allocation*/
#include <limits.h>			/* implementation-defined data type ranges and limits */
#include <stdlib.h>			/* C Standard General Utilities Library */
#include <string.h>			/* header file defines several functions to manipulate C strings and arrays */
#include <errno.h>			/* C Error Codes header file. */
/* #include <crtdbg.h> */	/* The primary tools for detecting memory leaks are the debugger and the C Run-Time Libraries (CRT) debug heap functions. */
/**************************************************************************************************
**********************************     CONTSTANT DEFINITIONS	 **********************************
**************************************************************************************************/
#define FIXED 'f'		    /* defined constant for fixed mode */
#define MULTI 'm'		    /* defined constant for multiplicative mode */
#define ADD 'a'			    /* defines constant for additive mode */
#define R_FAIL_1 -1         /* fail return value */
#define R_FAIL_2 -2         /* fail return value */
#define LOAD_FAIL -2        /* load fail error */
#define SET_R_FLAG 1        /* realloc flag set value */
#define MULTI_MAX 100       /* defined constant for multiplicative max factor */
#define MULTI_MIN 1		    /* defined constant for multiplicative min factor */	
#define ADD_MAX 255		    /* defined constant for addivitive max factor */
#define ADD_MIN 1		    /* defined constant for addivitive min facotr */
#define _CRTDBG_MAP_ALLOC	/* defined constant for detecting memory leaks */
/**************************************************************************************************
********************************		STRUCT BUFFER DEFINE	  *********************************
**************************************************************************************************/
typedef struct BufferDescriptor {
	char *ca_head;			/* pointer to the beginning of character array (character buffer) */
	short capacity;		    /* current dynamic memory size (in bytes) allocated to character buffer */
	short addc_offset;		/* the offset (in chars) to the add-character location */
	short getc_offset;		/* the offset (in chars) to the get-character location */
	short mark_offset;	    /* the offset (in chars) to the mark location */
	char  inc_factor;		/* character array increment factor */
	char  r_flag;			/* reallocation flag */
	char  mode;				/* operational mode indicator*/
	int   eob;				/* end-of-buffer flag */
} Buffer, *pBuffer;
/**************************************************************************************************
********************************		FUNCTION PROTOTYPES			*******************************
**************************************************************************************************/
Buffer * b_create(short init_capacity, char inc_factor, char o_mode);
pBuffer b_addc(pBuffer const pBD, char symbol);
int b_reset(Buffer * const pBD);
void b_destroy(Buffer * const pBD);
int b_isfull(Buffer * const pBD);
short b_getsize(Buffer * const pBD);
short b_getcapacity(Buffer * const pBD);
short b_setmark(Buffer * const pBD, short mark);
short b_getmark(Buffer * const pBD);
int b_getmode(Buffer * const pBD);
int b_load(FILE * const fi, Buffer * const pBD);
int b_isempty(Buffer * const pBD);
int b_eob(Buffer * const pBD);
char b_getc(Buffer * const pBD);
int b_print(Buffer * const pBD);
Buffer *b_pack(Buffer * const pBD);
char b_get_r_flag(Buffer * const pBD);
short b_retract(Buffer * const pBD);
short b_retract_to_mark(Buffer * const pBD);
short b_get_getc_offset(Buffer * const pBD);
char * b_get_chloc(Buffer * const pBD, short offset);
#endif
