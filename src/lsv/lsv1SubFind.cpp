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

int sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf );

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
Lsv_Ntk1SubFind( Abc_Ntk_t * pNtk )
{
  if( !pNtk ) return; // preconsition

  // external function declaration
  extern Aig_Man_t* Abc_NtkToDar( Abc_Ntk_t *pNtk, int fExors, int fRegisters );
  // end external function declaration

  // variable declaration
  Abc_Ntk_t   *pNtk1  = pNtk;
  Abc_Ntk_t   *pNtk2  = Abc_NtkDup( pNtk );
  Aig_Man_t   *pMan1  = Abc_NtkToDar( pNtk1, 0, 0 );
  Aig_Man_t   *pMan2  = Abc_NtkToDar( pNtk2, 0, 0 );
  Cnf_Dat_t   *pCnf1  = Cnf_DeriveSimple( pMan1, 0 );
  Cnf_Dat_t   *pCnf2  = Cnf_DeriveSimple( pMan2, 0 );
  sat_solver  *pSat   = sat_solver_new();
  // end variable declaration

  Cnf_DataLift( pCnf2, pCnf1->nVars );

  // release memory
  sat_solver_delete( pSat );
  ABC_FREE( pCnf1 );
  ABC_FREE( pCnf2 );
  Aig_ManStop( pMan1 );
  Aig_ManStop( pMan2 );
  Abc_NtkDelete( pNtk2 );
  // end release memory
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

int sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf )
{
  int success = 1;

  for( int i = 0 ; i < pCnf->nClauses ; ++i )
     success &= sat_solver_addclause( pSat, pCnf->pClauses[i], pCnf->pClauses[i+1] );

  return success;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

