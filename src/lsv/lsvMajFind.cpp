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

bool isMaj( Abc_Obj_t *pObj );
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

  Vec_Ptr_t *pInputs  = Vec_PtrAlloc( inSize + 1 );
  Abc_Obj_t *pNode    = pObj;
  Abc_Obj_t *pParent;
  int       i;
  // end variable declaration

  // first level
  if( Abc_ObjIsPi( pNode ) || Abc_ObjIsPo( pNode ) ) goto notMaj;

  pParent = Abc_ObjFanout0( pNode );

  if( !(  ( Abc_ObjFanin0( pParent ) == pNode && Abc_ObjFaninC0( pParent ) ) ||
          ( Abc_ObjFanin1( pParent ) == pNode && Abc_ObjFaninC1( pParent ) ) ) ) goto notMaj;
  if( Abc_ObjFaninC0( pNode ) == Abc_ObjFaninC1( pNode ) ) goto notMaj;
  // end first level

  // second level
  pParent = pNode;
  pNode   = ( Abc_ObjFaninC0( pParent ) ) ? Abc_ObjFanin0( pParent ) : Abc_ObjFanin1( pParent );

  if( Abc_ObjIsPi( pNode ) ) goto notMaj;

  Vec_PtrPush( pInputs, Abc_ObjChild0( pNode ) );
  Vec_PtrPush( pInputs, Abc_ObjChild1( pNode ) );

  pNode = ( Abc_ObjFaninC0( pParent ) ) ? Abc_ObjFanin1( pParent ) : Abc_ObjFanin0( pParent );

  if( Abc_ObjIsPi( pNode ) ) goto notMaj;
  if( Abc_ObjFaninC0( pNode ) != Abc_ObjFaninC1( pNode ) || !Abc_ObjFaninC0( pNode ) ) goto notMaj; 
  // end second level

  // third level
  pParent = pNode;
  pNode   = Abc_ObjFanin0( pParent );

  if( Abc_ObjIsPi( pNode ) ) goto notMaj;

  Vec_PtrPushUnique( pInputs, Abc_ObjChild0( pNode ) );
  Vec_PtrPushUnique( pInputs, Abc_ObjChild1( pNode ) );

  if( Vec_PtrSize( pInputs ) > inSize ) goto notMaj;

  pNode = Abc_ObjFanin1( pParent );

  if( Abc_ObjIsPi( pNode ) ) goto notMaj;

  Vec_PtrPushUnique( pInputs, Abc_ObjChild0( pNode ) );
  Vec_PtrPushUnique( pInputs, Abc_ObjChild1( pNode ) );

  if( Vec_PtrSize( pInputs ) > inSize ) goto notMaj;
  // end third level

  // print MAJ
  Abc_Print( ABC_STANDARD, "%i = MAJ( ", Abc_ObjId( pObj ) );

  Vec_PtrForEachEntry( Abc_Obj_t*, pInputs, pNode, i )
  {
    if( Abc_ObjIsComplement( pNode ) ) Abc_Print( ABC_STANDARD, "-" );
    Abc_Print( ABC_STANDARD, "%i", Abc_ObjId( Abc_ObjRegular( pNode ) ) );

    if( i == Vec_PtrSize( pInputs ) - 1 ) Abc_Print( ABC_STANDARD, " )\n" );
    else                                  Abc_Print( ABC_STANDARD, ", " );
  }
  // end print MAJ

  Vec_PtrFree( pInputs );
  return true;

notMaj:

  Vec_PtrFree( pInputs );
  return false;
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

