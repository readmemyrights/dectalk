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
 *    File Name:	ls_spel1.c
 *    Author:		Matthew Schnee                                         
 *    Creation Date:02/06/96                                                   
 *                                                                             
 *    Functionality:                                                           
 *    File to catch the language depend spell vs. speak include file                                                                             
 *                                                                             
 ***********************************************************************       
 *                                                                             
 * Rev	Who		Date			Description                    
 * ---	-----	-----------		---------------------------------------
 * 001	DR	07/21/1997		UK BUILD: ENGLISH_US was changed to ENGLISH or added a copy as ENGLISH_UK       
 * 
 */
   
#ifdef ENGLISH_US
#include "l_us_sp1.c"
#endif

#ifdef ENGLISH_UK
#include "l_uk_sp1.c"
#endif

#ifdef SPANISH
#include "l_sp_sp1.c"
#endif

#ifdef GERMAN
#include "l_gr_sp1.c"
#endif
