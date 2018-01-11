#ifndef LSV_MUX_DECOMP_H
#define LSV_MUX_DECOMP_H

struct DdManager;

// mux decomposition interface functions
// end mux decomposition interface functions

// mux decomposition core functions
void    muxDecompCore ( DdManager *dd, DdNode *f  );
bool    isMuxDecomp   ( DdNode *f                 );
DdNode* buildS        ( DdManager *dd, DdNode *f  );
// end mux decomposition core functions

#endif
