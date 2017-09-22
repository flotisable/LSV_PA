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

bool isMaj( Abc_Obj_t *pObj )
{
  return true;
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

