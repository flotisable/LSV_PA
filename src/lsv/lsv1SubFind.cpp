/**CFile****************************************************************

  FileName    [lsv1Subfind.cpp]

  SystemName  [ABC: Logic synthesis and verification system.]

  PackageName [lsv: Logic Synthesis and Verification PA.]

  Synopsis    [procedure for finding 1 input resubstitute candidate.]

  Author      [Nian-Ze Lee]

  Affiliation [NTU]

  Date        [17, Sep., 2017.]

***********************************************************************/

////////////////////////////////////////////////////////////////////////
///                          INCLUDES                                ///
////////////////////////////////////////////////////////////////////////

extern "C"
{
#include "base/main/mainInt.h"
#include "sat/cnf/cnf.h"
}

#include <map>
#include <vector>
using namespace std;

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

// external function declaration
extern "C" Aig_Man_t* Abc_NtkToDar( Abc_Ntk_t *pNtk, int fExors, int fRegisters );
// end external function declaration

int   sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf );
int   sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf, Aig_Man_t *pMan, map<int,int> &unitAssumptionVar );
void  addPiConstraints  ( sat_solver *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2, map<int,int> &unitAssumptionVar );
void  addPoConstraints  ( sat_solver *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 );
bool  isTfi             ( Abc_Ntk_t *pNtk, Abc_Obj_t *pNode, Abc_Obj_t *pTfiNode );
bool  is1SubCondidate   ( sat_solver *pSat, int variable1, int variable2, int auxiliaryIndex, lit *lits, lit *litsEnd, bool complement );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////
 
/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

