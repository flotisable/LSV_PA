/**CFile****************************************************************

  FileName    [lsvMajFind.cpp]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [lsv: Logic Synthesis and Verification PA.]

  Synopsis    [procedure for finding MAJ gates.]

  Author      [Nian-Ze Lee]
  
  Affiliation [NTU]

  Date        [17, Sep., 2017.]

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

#include "base/main/mainInt.h"

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

bool isMaj  ( Abc_Obj_t *pObj );

void processLeft  ( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs, Abc_Obj_t *thirdLevelNode );
void processRight ( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs );
////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////
 
/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

void
Lsv_NtkMajFind( Abc_Ntk_t * pNtk )
{
  // TODO
  if( !pNtk ) return; // preconsition

  // variable declaration
  Abc_Obj_t *pNode;
  int       i;
  int       majNum = 0;
  // end variable declaration

  // find MAJ and print
  Abc_NtkForEachNode( pNtk, pNode, i )
  {
    if( isMaj( pNode ) ) ++majNum;
  }
  Abc_Print( ABC_STANDARD, "number of found MAJ : %i\n", majNum );
  // end find MAJ and print
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

bool isMaj( Abc_Obj_t *pObj )
{
  // variable declaration
  const int inSize = 3;

  Vec_Ptr_t *pInputs          = Vec_PtrAlloc( inSize + 1 );
  Vec_Ptr_t *pThirdLevelNodes = Vec_PtrAlloc( 4 );
  Abc_Obj_t *pNode            = pObj;
  Abc_Obj_t *pFanin0;
  Abc_Obj_t *pFanin1;
  int       i;
  // end variable declaration

  // first level
  if( !Abc_ObjFaninC0( pNode ) || !Abc_ObjFaninC1( pNode ) ) goto notMaj;
  // end first level

  // second level
  pFanin0 = Abc_ObjFanin0( pNode );
  pFanin1 = Abc_ObjFanin1( pNode );

  if( Abc_ObjIsPi( pFanin0 ) || Abc_ObjIsPi( pFanin1 ) ) goto notMaj;

  if( Abc_ObjFaninC0( pFanin0 ) ) Vec_PtrPush( pThirdLevelNodes, Abc_ObjFanin0( pFanin0 ) );
  if( Abc_ObjFaninC1( pFanin0 ) ) Vec_PtrPush( pThirdLevelNodes, Abc_ObjFanin1( pFanin0 ) );
  if( Abc_ObjFaninC0( pFanin1 ) ) Vec_PtrPush( pThirdLevelNodes, Abc_ObjFanin0( pFanin1 ) );
  if( Abc_ObjFaninC1( pFanin1 ) ) Vec_PtrPush( pThirdLevelNodes, Abc_ObjFanin1( pFanin1 ) );
  // end second level

  // third level
  Vec_PtrForEachEntry( Abc_Obj_t*, pThirdLevelNodes, pNode, i )
  {
    if( Abc_ObjIsPi( pNode ) ) continue;

    Vec_PtrClear( pInputs );

    if( pNode == Abc_ObjFanin0( pFanin0 ) || pNode == Abc_ObjFanin1( pFanin0 ) )
    {
      processLeft ( pFanin0, pInputs, pNode );
      processRight( pFanin1, pInputs );
    }
    else
    {
      processLeft ( pFanin1, pInputs, pNode );
      processRight( pFanin0, pInputs );
    }

    if( Vec_PtrFind( pInputs, Abc_ObjNot( Abc_ObjChild0( pNode ) ) ) == -1 ) continue;
    if( Vec_PtrFind( pInputs, Abc_ObjNot( Abc_ObjChild1( pNode ) ) ) == -1 ) continue;
    goto maj;
  }
  goto notMaj;
  // end third level

  // print MAJ
maj:

  Abc_Print( ABC_STANDARD, "%i = MAJ( ", Abc_ObjId( pObj ) );

  Vec_PtrForEachEntry( Abc_Obj_t*, pInputs, pNode, i )
  {
    if( Abc_ObjIsComplement( pNode ) ) Abc_Print( ABC_STANDARD, "-" );
    Abc_Print( ABC_STANDARD, "%i", Abc_ObjId( Abc_ObjRegular( pNode ) ) );

    if( i == Vec_PtrSize( pInputs ) - 1 ) Abc_Print( ABC_STANDARD, " )\n" );
    else                                  Abc_Print( ABC_STANDARD, ", " );
  }
  // end print MAJ

  // release memory
  Vec_PtrFree( pInputs );
  Vec_PtrFree( pThirdLevelNodes );
  // end release memory
  return true;

notMaj:

  Vec_PtrFree( pInputs );
  Vec_PtrFree( pThirdLevelNodes );
  return false;
}

void processLeft( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs, Abc_Obj_t *thirdLevelNode )
{
  if( Abc_ObjFanin0( pObj ) == thirdLevelNode ) Vec_PtrPush( pInputs, Abc_ObjChild1( pObj ) );
  else                                          Vec_PtrPush( pInputs, Abc_ObjChild0( pObj ) );
}

void processRight( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs )
{
  Vec_PtrPush( pInputs, Abc_ObjChild0( pObj ) );
  Vec_PtrPush( pInputs, Abc_ObjChild1( pObj ) );
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

