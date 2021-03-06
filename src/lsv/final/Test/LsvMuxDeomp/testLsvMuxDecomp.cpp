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

void testBuildS()
{
  // variable declarations
  const string errorTitle = "Build S Error";

  DdNode    fa;
  DdNode    fb;
  DdManager dd;
  DdNode    top     = { &fb, &fa };
  DdNode    t;
  DdNode    *f      = Cudd_Not( &t );
  DdNode    node[2] = { { &t, f }, { &t, f } };
  DdNode    *s;

  const vector<vector<DdNode*>> correctResult =
    {
      //00  01  10  11
      {  f,  f,  f, &t },  // and
      {  f, &t, &t, &t },  // or
      {  f, &t, &t,  f }   // xor
    };
  const vector<vector<DdNode*>> pattern =
    {
      //   00       01       10       11
      { &node[0], &node[0], &node[0], &node[1] },  // and
      { &node[0], &node[1], &node[1], &node[1] },  // or
      { &node[0], &node[1], &node[1], &node[0] }   // xor
    };
  // end variable declarations

  // initialize variables
  dd.top = &top;
  dd.t   = &t;
  dd.f   = f;
  dd.var.resize( 2, { &t, f } );

  top.index = 0;
  fa.index  = 1;
  fb.index  = 1;
  s = &(dd.var[0]);
  // end initialize variables

  for( size_t i = 0 ; i < pattern.size() ; ++i )
  {
     DdNode *fs;
     DdNode *sThenNode;
     DdNode *sElseNode;

     // setup patterns
     fa.elseNode = pattern[i][0];
     fa.thenNode = pattern[i][1];
     fb.elseNode = pattern[i][2];
     fb.thenNode = pattern[i][3];
     // end setup patterns

     s->thenNode  = &t;
     s->elseNode  = f;
     fs           = buildS( &dd, dd.top, s ); 

     if( Cudd_T( fs ) ) sThenNode = Cudd_T( fs );
     if( Cudd_E( fs ) ) sElseNode = Cudd_E( fs );

     DdNode *y[] =
       { ( ( Cudd_E( sElseNode ) ) ? Cudd_E( sElseNode ): sElseNode ),    // 00
         ( ( Cudd_T( sElseNode ) ) ? Cudd_T( sElseNode ): sElseNode ),    // 01
         ( ( Cudd_E( sThenNode ) ) ? Cudd_E( sThenNode ): sThenNode ),    // 10
         ( ( Cudd_T( sThenNode ) ) ? Cudd_T( sThenNode ): sThenNode ) };  // 11

     bool expression = ( y[0] == correctResult[i][0] ) && // 00
                       ( y[1] == correctResult[i][1] ) && // 01
                       ( y[2] == correctResult[i][2] ) && // 10
                       ( y[3] == correctResult[i][3] );   // 11
     const string correctString = to_string( correctResult[i][0] == &t ) +
                                  to_string( correctResult[i][1] == &t ) +
                                  to_string( correctResult[i][2] == &t ) +
                                  to_string( correctResult[i][3] == &t );
     const string programString = to_string( y[0] == &t ) +
                                  to_string( y[1] == &t ) +
                                  to_string( y[2] == &t ) +
                                  to_string( y[3] == &t );

     if( !assert( expression, errorTitle, correctString, programString ) ) return;
  }
  cout << "build s test pass\n";
}
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
DdNode* Cudd_Regular( DdNode *node ) { return reinterpret_cast<DdNode*>( reinterpret_cast<long>( node ) & ~1 ); }
bool Cudd_IsComplement( DdNode *node ) { return reinterpret_cast<long>( node ) & 1; }
DdNode* Cudd_T( DdNode *node )
{
  DdNode *thenNode = Cudd_Regular( node )->thenNode;

  if( !thenNode ) return nullptr;
  return Cudd_IsComplement( node ) ? Cudd_Not( thenNode ): thenNode;
}

DdNode* Cudd_E( DdNode *node )
{
  DdNode *elseNode = Cudd_Regular( node )->elseNode;

  if( !elseNode ) return nullptr;
  return Cudd_IsComplement( node ) ? Cudd_Not( elseNode ): elseNode;
}

DdNode* Cudd_Not( DdNode *node ) { return reinterpret_cast<DdNode*>( reinterpret_cast<long>( node ) ^ 1 ); }
int Cudd_SupportSize( DdManager *dd, DdNode *f )
{
  DdNode  *node = f;
  int     size;

  for( size = 0 ; ( node != dd->t ) && ( node != dd->f ) ; ++size )
     node = Cudd_Regular( node )->thenNode;
  return size;
}

unsigned int  Cudd_NodeReadIndex( DdNode *node          ) { return Cudd_Regular( node )->index; }
int           Cudd_ReadPerm     ( DdManager *dd, int i  ) { return i;           }

DdNode* Cudd_bddIthVar( DdManager *dd, int i ) 
{
  if( i > dd->var.size() ) return nullptr;
  return &(dd->var[i]);
}

DdNode* Cudd_bddAnd( DdManager *dd, DdNode *f, DdNode *g )
{
  f->thenNode = g;
  f->elseNode = dd->f;

  return f;
}

DdNode* Cudd_bddXor( DdManager *dd, DdNode *f, DdNode *g )
{
  f->thenNode = Cudd_Not( g );
  f->elseNode = g;

  return f;
}

DdNode* Cudd_bddOr( DdManager *dd, DdNode *f, DdNode *g )
{
  f->elseNode = g;
  f->thenNode = dd->t;

  return f;
}

DdNode*       Cudd_bddNewVar( DdManager *dd ) { return nullptr; }
void          Cudd_Ref( DdNode *node ) {}
void          Cudd_RecursiveDeref( DdManager *dd, DdNode *node ) {}
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
void    Vec_PtrPush       ( Vec_Ptr_t *pVec, DdNode *node             ) {}
DdNode* Vec_PtrEntry      ( Vec_Ptr_t *pVec, int    i                 ) { return nullptr; }
void    Vec_PtrWriteEntry ( Vec_Ptr_t *pVec, int    i, DdNode *entry  ) {}
// end vector model
// abstract model
