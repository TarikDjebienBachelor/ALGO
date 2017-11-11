#ifndef LINEAR_PGM_H
#define LINEAR_PGM_H 1

/*
   François Boulier. February 2006.
   The design could be improved.
*/

#include "bav.h"

enum relop { lesseq, greatereq, equal };

struct constraint {
    struct ba0_tableof_mpq lhs;
    enum relop op;
    mpq_t rhs;
};

/*
   alloc = the number of allocated entries
   size  = the number of entries actually used
   tab   = a pointer to an array of alloc pointers.
*/

struct tableof_constraint {
    ba0_int_p alloc;
    ba0_int_p size;
    struct constraint** tab;
};

enum minmax { minimize, maximize };

struct objective {
    struct ba0_tableof_mpq couts;
    enum minmax type;
    char z;
    mpq_t minus_z0;
};

/*
   Data type explanation
   ---------------------

   The field vars is the table of the used variables. Variables actually
   play the role of column indices.

   The field extra is a table of non used variables which might be used
   later (for slack variables, artificial variables ...).

   The field dual is a table of non used variables which might be used
   later for forming dual programs.

   The field rel is the coefficient matrix of the constraints.

   The field obj is the objective function.

   The field positive is the table of the nonnegative variables.

   The field type gives the type of the linear program.

   The field negative is the table of the nonpositive variables.
   The field anysign is the table of the variables whose signs are
	not constrained.
   These fields should be meaningless in the case of standard form
   linear programs. They are actually kept in order to allow the
   inverse changes of variables.

   The field slack is the table of the slack variables.
   The field anysign_neg is the table of the variables coding
   the negative parts of the anysign variables.
   The field artificial is the table of the artificial variables.

   The field basis provides the basis variables.

   standard form: maximize + equality constraints + positive variables
   tableau  form: standard form + a basis I (= a table of variables) such that
	- the sequence of columns associated to the basis variables form
	  an identity matrix
	- the costs (= objective entries) associated to the basis variables
	  are zero
	- the basis solution is componentwise nonnegative (= the rhs of
	  the constraint are nonnegative)
	- minus_z0 is equal to the opposite value of the objective function
	  evaluated over the basis solution
*/

enum typeof_pgm { general_form, canonical_form, standard_form, tableau };

struct linear_pgm {
    struct bav_tableof_variable vars;
    struct bav_tableof_variable extra;
    struct bav_tableof_variable dual;
    struct tableof_constraint rels;
    struct objective obj;
    struct bav_tableof_variable positive;
    enum typeof_pgm type;
    struct bav_tableof_variable negative;
    struct bav_tableof_variable anysign;
/* if tableau or standard_form */
    struct bav_tableof_variable slacks;
    struct bav_tableof_variable anysign_neg;
    struct bav_tableof_variable artificial;
/* if tableau */
    struct bav_tableof_variable basis;
};

enum typeof_sol { bounded, unbounded, infeasible };

#endif /* !LINEAR_PGM_H */

