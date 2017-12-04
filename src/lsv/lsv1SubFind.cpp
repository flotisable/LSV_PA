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

extern "C"
{
#include "base/main/mainInt.h"
#include "sat/cnf/cnf.h"
}

#include <map>
using std::map;

ABC_NAMESPACE_IMPL_START

////////////////////////////////////////////////////////////////////////
///                        DECLARATIONS                              ///
////////////////////////////////////////////////////////////////////////

// external function declaration
extern "C" Aig_Man_t* Abc_NtkToDar( Abc_Ntk_t *pNtk, int fExors, int fRegisters );
// end external function declaration

int   sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf );
int   sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf, Aig_Man_t *pMan, map<int,int> &unitAssumptionVar, const int varBias );
bool  is1SubCondidate( sat_solver *pSat, int variable1, int variable2, int auxiliaryIndex, lit *lits, lit *litsEnd, bool complement );

////////////////////////////////////////////////////////////////////////
///                     FUNCTION DEFINITIONS                         ///
////////////////////////////////////////////////////////////////////////
 
/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

void Lsv_Ntk1SubFind( Abc_Ntk_t * pNtk )
{
  if( !pNtk ) return; // preconsition

  using std::distance;

  // variable declaration
  Abc_Ntk_t   *pNtk1  = pNtk;
  Abc_Ntk_t   *pNtk2  = Abc_NtkDup( pNtk );
  Aig_Man_t   *pMan1  = Abc_NtkToDar( pNtk1, 0, 0 );
  Aig_Man_t   *pMan2  = Abc_NtkToDar( pNtk2, 0, 0 );
  Cnf_Dat_t   *pCnf1  = Cnf_DeriveSimple( pMan1, Aig_ManCoNum( pMan1 ) );
  Cnf_Dat_t   *pCnf2  = Cnf_DeriveSimple( pMan2, Aig_ManCoNum( pMan2 ) );
  sat_solver  *pSat   = sat_solver_new();

  Aig_Obj_t *pObj;
  Aig_Obj_t *pObj2;
  int       i;
  int       j;

  map<int,int>  unitAssumptionVar;
  // end variable declaration

  Cnf_DataLift( pCnf2, pCnf1->nVars );

  // add cnf clauses
  sat_solver_add_cnf( pSat, pCnf1 );
  sat_solver_add_cnf( pSat, pCnf2, pMan2, unitAssumptionVar, pCnf1->nVars + pCnf2->nVars );
  // end add cnf clauses

  // the input of two circuits should be equivalent
  Aig_ManForEachCi( pMan1, pObj, i )
  {
    lit lits[3];

    pObj2 = Aig_ManCi( pMan2, i ); // get the corresponding Po in circuit 2

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 2 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 2 );
  }
  // end the input of two circuits should be equivalent

  // if the output of two circuits are equivalent, the miter output is 0
  const int outputVar = pCnf1->nVars + pCnf2->nVars + unitAssumptionVar.size() + 1;
  lit       outLit[2];

  outLit[0] = toLitCond( outputVar, 0 );

  Aig_ManForEachCo( pMan1, pObj, i )
  {
    lit lits[4];

    pObj2   = Aig_ManCo( pMan2, i ); // get the corresponding Po in circuit 2
    lits[2] = toLitCond( outputVar, 1 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 3 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj )],  1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 3 );
  }

  sat_solver_addclause( pSat, outLit, outLit + 1 );
  // end if the output of two circuits are equivalent, the miter output is 0

  // initialize literals
  const size_t            offset = 2;
  const size_t            litNum = offset + unitAssumptionVar.size() + 1;
  lit                     *lits  = new lit[litNum];
  map<int,int>::iterator  it;

  for( it = unitAssumptionVar.begin(), i = 0 ; it != unitAssumptionVar.end() ; ++it, ++i )
     lits[offset+i] = toLitCond( it->second, 0 );
  // end initialize literals

  // iterate all nodes except Po and search for the 1-input resubstitute candidates
  Aig_ManForEachNode( pMan2, pObj, i )
  {
    if( Aig_ObjIsCo( pObj ) ) continue;

    Aig_ManForEachNode( pMan2, pObj2, j )
    {
      if( Aig_ObjIsCo( pObj2 ) || ( i == j ) ) continue;

      // check if pObj2 is pObj's transitive fanin
      Aig_Obj_t *objs[2]  = { pObj };
      Vec_Ptr_t *tfi      = Aig_ManDfsNodes( pMan2, objs, 1 );

      if( Vec_PtrFind( tfi, pObj2 ) != -1 )
      {
        Vec_PtrFree( tfi );
        continue;
      }
      // end check if pObj2 is pObj's transitive fanin

      // variable declaration
      const int id1             = Aig_ObjId( pObj   );
      const int id2             = Aig_ObjId( pObj2  );
      const int variable1       = pCnf2->pVarNums[id1];
      const int variable2       = pCnf2->pVarNums[id2];
      const int auxiliaryIndex  = offset + distance( unitAssumptionVar.begin(), unitAssumptionVar.find( id2 ) );
      // end variable declaration

      // test for whether two node can have same value
      if( is1SubCondidate( pSat, variable1, variable2, auxiliaryIndex, lits, lits + litNum, false ) )
      {
        // pObj2 is a 1-input resubstitute candidate of pObj
        Abc_Print( ABC_STANDARD, "%i is a 1-input resubstitute condidate of %i\n", id1, id2 );
        continue;
      }
      // end test for whether two node can have same value

      // test for whether two node can have complement value
      if( is1SubCondidate( pSat, variable1, variable2, auxiliaryIndex, lits, lits + litNum, true ) )
      {
        // pObj2 is a 1-input resubstitute candidate of pObj
        Abc_Print( ABC_STANDARD, "%i is a 1-input resubstitute condidate of %i ( complement )\n", id1, id2 );
      }
      // end test for whether two node can have complement value
    }
  }
  // end iterate all nodes except Po and search for the 1-input resubstitute candidates

  // release memory
  delete[] lits;
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

/**Function*************************************************************

  Synopsis    []

  Description []

  SideEffects []

  SeeAlso     []

***********************************************************************/

int sat_solver_add_cnf( sat_solver *pSat, Cnf_Dat_t *pCnf, Aig_Man_t *pMan, std::map<int,int> &unitAssumptionVar, const int varBias )
{
  // variables declaration
  int success   = 1;
  int varIndex  = varBias + 1;
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

     for( j = 0 ; j < litNum ; ++j )
        lits[j] = pCnf->pClauses[i][j];

     // find the output literal and create a mapping
     Aig_ManForEachNode( pMan, pObj, j )
     {
       if( lit_var( lits[0] ) != pCnf->pVarNums[Aig_ObjId( pObj )] ) continue;

       // variable declaration
       map<int,int>::iterator it      = unitAssumptionVar.find( Aig_ObjId( pObj ) );
       int                    index;
       // end variable declaration

       // check if there is a mapping
       if( it == unitAssumptionVar.end() )
       {
         index = varIndex++;
         unitAssumptionVar.insert( map<int,int>::value_type( Aig_ObjId( pObj ), index ) );
       }
       else
         index = it->second;
       // end check if there is a mapping

       // add auxiliary literal to break the connection
       lits[litNum] = toLitCond( index, 1 );
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

