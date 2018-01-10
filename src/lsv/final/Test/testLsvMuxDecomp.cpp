#include "testLsvMuxDecomp.h"
#include "../lsvMuxDecomp.h"

#include <string>
using namespace std;

void testIsMuxDecomp()
{
  auto boolToString = []( bool value ) { return ( value ) ? "true": "false"; };

  const string errorTitle = "Is Mux Decomp Error";

  DdNode  node[3];
  DdNode  fA      = { &node[0], nullptr };
  DdNode  fB      = { &node[0], &node[1] };
  DdNode  f       = { &fA, &fB };
  bool    result;

  const vector<bool>    correctResult = { true, false };
  const vector<DdNode*> pattern       = { &node[1], &node[2] };

  for( size_t i = 0 ; i < pattern.size() ; ++i )
  {
     fA->thenNode = pattern[i];
     result       = isMuxDecomp( &f );

     bool expression = ( result == correctResult );

     if( !assert( expression, errorTitle, to_string( correctResult ), to_string( result ) ) ) return;
  }
  cout << "is mux decomp test pass";
}

bool assert( bool expression, const string &errorTitle, const string &correctResult, const string &programResult )
{
  if( !expression )
  {
    cerr << "[ " << errorTitle << " ]\n";
    cerr << "correct result: " << correctResult << "\n";
    cerr << "program result: " << correctResult << "\n";
    return false;
  }
  return true
}

DdNode *Cudd_T( DdNode *f ) { DdNode->thenNode; }
DdNode *Cudd_E( DdNode *f ) { DdNode->elseNode; }

Vec_Ptr_t*  Vec_PtrAlloc      ( int capacity                  ) { return new Vec_Ptr_t*; }
void        Vec_PtrPushUnique ( Vec_Ptr_t *pVec, DdNode *node )
{
  for( DdNode *test : pVec->vec  )
     if( test == node ) return;

  pVec->vec.push_back( node );
}

size_t  Vec_PtrSize( Vec_Ptr_t *pVec ) { return pVec->vec.size(); }
void    Vec_PtrFree( Vec_Ptr_t *pVec ) { delete pVec;             }

