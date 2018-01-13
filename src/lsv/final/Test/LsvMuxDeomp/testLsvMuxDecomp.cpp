#include "testLsvMuxDecomp.h"
#include "../../lsvMuxDecomp.h"

#include <iostream>
#include <string>
using namespace std;

// test functions
void testIsMuxDecomp()
{
  auto boolToString = []( bool value ) { return ( value ) ? "true": "false"; };

  const string errorTitle = "Is Mux Decomp Error";

  DdNode  node[3];
  DdNode  fA = { &node[0], &node[1] };
  DdNode  fB = { &node[0], &node[1] };
  DdNode  f  = { &fA, &fB };
  bool    result;

  const vector<bool>    correctResult = { true, false };
  const vector<DdNode*> pattern       = { &node[1], &node[2] };

  for( size_t i = 0 ; i < pattern.size() ; ++i )
  {
     fA.thenNode  = pattern[i];
     result       = isMuxDecomp( &f );

     bool expression = ( result == correctResult[i] );

     if( !assert( expression, errorTitle, to_string( correctResult[i] ), to_string( result ) ) ) return;
  }
  cout << "is mux decomp test pass\n";
}

void testBuildS() {}
// end test functions
bool assert( bool expression, const string &errorTitle, const string &correctResult, const string &programResult )
{
  if( !expression )
  {
    cerr << "[ " << errorTitle << " ]\n";
    cerr << "correct result: " << correctResult << "\n";
    cerr << "program result: " << programResult << "\n";
    return false;
  }
  return true;
}

// abstract model
// bdd model
DdNode* Cudd_T( DdNode *node )
{
  node->thenNode->complement = node->thenC;
  return node->thenNode;
}

DdNode* Cudd_E( DdNode *node )
{
  node->elseNode->complement = node->elseC;
  return node->elseNode;
}

DdNode* Cudd_Not( DdNode *node )
{
  node->complement ^= true;
  return node;
}

int Cudd_SupportSize( DdManager *dd, DdNode *f )
{
  DdNode  *node = f;
  int     size;

  for( bize = 0 ; ( node != dd->t ) && ( node != dd->f ) ; ++size )
     node = node->thenNode;
  return size;
}

unsigned int  Cudd_NodeReadIndex( DdNode *node          ) { return node->index; }
int           Cudd_ReadPerm     ( DdManager *dd, int i  ) { return i;           }

DdNode* Cudd_bddIthVar( DdManager *dd, int i ) 
{
  if( i > dd->var.size() ) return nullptr;
  return dd->var[i];
}

DdNode* Cudd_bddAnd( DdManager *dd, DdNode *f, DdNode *g )
{
  f->thenNode = g;
  f->thenC    = false;
  f->elseNode = dd->f;
  f->elseC    = false;
}

DdNode* Cudd_bddXor( DdManager *dd, DdNode *f, DdNode *g )
{
  f->thenNode = g;
  f->thenC    = true;
  f->elseNode = g;
  f->elseC    = false;
}

DdNode* Cudd_bddOr( DdManager *dd, DdNode *f, DdNode *g )
{
  f->elseNode = g;
  f->elseC    = false;
  f->thenNode = dd->t;
  f->thenC    = false;
}

int Abc_MinInt( int a, int b ) { return ( a < b ) ? a: b; }
// end bdd model

// vector model
Vec_Ptr_t*  Vec_PtrAlloc      ( int capacity                  ) { return new Vec_Ptr_t; }
void        Vec_PtrPushUnique ( Vec_Ptr_t *pVec, DdNode *node )
{
  for( DdNode *test : pVec->vec  )
     if( test == node ) return;

  pVec->vec.push_back( node );
}

size_t  Vec_PtrSize( Vec_Ptr_t *pVec ) { return pVec->vec.size(); }
void    Vec_PtrFree( Vec_Ptr_t *pVec ) { delete pVec;             }
// end vector model
// abstract model
