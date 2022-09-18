/* 
 ***********************************************************************
 *
 *                           Copyright �
 *    Copyright � 2000 Force Computer, Inc., a Solectron company. All rights reserved.
 *    � Digital Equipment Corporation 1996, 1997. All rights reserved.
 *
 *    Restricted Rights: Use, duplication, or disclosure by the U.S.
 *    Government is subject to restrictions as set forth in subparagraph
 *    (c) (1) (ii) of DFARS 252.227-7013, or in FAR 52.227-19, or in FAR
 *    52.227-14 Alt. III, as applicable.
 *
 *    This software is proprietary to and embodies the confidential
 *    technology of Force Computers Incorporated and other parties.
 *    Possession, use, or copying of this software and media is authorized
 *    only pursuant to a valid written license from Force or an
 *    authorized sublicensor.
 *
 ***********************************************************************
 *    File Name:    ph_time1.c
 *    Author:       Matthew Schnee
 *    Creation Date:06-Jun-1996
 *
 *    Functionality:
 *		File to catch the language dependent duration rules.
 *
 ***********************************************************************
 *    Revision History:
 *	Rev	Who		Date			Description
 *	---	-----	-----------		------------------------------------------- 
 *	001 SIK		07/18/1996  	Cleaning up and maintenance
 *  002	GL		04/21/1997		BATS#360  remove spaces before "#define" or "#if" 
 *  003 DR		09/30/1997		UK BUILD: added UK STUFF
 *  004	MFG		06/18/1998		SPANISH_SP & SPANISH_LA support added
 *	005	CHJ		07/20/2000		FRENCH support
 *  006 CAB		10/18/2000		Changed copyright info
 */                            
 
#if defined(OLD_INTONATION_AND_TIMING)
#include "p_us_tim0.c"
#else
#include "p_us_tim.c"
#endif
#include "p_uk_tim.c"
#include "p_sp_tim.c"
#include "p_la_tim.c"
#include "p_gr_tim.c"
#include "p_fr_tim.c"



