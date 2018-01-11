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
#include "lsvMuxDecomp.h"

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

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/
void muxDecompCore( DdManager *dd, DdNode *f )
{
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

