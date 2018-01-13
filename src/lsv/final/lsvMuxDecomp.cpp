/**CFile****************************************************************

  FileName    [lsvMuxDecomp.cpp]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [lsv: Logic Synthesis and Verification PA.]

  Synopsis    [procedure for .]

  Author      []
  
  Affiliation [NTU]

  Date        [17, Sep., 2017.]

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#ifdef TEST
#include "Test/LsvMuxDeomp/testLsvMuxDecomp.h"
#else
#include "base/main/mainInt.h"
#endif
#include "lsvMuxDecomp.h"

#ifndef TEST
ABC_NAMESPACE_IMPL_START
#endif

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

enum NodeValue
{
  value_ee = 8, // 1000
  value_et = 4, // 0100
  value_te = 2, // 0010
  value_tt = 1  // 0001
};

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////
 
/**Function*************************************************************

  Synopsis    []

  Description [return the decomposed fs, fa, fb. the memory should be managed by the caller.]

  SideEffects []

  SeeAlso     []

***********************************************************************/
Vec_Ptr_t* muxDecompCore( DdManager *dd, DdNode *f )
{
  // variables declarations
  Vec_Ptr_t *pFunc  = Vec_PtrAlloc( func_num );
  DdNode    *topVar = NULL;
  // end variables declarations

  // initialize current functions
  Vec_PtrPush( pFunc, Cudd_bddIthVar( dd, Cudd_NodeReadIndex( f ) ) );  // fs
  Vec_PtrPush( pFunc, Cudd_E( f )                                   );  // fa
  Vec_PtrPush( pFunc, Cudd_T( f )                                   );  // fb
  // end initialize current functions
  int i=0;
  topVar = Cudd_bddIthVar( dd, Cudd_ReadSize( dd ) - 1 );
  int order_array[Cudd_ReadSize(dd)];
  order_array[0] = Cudd_ReadSize( dd ) - 1;
  for (int i=1; i<Cudd_ReadSize( dd ); ++i)
  {
	  order_array[i] = i-1;
  }
  Cudd_ShuffleHeap( dd, order_array);
  while( isMuxDecomp( dd, f ) )
  {
	  printf("enter!\n");
    // variables declarations
	if ( Cudd_IsConstant( f ) )	break;
	Cudd_ShuffleHeap( dd, order_array);
    DdNode *fs = buildS( dd, f, ( DdNode* )Vec_PtrEntry( pFunc, func_s ) );
    if( !fs ) break; 
    DdNode *fa = CheckChild( dd,  Cudd_E(f), 0); // 00
    DdNode *fb = NULL;
    // end variables declarations

    // find fb
    if      ( CheckChild( dd,  Cudd_E( f ) , 1) != fa ) fb = CheckChild( dd,  Cudd_E( f ) , 1)	; // 01
    else if ( CheckChild( dd,  Cudd_T( f ) , 0) != fa ) fb = CheckChild( dd,  Cudd_T( f ) , 0)	; // 10
    else if ( CheckChild( dd,  Cudd_T( f ) , 1) != fa ) fb = CheckChild( dd,  Cudd_T( f ) , 1)	; // 11
	// end find fb
/*
    // deal the reference
    Cudd_Ref            ( fs    );
    Cudd_Ref            ( fa    );
    Cudd_Ref            ( fb    );
    Cudd_RecursiveDeref ( dd, f );
    // end deal the reference
*/
    // set current functions
    Vec_PtrWriteEntry( pFunc, func_s, fs );
    Vec_PtrWriteEntry( pFunc, func_a, fa );
    Vec_PtrWriteEntry( pFunc, func_b, fb );
    // end set current functions

    f = Cudd_bddOr( dd, Cudd_bddAnd( dd, fa, Cudd_Not( topVar ) ), Cudd_bddAnd( dd, fb, topVar ) ); //  create new mux decomposed circuit
    printf("%d\n", Cudd_ReadSize( dd ));
	FILE *pFile;
	char FileNameDot[100];
	sprintf( FileNameDot, "dumpdot-%d",i );
	if ( (pFile = fopen( FileNameDot, "w" )) == NULL )
		printf("open failed!\n");
	char *DumpName[] = {"fs", "fa", "fb"};
	DdNode* DumpArray[] = {fs, fa, fb};
	Cudd_DumpDot( dd, 3, DumpArray, NULL, NULL, pFile );
	++i;
  }
  return pFunc;
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
bool isMuxDecomp(DdManager *dd, DdNode *f )
{
  // variable declarations
  DdNode *fa  = Cudd_T( f  );
  DdNode *fb  = Cudd_E( f  );
  DdNode *y[4];

  y[0] = CheckChild( dd,  fa, 1) ;
  y[1] = CheckChild( dd,  fa, 0) ;
  y[2] = CheckChild( dd,  fb, 1) ;
  y[3] = CheckChild( dd,  fb, 0) ;
	
  Vec_Ptr_t *pSet = Vec_PtrAlloc( 3 );
  // end variable declarations
	printf("fa_T: %p, fa_E: %p, fb_T: %p, fb_E: %p", y[0], y[1], y[2], y[3]);
  for( int i = 0 ; i < 4 ; ++i )
  {
     Vec_PtrPushUnique( pSet, y[i] );
     if( Vec_PtrSize( pSet ) == 3 )
     {
       Vec_PtrFree( pSet );
       return false;
     }
  }
  Vec_PtrFree( pSet );
  return true;
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
DdNode* buildS( DdManager *dd, DdNode *f, DdNode *currentS )
{
  if( Cudd_SupportSize( dd, f ) < 2 ) return NULL; // precondition

  // variable declarations
  DdNode *fa  = Cudd_E( f );
  DdNode *fb  = Cudd_T( f );
  DdNode *y[4]; // 00, 01, 10, 11
  y[0] = CheckChild( dd,  fa, 0 );
  y[1] = CheckChild( dd,  fa, 1 );
  y[2] = CheckChild( dd,  fb, 0 );
  y[3] = CheckChild( dd,  fb, 1 );

  int fa_order = Cudd_ReadPerm( dd, Cudd_NodeReadIndex( fa ) );
  int fb_order = Cudd_ReadPerm( dd, Cudd_NodeReadIndex( fb ) );
  const int index       = fa_order > fb_order ? Cudd_NodeReadIndex( fb ) : Cudd_NodeReadIndex( fa );
  DdNode    *mergedVar  = Cudd_bddIthVar( dd, index ); // the second input of the BDD

  int value = 0; // indicate the truth table value of y
  // end variable declarations

  // select the truth table value of y
  // use y[0] as reference and y[0] is 0
  if( y[1] != y[0] ) value += value_et;
  if( y[2] != y[0] ) value += value_te;
  if( y[3] != y[0] ) value += value_tt;
  // end select the truth table value of y

  // build fs
  switch( value )
  {
    case 1:   return Cudd_bddAnd( dd, currentS,             mergedVar             ); // 0001 
    case 2:   return Cudd_bddAnd( dd, currentS,             Cudd_Not( mergedVar ) ); // 0010 
    case 4:   return Cudd_bddAnd( dd, Cudd_Not( currentS ), mergedVar             ); // 0100
    case 6:   return Cudd_bddXor( dd, currentS,             mergedVar             ); // 0110
    case 7:   return Cudd_bddOr ( dd, currentS,             mergedVar             ); // 0111
    default:  return NULL;
  };
  // end build fs
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

DdNode* CheckChild( DdManager *dd, DdNode* f, int type)
{
	const int limit = 2;
	if (Cudd_ReadPerm(dd, Cudd_NodeReadIndex(f)) > limit)
		return f;
	if(type)
		if( Cudd_IsConstant(f))
			return f;
		else
			return Cudd_T(f);
	else
		if(Cudd_IsConstant(f))
			return f;
		else
			return Cudd_E(f);
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#ifndef TEST
ABC_NAMESPACE_IMPL_END
#endif

