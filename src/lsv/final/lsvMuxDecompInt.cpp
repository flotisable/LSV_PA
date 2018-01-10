/**CFile****************************************************************

  FileName    [lsvMuxDecompInt.cpp]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [lsv: Logic Synthesis and Verification PA.]

  Synopsis    []

  Author      []
  
  Affiliation [NTU]

  Date        [17, Sep., 2017.]

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "base/main/mainInt.h"
}

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

int NtkTransToBdd( Abc_Ntk_t *pNtk );

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
Lsv_NtkMuxDecomp( Abc_Ntk_t * pNtk )
{
	if( !pNtk )
	{
		Abc_Print( ABC_ERROR, "Empty Network!\n" );
		return;
	}
	if( !NtkTransToBdd( pNtk ) )
	{
		Abc_Print( ABC_ERROR, "Covert Network to BDD failed\n" );
	}

}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int NtkTransToBdd( Abc_Ntk_t *pNtk )
{
	return Abc_NtkToBdd( pNtk );
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

