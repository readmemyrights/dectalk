/*
 ***********************************************************************
 *                                                                      
 *                           Coryright (c)                              
 *    � Digital Equipment Corporation 1996, 1997. All rights reserved.    
 *                                                                      
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.    
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.                                
 *                                                                      
 *    This software is proprietary to and embodies the confidential     
 *    technology of Digital Equipment Corporation and other parties.    
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Digital or an        
 *    authorized sublicensor.                                            
 *                                                                       
 *********************************************************************** 
 *    File Name:	ls_data.h
 *    Author:		Matthew Schnee                                         
 *    Creation Date:02/06/96                                                   
 *                                                                             
 *    Functionality:                                                           
 *	  Data structure definitions for LTS
 *                                                                             
 ***********************************************************************       
 *    Revision History:
 *                                                                             
 *	Rev	Who		Date			Description                    
 *	---	-----	-----------		---------------------------------------
 *	001	MGS		02/29/1996		Added LTS_T instance data structure for WIN32 merge
 *	002	MGS		03/18/1996		Finished WIN32 code merge, function headers need updating
 *  003 GL		10/03/1996		change rpart to U16 instead of S16.(to take care of the longer
 *								rule table indexing.    
 *  004	GL		04/21/1997		BATS#357  Add the code for __osf__ build 
 *  005	MGS		10/17/1997		add old_fc_index in LTS_TAG 
 *  006	GL		06/12/1998		"rpart" should be "unsigned short" for LDS_BUILD
 * 
 */

#ifndef LS_DATAH
#define LS_DATAH

/* 
 * Description : This file contains the declaration of instance specific
 * structure for LTS thread
 */
      
      
      
struct LTS_TAG 
{
#if defined (VMS) || defined (LDS_BUILD)
	unsigned short	rpart;
#else
	U16  	rpart ;
#endif

	GRAPH   graph[NGWORD];
	PHONE 	*pflp;           
	PHONE   *sylp[NSYL];    
	short   nsyl;                                   
	short   rsyl;                   
	short   psyl;                   
	short   lphone;
	unsigned char comp_str[128];
	unsigned char *str_vowel;
	U32 	fc_struct[256];
	int 	fc_index;
	int 	old_fc_index;
	short   wstate;
	PHONE   phead;
	short   fchar;
	short   schar;
	unsigned int no_pars;
	unsigned int  abbrev_look;
	int 	lflag;
	int 	isnumabr;
	ITEM    citem;                          /* Current item.                */
	ITEM    nitem;                          /* Next item, for ls_task_lookahead.    */
	LETTER  cword[NGWORD];                  /* Current word.                */
	LETTER  nword[NGWORD];                  /* Next word, for ls_task_lookahead.    */
	int 	tlflag;  
	int		sign;
	int		lbphone;
	int		rbphone;
	int   	pflag;
	unsigned int hit_type;

#ifdef SPANISH
    int ord;
    int flag; 
    short dic_offset;
    int got_quote;
#endif

}; 
typedef struct LTS_TAG LTS_T;
typedef LTS_T *PLTS_T;
typedef LTS_T **PPLTS_T;
  
/* GL 04/21/1997  change this for OSF build */
#ifdef MSDOS
typedef struct share_data far *PKSD_T;
struct TTS_HANDLE_TAG
{
	PKSD_T	pKernelShareData;
	PLTS_T	pLTSThreadData;
}; 
typedef struct TTS_HANDLE_TAG  *LPTTS_HANDLE_T;
#endif /* #ifdef MSDOS */


#endif
