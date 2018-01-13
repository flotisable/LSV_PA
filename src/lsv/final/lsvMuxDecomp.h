#ifndef LSV_MUX_DECOMP_H
#define LSV_MUX_DECOMP_H

struct DdManager;

// mux decomposition interface functions
int			NtkTransToBdd ( Abc_Ntk_t *pNtk								);
int			Cecsat		  ( Abc_Ntk_t * pNtk, Abc_Ntk_t * pNtk2			);
// end mux decomposition interface functions

// mux decomposition core functions
Vec_Ptr_t*  muxDecompCore ( DdManager *dd, DdNode *f                    );
bool        isMuxDecomp   ( DdNode *f                                   );
DdNode*     buildS        ( DdManager *dd, DdNode *f, DdNode *currentS  );
// end mux decomposition core functions

#endif
