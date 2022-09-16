/*
 ***********************************************************************
 *
 *                           Coryright (c)
 *    � Digital Equipment Corporation 1996, 1997. All rights reserved.
 *	WINprintf ("to  %d \n", ((pDph_t->allodurs[endcnt] * NSAMP_FRAME) / 10));
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
 *    File Name:    ph_aloph.c
 *    Author:
 *    Creation Date:
 *
 *    Functionality:
 *  	Convert string of phonemes (with accent/boundary features) into
 *    integer allophone sequence with accent/boundary features
 *    Add features FHAT_RISE and FHAT_FALL where appropriate
 *    No phonetic insertions or deletions allowed after this module
 *
 ***********************************************************************
 *    Revision History:
 *
 * Rev  Who     Date            Description
 * ---  -----   -----------     -------------------------------------------- 
 * 0001 DK		12/16/1984      Initial creation
 * 0002 DGC		12/27/1984	    Make version for 80186
 * 0003 DK		01/16/1984      Fix f0mode handling, fix flap rule
 * 0004 DK		01/29/1985	    Minor fix to palatalization rule
 * 0005 DK		02/14/1985      [dh] -> [d$] (a dental stop) after [t,d,n]
 * 0006 DK		02/26/1985      Can't flap [t] in [nt] until after [n] dur computed
#ifdef ENGLISH_US               From phalloph.c
 * 0007 DK 		04/25/1985      [t#y] -> [C] only if [y] not primary stressed
 * 0008 DK		05/15/1985	    Fix [t] -> [ch] rule, realization of "to", "into"
 * 0009 DK		05/28/1985      "For" not reduced before vowel or silence
 * 0010 DK		07/16/1985      Word-internal flapping only if post-stressed, not [nd]
 * 0011 DK 		08/08/1985	    or if involve "weak" vowels [x,|,R,i]
 * 0012 DK		08/23/1985	    Remove rule changing [yu] to [uw] after palatal
 * 0013 DK		08/28/1985	    Correct rule 1b, require unstressed [fR]
 * 0014 MGS		03/25/1996	    Merged WIN 95 code with 42c  
 * 0015 MGS		04/01/1996	    Added MSDBBG blocks for debugging
 * 0016 MGS		04/02/1996	    Initialized all local variables
#endif
#ifdef SPANISH                  From sphallph.c
 * 0007 MM		04/02/1985	    68000 hacks.
 * 0008 MM		05/02/1986	    Added phonemic dump
 * 0009 DF		06/10/1986	    DT3 updates: nbrphone -> nphonetot
#endif
#ifdef GERMAN                   From gphallph.c
 * 0014 BNK		07/22/1986	    preliminary change to secondary stress, take
 *                      		tertiary stress into account
 *                      		ormal FSTRESS-masking only refers to S1, S2, SEMPH
 *                      		introduction of new phonemes
 * 0015 BNK		01/14/1987	    german allophonic rules
 * 0016 CJL		03/30/1995	    File was phalloph.c. Is now gphallph.c. Changed
 *                      		phdefs.h->gphdefs.h
 * 0017 JDB		08/02/1995    	merged with EAB's pre-beta changes...
#endif
 * 0017	MGS		05/31/1996	 	Merged Spanish with english
 * 0018 MGS		06/06/1996 		File name changed from ph_alloph.c to ph_aloph.c
 * 0019	MGS		06/17/1996	 	Merged German with English/Spanish
 * 0020 SIK		07/09/1996  	Cleaning up and maintenance
 * 0021 EDB		01/10/1997		change allop rule in EL/EM/EN area.
 * 0022 EDB		02/14/1997		fix Spanish early stress drop off.
 * 0023 EAB		03/25/97		EAB unifiy phnemic codes dz df, BATS# 302
 * 0024	GL		04/21/1997		BATS#360  remove spaces before "#define" or "#if" 
 * 0025 DR		09/30/1997		UK BUILD: added UK STUFF
 * 0026 EAB		10/19/1997		use Cite_it variable and add syllable count support.
 * 0027 EAB		12/12/1997		fix many UK_English code.with Carolines direction
 * 0028 EAB		1/22/98			Modified UK to handle secondary stresses-this should eventually migrate to all
 *								languages PART OF BATS 574
 * 0029 EAB		12/17/98		Fix BATS 459 vowel for unreduce and wrong....
 * 0030 EAB		4/27/98			Modified behavior of various elements to process German better
 * 0031 EAB     7/8/98		    BATS 711 Changed feature bit array size to hold more feature bits.
 * 0032 EAB		9/11/98			Improved Spanish rules
 * 0033 EAB	9/29/98 EAB removed wx semi-vowel for now research it but for now leave u in place
 * 0034 EAB 	Rules handle dipthong better without semi-vowels get rid of them 
 * 0035 EAB		2/2/99 			Modified NWSNOAA to NWS_US for more languages for NOAA
 */
/* #define MSDBG6 */
/* #define MSDBG7 */

/* #include "phdefs.h" */
/* #include "phinst.h" *//* MVP : new instance header file */
#include "ph_def.h"

						 /* #define DEBUGALLO 1 *//* debugiing eab */
/****************************************************************************/
/* MVP : The following extern variables are now become elements of instance */
/* specific PH thread data structure DPH_T.                                	*/
/* Input arrays:                                                           	*/
/* extern short *phonemes;     Integer represent of real phonemes          	*/
/* extern short *sentstruc;    Struct feats, (mod phone[],nphonetot)       	*/
/* extern short *user_durs;    Optional user-specified durations           	*/
/* extern short *user_f0;      Optional user-specified f0 commands         	*/
/* extern short nphonetot;     Number of input symbols                     	*/
/* extern short f0mode;        State variable controlling f0 strategy      	*/
/* extern short symbols[];                                                 	*/
/* int docitation;                                                         	*/
/* Output arrays:                                                       	*/
/* extern short allophons[];   Integer rep of phonetic string              	*/
/* extern short allofeats[];   Structural features                         	*/
/* extern short nallotot;      Number of phones in phonetic string         	*/
/****************************************************************************/

/* From PHROM.C                                                      */
extern short            featb[];	   /* Phonetic feature values for phonemes */

	/* static short n,curr_outph,curr_outstruc; */
	/* static short curr_indur,curr_inf0;     */
	/* static short hatposition,emphasislock,stresses_in_phrase; */

	/* extern short remaining_stresses_til(short, short); */
	/* extern short promote_last_2(short); */
/* MVP added declaration of set_index_allo (defined in services.c)  */
#ifndef MSDOS
	extern void set_index_allo (PKSD_T pKsd_t, unsigned int nphone, unsigned int nallo);

#endif

/* Static function declarations MVP */
static void make_out_phonol (LPTTS_HANDLE_T phTTS, short n,
							 short curr_outph, U32 curr_outstruc,
							 short curr_indur, short curr_inf0);
