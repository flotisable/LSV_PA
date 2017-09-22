/**CFile****************************************************************

  FileName    [lsvCmd.cpp]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [lsv: Logic Synthesis and Verification PA.]

  Synopsis    [command file.]

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

// export to mainInit.c
extern "C" void Lsv_Init ( Abc_Frame_t * );
extern "C" void Lsv_End  ( Abc_Frame_t * );

// command functions
static int Abc_CommandMajFind( Abc_Frame_t * , int , char ** );

// external functions defined in lsv package
extern void Lsv_NtkMajFind( Abc_Ntk_t * );

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
Lsv_Init( Abc_Frame_t * pAbc )
{
   Cmd_CommandAdd( pAbc, "z LSV", "MAJ_find" , Abc_CommandMajFind , 0 );
}

void
Lsv_End( Abc_Frame_t * pAbc )
{
}

/**Function*************************************************************

  Synopsis    []

  Description []
               
  SideEffects []

  SeeAlso     []

***********************************************************************/

int
Abc_CommandMajFind( Abc_Frame_t * pAbc , int argc , char ** argv )
{
  // TODO:
  // step.1: get the current network
  // step.2: check whether the current network is strashed
  // step.3: call Lsv_NtkMajFind() to report MAJ gates

  // variable declaration
  char      c;
  Abc_Ntk_t *pNtk;
  // end variable declaration

  // process arguments
  Extra_UtilGetoptReset();
  while( ( c = Extra_UtilGetopt( argc, argv, "h" ) ) != EOF )
  {
    switch( c )
    {
      case 'h': goto usage;
      default:  goto usage;
    }
  }
  // end process arguments

  // get the current network
  pNtk = Abc_FrameReadNtk( pAbc );

  if( !pNtk )
  {
    Abc_Print( ABC_ERROR, "Empty network.\n" );
    return 1;
  }
  // end get the current network

  // check whether the current network is strashed
  if( !Abc_NtkIsStrash( pNtk ) )
  {
    const int fAllNodes = 0;
    const int fCleanup  = 1;
    const int fRecord   = 0;

    pNtk = Abc_NtkStrash( pNtk, fAllNodes, fCleanup, fRecord );

    Lsv_NtkMajFind( pNtk );
    Abc_NtkDelete( pNtk );
  }
  else
    Lsv_NtkMajFind( pNtk );
  // end check whether the current network is strashed
  return 0;

usage:

  Abc_Print( ABC_PROMPT, "usage: MAJ_find [-h]\n" );
  Abc_Print( ABC_PROMPT, "\t        prints the MAJ in network and the number of found MAJ \n" );
  Abc_Print( ABC_PROMPT, "\t-h    : print the command usage\n");
  return 1;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

