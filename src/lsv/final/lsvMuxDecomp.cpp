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

#include "lsvMuxDecomp.h"

#ifdef TEST
#include "Test/LsvMuxDeomp/testLsvMuxDecomp.h"
#else
#include "base/main/mainInt.h"

ABC_NAMESPACE_IMPL_START
#endif

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

enum FuncIndex
{
  func_s,
  func_a,
  func_b,
  func_num
};

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

  while( isMuxDecomp( f ) )
  {
    // variables declarations
    DdNode *fs = buildS( dd, f, ( DdNode* )Vec_PtrEntry( pFunc, func_s ) );

    if( !fs ) break;

    DdNode *fa = Cudd_E( Cudd_E( f ) ); // 00
    DdNode *fb = NULL;
    // end variables declarations

    // find fb
    if      ( Cudd_T( Cudd_E( f ) ) != fa ) fb = Cudd_T( Cudd_E( f ) ); // 01
    else if ( Cudd_E( Cudd_T( f ) ) != fa ) fb = Cudd_E( Cudd_T( f ) ); // 10
    else if ( Cudd_T( Cudd_T( f ) ) != fa ) fb = Cudd_T( Cudd_T( f ) ); // 11
    // end find fb

    // deal the reference
    Cudd_Ref            ( fs    );
    Cudd_Ref            ( fa    );
    Cudd_Ref            ( fb    );
    Cudd_RecursiveDeref ( dd, f );
    // end deal the reference

    if( !topVar ) topVar = Cudd_bddNewVar( dd );

    // set current functions
    Vec_PtrWriteEntry( pFunc, func_s, fs );
    Vec_PtrWriteEntry( pFunc, func_a, fa );
    Vec_PtrWriteEntry( pFunc, func_b, fb );
    // end set current functions

    f = Cudd_bddOr( dd, Cudd_bddAnd( dd, fa, Cudd_Not( topVar ) ), Cudd_bddAnd( dd, fb, topVar ) ); //  create new mux decomposed circuit
  }
  return pFunc;
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
bool isMuxDecomp( DdNode *f )
{
  // variable declarations
  DdNode *fa  = Cudd_T( f  );
  DdNode *fb  = Cudd_E( f  );
  DdNode *y[] = { Cudd_T( fa ), Cudd_E( fa ), Cudd_T( fb ), Cudd_E( fb ) };

  Vec_Ptr_t *pSet = Vec_PtrAlloc( 3 );
  // end variable declarations

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
  DdNode *y[] = { Cudd_E( fa ), Cudd_T( fa ), Cudd_E( fb ), Cudd_T( fb ) }; // 00, 01, 10, 11

  const int index       = Abc_MinInt( Cudd_ReadPerm( dd, Cudd_NodeReadIndex( fa ) ),
                                      Cudd_ReadPerm( dd, Cudd_NodeReadIndex( fb ) ) );
  DdNode    *mergedVar  = Cudd_bddIthVar( dd, index ); // the second input of the BDD

  int value = 0; // indicate the truth table value of y
  // end variable declarations

  // select the truth table value of y
  // use y[0] as reference and y[0] is 0
  if( y[1] == y[0] ) value += value_et;
  if( y[2] == y[0] ) value += value_te;
  if( y[3] == y[0] ) value += value_tt;
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

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#ifndef TEST
ABC_NAMESPACE_IMPL_END
#endif

