#ifndef TEST_LSV_MUX_DECOMP
#define TEST_LSV_MUX_DECOMP

#include <vector>
#include <string>
using std::vector;

bool assert( bool expression, const string &errorTitle, const string &correctResult, const string &programResult );

struct DdNode
{
  DdNode *thenNode = nullptr;
  DdNode *elseNode = nullptr;
};

DdNode *Cudd_T( DdNode *f );
DdNode *Cudd_E( DdNode *f );

struct Vec_Ptr_t
{
  vector<DdNode*> vec;
};

Vec_Ptr_t*  Vec_PtrAlloc      ( int capacity                  );
void        Vec_PtrPushUnique ( Vec_Ptr_t *pVec, DdNode *node );
size_t      Vec_PtrSize       ( Vec_Ptr_t *pVec               );
void        Vec_PtrFree       ( Vec_Ptr_t *pVec               );
#endif
