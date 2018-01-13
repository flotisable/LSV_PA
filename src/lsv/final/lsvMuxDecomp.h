#ifndef LSV_MUX_DECOMP_H
#define LSV_MUX_DECOMP_H

struct DdManager;
struct DdNode;
struct Vec_Ptr_t;

// mux decomposition interface functions
// end mux decomposition interface functions

// mux decomposition core functions
Vec_Ptr_t*  muxDecompCore ( DdManager *dd, DdNode *f                    );
bool        isMuxDecomp   ( DdNode *f                                   );
DdNode*     buildS        ( DdManager *dd, DdNode *f, DdNode *currentS  );
// end mux decomposition core functions

#endif
