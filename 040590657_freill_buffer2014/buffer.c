/*******************************************************************************************************************
File name: buffer.c
Author: James Freill, ID# 040590657
Course: CST8152 - Compilers Lab Section: 010
Assignment: Assignment 1
Date: Semptember 1st 2014
Professor: Sv. Ranev
Purpose: This is a review of and an exercise in C coding style, programming techniques, data types and
		 structures, memory management, and simple file input/output. It will give you a better
		 understanding of the type of internal data structures used by a simple compiler you will be building
		 this semester. This assignment will be also an exercise in “excessively defensive programming”.
		 You are to write functions that should be “overly” protected and should not abruptly terminate or
		 “crash” at run-time due to invalid function parameters, erroneous internal calculations, or memory
		 violations.
Function list: Buffer * b_create (), pBuffer b_addc(), int b_reset (), void b_destroy(),
			   int b_isfull (), short b_getsize (), short b_getcapacity(), short b_setmark (),
			   short b_getmark (), int b_getmode (), int b_load (), int b_isempty (),int b_eob (),
			   char b_getc (), int b_print (), Buffer *b_pack(), char b_get_r_flag (),
			   short b_retract (), short b_retract_to_mark (), short b_get_getc_offset (),
			   char * b_get_chloc ();
********************************************************************************************************************/
/***************************		INCLUDE FILES		************************************************************/
#include "buffer.h"
/*******************************************************************************************************************
Purpose: This function creates a new buffer in memory (on the program heap). After checking for abnormal
		 (bad) function parameters the function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: short init_capacity, char inc_factor, char o_mode
Return value: On success, the function returns a pointer to the Buffer structure. It must return NULL
			  pointer on any error which violates the constraints imposed upon the buffer parameters or prevents
			  the creation of a working buffer. If error occurs, the function must return immediately after the error
			  is discovered.
Algorithm: Tries to allocate memory for one Buffer structure using calloc();
		   Calls malloc() again with the given initial capacity init_capacity and tries to allocates memory for
		   one dynamic character buffer (character array). The returned pointer is assigned to ca_head;
		   Sets the buffer operational mode indicator mode and the Buffer structure increment factor inc_factor.
		   Copies the given init_capacity into the Buffer structure capacity variable.
*******************************************************************************************************************/
Buffer * b_create(short init_capacity, char inc_factor, char o_mode) {

	Buffer *pBD;

	if (init_capacity < (short)0) return NULL;
	if (o_mode != ADD && o_mode != FIXED && o_mode != MULTI) return NULL;

	pBD = (Buffer *)calloc(1, sizeof(Buffer));
	if (pBD == NULL && init_capacity > 0) return NULL;

	pBD->ca_head = (char *)malloc(init_capacity * sizeof(char));
	if (pBD->ca_head == NULL){
		b_destroy(pBD);
		return NULL;
	}

	switch (o_mode) {
	case FIXED:
		if (inc_factor == 0 || inc_factor != 0) {
			pBD->mode = 0;
			pBD->inc_factor = 0;
		}
		break;
	case ADD:
		if (inc_factor >= ADD_MIN && inc_factor <= ADD_MAX) {
			pBD->mode = 1;
			pBD->inc_factor = inc_factor;
		}
		break;
	case MULTI:
		if (inc_factor >= MULTI_MIN && inc_factor <= MULTI_MAX) {
			pBD->mode = -1;
			pBD->inc_factor = inc_factor;
		}
		break;
	default:
		b_destroy(pBD);
		return NULL;
		break;
	}

	pBD->capacity = init_capacity;
	pBD->addc_offset = pBD->mark_offset = pBD->getc_offset = pBD->r_flag = 0;

	return pBD;
}
/*******************************************************************************************************************
Purpose: The function resets r_flag to 0 and tries to add the character symbol to the character array of the
		 given buffer pointed by pBD. If the buffer is operational and it is not full, the symbol can be stored
		 in the character buffer. In this case, the function adds the character to the content of the character
		 buffer, increments addc_offset by 1 and returns.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: pBuffer const pBD, char symbol
Return value: The function adds the character to the content of the character buffer, increments addc_offset by 1 and returns.
			  If the operational mode is 0: the function returns NULL.
			  If the operational mode is 1: If the result from the operation is positive,
			  the function proceeds; otherwise it returns NULL.
			  If the operational mode is -1: If successful, the function continues; Otherwise, If the current 
			  capacity can not be incremented anymore because it has already reached the maximum size of the 
			  buffer, the function returns NULL.
			  The function must return NULL on any error.
Algorithm: 
*******************************************************************************************************************/
pBuffer b_addc(pBuffer const pBD, char symbol) {

	float new_capacity = 0, available_space = 0, new_Increment = 0;
	char* pBD_temp;

	if (pBD == NULL) return NULL;
	if ((pBD->addc_offset * sizeof(char)) == SHRT_MAX) return NULL;

	pBD->r_flag = 0;

	if (!b_isfull(pBD)){    /*Checks the buffer size*/
		*(pBD->ca_head + pBD->addc_offset) = symbol; /*sets the buffer character   */
		++pBD->addc_offset;  /*increments the offset of the buffer   */
		return pBD;
	}

	if (pBD->mode == 0) return NULL;
	else if (pBD->mode == 1) { /* additive mode */
		new_capacity = (float)pBD->capacity + ((float)pBD->inc_factor * sizeof(char)); /* increase capacity */
		if (new_capacity < 0) return NULL; /* checks for overflow */
	}
	else if (pBD->mode == -1) { /* multiplicative  mode*/
		available_space = (float)SHRT_MAX - pBD->capacity;
		new_Increment = available_space * (((float)pBD->inc_factor * sizeof(char)) / 100);
		if (new_Increment <= 0.0 || new_Increment > SHRT_MAX) new_capacity = SHRT_MAX; /* checking for new increment == 0 and for overflow */
		else new_capacity = pBD->capacity + new_Increment; /* calculate the new capacity */
	}

	pBD_temp = pBD->ca_head;
	pBD->ca_head = (char *)realloc(pBD->ca_head, (size_t)new_capacity);
	if (pBD->ca_head == NULL) return NULL; /* could not reallocate */
	if (pBD_temp != pBD->ca_head){ /* memory location of character buffer has changed */
		pBD->r_flag = SET_R_FLAG; /* sets the flag of the buffer */
	}

	*(pBD->ca_head + pBD->addc_offset) = symbol; /* assign the character to the new allocated buffer array */
	++pBD->addc_offset;   /*increments offset     */
	pBD->capacity = (short)new_capacity;  /*sets new capacity  */
	return pBD;
}
/*******************************************************************************************************************
Purpose: The function retains the memory space allocated to the buffer at the moment, but re-initializes all
		 appropriate data members of the given Buffer structure (buffer descriptor), such that the buffer will
		 appear empty and the next call to b_addc() will put the character at the beginning of the character
		 buffer.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: If a runtime error is possible, the function should return –1 in order to notify
			  the calling function about the failure.
Algorithm: 
*******************************************************************************************************************/
int b_reset(Buffer * const pBD) {
	if (pBD == NULL)return R_FAIL_1;
	pBD->addc_offset = pBD->getc_offset = pBD->mark_offset = 0;	
	pBD->r_flag = 0;
	return 0;
}
/*******************************************************************************************************************
Purpose: The function de-allocates (frees) the memory occupied by the character buffer and the Buffer
		 structure (buffer descriptor). The function should not crash.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: free();
Parameters: Buffer * const pBD
Return value: returns NULL if pBD is NULL, otherwise frees pBD and continues.
Algorithm:
*******************************************************************************************************************/
void b_destroy(Buffer * const pBD) {
	if (pBD != NULL) {
		if (pBD->ca_head != NULL) free(pBD->ca_head);
		free(pBD);
	}
}
/*******************************************************************************************************************
Purpose: Checks to see if the buffer is full.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions:
Parameters: Buffer * const pBD
Return value: The function returns 1 if the character buffer is full;
			  It returns 0 otherwise.
			  If a run-time error is
			  possible, the function should return –1.
Algorithm:
*******************************************************************************************************************/
int b_isfull(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	if ((pBD->addc_offset * sizeof(char)) == (unsigned short)pBD->capacity) return 1;
	return 0;	
}
/*******************************************************************************************************************
Purpose: The function returns the current size of the character buffer.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns the current size of the character buffer.
			  If a runtime error is possible, the function should return –1.
Algorithm: 
*******************************************************************************************************************/
short b_getsize(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	return pBD->addc_offset;
}
/*******************************************************************************************************************
Purpose: The function returns the current capacity of the character buffer.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns the current capacity of the character buffer. 
			  If a run-time error is possible, the
function should return –1.
Algorithm: 
*******************************************************************************************************************/
short b_getcapacity(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	return pBD->capacity;
}
/*******************************************************************************************************************
Purpose: The function sets mark_offset to mark.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: sizeof();
Parameters: Buffer * const pBD, short mark
Return value: If a run-time error is possible, the function should return –1; 
			  otherwise it returns mark_offset.
Algorithm: Sets mark_offset to mark.
		   If the buffer is empty, the mark_offset is set to addc_offset.
*******************************************************************************************************************/
short b_setmark(Buffer * const pBD, short mark) {
	if (pBD == NULL)return R_FAIL_1;
	if (mark < 0 || mark > pBD->capacity) return R_FAIL_1;
	if (b_isempty(pBD)) pBD->mark_offset = pBD->addc_offset;
	else pBD->mark_offset = mark;
	return pBD->mark_offset;
}
/*******************************************************************************************************************
Purpose: The function returns mark_offset to the calling function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns mark_offset to the calling function. 
			  If a run-time error is possible, the function should return –1.
Algorithm: 
*******************************************************************************************************************/
short b_getmark(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	return pBD->mark_offset;
}
/*******************************************************************************************************************
Purpose: The function returns the value of mode to the calling function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns the value of mode to the calling function. 
			  If a run-time error is possible, the function should notifies the calling function about the failure.
Algorithm: 
*******************************************************************************************************************/
int b_getmode(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	return pBD->mode;
}
/*******************************************************************************************************************
Purpose: The function loads (reads) an open input file specified by fi into a buffer specified by pB.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: FILE * const fi, Buffer * const pBD
Return value: If the loading operation is successful, the function must return the number
			  of characters added to the buffer.
			  If some other run-time error is possible, the function should return –1.
			  If the current character cannot be put in the buffer, the function must return -2(LOAD_FAIL)
Algorithm: Use the standard function fgetc(fi) to read one character at a time.
		   Then b_addc() to add the character to the buffer.
		   The operation is repeated until the standard macro feof(fi) detects end-of-file on the input file.
		   The end-of-file character must not be added to the content of the buffer.
*******************************************************************************************************************/
int b_load(FILE * const fi, Buffer * const pBD) {

	int i = 0;
	unsigned char c;

	if (pBD == NULL || fi == NULL) return R_FAIL_1;
	
	c = (unsigned char)fgetc(fi);
	while (!feof(fi) || c == '\0'){
		if (b_addc(pBD, c) == NULL) return LOAD_FAIL;
		i++;
		c = (unsigned char)fgetc(fi);		
	}
	return i;
}
/*******************************************************************************************************************
Purpose: If the addc_offset is 0, the function returns 1; otherwise it returns 0. If a run-time error is possible,
		 it should return –1.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: If the addc_offset is 0, the function returns 1;
			  Otherwise it returns 0. 
			  If a run-time error is possible, it should return –1.
Algorithm: 
*******************************************************************************************************************/
int b_isempty(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	if (pBD->addc_offset == 0) return 1;
	else return 0;
}
/*******************************************************************************************************************
Purpose: The function returns eob to the calling function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns eob to the calling function.
			  If a run-time error is possible, it should return –1.
Algorithm: 
*******************************************************************************************************************/
int b_eob(Buffer * const pBD) {
	if (pBD == NULL)return R_FAIL_1;
	return pBD->eob;
}
/*******************************************************************************************************************
Purpose: Returns the character located at getc_offset. Before returning it increments getc_offset by 1
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: [list of function(s) that are called by this function]
Parameters: Buffer * const pBD
Return value: If it is not valid, it returns -2;
			  If getc_offset and addc_offset are equal, it sets eob to 1 and returns -1;
			  Returns the character located at getc_offset.
Algorithm: Checks the argument for validity. If it is not valid, it returns -2;
		   If getc_offset and addc_offset are equal, it sets eob to 1 and returns -1;
		   Otherwise it sets eob to 0;
		   Before returning it increments getc_offset by 1.
*******************************************************************************************************************/
char b_getc(Buffer * const pBD) {

	char temp;

	if (pBD == NULL) return R_FAIL_2;

	if (pBD->getc_offset == pBD->addc_offset) {
		pBD->eob = 1;
		return -1;
	} else pBD->eob = 0;

	temp = *(pBD->ca_head + pBD->getc_offset);
	++pBD->getc_offset;

	return temp;
}
/*******************************************************************************************************************
Purpose: The function is intended to be used for used for diagnostic purposes only.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: It returns the number of characters printed. 
			  The function returns –1 on failure.
Algorithm: Prints character by character the contents of the character buffer to the standard output (stdout).
		   Checks if the buffer is empty.
		   Then the function sets the getc_offset to 0.
		   Prints the content calling b_getc() in a loop and using b_eob() to detect the end of the buffer content.
		   Then the function sets getc_offset to 0 again.
*******************************************************************************************************************/
int b_print(Buffer * const pBD) {

	unsigned int i = 0;
	unsigned char temp_char;

	if (pBD == NULL)return R_FAIL_1;

	if (b_isempty(pBD)) {  /* check to see if buffer is empty */
		printf("The buffer is empty.\n");
		return i;
	}

	pBD->getc_offset = 0;

	temp_char = b_getc(pBD);
	while (!b_eob(pBD)){
		if (temp_char == EOF || temp_char == '\0') return i;
		++i;
		printf("%c", temp_char);
		temp_char = b_getc(pBD);
	}
	printf("\n");
	pBD->getc_offset = 0;

	return i;
}
/*******************************************************************************************************************
Purpose: The function uses realloc() to adjust the new capacity, and then updates all the necessary 
		 members of the buffer descriptor structure. Finally it returns a pointer to Buffer.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions:
Parameters: Buffer * const pBD
Return value: Returns a pointer to Buffer. 
		      The function must return NULL if for some reason it cannot to perform the required operation.
Algorithm: Uses realloc() to adjust the new capacity, and then updates all the necessary
		   members of the buffer descriptor structure.
		   It must set the buffer r_flag appropriately.
*******************************************************************************************************************/
Buffer *b_pack(Buffer * const pBD) {

	short new_capacity = 0;
	char* pBD_temp;

	if (pBD == NULL)return NULL;
	if ((pBD->addc_offset * sizeof(char)) == SHRT_MAX)return NULL; /* possible overflow */

	pBD->r_flag = 0; /* reset the reallocation flag */
	new_capacity = (pBD->addc_offset + 1) * sizeof(char);

	pBD_temp = pBD->ca_head;
	pBD->ca_head = (char *)realloc(pBD->ca_head, new_capacity);
	if (pBD->ca_head == NULL) return NULL; /* could not reallocate */
	if (pBD_temp != pBD->ca_head){ /* memory location of character buffer has changed */
		pBD->r_flag = SET_R_FLAG; /*sets the flag of the buffer*/
	}

	pBD->capacity = new_capacity;
	return pBD;
}
/*******************************************************************************************************************
Purpose: The function returns r_flag to the calling function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: The function returns r_flag to the calling function.
			  If a run-time error is possible, it should return –1.
Algorithm: 
*******************************************************************************************************************/
char b_get_r_flag(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	return pBD->r_flag;
}
/*******************************************************************************************************************
Purpose: The function decrements getc_offset by 1.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: If a run-time error is possible, it should return –1;
			  Otherwise it returns getc_offset.
Algorithm: The function decrements getc_offset by 1.
*******************************************************************************************************************/
short b_retract(Buffer * const pBD) {
	if (pBD == NULL) return R_FAIL_1;
	if (pBD->getc_offset < 0) return R_FAIL_1;
	--pBD->getc_offset;
	return pBD->getc_offset;
}
/*******************************************************************************************************************
Purpose: The function sets getc_offset to the value of the current mark_offset.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions:
Parameters: Buffer * const pBD
Return value: If a run-time error is possible, it should return –1; 
			  Otherwise it returns getc_offset.
Algorithm: Sets getc_offset to the value of the current mark_offset.
*******************************************************************************************************************/
short b_retract_to_mark(Buffer * const pBD) {
	if (pBD == NULL)return R_FAIL_1;
	pBD->getc_offset = pBD->mark_offset;
	return pBD->getc_offset;
}
/*******************************************************************************************************************
Purpose: The function returns getc_offset to the calling function.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD
Return value: Returns getc_offset.
			  If a run-time error is possible, it should return -1.
Algorithm: 
*******************************************************************************************************************/
short b_get_getc_offset(Buffer * const pBD) {
	if (pBD == NULL)return R_FAIL_1;
	return pBD->getc_offset;
}
/*******************************************************************************************************************
Purpose: The function returns a pointer to the location of a character in the character buffer.
Author: James Freill #040590657
History/Versions: V1.0, Semptember 2014
Called functions: 
Parameters: Buffer * const pBD, short offset
Return value: The function returns a pointer to the location of a character in the character buffer.
			  If a run-time error is possible, the function should return NULL pointer.
Algorithm: The location of the character is specified by the parameter offset (it indicates the
		   offset from ca_head). The parameter offset must be within the size of the buffer (0 to addc_offset inclusive).
*******************************************************************************************************************/
char * b_get_chloc(Buffer * const pBD, short offset) {
	if (pBD == NULL) return NULL;
	if (offset < 0 || offset > pBD->addc_offset) return NULL;
	return (pBD->ca_head + offset);
}