static short remaining_stresses_til (PDPH_T pDph_t, short msym, short b_type);
static short promote_last_2 (PDPH_T pDph_t, short msym);

#ifdef SPANISH
static short hphone[] =
{
	SIL,
	HAT_RISE,
	HAT_FALL,
	HAT_RF
};

static short sphone[] =
{
	SIL,
	S1,
	S2,
	SEMPH
};

static short bphone[] =
{
	MBOUND,							   /* FMBNEXT      */
	WBOUND,							   /* FWBNEXT      */
	PPSTART,						   /* FPPNEXT      */
	VPSTART,						   /* FVPNEXT      */
	RELSTART,						   /* FRELNEXT     */
	COMMA,							   /* FCBNEXT      */
	PERIOD,							   /* FPERNEXT     */
	QUEST,							   /* FQUENEXT     */
	EXCLAIM							   /* FEXCLNEXT    */
};
#endif

/*
 *      Function Name: phalloph()      
 *
 *  	Description: Generates allophones for phoneme code. 
 *					 Input symbols are taken from phonemes[],
 *					 output symbols are put in array allophons[].
 *
 *      Arguments: LPTTS_HANDLE_T phTTS
 *
 *      Return Value: void
 *
 *      Comments:
 *
 */
void phalloph (LPTTS_HANDLE_T phTTS)
{

	short                   curr_inph = 0;
	U32						curr_instruc = 0;
	short                   next_inph = 0, last_outph = 0;
	int                     ph_delcnt = 0;
#ifdef ENGLISH
	short					symlas = 0;
#endif
	short					delete = 0;
	short                   n, curr_outph = 0; 
	U32						curr_outstruc = 0;
	short					curr_indur = 0, curr_inf0 = 0;	/* MVP made local */
	short                   hatposition = 0, emphasislock = 0, stresses_in_phrase = 0;
	short					offset = 0, temp = 0;       
	short					Cite_It;
	short					sylcount;
#if defined (GERMAN) || defined (SPANISH)
	short					count;
	short					m;
	short					doitlater=0;

	/* short				tmp=0; */
#endif
	PKSD_T                  pKsd_t = phTTS->pKernelShareData;
	PDPH_T                  pDph_t = phTTS->pPHThreadData;

	/* Main loop: for each input symbol, see if modify before pass to output */

	pDph_t->nallotot = 0;
	delete = FALSE;
	hatposition = AT_BOTTOM_OF_HAT;	   /* Must be reset because */
	emphasislock = FALSE;			   /* of possibility of an */
	stresses_in_phrase = 0;			   /* emergency halt.      */
	curr_inf0 = 0;
#ifdef ENGLISH
	last_outph = SIL;
#endif
#ifdef GERMAN
	last_outph = SIL;
#endif
#ifdef SPANISH
   	last_outph = NOVALID;			   /* Can't happen         */
#endif

#ifdef ENGLISH
	if (pDph_t->nphonetot >= 6)
	{
		pDph_t->docitation = 0;		   /* phrase is too long for citing */
	}
	Cite_It= 0;
	/* EAB 10/7/97 Changed the claculation od cit-it becuase it only needs to be calculated
	once per phrase and it was being calculated many times for each phoneme also wanted to modify
	it's setting for slowtalk*/

	if((pKsd_t->modeflag & MODE_CITATION) && pDph_t->docitation)
			Cite_It = 1;
#ifdef SLOWTALK /* When we get slower we don't start slurring we seprate the words and cite them*/
		if (pKsd_t->sprate < 100)
			Cite_It=TRUE;
#endif
	sylcount=0;
	
#endif
	for (n = 0; n < pDph_t->nphonetot; n++)
	{
		
	
#ifdef ENGLISH
	if( (pDph_t->sentstruc[n] & FFIRSTSYL )IS_PLUS )
		sylcount=1;
	else if((featb[pDph_t->phonemes[n]] & FSYLL) IS_PLUS)
		sylcount++;
	
#endif

		
#ifdef GERMAN
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* by default, don't insert any phonemes              					  */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		pDph_t->curr_insph = -1;
#endif
		/* 
		 * Input symbols are taken from phonemes[], and this array is never    
		 * changed.  Variables that refer to aspects of the input arrary are:  
		 */

		curr_inph = pDph_t->phonemes[n];
#ifdef DEBUGALLO
		printf ("input phonenme to alloph is %d", curr_inph);
#endif
		curr_instruc = pDph_t->sentstruc[n];
		if (n < pDph_t->nphonetot - 1)       
		{
			next_inph = pDph_t->phonemes[n + 1];
		}
		else
		{
			next_inph = SIL;
		}
		/* printf(" cur=%d ,nx = %d tot= %d \n ",curr_inph,next_inph,pDph_t->nphonetot); */

		/* 
		 * Output symbols are put in array allophons[].  Variables that refer   
		 * to this array are:                                                  
		 */

		curr_outph = curr_inph;
		curr_outstruc = curr_instruc;
		if (n > 0)
		{
			last_outph = pDph_t->allophons[pDph_t->nallotot - 1];
		}

		/* 
		 * Deal carefully with user specified durations or f0-commands because  
		 * input symbols may be deleted or combined.                           
		 */

#if defined (ENGLISH) || defined (GERMAN)
		curr_indur = pDph_t->user_durs[n];	/* User-specified durations */
		pDph_t->user_durs[n] = 0;
#endif
#ifdef SPANISH
		curr_indur = pDph_t->user_durs[n+offset];	/* User-specified durations */
		pDph_t->user_durs[n+offset] = 0;
#endif
		if (pDph_t->f0mode != HAT_F0_SIZES_SPECIFIED)
		{
			curr_inf0 = pDph_t->user_f0[n];		/* User-specified f0 commands */
			pDph_t->user_f0[n] = 0;	   /* that are locked to phones */
		}

		/* Skip allophone rules if current phoneme has feature +FBLOCK */
		if ((curr_instruc & FBLOCK) != 0)
		{
			goto skiprules;
		}
#if defined (GERMAN) || defined (SPANISH)
		/* eab second half of delaying rule 12/95 eab 
		 imporved 4/27/98 */
		if ( doitlater && (featb[curr_inph] & FSYLL) IS_PLUS)
		{
			count--;
			if (((pDph_t->nphonetot - n) < 4 && count ==1) || count == 0)
			{
				curr_outstruc |= FHAT_ENDS;
				hatposition = AT_BOTTOM_OF_HAT;
				stresses_in_phrase = 0;
				doitlater = 0;
			}
		}
		
#ifndef SPANISH
		switch (last_outph)
		{
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* make transition from I, IH into vowel more clear     				  */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* eab 9/95 new rule from sonia ch after a/o/u/au is changed to a kh */
		case A:
		case O:
		case U:
		case AU:
		case OH:
			if (curr_outph == CH)
			{
				curr_outph = KH;
			}
			break;

		case I:
		case IH:
			/* not correct per caroline*/
			if ((featb[curr_inph] & FVOWEL) IS_PLUS)
			{
			//	pDph_t->curr_insph = J;

#ifdef DBGALO
				fprintf (stderr, "\n(phalloph)ins %s between %s-%s(%x)\n",
						 phprint (curr_insph), phprint (last_outph),
						 phprint (curr_inph), featb[curr_inph]);
#endif
			} 
			break;
		}
		/* switch */

		switch (curr_inph)
		{


		case R:

			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* use /RR/ instead of /R/ before syllabic and eab 5/12/98 need to remove   
			   insertion of RR before glottal stop at a word boundary                 */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


		/*	if (featb[curr_inph] & FWBNEXT ) eab not working did not find fwbnext set 4 /9/98
			however don't see the need for the word boundary check eabprob*/
				if((featb[next_inph] & FSYLL) IS_PLUS)   
			/*		(next_inph == KH) || (next_inph == CH)) out per anaylsis with oliver*/
			{

				curr_outph = RR;

			}
			break;

		case L:
		case M:
		case N:
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
			/* use syllabic for /EL/, /EM/, /EN/ at high speaking rates      */
			/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

			if ((curr_instruc & (FSTRESS | FWINITC)) IS_PLUS )
			{
				break;
			}
			if (pDph_t->phonemes[n - 1] == EX && 
				featb[pDph_t->phonemes[n - 2]] & FNASAL IS_MINUS)
			{
				switch (curr_inph)
				{


				case L:
								
						pDph_t->allophons[pDph_t->nallotot - 1] = EL;
						delete = TRUE;
					
					break;



				case M:
									
						pDph_t->allophons[pDph_t->nallotot - 1] = EM;
						delete = TRUE;
					
					break;
				case N:
						pDph_t->allophons[pDph_t->nallotot - 1] = EN;
						delete = TRUE;
					
					break;
				}					   /* switch */

			}						   /* if last one was EX */
			break;
		}							   /* switch */

#endif /* #ifndef SPANISH */
#endif /* #ifdef GERMAN */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/* Morpho-phonemic Rules: (Most have already been applied)   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef ENGLISH_US
		/* Rule 1a: The word "the" should be /dh iy/ before a syllabic */
		if (((featb[pDph_t->phonemes[n + 1]] & FSYLL) IS_PLUS)
			&& (curr_inph == AX)
			&& ((curr_instruc & FBOUNDARY) IS_PLUS)
			&& (pDph_t->phonemes[n - 1] == DH)
			&& ((pDph_t->sentstruc[n - 1] & FWINITC) IS_PLUS))
		{
			curr_outph = IY;
		}
		/* rule 1c make a long a be ah */
/*		Cite_It= 0;

		if((pKsd_t->modeflag & MODE_CITATION) && pDph_t->docitation)to go fro
			Cite_It = 1;  */

		if ((curr_inph == AX) && (next_inph == SIL)
			&& n == 1 && (Cite_It) )
		{
			curr_outph = EY;
		}
//#ifndef NWS_US
		/* Rule 1b:  Unreduce vowel in "for" if following segment is vowel or sil */
		if ((curr_inph == F) && (next_inph == RR)
			&& ((((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS) && ((pDph_t->sentstruc[n +1 ] & FTYPESYL) == FMONOSYL))
				|| Cite_It))
		{
			if (((featb[pDph_t->phonemes[n + 2]] & FSYLL) IS_PLUS)
				|| (pDph_t->phonemes[n + 2] == SIL))
			{
				pDph_t->phonemes[n + 1] = OR;
				next_inph = OR;
#ifdef DEBUGALLO
				printf ("for rr->or rule firing");
#endif
			}
		}
//#else /*NWS_US*/
//		/* eab 10/15/97 */
//		if ((curr_inph == F) && (next_inph == RR))
//			if (((pDph_t->sentstruc[n +1 ] & 030/*FTYPESYL*/) == 00 /*FMONOSYL*/))
//			//	|| Cite_It))
//		{
//				pDph_t->phonemes[n + 1] = OR;
//				next_inph = OR;
//			
//		}
//#endif /*NOAA*/
		/* Rule 1c:  Unreduce vowel in clause-initial "and" to be [ae] */
		/* BATS 459 vowel for unreduce and wrong.... note-comment above was correct who changed it and why
		(it wasn't me) EAB 3/17/98*/

		if ((curr_inph == SIL)
			&& (pDph_t->phonemes[n + 1] == AE)
			&& (pDph_t->phonemes[n + 2] == N)
			&& (pDph_t->phonemes[n + 3] == D)
			&& ((((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS) && ((pDph_t->sentstruc[n + 3] & FSTRESS) IS_MINUS))
				|| Cite_It))
		{
#ifdef DEBUGALLO
			printf ("and eh->ae rule firing");
#endif
			pDph_t->phonemes[n + 1] = AE;
			next_inph = AE;
		}

		/* Rule 1d:  Unreduce vowel in "to,into" if next segment is vowel or sil */
		/* [See code below involving flapping rule] */

		/* Rule 1c:  Unreduce vowel in citation mode for  "at" to be [ae] */
		if ((curr_inph == SIL) && (pDph_t->phonemes[n + 1] == EH) && (pDph_t->phonemes[n + 2] == T)
			&& Cite_It )
		{
#ifdef DEBUGALLO
			printf ("at eh->ae rule firing");
#endif
				
			pDph_t->phonemes[n + 1] = AE;
			next_inph = AE;
		}

		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
		/* Phonological rules I:                                     	   */
		/* (rules involving glottal attack are in PHDRAWT0.C)      		   */
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

		/* Rule 2: Postvocalic allophones of /R/ and /LL/ are special */

		if (((curr_instruc & (FSTRESS | FWINITC)) IS_MINUS)
			&& ((featb[pDph_t->phonemes[n - 1]] & FVOWEL) IS_PLUS))
		{
			if (curr_inph == LL)
				curr_outph = LX;

			/* See if one of the special vowel + R combinations */
			/* if so, replace last output symbol and discard input symbol */
			if (curr_inph == R)
			{
				curr_outph = RX;
				symlas = pDph_t->phonemes[n - 1];
				if (symlas == AX)
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = RR;
					delete = TRUE;
				}

				if ((symlas == IY) || (symlas == IH))
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = IR;
					delete = TRUE;
				}

				if ((symlas == EY)
					|| (symlas == EH) || (symlas == AE))
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = ER;
					delete = TRUE;
				}

				if ((symlas == AA) || (symlas == AH))
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = AR;
					delete = TRUE;
				}

				if ((symlas == OW) || (symlas == AO))
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = OR;
					delete = TRUE;
				}

				if ((symlas == UW) || (symlas == UH))
				{
					pDph_t->allophons[pDph_t->nallotot - 1] = UR;
					delete = TRUE;
				}
			}
		}

		/* Rule 3: Select appropriate allophones for unstressed /t/ and /d/ */

		/* Palatalize /t/ or /d/ if next phoneme is unstressed /y/ */
		if (((next_inph == YU) || (next_inph == Y))
			&& ((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS))
		{
			if (curr_inph == T)
			{
				curr_outph = CH;
				goto endrul3;
			}
			if (curr_inph == D)
			{
				curr_outph = JH;
				goto endrul3;
			}
		}
		/* 
		 * Rule to correct mis-transcriptions of e.g. "chew" as [chyu] 
		 * OUT 23-Aug-85 because applied inappropriately to "average users" 
		 * if ((last_outph == CH) || (last_outph == JH)) { if (curr_inph == Y) delete = 
 		 * TRUE; if (curr_inph == YU)    curr_outph = UW; } END OUT 
 		 */

		/* Glottalize word-final /t/ before a word-init sonor cons or /dh/ */
		if (curr_inph == T)
		{
			if ((next_inph == LL) || (next_inph == DH)
				|| ((((curr_instruc & FBOUNDARY) >= FMBNEXT)
					 && (((featb[next_inph] & FSON2) IS_PLUS)
						 || (next_inph == HX)))
					|| (next_inph == EN)))
			{
				curr_outph = D;
				if ((featb[last_outph] & FSON1) IS_PLUS)
				{
					curr_outph = TX;
				}
				goto endrul3;
			}
				/* Unreduce the O in "to" if following segment is a vowel */
#ifdef NWS_US
			if (next_inph == UH)
			{
				pDph_t->phonemes[n + 1] = UW;	
			}
#endif

			if ((next_inph == UH) &&
				(((curr_instruc & FSTRESS) IS_MINUS) ||
				 (Cite_It)))
			{
				if (((featb[pDph_t->phonemes[n + 2]] & FSYLL) IS_PLUS) ||
					(pDph_t->phonemes[n + 2] == SIL))
				{
					pDph_t->phonemes[n + 1] = UW;
				}
			
				/* And flap the initial /t/ of 'to' if preceeding syllabic */
				else if ((Cite_It) == 0 && ((featb[last_outph] & FSYLL) IS_PLUS)
						 && ((featb[last_outph] & FNASAL) IS_MINUS))
				{
#ifndef NWS_US
					curr_outph = DF;
					goto endrul3;
#endif
				}
			}
		}
#ifndef NWS_US
		/* Flapping rule:  for non-stressed /t/ and /d/ */
		/* (can't flap [t] in [nt] until after durs computed) */
		if (((curr_inph == D) || (curr_inph == T))
			&& ((curr_instruc & FSTRESS) IS_MINUS))
		{
			/* Consonant must be preceded by vowel,rx,en,G,el */
			if (((featb[last_outph] & FSON1) IS_PLUS)
				&& (last_outph != M)
				&& (last_outph != NX)
				&& ((last_outph != N))
				/* And followed by a syllabic */
				&& ((featb[next_inph] & FSYLL) IS_PLUS))
			{
				/* Flap if consonant is word-final */
				if ((curr_instruc & FBOUNDARY) >= FMBNEXT)
				{

					if(curr_inph == T)
					{
						curr_outph = DF;

					}
					else
						curr_outph = DX;

				}

				/* Flap word-initial /t,d/ if next vowel is reduced */
				else if ((curr_instruc & FWINITC) IS_PLUS)
				{
					if ((next_inph == AX) || (next_inph == IX))
					{
						if (curr_inph == T)
						{
							curr_outph = DF;

						}
						else
							curr_outph = DX;

					}
				}
				/* 
 				 * Flap if [t,d] is word-internal and 
 				 * (1) next vowel is "weak", or 
 				 * (2) previous vowel stressed and next vowel is
 				 * [ow] 
 				 */
				else if((((pDph_t->allofeats[pDph_t->nallotot - 1] & FSTRESS) IS_PLUS)
						  && (next_inph == OW))
						 || (next_inph == AX)
						 || (next_inph == RR)
						 || (next_inph == IY)
						 || (next_inph == IX)
						 || (next_inph == EL))
				{
					if (curr_inph == T)
					{
						curr_outph = DF;

					}
					else
						curr_outph = DX;

				}

				/* 
				 *  HISTORY of the flap rule:  It used to work well without the prior stress 
				 *  alternative, except for words with a [ow] following the [t]. 
				 *  It is my hunch that the safest course at this late date is to go back 
				 *  to the requirement that the next vowel be [ow] when triggering the 
				 *  rule by a prior stress. (Problem words: 
				 *   "photo, tomato, potato, monotone, protest, detail, detour, pretense" 
 			     */
			} /* if (((featb[last_outph] & FSON1) IS_PLUS) */
		} /* if (((curr_inph == D) || (curr_inph == T)) */
#endif /* NWS_US */
		/* Rule 4: Unstressed [dh] becomes dental stop after [t,d], nasal after [n] */

		if ((curr_inph == DH)
			&& ((curr_instruc & FSTRESS) IS_MINUS))
		{
			if ((last_outph == T)
				|| (last_outph == TX)
				|| (last_outph == D))
			{
				curr_outph = DZ;
			}
			if (last_outph == N)
			{
				curr_outph = N;
			}
		}

#ifdef  NEVER_USED
		/* Rule 5: Delete unstressed [hx] between two voiced segments */

		if ((curr_inph == HX)
			&& ((featb[last_outph] & FVOICD) IS_PLUS)
			&& ((curr_instruc & FSTRESS) IS_MINUS))
		{
			delete = TRUE;
		}
		/* eab latin mode rule */
		if ((pKsd_t->modeflag & MODE_LATIN) && (curr_inph == TH))
			curr_inph = E_S;
#endif

		/* Rule 6: Determine locations of hat pattern rises and falls */
#endif	/* #ifdef ENGLISH */
		
#ifdef ENGLISH_UK
		/* Rule 1a: The word "the" should be /dh iy/ before a syllabic */
		if (((featb[pDph_t->phonemes[n + 1]] & FSYLL) IS_PLUS)
			&& (curr_inph == AX)
			&& ((curr_instruc & FBOUNDARY) IS_PLUS)
			&& (pDph_t->phonemes[n - 1] == DH)
			&& ((pDph_t->sentstruc[n - 1] & FWINITC) IS_PLUS))
		{
			curr_outph = IY;
		}
		/* rule 1c make a long a be ah */
		Cite_It= 0;
		Cite_It = ((pKsd_t->modeflag & MODE_CITATION) && pDph_t->docitation);
		if (pKsd_t->sprate < 100)
			Cite_It=TRUE;
		if ((curr_inph == AX) && (next_inph == SIL)
			&& n == 1 && (Cite_It) )
		{
			curr_outph = EY;
		}

		/* Rule 1b:  Unreduce vowel in "for" if following segment is vowel or sil */

		if ((curr_inph == F) && (next_inph == AX)
			&& ((((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS) && ((pDph_t->sentstruc[n + 1] & FTYPESYL) == FMONOSYL))
				|| Cite_It))
		{
			if (((featb[pDph_t->phonemes[n + 2]] & FSYLL) IS_PLUS)
				|| (pDph_t->phonemes[n + 2] == SIL))
			{
				pDph_t->phonemes[n + 1] = AO;
				next_inph = AO;
#ifdef DEBUGALLO
				printf ("for rr->or rule firing");
#endif
			}
		}

		/* Rule 1c:  Reduce vowel in clause-initial "and" to be  */

		if (curr_inph == SIL)
		
			if(pDph_t->phonemes[n + 1] == AX)
			if (pDph_t->phonemes[n + 2] == N)
			if (pDph_t->phonemes[n + 3] == D)
			if ((((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS) && ((pDph_t->sentstruc[n + 3] & FSTRESS) IS_MINUS))
				|| Cite_It)
			{
#ifdef DEBUGALLO
			printf ("and eh->ae rule firing");
#endif
			pDph_t->phonemes[n + 1] = AE;
			next_inph = AE;
		}

		/* Rule 1d:  Unreduce vowel in "to,into" if next segment is vowel or sil */
		
	if ((curr_inph == T) && (next_inph == AX)
			&& ((((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS) && ((pDph_t->sentstruc[n + 1] & FTYPESYL) == FMONOSYL))
				|| Cite_It))
		{
			if (((featb[pDph_t->phonemes[n + 2]] & FSYLL) IS_PLUS)
				|| (pDph_t->phonemes[n + 2] == SIL))
			{
				pDph_t->phonemes[n + 1] = UW;
				next_inph = UW;

			}
		}

		


		

		/* Rule 1c:  Unreduce vowel in citation mode for  "at" to be [ae] */
		if ((curr_inph == SIL) && (pDph_t->phonemes[n + 1] == AX) && (pDph_t->phonemes[n + 2] == T)
			&& Cite_It )
		{
#ifdef DEBUGALLO
			printf ("at eh->ae rule firing");
#endif
				
			pDph_t->phonemes[n + 1] = AE;
			next_inph = AE;
		}

		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
		/* Phonological rules I:                                     	   */
		/* (rules involving glottal attack are in PHDRAWT0.C)      		   */
		/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

		/* Rule 2: Postvocalic allophones of /R/ and /LL/ are special */

		if (((curr_instruc & (FSTRESS | FWINITC)) IS_MINUS)
			&& ((featb[pDph_t->phonemes[n - 1]] & FVOWEL) IS_PLUS))
		{
			if (curr_inph == LL)
				curr_outph = LX;

		}

		/* Rule 3: Select appropriate allophones for unstressed /t/ and /d/ */

		/* Palatalize /t/ or /d/ if next phoneme is unstressed /y/ */
		if (((next_inph == YU) || (next_inph == Y))
			&& ((pDph_t->sentstruc[n + 1] & FSTRESS) IS_MINUS))
		{
			if (curr_inph == T)
			{
				curr_outph = CH;
				goto endrul3;
			}
			if (curr_inph == D)
			{
				curr_outph = JH;
				goto endrul3;
			}
		}
		/* 
		 * Rule to correct mis-transcriptions of e.g. "chew" as [chyu] 
		 * OUT 23-Aug-85 because applied inappropriately to "average users" 
		 * if ((last_outph == CH) || (last_outph == JH)) { if (curr_inph == Y) delete = 
 		 * TRUE; if (curr_inph == YU)    curr_outph = UW; } END OUT 
 		 */

		/* Glottalize word-final /t/ before a word-init sonor cons or /dh/ */
		/* eab 11/21/97 modified per caroline so that tx only occurs before
		the nasal en and ll ditto for d->dz but it needs to be added harden leaden*/
		if (curr_inph == T )
		{
			if ((next_inph == EL) 
				&& ((((curr_instruc & FBOUNDARY) >= FMBNEXT)
					 && (((featb[next_inph] & FSON2) IS_PLUS)))
					|| (next_inph == EN)))
			{
				
				if ((featb[last_outph] & FSON1) IS_PLUS)
					curr_outph = TX;
				
				goto endrul3;
			}
			/* Unreduce the O in "to" if following segment is a vowel */
#ifndef NWS_US /* Leave in as it may change from noaa to announce_mode*/
			if((next_inph == UH) && Cite_It)
				pDph_t->phonemes[n + 1] = UW;
#else
			if(next_inph == UH )
				pDph_t->phonemes[n + 1] = UW;
#endif
			if ((next_inph == UH) &&
				((curr_instruc & FSTRESS) IS_MINUS))
			{
				if (((featb[pDph_t->phonemes[n + 2]] & FSYLL) IS_PLUS) ||
					(pDph_t->phonemes[n + 2] == SIL))
				{
					pDph_t->phonemes[n + 1] = UW;
				}
				
#ifndef NWS_US
					curr_outph = DF;
					goto endrul3;
#endif
				
			}
		}

		/* Rule 4: Unstressed [dh] becomes dental stop after [t,d], nasal after [n] */

		if ((curr_inph == DH)
			&& ((curr_instruc & FSTRESS) IS_MINUS))
		{
			if ((last_outph == T)
				|| (last_outph == TX)
				|| (last_outph == D))
			{
				curr_outph = DZ;
			}
			if (last_outph == N)
			{
				curr_outph = N;
			}
		}

#ifdef     NEVER
		/* Rule 5: Delete unstressed [hx] between two voiced segments */

		if ((curr_inph == HX)
			&& ((featb[last_outph] & FVOICD) IS_PLUS)
			&& ((curr_instruc & FSTRESS) IS_MINUS))
		{
			delete = TRUE;
		}
		/* eab latin mode rule */
		
#endif

		/* Rule 6: Determine locations of hat pattern rises and falls */
#endif	/* #ifdef ENGLISH */
      endrul3:
#ifdef SPANISH
		if ((pKsd_t->modeflag & MODE_LATIN) && curr_inph == E_TH)
		{
			curr_outph = E_S;
		}

		/* Rule 1: Remove geminates and combine dipthongs.   
		Here just check that we have a vowel contendor for dipth */
		if ((featb[curr_inph] & (FVOWEL | FSEMIV)) IS_MINUS
			/*|| (featb[last_outph] & (FVOWEL | FSEMIV)) IS_MINUS*/)
			goto nodipth;			   /* Can't be a dipthong          */
		else if ((curr_instruc & FSBOUND) IS_PLUS)
		{
			goto nodipth;
		}
		else if (curr_inph == last_outph)
		{
		}
		/* 
		 * The test for [nallotot-2] is to properly position
		 * the dipthong in phrases such as "uno u otro"->['uno wx'otro]
		 EAb 10/29/98 Doesn't check that preceding phoneme was a vowel causing
		 inappropriate changes But since the rules seem to handle the shortening and combination
		 by thems selves the semi-vowel seems to cause more trouble than it's worth it will be verified
		 by Juan it was already removed from another place in the code 
		 */
		else if ((curr_outstruc & FSTRESS) IS_PLUS
				 && (pDph_t->allofeats[pDph_t->nallotot - 1] & FBOUNDARY) >= FWBNEXT
				 && (pDph_t->allofeats[pDph_t->nallotot - 2] & FBOUNDARY) < FWBNEXT)
		{
		}
#ifdef SPANISH_OUT
		else if ((pDph_t->allofeats[pDph_t->nallotot - 1] & FSTRESS) IS_MINUS)
			if(/*
				 &&*/ (featb[last_outph] & FDIPTH) IS_PLUS)
		{
			/* 
			 * rising dipthong (semivowel-vowel): i a -> yx a, etc.
			 */
			switch (last_outph)
			{
			case E_I:
				last_outph = E_YX;
				break;
			case E_U:
				break;
				//last_outph = E_WX; 
				//let's try not using this semovowel and just use the vowel per JUAN
				//	eab sep 28,1998
				break;
			default:
				goto nodipth;
			}
			pDph_t->allophons[pDph_t->nallotot - 1] = last_outph;
			pDph_t->allofeats[pDph_t->nallotot - 1] &= ~FBOUNDARY;
		}
#endif
#ifdef SPANISH_OUT
		else if ((curr_outstruc & FSTRESS) IS_MINUS
				 && (featb[curr_inph] & FDIPTH) IS_PLUS)
		{
			/* 
			 * First, check whether we should defer dipthongization
			 * to the following segment: "uno u otro" -> ['uno wx'otro]
			 */
			if ((featb[next_inph] & (FVOWEL | FDIPTH)) == FVOWEL
				&& (pDph_t->sentstruc[n + 1] & FSBOUND) IS_MINUS)
			{
				goto nodipth;
			}
			/* 
			 * Falling dipthong (vowel-semivowel): ou -> owx
			 */
			switch (curr_inph)
			{
			case E_I:
				curr_outph = E_YX;
				break;
			case E_U:
				//last_outph = E_WX; 
				//let's try not using this semovowel and just use the vowel per JUAN
				//	eab sep 28,1998
				//curr_outph = E_WX;
				break;
			default:
				goto nodipth;
			}
			/* allofeats[nallotot-1] &= ~FBOUNDARY; */
		}							   /* geminate/dipthong    */
#endif
	  nodipth:;
#endif 	/* #ifdef SPANISH */	  	
	  	if (curr_inph == SIL)
		{
			emphasislock = FALSE;
		}

		/* Locate a stressed syllabic */
#ifdef GERMAN
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
		/* masking with FSTRESS and FSTRESS_2 also takes S3 into account  		  */
		/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
#endif
		if ((pDph_t->f0mode == NORMAL)
			&& ((featb[curr_inph] & FSYLL) IS_PLUS)
			&& ((curr_instruc & FSTRESS) IS_PLUS)	/* 1-str 2-str emph */  /* S1 S2 S3 SENPH in GERMAN */
			&& (emphasislock == FALSE))
		{

			/* Rise occurs on first stress of any type in phrase */
			/* (If curr stress is not a primary stress, routine  */
			/* checks if there will be a primary stress to fall on later) */

			if ((hatposition != AT_TOP_OF_HAT)
				&& (((curr_instruc & FSTRESS_1) IS_PLUS)
#ifdef ENGLISH
					|| (remaining_stresses_til (pDph_t, n, FCBNEXT) > 0)))
#endif
#ifdef GERMAN
					|| (remaining_stresses_til (pDph_t, n, FCBNEXT) > 0)))
#endif
#ifdef SPANISH
					|| (remaining_stresses_til (pDph_t, n, FCBNEXT) > 1)))
				 /* || (remaining_stresses_til(n,FCBNEXT) > 0) ))  ignore first stress experiment by recommend of Anna Colbert */
#endif
			{

				curr_outstruc |= FHAT_BEGINS;

				hatposition = AT_TOP_OF_HAT;
			}

			/* Count number of primary stresses */
			if ((curr_instruc & FSTRESS_1) IS_PLUS)
			{
				stresses_in_phrase++;
			}

			/*
			 * Fall occurs 
			 * 	(1) on any emphasized syll, 
			 *  (2) on last 1 stress of clause, 
			 *  (3) on last 1 stress of phrase containing 2 or more stresses 
			 *  followed by phrase with 2 or more stresses 
			 */
			if ((hatposition == AT_TOP_OF_HAT)
				&& ((curr_instruc & FSTRESS_1) IS_PLUS))
			{						   /* 1-str emph */
				/* Emphasis: prevent any more hat rises until end of clause */
				if ((curr_instruc & FEMPHASIS) == FEMPHASIS)
				{
					emphasislock = TRUE;
				}
#ifndef NWS_US
				/* eab 10/19/97 This isn't working correct for embeeded amphasis commands
				and effectly cancels them I'll figure out how it "suppose" towork later*/
				/* Fall now if emphasis */
				if (emphasislock == TRUE)
				{
					curr_outstruc |= FHAT_ENDS;
					hatposition = AT_BOTTOM_OF_HAT;
					stresses_in_phrase = 0;
				}
#endif /*NWS_US*/

				/* Fall now if last stress in clause */
#ifdef GERMAN
				/* But in German we can have many unstressed vowels left so we don't want to do this too soon or it will sound real bad eab 12/95 so
				 * if we put it off set flag to do it later */
/*				tmp = remaining_stresses_til (pDph_t,n, FCBNEXT);            */ /* useless value is never used */
#endif
				if ((remaining_stresses_til (pDph_t, n, FCBNEXT) == 0)
		
#if defined (GERMAN) || defined (SPANISH)   
					|| doitlater
#endif
				)
				{
					/* 
					 * First check if this is phrase boundary and there exist 
					 * secondary stresses in the remaining phrasal fragment   
					 */
#if defined (ENGLISH) || defined (SPANISH) || defined (GERMAN)
					if (((curr_instruc & FBOUNDARY) == FVPNEXT)
						&& (promote_last_2 (pDph_t, n)))
					{
						/* Last secondary stress of next phrase promoted */
					}
#endif
/* doitlater suffers from the problem of unknown deletes so you don't really know how far from the end you really are*/
#if defined (GERMAN) || defined (SPANISH) 
					if(pDph_t->clausetype == DECLARATIVE)
					{
					if ((pDph_t->nphonetot - n) > 4)
					{
						count=0;
						for (m = n+1; m < pDph_t->nphonetot; m++)
						{
							if ((featb[pDph_t->phonemes[m]] & FSYLL) IS_PLUS)
							{
								count++;
								
							}
						}
						if(count >=2)
						{
							doitlater = 1;
						}
						else
						{
							curr_outstruc |= FHAT_ENDS;
							hatposition = AT_BOTTOM_OF_HAT;
							stresses_in_phrase = 0;
							doitlater=0;
						}
					}
#endif
					else   

					{
						curr_outstruc |= FHAT_ENDS;
						hatposition = AT_BOTTOM_OF_HAT;
						stresses_in_phrase = 0;
#if defined (GERMAN) || defined (SPANISH)
						doitlater=0;
#endif
					}
#if defined (GERMAN) || defined (SPANISH) 
					}/* clausetype if closure */
#endif
				
				}
				/* Fall now if last str in phrase and both phrases have 2 str */
				if ((stresses_in_phrase > 1)
					&& (remaining_stresses_til (pDph_t, n, FVPNEXT) == 0)
					&& (remaining_stresses_til (pDph_t, n, FCBNEXT) > 1))
				{
					curr_outstruc |= FHAT_ENDS;
					hatposition = AT_BOTTOM_OF_HAT;
					stresses_in_phrase = 0;
				}
			}
		}

	  skiprules:
		if (delete)
		{
#if defined ENGLISH || defined SPANISH || defined GERMAN
			ph_delcnt++;
#ifdef DEBUGALLO

			printf ("adjusting in alloph,n= %d n+del=%d  \n", n, n + ph_delcnt);

#endif
#ifdef MSDOS
				adjust_allo (n + ph_delcnt, -1);
#else
				adjust_allo (pKsd_t, n + ph_delcnt, -1);
#endif

			if (curr_indur != 0)
			{
#ifdef DEBUG_USER_PROSODICS
				printf (
						   "\t  Delete [%s], add dur=%d ms to that of previous phone [%s]\n",
						   phprint (curr_inph), curr_indur, phprint (curr_outph));
				pDph_t->user_durs[pDph_t->nallotot - 1] += curr_indur;
#endif
			}
#endif	/* #if defined ENGLISH || defined SPANISH */
#ifdef GERMAN
			pDph_t->nphonetot--;
#endif
			if (curr_inf0 != 0)
				pDph_t->user_f0[pDph_t->nallotot - 1] = curr_inf0;
			delete = FALSE;
		}

		else
		{
			/* 
			 * if (curr_inph != curr_outph) { printf("changed phonene %d from %d to %d\n",
			 * n,curr_inph,curr_outph); } 
			 */
			make_out_phonol (phTTS, n, curr_outph, curr_outstruc, curr_indur, curr_inf0);		
						/* Save curr_outph in allophons[] */
#ifdef SPANISH
			if (curr_outph == E_NH &&  /* Realize [nh] as      */
				(curr_instruc & FBLOCK) == 0)
			{
				curr_outph = E_IX;	   /* as [nh ix].          */
				curr_outstruc &= ~FSBOUND;	/* No syllable now      */
				/* 
				 * eab 2/19/95 UGLY BUG Userd durs reused so all userdurs get broken without
				 * this code to realign values after an insert
				 */
				for (temp = pDph_t->nphonetot; temp >= pDph_t->nallotot; temp--)

				{
					pDph_t->user_durs[temp + 1] = pDph_t->user_durs[temp];
					/* printf("correc %d at %d \n",user_durs[temp],temp); */
					/* printf("correc++ %d at %d \n",user_durs[temp+1],temp+1); */
				}
				/* 
				 * shifting array by different index so need to keep track of ugly 
				 * offsets so we don't get lost. 
				 */
				offset++; 
				pDph_t->user_durs[pDph_t->nallotot - 1] = 0;
				make_out_phonol (phTTS, n, curr_outph, curr_outstruc, curr_indur, curr_inf0);
			}		
#endif


		}
	}

	/* 
	 * After string blocked from Rule 4 via the f0mode=HAT_LOCATIONS_SPECIFIED 
	 * state variable, set back to normal 
	 */
	if(pDph_t->f0mode == HAT_LOCATIONS_SPECIFIED)
	{
		pDph_t->f0mode = NORMAL;
	}

	/* Zap last position in array */
	pDph_t->allophons[pDph_t->nallotot] = SIL;
	pDph_t->allofeats[pDph_t->nallotot] = 0; 