void Lsv_Ntk1SubFind( Abc_Ntk_t *pNtk )
{
  if( !pNtk ) return; // precondition

  // variable declaration
  Abc_Ntk_t   *pNtk1  = pNtk;
  Abc_Ntk_t   *pNtk2  = Abc_NtkDup( pNtk );
  Aig_Man_t   *pMan1  = Abc_NtkToDar( pNtk1, 0, 0 );
  Aig_Man_t   *pMan2  = Abc_NtkToDar( pNtk2, 0, 0 );
  Cnf_Dat_t   *pCnf1  = Cnf_DeriveSimple( pMan1, Aig_ManCoNum( pMan1 ) );
  Cnf_Dat_t   *pCnf2  = Cnf_DeriveSimple( pMan2, Aig_ManCoNum( pMan2 ) );
  sat_solver  *pSat   = sat_solver_new();

  Abc_Obj_t *pObj;
  Abc_Obj_t *pObj2;
  int       i;
  int       j;

  map<int,int>                        unitAssumptionVar;
  vector< vector< pair<int, bool> > > subCand( Aig_ManObjNum( pMan1 ) );
  // end variable declaration

  sat_solver_setnvars( pSat, pCnf1->nVars + pCnf2->nVars );
  Cnf_DataLift( pCnf2, pCnf1->nVars );

  // add cnf clauses
  sat_solver_add_cnf( pSat, pCnf1 );
  sat_solver_add_cnf( pSat, pCnf2, pMan2, unitAssumptionVar );
  addPiConstraints  ( pSat, pMan1, pMan2, pCnf1, pCnf2, unitAssumptionVar );
  addPoConstraints  ( pSat, pMan1, pMan2, pCnf1, pCnf2 );
  // end add cnf clauses

  // initialize unit assumption literals
  const size_t            offset = 2;
  const size_t            litNum = offset + unitAssumptionVar.size() + 1;
  lit                     *lits  = new lit[litNum];
  map<int,int>::iterator  it;

  for( it = unitAssumptionVar.begin(), i = 0 ; it != unitAssumptionVar.end() ; ++it, ++i )
     lits[offset+i] = toLitCond( it->second, 0 );
  // end initialize unit assumption literals

  // iterate all nodes except Po and search for the 1-input resubstitute candidates
  Abc_NtkForEachObj( pNtk, pObj, i )
  {
    if( !Abc_ObjIsCi( pObj ) && !Abc_ObjIsNode( pObj ) ) continue;

    Abc_NtkForEachObj( pNtk, pObj2, j )
    {
      // precondition
      if( pObj == pObj2 ) continue;
      if( !Abc_ObjIsCi( pObj2 ) && !Abc_ObjIsNode( pObj2 ) ) continue;
      if( isTfi( pNtk, pObj, pObj2 ) ) continue;
      // end precondition

      // variable declaration
      const int id1             = Aig_ObjId( reinterpret_cast<Aig_Obj_t*>( Abc_ObjCopy( pObj )  ) );
      const int id2             = Aig_ObjId( reinterpret_cast<Aig_Obj_t*>( Abc_ObjCopy( pObj2 ) ) );
      const int variable1       = pCnf2->pVarNums[id1];
      const int variable2       = pCnf2->pVarNums[id2];
      const int auxiliaryIndex  = offset + distance( unitAssumptionVar.begin(), unitAssumptionVar.find( id2 ) );
      // end variable declaration

      // test for whether two node can have same value
      if( is1SubCondidate( pSat, variable1, variable2, auxiliaryIndex, lits, lits + litNum, false ) )
        subCand[Abc_ObjId( pObj2 )].push_back( pair<int,bool>( Abc_ObjId( pObj ), false ) );
      // end test for whether two node can have same value

      // test for whether two node can have complement value
      if( is1SubCondidate( pSat, variable1, variable2, auxiliaryIndex, lits, lits + litNum, true ) )
        subCand[Abc_ObjId( pObj2 )].push_back( pair<int,bool>( Abc_ObjId( pObj ), true ) );
      // end test for whether two node can have complement value
    }
  }
  // end iterate all nodes except Po and search for the 1-input resubstitute candidates

  // output answer
  for( size_t i = 0 ; i < subCand.size() ; ++i )
  {
     if( subCand[i].empty() ) continue;
     Abc_Print( ABC_STANDARD, "%s:", Abc_ObjName( Abc_NtkObj( pNtk, i ) ) );

     for( size_t j = 0 ; j < subCand[i].size() ; ++j )
     {
        Abc_Print( ABC_STANDARD, " " );
        if( subCand[i][j].second ) Abc_Print( ABC_STANDARD, "-" );
        Abc_Print( ABC_STANDARD, "%s", Abc_ObjName( Abc_NtkObj( pNtk, subCand[i][j].first ) ) );
     }
     Abc_Print( ABC_STANDARD, "\n" );
  }
  // end output answer

  // release memory
  delete[] lits;
  sat_solver_delete( pSat );
  Cnf_DataFree( pCnf1 );
  Cnf_DataFree( pCnf2 );
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

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

int sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf, Aig_Man_t *pMan, map<int,int> &unitAssumptionVar )
{
  // variables declaration
  int success = 1;
  lit lits[5];
  // end variables declaration

  // add clauses
  for( int i = 0 ; i < pCnf->nClauses ; ++i )
  {
     // variables declaration
     int        litNum = pCnf->pClauses[i+1] - pCnf->pClauses[i];
     int        j;
     Aig_Obj_t  *pObj;
     // end variables declaration

     // add the original cnf clauses
     for( j = 0 ; j < litNum ; ++j )
        lits[j] = pCnf->pClauses[i][j];
     // end add the original cnf clauses

     // find the output literal and create a mapping
     Aig_ManForEachNode( pMan, pObj, j )
     {
       if( lit_var( lits[0] ) != pCnf->pVarNums[Aig_ObjId( pObj )] ) continue;

       // variable declaration
       map<int,int>::iterator it    = unitAssumptionVar.find( Aig_ObjId( pObj ) );
       int                    var;
       // end variable declaration

       // check if there is a mapping
       if( it == unitAssumptionVar.end() )
       {
         var = sat_solver_addvar( pSat );
         unitAssumptionVar.insert( map<int,int>::value_type( Aig_ObjId( pObj ), var ) );
       }
       else
         var = it->second;
       // end check if there is a mapping

       // add auxiliary literal to break the connection
       lits[litNum] = toLitCond( var, 1 );
       ++litNum;
       break;
       // end add auxiliary literal to break the connection
     }
     // end find the output literal and create a mapping

     success &= sat_solver_addclause( pSat, lits, lits + litNum );
  }
  // end add clauses

  return success;
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

void addPiConstraints( sat_solver *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2, map<int,int> &unitAssumptionVar )
{
  // variable declaration
  Aig_Obj_t *pObj;
  Aig_Obj_t *pObj2;
  int       i;
  lit       lits[4];
  // end variable declaration

  // the input of two circuits should be equivalent
  Aig_ManForEachCi( pMan1, pObj, i )
  {
    const int var = sat_solver_addvar( pSat );

    pObj2 = Aig_ManCi( pMan2, i ); // get the corresponding Po in circuit 2

    unitAssumptionVar.insert( map<int,int>::value_type( Aig_ObjId( pObj2 ), var ) );

    lits[2] = toLitCond( var, 1 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 3 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 3 );
  }
  // end the input of two circuits should be equivalent
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

void addPoConstraints( sat_solver *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 )
{
  // if the output of miter is 1, the two circuits should be defferent
  // variable declaration
  const int coNum     = Aig_ManCoNum( pMan1 );
  const int outputVar = sat_solver_addvar( pSat );
  lit       *outLits  = new lit[1 + coNum + 1];
  lit       lits[4];
  Aig_Obj_t *pObj;
  Aig_Obj_t *pObj2;
  int       i;
  // end variable declaration

  // setup Po constraints
  Aig_ManForEachCo( pMan1, pObj, i )
  {
    const int var = sat_solver_addvar( pSat );

    pObj2 = Aig_ManCo( pMan2, i ); // get the corresponding Po in circuit 2

    // setup xor constraint
    lits[2] = toLitCond( var, 1 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 3 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 3 );

    lits[2] = toLitCond( var, 0 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 3 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 3 );
    // end setup xor constraint

    // setup output or constraint
    lits[0] = toLitCond( var,       1 );
    lits[1] = toLitCond( outputVar, 0 );
    sat_solver_addclause( pSat, lits, lits + 2 );
    // end setup output or constraint

    outLits[1+i] = toLitCond( var, 0 );
  }
  // end setup Po constraints

  // set miter output constraints
  outLits[0] = toLitCond( outputVar, 1 );

  sat_solver_addclause( pSat, outLits, outLits + 1 + coNum );

  outLits[0] = toLitCond( outputVar, 0 );

  sat_solver_addclause( pSat, outLits, outLits + 1 );
  // end set miter output constraints

  delete[] outLits;
  // end if the output of miter is 1, the two circuits should be defferent
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

bool isTfi( Abc_Ntk_t *pNtk, Abc_Obj_t *pNode, Abc_Obj_t *pTfiNode )
{
  if( Abc_ObjIsCi( pTfiNode ) && !Abc_ObjIsCi( pNode ) ) return true;

  Abc_Obj_t *objs[] = { pNode };
  Vec_Ptr_t *tfi    = Abc_NtkDfsNodes( pNtk, objs, 1 );

  if( Vec_PtrFind( tfi, pTfiNode ) != -1 )
  {
    Vec_PtrFree( tfi );
    return true;
  }
  Vec_PtrFree( tfi );
  return false;
}

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

bool is1SubCondidate( sat_solver *pSat, int variable1, int variable2, int auxiliaryIndex, lit *lits, lit *litsEnd, bool complement )
{
  int satResult;

  lits[auxiliaryIndex] = lit_neg( lits[auxiliaryIndex] );

  // equivalence checking
  lits[0] = toLitCond( variable1, 1 );
  lits[1] = toLitCond( variable2, 1 ^ complement );

  satResult = sat_solver_solve( pSat, lits, litsEnd, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );

  if( satResult != l_False ) goto satFalse;

  lits[0] = toLitCond( variable1, 0 );
  lits[1] = toLitCond( variable2, 0 ^ complement );

  satResult = sat_solver_solve( pSat, lits, litsEnd, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );

  if( satResult != l_False ) goto satFalse;
  // end equivalence checking

  lits[auxiliaryIndex] = lit_neg( lits[auxiliaryIndex] );
  return true;

satFalse:

  lits[auxiliaryIndex] = lit_neg( lits[auxiliaryIndex] );
  return false;
}

////////////////////////////////////////////////////////////////////////
///                       END OF FILE                                ///
////////////////////////////////////////////////////////////////////////


ABC_NAMESPACE_IMPL_END

