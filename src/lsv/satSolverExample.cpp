/*
 * this file is written to demonstrate how to use the sat solver API in abc
 *
 * following summarize what is demonstrated:
 *
 *   1. how to convert an abc network Abc_Ntk_t to cnf formula Cnf_Dat_t
 *   2. how to create a new sat solver instance
 *   3. how to add clause of Cnf_Dat_t into sat solver
 *   4. how to add new variable in the sat solver
 *   5. how to use the unit assumption
 *   6. how to use the sat solver to solver a sat problem
 *   7. how to get the transitive fanin of a node
 *
 * this file can be correctly compiled, so it should work by just copy the code
 *
 * for more information about using sat solver in abc, please visit NTU MD-526 and ask NZ
 */

// to link the C code of abc API
extern "C"
{
#include "base/main/mainInt.h"
#include "sat/cnf/cnf.h"

Aig_Man_t* Abc_NtkToDar( Abc_Ntk_t *pNtk, int fExors, int fRegisters );
}
// end to link the C code of abc API

// function declaration
static int   sat_solver_add_cnf( sat_solver_t *pSat, Cnf_Dat_t *pCnf );
static void  addPiConstraints( sat_solver_t *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 );
static int   addPoConstraints( sat_solver_t *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 );
static void  printAigCoTfi( Aig_Man_t *pMan );
// end function declaration

void satSolverExample( Abc_Ntk_t *pNtk )
{
  // varaible declaration
  Abc_Ntk_t     *pNtk1  = pNtk;
  Abc_Ntk_t     *pNtk2  = Abc_NtkDup( pNtk );
  Aig_Man_t     *pMan1  = Abc_NtkToDar( pNtk1, 0, 0 );
  Aig_Man_t     *pMan2  = Abc_NtkToDar( pNtk2, 0, 0 );
  Cnf_Dat_t     *pCnf1  = Cnf_DeriveSimple( pMan1, Aig_ManCoNum( pMan1 ) ); // the second parameter tells how many Po constraints should be build
  Cnf_Dat_t     *pCnf2  = Cnf_DeriveSimple( pMan2, Aig_ManCoNum( pMan2 ) );
  sat_solver_t  *pSat   = sat_solver_new();

  int           satResult;
  int           outVar;
  lit           unitAssumpLits[2];
  // end varaible declaration

  Cnf_DataLift( pCnf2, pCnf1->nVars ); // shift the variables of pCnf2 by pCnf1-nVars to make the variables of the two cnf different

  sat_solver_setnvars( pSat, pCnf1->nVars + pCnf2->nVars ); // set the variable number of sat solver
  // add cnf constraints to the sat solver
  sat_solver_add_cnf( pSat, pCnf1 );
  sat_solver_add_cnf( pSat, pCnf2 );
  addPiConstraints( pSat, pMan1, pMan2, pCnf1, pCnf2 );

  outVar = addPoConstraints( pSat, pMan1, pMan2, pCnf1, pCnf2 );
  // end add cnf constraints to the sat solver

  unitAssumpLits[0] = toLitCond( outVar, 0 );

  // solve the sat problem
  satResult = sat_solver_solve( pSat, unitAssumpLits, unitAssumpLits + 1, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0, (ABC_INT64_T)0 );

  switch( satResult )
  {
    case l_True:  Abc_Print( ABC_STANDARD, "sat\n"        ); break;
    case l_False: Abc_Print( ABC_STANDARD, "unsat\n"      ); break;
    default:      Abc_Print( ABC_STANDARD, "undefined\n"  ); break;
  }
  // end solve the sat problem

  printAigCoTfi( pMan1 );

  // release memory
  sat_solver_delete( pSat );
  Cnf_DataFree( pCnf1 );
  Cnf_DataFree( pCnf2 );
  Aig_ManStop( pMan1 );
  Aig_ManStop( pMan2 );
  Abc_NtkDelete( pNtk2 );
  // end release memory
}

static int sat_solver_add_cnf( sat_solver_t *pSat, Cnf_Dat_t *pCnf )
{
  int success = true;

  for( int i = 0 ; i < pCnf->nClauses ; ++i )
     success &= sat_solver_addclause( pSat, pCnf->pClauses[i], pCnf->pClauses[i+1] );

  return success;
}

static void addPiConstraints( sat_solver_t *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 )
{
  // variable declaration
  Aig_Obj_t *pObj;
  Aig_Obj_t *pObj2;
  int       i;
  lit       lits[3];
  // end variable declaration

  // the Pi of the two circuits should be equivalent
  Aig_ManForEachCi( pMan1, pObj, i )
  {
    pObj2 = Aig_ManCi( pMan2, i );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 0 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
    sat_solver_addclause( pSat, lits, lits + 2 );

    lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 1 );
    lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
    sat_solver_addclause( pSat, lits, lits + 2 );
  }
  // end the Pi of the two circuits should be equivalent
}

static int addPoConstraints( sat_solver_t *pSat, Aig_Man_t *pMan1, Aig_Man_t *pMan2, Cnf_Dat_t *pCnf1, Cnf_Dat_t *pCnf2 )
{
  // variable declaration
  Aig_Obj_t *pObj   = Aig_ManCo( pMan1, 0 );
  Aig_Obj_t *pObj2  = Aig_ManCo( pMan2, 0 );
  int       outVar  = sat_solver_addvar( pSat ); // add new variable to the sat solver
  lit       lits[4];
  // end variable declaration

  // add xor to the first Po of the two circuits
  lits[2] = toLitCond( outVar, 0 );

  lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 0 );
  lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
  sat_solver_addclause( pSat, lits, lits + 2 );

  lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 1 );
  lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
  sat_solver_addclause( pSat, lits, lits + 2 );

  lits[2] = toLitCond( outVar, 1 );

  lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 0 );
  lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 0 );
  sat_solver_addclause( pSat, lits, lits + 2 );

  lits[0] = toLitCond( pCnf1->pVarNums[Aig_ObjId( pObj  )], 1 );
  lits[1] = toLitCond( pCnf2->pVarNums[Aig_ObjId( pObj2 )], 1 );
  sat_solver_addclause( pSat, lits, lits + 2 );
  // end add xor to the first Po of the two circuits

  return outVar;
}

static void printAigCoTfi( Aig_Man_t *pMan )
{
  // variable declaration
  Aig_Obj_t *objs[] = { Aig_ManCo( pMan, 0 ) };         // only get the TFI of the first Po
  Vec_Ptr_t *tfi    = Aig_ManDfsNodes( pMan, objs, 1 ); // get the TFI of objs
  Aig_Obj_t *pObj;
  int       i;
  // end variable declaration

  Aig_ManForEachObj( pMan, pObj, i )
  {
    if( Vec_PtrFind( tfi, pObj ) == -1 )
      Abc_Print( ABC_STANDARD, "%i is not the TFI of %i\n", Aig_ObjId( pObj ), Aig_ObjId( Aig_ManCo( pMan, 0 ) ) );
    else
      Abc_Print( ABC_STANDARD, "%i is the TFI of %i\n", Aig_ObjId( pObj ), Aig_ObjId( Aig_ManCo( pMan, 0 ) ) );
  }

  // release memory
  Vec_PtrFree( tfi );
  // end release memory
}