#ifdef SPANISH
	pDph_t->user_durs[pDph_t->nallotot] = 0;
#endif

	/* Print summary of output results   */
	prphonol ();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
                              
/*
 *      Function Name: remaining_stresses_til()      
 *
 *  	Description: Process rest of input string from position arg1, 
 *					 count primary-stressed vowels, return when encounter 
 * 					 feature arg2
 *
 *      Arguments:	PDPH_T pDph_t, 
 *					short msym, 
 *					short b_type 
 *
 *      Return Value: short
 *
 *      Comments:
 *
 */
static short remaining_stresses_til (PDPH_T pDph_t, short msym, short b_type)
{

	register int m, count;
#ifndef ENGLISH_UK
	/* EAB 1/21/98 This was propbably  wrong for all languages but needs to be verified 
	before enabling for all languages  i.e enabling secondary stress*/
	count = 0;
	for (m = msym; m < pDph_t->nphonetot; m++)
	{

		if ((m != msym)
			&& ((pDph_t->sentstruc[m] & FSTRESS_1) IS_PLUS)
			&& ((featb[pDph_t->phonemes[m]] & FSYLL) IS_PLUS))
		{
			count++;
		}
		if (((pDph_t->sentstruc[m] & FBOUNDARY) >= b_type)
			|| (pDph_t->phonemes[m] == SIL))
		{
			return (count);
		}
	}
	return (count);
#else
		count = 0;
	for (m = msym; m < pDph_t->nphonetot; m++)
	{

		if ((m != msym)
			&& ((pDph_t->sentstruc[m] & FSTRESS) IS_PLUS)
			&& ((featb[pDph_t->phonemes[m]] & FSYLL) IS_PLUS))
		{
			count++;
		}
		if (((pDph_t->sentstruc[m] & FBOUNDARY) >= b_type)
			|| (pDph_t->phonemes[m] == SIL))
		{
			return (count);
		}
	}
	return (count);

#endif

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 *      Function Name: promote_last_2()      
 *
 *  	Description: Promote secondary stress in phrases like "J'ohn ) is h`im", 
 *					 but not in phrases like "J'ohn ) s'aw h`im".
 *
 *      Arguments:	PDPH_T pDph_t, 
 *					short msym 
 *
 *      Return Value: short
 *
 *      Comments:
 *
 */
static short promote_last_2 (PDPH_T pDph_t, short msym)
{

	short done_it, m;

	done_it = 0;
	for (m = msym; m < pDph_t->nphonetot; m++)
	{

		if ((m != msym)
#if defined ENGLISH || defined SPANISH
			&& ((pDph_t->sentstruc[m] & FSTRESS) == FSTRESS_2)
#endif
#ifdef GERMAN
			&& ((pDph_t->sentstruc[m] & FIFSTRESS) == FSTRESS_2)
#endif
			&& ((featb[pDph_t->phonemes[m]] & FSYLL) IS_PLUS))
		{
			done_it = m;			   /* pointer to last secondary stress */
		}
		if (((pDph_t->sentstruc[m] & FBOUNDARY) >= FCBNEXT)
			|| (pDph_t->phonemes[m] == SIL))
		{
			if (done_it != 0)
			{
				pDph_t->sentstruc[done_it] &= ~FSTRESS_2;
				pDph_t->sentstruc[done_it] |= FSTRESS_1;
#ifdef MSDBG7
					printf ("sentstruc[done_it] |= FSTRESS_1 sentstruc[%d]=%d\n", done_it, pDph_t->sentstruc[done_it]);
#endif
				return (TRUE);
			}
		}
	}
	return (FALSE);
}

/*
 *      Function Name: make_out_phonol()      
 *
 *  	Description: 
 *
 *      Arguments: 	LPTTS_HANDLE_T phTTS, 
 *					short n,
 *				 	short curr_outph, 
 *					short curr_outstruc,
 *				 	short curr_indur, 
 *					short curr_inf0
 *
 *      Return Value: void
 *
 *      Comments:
 *
 */
static void make_out_phonol (LPTTS_HANDLE_T phTTS, short n,
				 short curr_outph, U32 curr_outstruc,
				 short curr_indur, short curr_inf0)
{
	PKSD_T                  pKsd_t = phTTS->pKernelShareData;
	PDPH_T                  pDph_t = phTTS->pPHThreadData;

#ifdef DEBUGALLO
	struct spc_packet _far *spc_pkt;   /* debug eab */

#endif                               

    /*printf("nallotot %d n %d curr_outph %d curr_outstruc %d curr_indur %d curr_inf0 %d\n",pDph_t->nallotot,n,curr_outph,curr_outstruc,curr_indur,curr_inf0); */
#ifdef MSDOS
	set_index_allo (n, pDph_t->nallotot);	/* At minimum do set_index eab */
#else
	set_index_allo (pKsd_t, n, pDph_t->nallotot);	/* At minimum do set_index eab */
#endif

#ifdef DEBUGALLO

	if ((spc_pkt = (struct spc_packet _far *) pKsd_t->index_pending.head) != NULL_SPC_PACKET)
	{
		printf ("n = %d pDph_t->nallotot = %d\n", n, pDph_t->nallotot);
		if (n >= pDph_t->nallotot)
		{
			while (spc_pkt != NULL_SPC_PACKET)
			{
				WAIT_PRINT;
				printf ("1 index %d = symbol %d \n", (*spc_pkt).data[0], pDph_t->symbols[(*spc_pkt).data[0]]);
				printf ("2 index %d = symbol %d \n", (*spc_pkt).data[4], pDph_t->phonemes[(*spc_pkt).data[4]]);
				printf ("3 index %d = symbol %d \n", (*spc_pkt).data[5], pDph_t->allophons[(*spc_pkt).data[5]]);
				SIGNAL_PRINT;
				spc_pkt = (struct spc_packet _far *) (*spc_pkt).link;
			}
		}
	}								   /* debug eab  */
#endif
#ifdef GERMAN
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* insert curr_insph with characteristics of curr_outph      			  */
	/* make sure that inserted phonemes have left enough space       		  */
	/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	if (pDph_t->curr_insph >= SIL)
	{
		if (pDph_t->nallotot >= NPHON_MAX)
		{

			return;
		}
		pDph_t->allophons[pDph_t->nallotot] = pDph_t->curr_insph;
		pDph_t->allofeats[pDph_t->nallotot] = curr_outstruc;
		pDph_t->user_durs[pDph_t->nallotot] = curr_indur;
		if (pDph_t->f0mode != HAT_F0_SIZES_SPECIFIED)
		{
			pDph_t->user_f0[pDph_t->nallotot] = curr_inf0;
		}

		pDph_t->nallotot++;
	}
#endif
#ifdef ENGLISH
	if ((curr_outph < 0) || (curr_outph > TOT_ALLOPHONES))
#endif
#if defined (GERMAN) || defined (SPANISH)
	if ((curr_outph < 0) || (curr_outph >= TOT_ALLOPHONES)) /* spanish and german  */
#endif
	{
#ifdef DEBUGALLO
		printf ("Error outputing phoneme \n");
		printf ("outputting phoneme %d\n ", curr_outph);
		return;
#endif
	}
#ifdef DEBUGALLO
	printf ("outputting phoneme %d\n ", curr_outph);
#endif

	if (pDph_t->nallotot > (n + 8))
	{


		return;
	}
	/* Put phoneme away */

	pDph_t->allophons[pDph_t->nallotot] = curr_outph;
	pDph_t->allofeats[pDph_t->nallotot] = curr_outstruc;
	/* printf("hah %d %d %d  \n",user_durs[nallotot],curr_indur,nallotot); */
#ifdef MSDBG6
		printf ("allofeats[nallotot] allofeats[%d]=%d\n", pDph_t->nallotot, pDph_t->allofeats[pDph_t->nallotot]);
#endif
	pDph_t->user_durs[pDph_t->nallotot] = curr_indur;
	if (pDph_t->f0mode != HAT_F0_SIZES_SPECIFIED)
	{
		pDph_t->user_f0[pDph_t->nallotot] = curr_inf0;	/* Move user_f0 if phone moved */
	}

#ifdef DEBUG_USER_PROSODICS
	if (curr_indur != 0)
	{
		printf ("\tFound user_dur[%s] = %3d ms in PHALLOPH\n",
				phprint (curr_outph), curr_indur);
	}
	if ((curr_inf0 != 0) && (pDph_t->f0mode != HAT_F0_SIZES_SPECIFIED))
	{
		printf ("\tFound  user_f0[%s] = %3d Hz in PHALLOPH, f0mode = %d\n",
				phprint (curr_outph), curr_inf0, pDph_t->f0mode);
	}
#endif

	/* See if there is room for next phoneme */
	if (pDph_t->nallotot < NPHON_MAX)
	{
		pDph_t->nallotot++;
	}


}
 
/*
 *      Function Name: prphonol()      
 *
 *  	Description: 
 *
 *      Arguments: 
 *
 *      Return Value: 
 *
 *      Comments:
 *
 */
void prphonol ()
{

}
/*********************************end of phalloph.c******************************************/
