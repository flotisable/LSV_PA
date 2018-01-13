#ifndef TEST_LSV_MUX_DECOMP
#define TEST_LSV_MUX_DECOMP

#include <vector>
#include <string>
using std::vector;
using std::string;

bool assert( bool expression, const string &errorTitle, const string &correctResult, const string &programResult );

// test functions
void testIsMuxDecomp();
void testBuildS     ();
// end test functions

// abstract model
// bdd model
struct DdNode;

struct DdManager
{
  DdNode *top;
  DdNode *t;
  DdNode *f;

  vector<DdNode> var;
};

struct DdNode
{
  DdNode  *thenNode = nullptr;
  DdNode  *elseNode = nullptr;
  int     index;

  DdNode() = default;
  DdNode( DdNode *thenNode, DdNode *elseNode ) : thenNode{ thenNode }, elseNode{ elseNode } {}
};

DdNode*       Cudd_Regular( DdNode *node );
bool          Cudd_IsComplement( DdNode *node );
DdNode*       Cudd_T  ( DdNode *node );
DdNode*       Cudd_E  ( DdNode *node );
DdNode*       Cudd_Not( DdNode *node );
int           Cudd_SupportSize( DdManager *dd, DdNode *f );
unsigned int  Cudd_NodeReadIndex( DdNode *node );
int           Cudd_ReadPerm( DdManager *dd, int i );
DdNode*       Cudd_bddIthVar( DdManager *dd, int i );
DdNode*       Cudd_bddAnd( DdManager *dd, DdNode *f, DdNode *g );
DdNode*       Cudd_bddXor( DdManager *dd, DdNode *f, DdNode *g );
DdNode*       Cudd_bddOr( DdManager *dd, DdNode *f, DdNode *g );
DdNode*       Cudd_bddNewVar( DdManager *dd );
void          Cudd_Ref( DdNode *node );
void          Cudd_RecursiveDeref( DdManager *dd, DdNode *node );
int           Abc_MinInt( int a, int b );
// end bdd model

// vector model
struct Vec_Ptr_t
{
  vector<DdNode*> vec;
};

Vec_Ptr_t*  Vec_PtrAlloc      ( int capacity                              );
void        Vec_PtrPushUnique ( Vec_Ptr_t *pVec, DdNode *node             );
size_t      Vec_PtrSize       ( Vec_Ptr_t *pVec                           );
void        Vec_PtrFree       ( Vec_Ptr_t *pVec                           );
void        Vec_PtrPush       ( Vec_Ptr_t *pVec, DdNode *node             );
DdNode*     Vec_PtrEntry      ( Vec_Ptr_t *pVec, int    i                 );
void        Vec_PtrWriteEntry ( Vec_Ptr_t *pVec, int    i, DdNode *entry  );
// end vector model
// end abstract model

#endif
