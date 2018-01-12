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

#include "base/abci/abcMulti.c"
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

int Cecsat(Abc_Ntk_t * pNtk, Abc_Ntk_t * pNtk2) {
	extern Abc_Ntk_t * Abc_NtkMulti( Abc_Ntk_t * pNtk, int nThresh, int nFaninMax, int fCnf, int fMulti, int fSimple, int fFactor );
	Abc_Ntk_t * pMiter;
	Abc_Ntk_t * pCnf;
	int RetValue;
	pMiter = Abc_NtkMiter( pNtk, pNtk2, 1, 0, 0, 0 );
	if(pMiter == NULL) {
		//printf("GG");
		return 0;
	}
	RetValue = Abc_NtkMiterIsConstant( pMiter );
	if ( RetValue == 0 ) {
		//printf( "Networks are NOT EQUIVALENT after structural hashing.\n" );
		ABC_FREE( pMiter->pModel );
		Abc_NtkDelete( pMiter );
		return 1;
	}
	if( RetValue == 1 ) {
		Abc_NtkDelete( pMiter );
		//printf("equivalent after structural hashing.\n");
		return 1;
	}
	else if(RetValue == -1) {
		pCnf = Abc_NtkMulti( pMiter, 0, 100, 1, 0, 0, 0 );
		Abc_NtkDelete( pMiter );
		if ( pCnf == NULL ) {
			return 0;
		}
		RetValue = Abc_NtkMiterSat( pCnf, 0, 0, 0, NULL, NULL );
		ABC_FREE( pCnf->pModel );	
	}
	Abc_NtkDelete( pCnf );
	if(RetValue == -1 || RetValue == 0)
		return 0;
	else
		return 1;
}
////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

