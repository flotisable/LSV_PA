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
bool isLeft ( Abc_Obj_t *pObj );
bool isRight( Abc_Obj_t *pObj );

Abc_Obj_t*  processLeft ( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs );
void        processRight( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs );
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
  if( !Abc_ObjFaninC0( pNode ) || !Abc_ObjFaninC1( pNode ) ) goto notMaj;
  // end first level

  // second level
  pParent = pNode;
  pNode   = Abc_ObjFanin0( pParent );

  if( Abc_ObjIsPi( pNode ) ) goto notMaj;

  if( isLeft( pNode ) )
  {
    if( !isRight( Abc_ObjFanin1( pParent ) ) ) goto notMaj;

    processRight( Abc_ObjFanin1( pParent ), pInputs );
    pNode = processLeft( pNode, pInputs );
  }
  else if( isRight( pNode ) )
  {
    if( !isLeft( Abc_ObjFanin1( pParent ) ) ) goto notMaj;

    processRight( pNode, pInputs );
    pNode = processLeft( Abc_ObjFanin1( pParent ), pInputs );
  }
  else
    goto notMaj;
  // end second level

  // third level
  if( Abc_ObjIsPi( pNode ) ) goto notMaj;

  if( Vec_PtrFind( pInputs, Abc_ObjNot( Abc_ObjChild0( pNode ) ) ) == -1 ) goto notMaj;
  if( Vec_PtrFind( pInputs, Abc_ObjNot( Abc_ObjChild1( pNode ) ) ) == -1 ) goto notMaj;
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

bool isLeft( Abc_Obj_t *pObj )
{
  return ( Abc_ObjFaninC0( pObj ) != Abc_ObjFaninC1( pObj ) );
}

bool isRight( Abc_Obj_t *pObj )
{
  return ( !Abc_ObjFaninC0( pObj ) && !Abc_ObjFaninC1( pObj ) );
}

Abc_Obj_t* processLeft( Abc_Obj_t *pObj, Vec_Ptr_t *pInputs )
{
  if( Abc_ObjFaninC0( pObj ) )
  {
    Vec_PtrPush( pInputs, Abc_ObjChild1( pObj ) );
    return Abc_ObjFanin0( pObj );
  }
  else
  {
    Vec_PtrPush( pInputs, Abc_ObjChild0( pObj ) );
    return Abc_ObjFanin1( pObj );
  }
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

