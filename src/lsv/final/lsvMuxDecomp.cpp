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
#include "Test/testLsvMuxDecomp.h"
#else
#include "base/main/mainInt.h"
#include "lsvMuxDecomp.h"

ABC_NAMESPACE_IMPL_START
#endif

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

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
DdNode* buildS( DdManager *dd, DdNode *f )
{
  return NULL;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////

#ifndef TEST
ABC_NAMESPACE_IMPL_END
#endif

