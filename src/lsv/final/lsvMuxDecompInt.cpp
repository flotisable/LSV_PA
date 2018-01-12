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
	if( NtkTransToBdd( pNtk ) )
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
	Abc_Ntk_t *pNtkRes;
    int fVerbose;
    int fBddSizeMax;
    int fDualRail;
    int fReorder;

    // set defaults
    fVerbose = 0;
    fReorder = 1;
    fDualRail = 0;
    fBddSizeMax = ABC_INFINITY;
    

    if ( !Abc_NtkIsLogic(pNtk) && !Abc_NtkIsStrash(pNtk) )
    {
        Abc_Print( -1, "Can only collapse a logic network or an AIG.\n" );
        return 1;
    }

    // get the new network
    if ( Abc_NtkIsStrash(pNtk) )
        pNtkRes = Abc_NtkCollapse( pNtk, fBddSizeMax, fDualRail, fReorder, fVerbose );
    else
    {
        pNtk = Abc_NtkStrash( pNtk, 0, 0, 0 );
        pNtkRes = Abc_NtkCollapse( pNtk, fBddSizeMax, fDualRail, fReorder, fVerbose );
        Abc_NtkDelete( pNtk );
    }
    if ( pNtkRes == NULL )
    {
        Abc_Print( -1, "Collapsing has failed.\n" );
        return 1;
    }
    // replace the current network
    //Abc_FrameReplaceCurrentNetwork( pAbc, pNtkRes );
    pNtk = pNtkRes;
	return 0;

}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

