/*
   Author: François Boulier
   February 2006.
   An implementation of the Dantzig algorithm for teaching purposes.

   It relies on the BLAD libraries and the GMP library.

   This is a very quick implementation. 
   The programming style could be much improved.
*/

#include "simplex.h"
#include "bap.h"

#define BUFSIZE 1024
static char buffer [BUFSIZE];

/*
   expr = scalar product (vars, vals)
*/

static void linear_expression 
	(bap_polynom_mpq expr, bav_tableof_variable vars, ba0_tableof_mpq vals)
{   static char buffer2 [BUFSIZE];
    ba0_int_p i;

    strcpy (buffer, "0");
    for (i = 0; i < vars->size; i++)
    {	ba0_sprintf (buffer2, " + (%q)*%v", vals->tab [i], vars->tab [i]);
	strcat (buffer, buffer2);
    }
    ba0_sscanf2 (buffer, "%Aq", expr);
}

/**********************************************************************
 CONSTRAINTS
 **********************************************************************/

void init_constraint (struct constraint* rel)
{
    ba0_init_table ((ba0_table)&rel->lhs);
    mpq_init (rel->rhs);
}

struct constraint* new_constraint ()
{
    struct constraint* rel;

    rel = (struct constraint*)ba0_alloc (sizeof (struct constraint));
    init_constraint (rel);
    return rel;
}

static void copy_constraint (struct constraint* A, struct constraint* B)
{   ba0_int_p i;

    A->lhs.size = 0;
    ba0_realloc2_table ((ba0_table)&A->lhs, B->lhs.size,
					(ba0_new_function*)&ba0_new_mpq);
    for (i = 0; i < B->lhs.size; i++)
	mpq_set (A->lhs.tab [A->lhs.size++], B->lhs.tab [i]);
    A->op = B->op;
    mpq_set (A->rhs, B->rhs);
}

static void set_constraint
        (struct linear_pgm* PL, bap_polynom_mpq P, enum relop op, mpq_t q)
{   struct bap_itermon_mpq iter;
    struct bap_polynom_mpq coeff;
    struct bav_term T;
    ba0_int_p i;

    ba0_realloc2_table ((ba0_table)&PL->rels, PL->rels.size + 1,
                                        (ba0_new_function*)&new_constraint);
    ba0_realloc2_table ((ba0_table)&PL->rels.tab [PL->rels.size]->lhs,
			PL->vars.size, (ba0_new_function*)&ba0_new_mpq);
    bap_init_polynom_mpq (&coeff);
    for (i = 0; i < PL->vars.size; i++)
    {
        bap_coeff_polynom_mpq (&coeff, P, PL->vars.tab [i], 1);
	if (! bap_is_zero_polynom_mpq (&coeff))
	    mpq_set (PL->rels.tab [PL->rels.size]->lhs.tab [i],
				*bap_numeric_initial_polynom_mpq (&coeff));
    }
    PL->rels.tab [PL->rels.size]->lhs.size = PL->vars.size;
    PL->rels.tab [PL->rels.size]->op = op;
    mpq_set (PL->rels.tab [PL->rels.size]->rhs, q);

    bav_init_term (&T);
    bap_end_itermon_mpq (&iter, P);
    bap_term_itermon_mpq (&T, &iter);
    if (bav_is_one_term (&T))
        mpq_sub (PL->rels.tab [PL->rels.size]->rhs, 
			PL->rels.tab [PL->rels.size]->rhs,
			*bap_coeff_itermon_mpq (&iter));
    bap_close_itermon_mpq (&iter);

    PL->rels.size += 1;
}

static void printf_constraint 
			(struct constraint* rel, bav_tableof_variable vars)
{   struct bap_polynom_mpq expr;
    struct ba0_mark M;

    ba0_record (&M);
    bap_init_polynom_mpq (&expr);
    linear_expression (&expr, vars, &rel->lhs);
    ba0_printf ("%Aq", &expr);
    switch (rel->op)
    {	case lesseq:
	    ba0_printf ("%s", " <= ");
	    break;
	case greatereq:
	    ba0_printf ("%s", " >= ");
	    break;
	case equal:
	    ba0_printf (" = ");
	    break;
     }
     ba0_printf ("%q\n", rel->rhs);
     ba0_restore (&M);
}

/**********************************************************************
 OBJECTIVE
 **********************************************************************/

void init_objective (struct objective* obj)
{
    ba0_init_table ((ba0_table)&obj->couts);
    mpq_init (obj->minus_z0);
    obj->z = 'z';
}

static void copy_objective (struct objective* A, struct objective* B)
{   ba0_int_p i;

    ba0_realloc2_table ((ba0_table)&A->couts, B->couts.size,
					(ba0_new_function*)&ba0_new_mpq);
    A->couts.size = 0;
    for (i = 0; i < B->couts.size; i++)
	mpq_set (A->couts.tab [A->couts.size++], B->couts.tab [i]);
    A->type = B->type;
    mpq_set (A->minus_z0, B->minus_z0);
    A->z = B->z;
}

static void set_objective 
	(struct linear_pgm* PL, bap_polynom_mpq P, enum minmax type, char z)
{   struct bap_itermon_mpq iter;
    struct bap_polynom_mpq coeff;
    struct bav_term T;
    ba0_int_p i;

    ba0_realloc2_table ((ba0_table)&PL->obj.couts, PL->vars.size,
					(ba0_new_function*)&ba0_new_mpq);
    bap_init_polynom_mpq (&coeff);
    PL->obj.type = type;
    for (i = 0; i < PL->vars.size; i++)
    {	bap_coeff_polynom_mpq (&coeff, P, PL->vars.tab [i], 1);
	if (! bap_is_zero_polynom_mpq (&coeff))
	    mpq_set (PL->obj.couts.tab [PL->obj.couts.size],
		*bap_numeric_initial_polynom_mpq (&coeff));
	PL->obj.couts.size += 1;
    }

    bav_init_term (&T);
    bap_end_itermon_mpq (&iter, P);
    bap_term_itermon_mpq (&T, &iter);
    if (bav_is_one_term (&T))
	mpq_neg (PL->obj.minus_z0, *bap_coeff_itermon_mpq (&iter));
    bap_close_itermon_mpq (&iter);

    PL->obj.z = z;
}

static void printf_objective (struct objective* obj, bav_tableof_variable vars)
{   struct bap_polynom_mpq expr;
    struct ba0_mark M;

    ba0_record (&M);
    bap_init_polynom_mpq (&expr);
    linear_expression (&expr, vars, &obj->couts);
    ba0_scanf_printf ("%Aq", "%Aq - %q", &expr, &expr, obj->minus_z0);
    ba0_printf ("%Aq", &expr);
    switch (obj->type)
    {	case minimize:
	    sprintf (buffer, " = %c[min]\n", obj->z);
	    break;
	case maximize:
	    sprintf (buffer, " = %c[max]\n", obj->z);
	    break;
     }
     ba0_printf ("%s", buffer);
     ba0_restore (&M);
}

/**********************************************************************
 LINEAR PGMS
 **********************************************************************/

void init_linear_pgm (struct linear_pgm* PL)
{
    ba0_init_table ((ba0_table)&PL->vars);
    ba0_init_table ((ba0_table)&PL->extra);
    ba0_init_table ((ba0_table)&PL->dual);
    ba0_init_table ((ba0_table)&PL->rels);
    init_objective (&PL->obj);
    ba0_init_table ((ba0_table)&PL->positive);
    PL->type = general_form;
    ba0_init_table ((ba0_table)&PL->negative);
    ba0_init_table ((ba0_table)&PL->anysign);
    ba0_init_table ((ba0_table)&PL->slacks);
    ba0_init_table ((ba0_table)&PL->anysign_neg);
    ba0_init_table ((ba0_table)&PL->artificial);
    ba0_init_table ((ba0_table)&PL->basis);
}

static void copy_linear_pgm (struct linear_pgm* A, struct linear_pgm* B)
{   ba0_int_p i;

    ba0_set_table ((ba0_table)&A->vars, (ba0_table)&B->vars);
    ba0_set_table ((ba0_table)&A->extra, (ba0_table)&B->extra);
    ba0_set_table ((ba0_table)&A->dual, (ba0_table)&B->dual);
    ba0_realloc2_table ((ba0_table)&A->rels, B->rels.size,
				(ba0_new_function*)&new_constraint);
    for (i = 0; i < B->rels.size; i++)
	copy_constraint (A->rels.tab [A->rels.size++], B->rels.tab [i]);
    copy_objective (&A->obj, &B->obj);
    ba0_set_table ((ba0_table)&A->positive, (ba0_table)&B->positive);
    A->type = B->type;
    ba0_set_table ((ba0_table)&A->negative, (ba0_table)&B->negative);
    ba0_set_table ((ba0_table)&A->anysign, (ba0_table)&B->anysign);
    ba0_set_table ((ba0_table)&A->slacks, (ba0_table)&B->slacks);
    ba0_set_table ((ba0_table)&A->anysign_neg, (ba0_table)&B->anysign_neg);
    ba0_set_table ((ba0_table)&A->artificial, (ba0_table)&B->artificial);
    ba0_set_table ((ba0_table)&A->basis, (ba0_table)&B->basis);
}

struct linear_pgm* new_linear_pgm ()
{
    struct linear_pgm* PL;

    PL = (struct linear_pgm*)ba0_alloc (sizeof (struct linear_pgm));
    init_linear_pgm (PL);
    return PL;
}

/**********************************************************************
 PRINTING LINEAR PGMS
 **********************************************************************/

void printf_linear_pgm (struct linear_pgm* PL)
{   ba0_int_p i;

    switch (PL->type)
    {	case canonical_form:
	    ba0_printf ("linear program under canonical form\n");
	    break;
	case general_form:
	    ba0_printf ("general linear program\n");
	    break;
	case standard_form:
	    ba0_printf ("linear program under standard form\n");
	    break;
	case tableau:
	    ba0_printf ("linear program equivalent to a tableau\n");
	    break;
    }
    printf_objective (&PL->obj, &PL->vars);
    for (i = 0; i < PL->rels.size; i++)
	printf_constraint (PL->rels.tab [i], &PL->vars);
    if (PL->positive.size > 0)
    {	for (i = 0; i < PL->positive.size - 1; i++)
	    ba0_printf ("%v, ", PL->positive.tab [i]);
	ba0_printf 
		("%v %s 0\n", PL->positive.tab [PL->positive.size - 1], ">=");
    }

    if (PL->type != general_form && PL->type != canonical_form)
	return;

    if (PL->negative.size > 0)
    {	for (i = 0; i < PL->negative.size - 1; i++)
	    ba0_printf ("%v, ", PL->negative.tab [i]);
	ba0_printf 
		("%v %s 0\n", PL->negative.tab [PL->negative.size - 1], "<=");
    }
    if (PL->anysign.size > 0)
    {	for (i = 0; i < PL->anysign.size - 1; i++)
	    ba0_printf ("%v, ", PL->anysign.tab [i]);
	ba0_printf ("%v of any sign\n", 
				PL->anysign.tab [PL->anysign.size - 1]);
    }
}

/*
   Restricted to linear programs of type "tableau"
*/

static void column_widths (ba0_tableof_int_p w, struct linear_pgm* PL)
{   ba0_int_p l, c;

    ba0_realloc_table ((ba0_table)w, PL->vars.size + 2);
    w->size = PL->vars.size + 2;

    w->tab [0] = 0;
    for (l = 0; l < PL->basis.size; l++)
    {	ba0_sprintf (buffer, "%v", PL->basis.tab [l]);
	if (w->tab [0] < strlen (buffer))
	    w->tab [0] = strlen (buffer);
    }

    for (c = 0; c < PL->vars.size; c++)
    {	ba0_sprintf (buffer, "%v", PL->vars.tab [c]);
	w->tab [c+1] = strlen (buffer);
	for (l = 0; l < PL->rels.size; l++)
	{   ba0_sprintf (buffer, "%q", PL->rels.tab [l]->lhs.tab [c]);
	    if (w->tab [c+1] < strlen (buffer))
		w->tab [c+1] = strlen (buffer);
	}
	ba0_sprintf (buffer, "%q", PL->obj.couts.tab [c]);
	if (w->tab [c+1] < strlen (buffer))
	    w->tab [c+1] = strlen (buffer);
    }
    w->tab [w->size - 1] = 0;
/*
    ba0_sprintf (buffer, "%q", PL->obj.minus_z0);
    w->tab [w->size - 1] = strlen (buffer);
*/
    for (l = 0; l < PL->rels.size; l++)
    {	ba0_sprintf (buffer, "%q", PL->rels.tab [l]->rhs);
	if (w->tab [w->size - 1] < strlen (buffer))
	    w->tab [w->size - 1] = strlen (buffer);
    }
}

static void multi_putchar (char c, int n)
{   int i;

    for (i = 0; i < n; i++)
	putchar (c);
}

void printf_tableau (struct linear_pgm* PL)
{   struct ba0_tableof_int_p w;
    ba0_int_p l, c, n;
    bool found;

    ba0_init_table ((ba0_table)&w);
    column_widths (&w, PL);

/*
   Line of variables
*/
    multi_putchar (' ', 1 + w.tab [0]);

    for (c = 0; c < PL->vars.size; c++)
    {	ba0_sprintf (buffer, "%v", PL->vars.tab [c]);
	n = w.tab [c+1] - strlen (buffer);
	multi_putchar (' ', 1 + n);
	printf ("%s", buffer);
	putchar (' ');
    }
    putchar ('\n');
/*
   Dotted line
*/
    multi_putchar (' ', 1 + w.tab [0]);
    for (c = 1; c < w.size; c++)
	multi_putchar ('-', 2 + w.tab [c]);
    multi_putchar ('-', 3);
    putchar ('\n');
/*
   Lines with coefficients
*/
    for (l = 0; l < PL->rels.size; l++)
    {	ba0_sprintf (buffer, "%v", PL->basis.tab [l]);
	multi_putchar (' ', w.tab [0] - strlen (buffer));
	printf ("%s", buffer);
	putchar (' ');

	for (c = 0; c < PL->vars.size; c++)
	{   ba0_sprintf (buffer, "%q", PL->rels.tab [l]->lhs.tab [c]);
	    n = w.tab [c+1] - strlen (buffer);
	    multi_putchar (' ', 1 + n);
	    printf ("%s", buffer);
	    putchar (' ');
	}
	printf (" | ");
	ba0_sprintf (buffer, "%q", PL->rels.tab [l]->rhs);
	n = w.tab [w.size - 1] - strlen (buffer);
	multi_putchar (' ', 1 + n);
	printf ("%s", buffer);
	putchar ('\n');
    }
/*
   Dotted line
*/
    multi_putchar (' ', 1 + w.tab [0]);
    for (c = 1; c < w.size; c++)
        multi_putchar ('-', 2 + w.tab [c]);
    multi_putchar ('-', 3);
    putchar ('\n');
/*
   Objective line
*/
    multi_putchar (' ', 1 + w.tab [0]);
    for (c = 0; c < PL->vars.size; c++)
    {	ba0_sprintf (buffer, "%q", PL->obj.couts.tab [c]);
	n = w.tab [c+1] - strlen (buffer);
	multi_putchar (' ', 1 + n);
	printf ("%s", buffer);
	putchar (' ');
    }
    printf (" | ");
    ba0_sprintf (buffer, "%q", PL->obj.minus_z0);
/*
    n = w.tab [w.size - 1] - strlen (buffer);
    multi_putchar (' ', 1 + n);
*/
    printf ("%s = - %c0\n", buffer, PL->obj.z);
    putchar ('\n');

    found = false;
    for (l = 0; l < PL->rels.size && ! found; l++)
	found = mpq_sgn (PL->rels.tab [l]->rhs) == 0;
    if (found)
	printf ("Multiple vertex\n\n");
}

/*
   Transforming a general linear program into standard form may require
   some changes of variables. The following function, applied to the
   transformed linear program, explains how to perform the inverse changes
   of variables.
*/

void printf_correspondence (struct linear_pgm* PL, bool objopp)
{   ba0_int_p i;

    if (PL->slacks.size > 0)
	ba0_printf ("slack variables = %t[%v]\n", &PL->slacks);
    if (PL->artificial.size > 0)
	ba0_printf ("artificial variables = %t[%v]\n", &PL->artificial);
    if (PL->negative.size > 0 || PL->anysign.size > 0 || objopp)
	ba0_printf ("After resolution ...\n");
    if (PL->negative.size > 0)
	ba0_printf ("take the opposite values of %t[%v]\n", &PL->negative);
    if (PL->anysign.size > 0)
    {   for (i = 0; i < PL->anysign.size; i++)
	    ba0_printf ("take %v = %v - %v\n", 
		    PL->anysign.tab [i], PL->anysign.tab [i], 
		    PL->anysign_neg.tab [i]);
    }
    if (objopp)
	ba0_printf ("take the opposite value of the objective\n");
}

/**********************************************************************
 RECOVERING THE OPTIMAL SOLUTION
 **********************************************************************/

/*
   Data type explanation
   ---------------------

   Solutions are stored in tables of mpq_t.
   An extra entry is allocated at the end of the tables to store the objective.
*/

/*
   PL is a program under standard form obtained from the general linear
   program PL0. The parameter x is the optimal solution of PL.

   The following function performs over x the inverse change of variables
   described just above. It thereby obtains the optimal solution of PL0
   and stores it in x0.
*/

void corresponding_basis_solution 
		(ba0_tableof_mpq x0, struct linear_pgm* PL0,
		 ba0_tableof_mpq x, struct linear_pgm* PL)
{   ba0_int_p i, j;

    ba0_realloc2_table ((ba0_table)x0, PL0->vars.size + 1,
		    (ba0_new_function*)&ba0_new_mpq);
    for (x0->size = 0; x0->size < PL0->vars.size; x0->size++)
    {	if (ba0_member_table 
		    (PL0->vars.tab [x0->size], (ba0_table)&PL->negative))
	    mpq_neg (x0->tab [x0->size], x->tab [x0->size]);
        else if (ba0_member2_table
		    (PL0->vars.tab [x0->size], (ba0_table)&PL->anysign, &i))
	{   ba0_member2_table 
			(PL->anysign_neg.tab [i], (ba0_table)&PL->vars, &j);
	    mpq_sub (x0->tab [x0->size], x->tab [x0->size], x->tab [j]);
	} else
	    mpq_set (x0->tab [x0->size], x->tab [x0->size]);
    }
    if (PL0->obj.type == PL->obj.type)
	mpq_set (x0->tab [x0->size++], x->tab [x->size - 1]);
    else
	mpq_neg (x0->tab [x0->size++], x->tab [x->size - 1]);
}
 
/*
   PL is the last tableau produced by the Dantzig algorithm over the
   dual of PL0. Assigns the optimal solution of PL0 to x0.
*/

void basis_solution_from_dual
	(ba0_tableof_mpq x0, struct linear_pgm* PL0, struct linear_pgm* PL)
{
    ba0_realloc2_table ((ba0_table)x0, PL0->vars.size + 1,
			(ba0_new_function*)&ba0_new_mpq);
    for (x0->size = 0; x0->size < PL0->vars.size; x0->size++)
	mpq_neg (x0->tab [x0->size], 
			PL->obj.couts.tab [PL0->rels.size + x0->size]);
    if (PL0->obj.type == PL->obj.type)
	mpq_set (x0->tab [PL0->vars.size], PL->obj.minus_z0);
    else
	mpq_neg (x0->tab [PL0->vars.size], PL->obj.minus_z0);
    x0->size = PL0->vars.size + 1;
}

/*
   Compute the optimal solution of PL. Result in x.
*/

void basis_solution (ba0_tableof_mpq x, struct linear_pgm* PL)
{   ba0_int_p i;
    
    ba0_realloc2_table ((ba0_table)x, PL->vars.size + 1,
		    			(ba0_new_function*)&ba0_new_mpq);
    for (x->size = 0; x->size < PL->vars.size; x->size++)
    {	
	if (! ba0_member2_table 
			(PL->vars.tab [x->size], (ba0_table)&PL->basis, &i))
	    mpq_set_ui (x->tab [x->size], 0, 1);
	else
	    mpq_set (x->tab [x->size], PL->rels.tab [i]->rhs);
    }
    mpq_neg (x->tab [x->size++], PL->obj.minus_z0);
}

void printf_solution (ba0_tableof_mpq x, struct linear_pgm* PL)
{   ba0_int_p i;

    ba0_printf ("Basis solution: ");
    for (i = 0; i < x->size - 1; i++)
    {	if (i > 0)
	    ba0_printf (", ");
	ba0_printf ("%v = %q", PL->vars.tab [i], x->tab [i]);
    }
    ba0_printf ("\nObjective %q\n", x->tab [x->size - 1]);
}

/**********************************************************************
 PARSING LINEAR PGMS
 **********************************************************************/

static void get_operator_rhs (enum relop* op, mpq_t q)
{
    if (ba0_sign_token_analex ("="))
	*op = equal;
    else if (ba0_sign_token_analex (">") &&
			(ba0_get_token_analex (), ba0_sign_token_analex ("=")))
	*op = greatereq;
    else if (ba0_sign_token_analex ("<") &&
			(ba0_get_token_analex (), ba0_sign_token_analex ("=")))
	*op = lesseq;
    else
    {	fprintf (stderr, "syntax error\n");
	exit (1);
    }
    ba0_get_token_analex ();
    ba0_scanf ("%q", q);
}

static void get_variables (bav_tableof_variable T, enum relop* op, mpq_t q)
{   bav_variable v;

    while (ba0_sign_token_analex (","))
    {	
	ba0_get_token_analex ();
	ba0_scanf ("%v", &v);
	if (! ba0_member_table (v, (ba0_table)T))
	{   ba0_realloc_table ((ba0_table)T, T->size + 1);
	    T->tab [T->size++] = v;
	} else
	{   fprintf (stderr, "variable defined twice\n");
	    exit (1);
	}
	ba0_get_token_analex ();
    }
    get_operator_rhs (op, q);
}

static void set_variable (bav_tableof_variable T, bav_variable v)
{
    ba0_realloc_table ((ba0_table)T, 1);
    T->tab [0] = v;
    T->size = 1;
}

static void append_variables (bav_tableof_variable T, bav_tableof_variable U)
{   ba0_int_p i;

    ba0_realloc_table ((ba0_table)T, T->size + U->size);
    for (i = 0; i < U->size; i++)
	if (! ba0_member_table (U->tab [i], (ba0_table)T))
	    T->tab [T->size++] = U->tab [i];
}

static void generate_extra_vars (char* extra, char* vars, ba0_int_p n)
{
    ba0_int_p i, j;
    char x;
 
    if (isalpha ((int)vars [0]))
	x = vars [0];
    else
	x = 'x';

    j = 0;
    i = 1;
    while (j < n)
    {	sprintf (buffer, "%c%d", x, (int)i);
	if (! strstr (vars, buffer))
	{   if (j != 0)
		strcat (extra, ", ");
	    strcat (extra, buffer);
	    j += 1;
	}
	i += 1;
    }
}

static void generate_dual_vars (char* dual, char* vars, ba0_int_p n)
{   ba0_int_p i, j;
    char y;

    if (isalpha ((int)vars [0]) && vars [0] == 'y')
	y = 'x';
    else
	y = 'y';

    j = 0;
    i = 1;
    while (j < n)
    {   sprintf (buffer, "%c%d", y, (int)i);
	if (! strstr (vars, buffer))
	{   if (j != 0)
		strcat (dual, ", ");
	    strcat (dual, buffer);
            j += 1;
        }
        i += 1;
    }
}

static void set_anysign (struct linear_pgm* PL)
{   ba0_int_p i;
/*
   The variables of any sign are those which are neither positive nor negative
*/
    ba0_realloc_table ((ba0_table)&PL->anysign, PL->vars.size);
    for (i = 0; i < PL->vars.size; i++)
    {   if (! ba0_member_table (PL->vars.tab [i], (ba0_table)&PL->positive) &&
            ! ba0_member_table (PL->vars.tab [i], (ba0_table)&PL->negative))
            PL->anysign.tab [PL->anysign.size++] = PL->vars.tab [i];
    }

}

/*
   This function is called from the parser.
   It sets the type of the linear program.
   It only chooses between general_form, canonical_form and standard_form 
	(not tableau). 
*/

static void set_type (struct linear_pgm* PL)
{   ba0_int_p i;
    enum relop op;
    bool same_op;

    if (PL->rels.size == 0)
    {	fprintf (stderr, "empty set of constraints\n");
	exit (1);
    }

    PL->type = general_form;

    if (PL->negative.size != 0 || PL->anysign.size != 0)
	return;

    same_op = true;
    op = PL->rels.tab [0]->op;
    for (i = 1; i < PL->rels.size && same_op; i++)
	same_op = op == PL->rels.tab [i]->op;

    if (! same_op)
	return;

    if ((op == lesseq && PL->obj.type == maximize) ||
	(op == greatereq && PL->obj.type == minimize))
	PL->type = canonical_form;
    else if (op == equal && PL->obj.type == maximize)
	PL->type = standard_form;
}

/*
   For the parser convenience
*/

enum status_type { success, retry, giveup };

/*
   Parses a linear program.
   The parser restarts the BLAD libraries
*/

struct linear_pgm* scanf_linear_pgm (char* dirname, char* fname) 
{   struct linear_pgm* PL;
    struct linear_pgm* PL2;
    static char vars [BUFSIZE];
    static char extra [BUFSIZE];
    static char dual [BUFSIZE];
    struct bap_polynom_mpq p;
    bav_Iordering r;
    struct bav_tableof_variable T;
    bav_variable v;
    bav_symbol dmin, dmax;
    struct ba0_exception_code code;
    struct ba0_mark M;
    mpq_t q;
    enum relop op;
    enum status_type status;
    ba0_int_p i;
    FILE* f;
/*
   First check that the file exists and is readable
*/
    sprintf (buffer, "%s/%s", dirname, fname);
    if ((f = fopen (buffer, "r")) == NULL)
    {	fprintf (stderr, "unable to open: %s\n", buffer);
	exit (1);
    } else
	fclose (f);
/*
   Main loop
*/
    vars [0] = '\0';
    extra [0] = '\0';
    dual [0] = '\0';
    do
    {	bap_restart (0, 0);
	ba0_push_another_stack ();
	ba0_record (&M);

	PL = new_linear_pgm ();

	sprintf (buffer, 
	    "ordering (derivations = [min, max], blocks = [[%s], [%s], [%s]])", 
	    vars, extra, dual);
	ba0_sscanf2 (buffer, "%ordering", &r);
	bav_R_push_ordering (r);

	ba0_sscanf2 ("min", "%y", &dmin);
	ba0_sscanf2 ("max", "%y", &dmax);

	bap_init_polynom_mpq (&p);
	ba0_init_table ((ba0_table)&T);
	mpq_init (q);

	sprintf (buffer, "%s/%s", dirname, fname);
	f = fopen (buffer, "r");

	ba0_set_analex_FILE (f);

	BA0_PUSH_EXCEPTION (code);

	if (ba0_exception_is_raised (code))
	{   if (ba0_mesgerr == BAV_ERRUSY)
	    {   if (vars [0] == '\0')
		    strcat (vars, bav_unknown);
	        else
	        {   strcat (vars, ", ");
		    strcat (vars, bav_unknown);
	        }
		fclose (f);
		bap_terminate (ba0_init_level);
	        status = retry;
	    } else
		status = giveup;
	} else
	{
	    ba0_scanf_printf ("%t[%v]", "[%s]", &PL->vars, vars);
	    ba0_scanf_printf ("%t[%v]", "[%s]", &PL->extra, extra);
	    ba0_scanf_printf ("%t[%v]", "[%s]", &PL->dual, dual);
/*
   Read the objective
*/
	    ba0_scanf2 ("%Aq = %v", &p, &v);
/*
   Suppress the objective identifier from the list of variables
*/
	    i = 0;
	    while (i < PL->vars.size && 
			PL->vars.tab [i] != bav_order_zero_variable (v))
		i++;
	    ba0_delete_table ((ba0_table)&PL->vars, i);
/*
   Set the objective
*/
	    if (bav_order_variable (v, dmax) == 1)
		set_objective (PL, &p, maximize, v->root->ident [0]);
	    else if (bav_order_variable (v, dmin) == 1)
		set_objective (PL, &p, minimize, v->root->ident [0]);
	    else
	    {	fprintf (stderr, "syntax error in the objective\n");
		exit (1);
	    }
/*
   Inner loop to read the constraints
*/
	    do
	    {
		ba0_get_token_analex ();
	        if (ba0_type_token_analex () != ba0_no_token)
	        {   
		    ba0_scanf ("%Aq", &p);
		    ba0_get_token_analex ();
		    if (ba0_sign_token_analex (",") &&
					bap_is_variable_polynom_mpq (&p))
		    {	set_variable (&T, bap_leader_polynom_mpq (&p));
			get_variables (&T, &op, q);
			if (mpq_sgn (q) != 0 || op == equal)
			{   fprintf (stderr, "bad list of variables\n");
			    exit (1);
			} else if (op == lesseq)
			    append_variables (&PL->negative, &T);
			else 
			    append_variables (&PL->positive, &T);
		    } else
		    {	get_operator_rhs (&op, q);
			if (mpq_sgn (q) == 0 && op != equal &&
					bap_is_variable_polynom_mpq (&p))
			{   set_variable (&T, bap_leader_polynom_mpq (&p));
			    if (op == lesseq)
				append_variables (&PL->negative, &T);
			    else
				append_variables (&PL->positive, &T);
			} else
			    set_constraint (PL, &p, op, q);
		    }
	        } 
	    } while (ba0_type_token_analex () != ba0_no_token);

	    ba0_pull_exception (code);

	    fclose (f);
	    ba0_reset_analex ();

	    if (PL->extra.size == 0)
	    {	generate_extra_vars 
			(extra, vars, 2 * (PL->vars.size + PL->rels.size));
		generate_dual_vars 
			(dual, vars, 2 * (PL->vars.size + PL->rels.size));
		bap_terminate (ba0_init_level);
		status = retry;
	    } else
		status = success;
	}
    } while (status == retry);

    if (status == giveup)
    {	fprintf (stderr, "syntax error\n");
	exit (1);
    }

    set_anysign (PL);
    set_type (PL);

    ba0_pull_stack ();
    PL2 = new_linear_pgm ();
    copy_linear_pgm (PL2, PL);
    ba0_restore (&M);

    return PL2;
}
 
/**********************************************************************
 CONVERTING LINEAR PGMS TO STANDARD FORM
 **********************************************************************/

/*
   This function applies to a standard form linear program.
   It decides if the linear program is a tableau or not.
*/

static void set_type2 (struct linear_pgm* PL)
{   ba0_int_p l, c;
    bool found;

    found = PL->slacks.size != PL->rels.size;
    c = PL->vars.size - PL->slacks.size - PL->anysign_neg.size;
    for (l = 0; l < PL->rels.size && ! found; l++)
    {	found = mpq_sgn (PL->rels.tab [l]->lhs.tab [c]) < 0;
	c++;
    }
    PL->type = found ? standard_form : tableau;
}

/*
   Converts a general linear program into standard form.
   In the case of a linear program which is already under standard
   form, the function returns a copy of its parameter.
*/

struct linear_pgm* to_standard_form (struct linear_pgm* PL)
{   struct linear_pgm *PL2, *PL3;
    struct ba0_mark M;
    ba0_int_p slacks, anynegs;
    ba0_int_p i, j, k, l;

    ba0_push_another_stack ();
    ba0_record (&M);

    PL2 = new_linear_pgm ();
    copy_linear_pgm (PL2, PL);
    if (PL->type == tableau || PL->type == standard_form)
	goto fin;
/*
   Counting slack variables and one new variable per anysign
*/
    slacks = 0;
    for (i = 0; i < PL->rels.size; i++)
	if (PL->rels.tab [i]->op != equal)
	    slacks++;
    anynegs = PL->anysign.size;
/*
   Updating vars, positive and anysign_neg
*/
    ba0_realloc_table 
		((ba0_table)&PL2->vars, PL->vars.size + slacks + anynegs);
    ba0_realloc_table 
		((ba0_table)&PL2->positive, 
		 PL->vars.size + slacks + PL->negative.size + anynegs);
    ba0_realloc_table ((ba0_table)&PL2->slacks, slacks);
    ba0_realloc_table ((ba0_table)&PL2->anysign_neg, anynegs);
    for (i = 0; i < slacks; i++)
    {	PL2->vars.tab [PL2->vars.size++] = PL2->extra.tab [0];
	PL2->positive.tab [PL2->positive.size++] = PL2->extra.tab [0];
	PL2->slacks.tab [PL2->slacks.size++] = PL2->extra.tab [0];
	ba0_delete_table ((ba0_table)&PL2->extra, 0);
    }
    for (i = 0; i < PL->negative.size; i++)
	PL2->positive.tab [PL2->positive.size++] = PL->negative.tab [i];
    for (i = 0; i < anynegs; i++)
    {	PL2->vars.tab [PL2->vars.size++] = PL2->extra.tab [0];
	PL2->positive.tab [PL2->positive.size++] = PL2->extra.tab [0];
	PL2->anysign_neg.tab [PL2->anysign_neg.size++] = PL2->extra.tab [0];
	ba0_delete_table ((ba0_table)&PL2->extra, 0);
    }
/*
   Resizing the objective
*/
    ba0_realloc2_table ((ba0_table)&PL2->obj.couts, 
				PL->vars.size + slacks + anynegs,
				(ba0_new_function*)&ba0_new_mpq);
    PL2->obj.couts.size = PL->vars.size + slacks + anynegs;
    if (PL->obj.type == minimize)
    {   PL2->obj.type = maximize;
	for (i = 0; i < PL2->obj.couts.size; i++)
	    mpq_neg (PL2->obj.couts.tab [i], PL2->obj.couts.tab [i]);
	mpq_neg (PL2->obj.minus_z0, PL2->obj.minus_z0);
    }
/*
   Resizing and updating the constraints
*/
    j = PL->vars.size;
    for (i = 0; i < PL->rels.size; i++)
    {	ba0_realloc2_table ((ba0_table)&PL2->rels.tab [i]->lhs, 
				PL->vars.size + slacks + anynegs,
				(ba0_new_function*)&ba0_new_mpq);
	PL2->rels.tab [i]->lhs.size = PL->vars.size + slacks + anynegs;
/*
   Introduce a slack variable
*/
	if (PL->rels.tab [i]->op == lesseq)
	    mpq_set_si (PL2->rels.tab [i]->lhs.tab [j++], 1, 1);
	else if (PL->rels.tab [i]->op == greatereq)
	    mpq_set_si (PL2->rels.tab [i]->lhs.tab [j++], -1, 1);
/*
   Update operator
*/
	PL2->rels.tab [i]->op = equal;
/*
   Set the rhs positive
*/
	if (mpq_sgn (PL2->rels.tab [i]->rhs) < 0)
	{   mpq_neg (PL2->rels.tab [i]->rhs, PL2->rels.tab [i]->rhs);
	    for (l = 0; l < PL2->vars.size; l++)
		mpq_neg (PL2->rels.tab [i]->lhs.tab [l],
			 PL2->rels.tab [i]->lhs.tab [l]);
	}
    }
/*
   Update anysign
*/
    k = PL->vars.size + slacks;
    for (i = 0; i < PL->vars.size; i++)
    {   if (ba0_member_table (PL->vars.tab [i], (ba0_table)&PL->anysign))
	{   for (j = 0; j < PL->rels.size; j++)
		mpq_neg (PL2->rels.tab [j]->lhs.tab [k], 
			 PL2->rels.tab [j]->lhs.tab [i]);
	    mpq_neg (PL2->obj.couts.tab [k], PL2->obj.couts.tab [i]);
	    k++;
	}
    }
/*
   Change the sign for the negative variables
*/
    for (i = 0; i < PL->vars.size; i++)
    {	if (ba0_member_table (PL->vars.tab [i], (ba0_table)&PL->negative))
	{   mpq_neg (PL2->obj.couts.tab [i], PL2->obj.couts.tab [i]);
	    for (j = 0; j < PL2->rels.size; j++)
		mpq_neg (PL2->rels.tab [j]->lhs.tab [i],
			 PL2->rels.tab [j]->lhs.tab [i]);
	}
    }
    set_type2 (PL2);
    if (PL2->type == tableau)
	ba0_set_table ((ba0_table)&PL2->basis, (ba0_table)&PL2->slacks);
fin:
    ba0_pull_stack ();
    PL3 = new_linear_pgm ();
    copy_linear_pgm (PL3, PL2);
    ba0_restore (&M);
    return PL3;
}

/**********************************************************************
 CREATING THE DUAL PROGRAM
 **********************************************************************/

/*
   The PL is assumed to be under canonical form with z0 = 0
*/

struct linear_pgm* dual_pgm (struct linear_pgm* PL)
{   struct linear_pgm* PL2;
    ba0_int_p i, j;

    PL2 = new_linear_pgm ();
    PL2->type = PL->type;
/*
   The objective
*/
    mpq_set (PL2->obj.minus_z0, PL->obj.minus_z0);
    PL2->obj.z = PL->obj.z == 'z' ? 'w' : 'z';
    PL2->obj.type = PL->obj.type == maximize ? minimize : maximize;
    
    ba0_realloc2_table ((ba0_table)&PL2->obj.couts, PL->rels.size,
                                        (ba0_new_function*)&ba0_new_mpq);
    for (i = 0; i < PL->rels.size; i++)
	mpq_set (PL2->obj.couts.tab [PL2->obj.couts.size++], 
						PL->rels.tab [i]->rhs);
/*
   The constraints
*/
    ba0_realloc2_table ((ba0_table)&PL2->rels, PL->vars.size,
					(ba0_new_function*)&new_constraint);
    for (i = 0; i < PL->vars.size; i++)
    {	ba0_realloc2_table ((ba0_table)PL2->rels.tab [i], PL->rels.size,
					(ba0_new_function*)&ba0_new_mpq);
	for (j = 0; j < PL->rels.size; j++)
	    mpq_set (PL2->rels.tab [i]->lhs.tab [j], 
					PL->rels.tab [j]->lhs.tab [i]);
	PL2->rels.tab [i]->lhs.size = PL->rels.size;
	PL2->rels.tab [i]->op = PL2->obj.type == maximize ? lesseq : greatereq;
	mpq_set (PL2->rels.tab [i]->rhs, PL->obj.couts.tab [i]);
	PL2->rels.size += 1;
    }
/*
   The variables
*/
    ba0_set_table ((ba0_table)&PL2->extra, (ba0_table)&PL->dual);

    ba0_realloc_table ((ba0_table)&PL2->vars, PL->rels.size);
    for (i = 0; i < PL->rels.size; i++)
    {	PL2->vars.tab [PL2->vars.size++] = PL2->extra.tab [0];
	ba0_delete_table ((ba0_table)&PL2->extra, 0);
    }
    ba0_set_table ((ba0_table)&PL2->positive, (ba0_table)&PL2->vars);
    ba0_concat_table ((ba0_table)&PL2->dual, 
				(ba0_table)&PL->vars, (ba0_table)&PL->extra);

    return PL2;
}

/**********************************************************************
 CONVERTING STANDARD FORM TO TABLEAU BY MEANS OF ARTIFICIAL VARIABLES
 **********************************************************************/

/*
   PL is a linear program under standard form but it is not a tableau.

   The following function computes a so-called artificial program,
   which is a tableau and whose optimal solution will provide the
   initial tableau of PL.
*/

struct linear_pgm* phase_I_pgm (struct linear_pgm* PL)
{   struct linear_pgm *PL2, *PL3;
    struct ba0_mark M;
    ba0_int_p i, l, c, k, first_slack, first_non_slack, nbarts, nbvars, ia, is;

    nbvars = PL->vars.size;

    ba0_push_another_stack ();
    ba0_record (&M);

    PL2 = new_linear_pgm ();
    copy_linear_pgm (PL2, PL);
    PL2->obj.z = PL->obj.z == 'z' ? 'w' : 'z';
/*
   Counting the number of artificial variables needed
*/
    if (PL2->slacks.size > 0)
    {	ba0_member2_table 
	    	(PL2->slacks.tab [0], (ba0_table)&PL2->vars, &first_slack);
	first_non_slack = first_slack + PL2->slacks.size;
    } else
	first_non_slack = first_slack = 0;
    nbvars = PL2->vars.size;	/* nbvars *before* creation of artificial */
    nbarts = 0;			/* number of artificial variables */
    l = 0;
    c = first_slack;
    while (l < PL2->rels.size)
    {   if (c < first_non_slack && 
				mpq_sgn (PL2->rels.tab [l]->lhs.tab [c]) > 0)
	{   l++;
            c++;
        } else
        {   nbarts++;
            if (c < first_non_slack &&
			    	mpq_sgn (PL2->rels.tab [l]->lhs.tab [c]) < 0)
                c++;
            l++;
        } 
    }
/*
   Creating them and resizing the tables
*/
    PL2->artificial.size = 0;
    ba0_realloc_table ((ba0_table)&PL2->artificial, nbarts);
    ba0_realloc_table ((ba0_table)&PL2->vars, nbvars + nbarts);
    ba0_realloc_table ((ba0_table)&PL2->positive, PL2->positive.size + nbarts);
    for (i = 0; i < nbarts; i++)
    {	PL2->artificial.tab [PL2->artificial.size++] = PL2->extra.tab [0];
	PL2->vars.tab [PL2->vars.size++] = PL2->extra.tab [0];
	PL2->positive.tab [PL2->positive.size++] = PL2->extra.tab [0];
	ba0_delete_table ((ba0_table)&PL2->extra, 0);
    }
/*
   Resizing the objective and resetting it to zero
*/
    for (i = 0; i < PL2->obj.couts.size; i++)
	mpq_set_ui (PL2->obj.couts.tab [i], 0, 1);
    mpq_set_ui (PL2->obj.minus_z0, 0, 1);
    ba0_realloc2_table ((ba0_table)&PL2->obj.couts, PL2->vars.size,
				(ba0_new_function*)&ba0_new_mpq);
    PL2->obj.couts.size = PL2->vars.size;
/*
   Resizing the constraints
*/
    for (i = 0; i < PL2->rels.size; i++)
    {	ba0_realloc2_table ((ba0_table)&PL2->rels.tab [i]->lhs, PL2->vars.size,
				(ba0_new_function*)&ba0_new_mpq);
	PL2->rels.tab [i]->lhs.size = PL2->vars.size;
    }
/*
   Resizing the basis
*/
    PL2->basis.size = 0;
    ba0_realloc_table ((ba0_table)&PL2->basis, PL2->rels.size);
/*
   Inserting artificial variables in the relation.
   Computing the initial basis.
   Reformulating the artificial objective so that the costs associated
   to the basis variables are all zero.
*/
    l = 0;
    c = first_slack;	/* first column index for slack variables */
    k = nbvars;		/* first column index for artificial variables */
    ia = 0;
    while (l < PL2->rels.size)
    {   is = c - first_slack;
	if (c < first_non_slack &&
			    mpq_sgn (PL2->rels.tab [l]->lhs.tab [c]) > 0)
	{   PL2->basis.tab [PL2->basis.size++] = PL2->slacks.tab [is];
	    l++;
	    c++;
	} else 
	{   PL2->basis.tab [PL2->basis.size++] = PL2->artificial.tab [ia++];
	    mpq_set_ui (PL2->rels.tab [l]->lhs.tab [k++], 1, 1);
	    for (i = 0; i < nbvars; i++)
		mpq_add (PL2->obj.couts.tab [i], PL2->obj.couts.tab [i], 
			 PL2->rels.tab [l]->lhs.tab [i]);
	    mpq_add (PL2->obj.minus_z0, PL2->obj.minus_z0, 
			 PL2->rels.tab [l]->rhs);
	    if (c < first_non_slack &&
			    	mpq_sgn (PL2->rels.tab [l]->lhs.tab [c]) < 0)
		c++;
	    l++;
	} 
    }
    PL2->type = tableau;
    ba0_pull_stack ();
    PL3 = new_linear_pgm ();
    copy_linear_pgm (PL3, PL2);
    ba0_restore (&M);
    return PL3;
}

/*
   Subfunction of the Dantzig algorithm which may be called at the beginning
   of phase II in order to remove artificial variables from the basis.
*/

static void pivoter (struct linear_pgm* PL, ba0_int_p lig, ba0_int_p col);

/*
   PL is under standard form but is not a tableau.
   An artificial linear program was computed form PL. 
   The Dantzig algorithm was applied over it (phase I), providing a
   last tableau: PLA. We checked already that PL is feasible.

   The following function recovers an initial tableau from PLA and PL,
   which is equivalent to PL (phase II).
*/

struct linear_pgm* phase_II_pgm (struct linear_pgm* PLA, struct linear_pgm* PL)
{   struct linear_pgm *PL2, *PL3;
    struct ba0_mark M;
    ba0_int_p i, c, l;
    mpq_t tmp;
    bool extra;
    
    ba0_push_another_stack ();
    ba0_record (&M);

    PL2 = new_linear_pgm ();
    copy_linear_pgm (PL2, PLA);
    PL2->obj.z = PL->obj.z;
/*
   If some artificial variables remain in the basis at the end of phase I,
   they can be withdrawn by some extra pivoting. This is the task of the
   following loop.

		x1 - 2*x2 + x3 = z[min]
		3*x1 + x2 - x3 = 1
		-2*x1 + x2 - 2*x3 = 1
		x1, x2, x3 >= 0
*/
    extra = false;
    for (i = 0; i < PL2->basis.size; i++)
    {	if (ba0_member2_table 
		    (PL2->basis.tab [i], (ba0_table)&PL2->artificial, &l))
        {   c = 0;
            while (c < PL2->vars.size && 
			(ba0_member_table 
				(PL2->vars.tab [c], (ba0_table)&PL2->basis) ||
			mpq_sgn (PL2->rels.tab [i]->lhs.tab [c]) == 0))
	        c++;
	    pivoter (PL2, i, c);
	    extra = true;
	}
    }
    if (extra)
    {	printf ("Artificial variables removed from the basis\n\n");
	printf_tableau (PL2);
    }
/*
  Shrinking all the tables (removal of the coordinates corresponding to
  the artificial variables).
*/ 
    ba0_member2_table (PL2->artificial.tab [0], (ba0_table)&PL2->vars, &c);
    PL2->vars.size = c;
    PL2->positive.size = c;
    PL2->artificial.size = 0;
    PL2->obj.couts.size = c;
    for (i = 0; i < PL2->rels.size; i++)
	PL2->rels.tab [i]->lhs.size = c;
/*
   Reformulating the objective so that the costs corresponding to the basis
   variables are zero.
*/
    mpq_set (PL2->obj.minus_z0, PL->obj.minus_z0);
    mpq_init (tmp);
    for (c = 0; c < PL2->vars.size; c++)
	mpq_set_ui (PL2->obj.couts.tab [c], 0, 1);
    for (c = 0; c < PL2->vars.size; c++)
    {	if (ba0_member_table (PL2->vars.tab [c], (ba0_table)&PL2->basis) &&
		    mpq_sgn (PL->obj.couts.tab [c]) != 0)
	{   l = 0;
	    while (mpq_sgn (PL2->rels.tab [l]->lhs.tab [c]) == 0)
		l++;
	    for (i = 0; i < PL2->vars.size; i++)
	    {	if (i != c)
		{   mpq_mul (tmp, PL->obj.couts.tab [c], 
					PL2->rels.tab [l]->lhs.tab [i]);
		    mpq_sub (PL2->obj.couts.tab [i], 
				    	PL2->obj.couts.tab [i], tmp);
		}
	    }
	    mpq_mul (tmp, PL->obj.couts.tab [c], PL2->rels.tab [l]->rhs);
	    mpq_sub (PL2->obj.minus_z0, PL2->obj.minus_z0, tmp);
	} else
	    mpq_add (PL2->obj.couts.tab [c], 
			    	PL2->obj.couts.tab [c], PL->obj.couts.tab [c]);
    }
    ba0_pull_stack ();
    PL3 = new_linear_pgm ();
    copy_linear_pgm (PL3, PL2);
    ba0_restore (&M);
    return PL3;
}

/**********************************************************************
 THE DANTZIG ALGORITHM 
 **********************************************************************/

/*
   Constraint dst -= k * Constraint src
*/

static void soustraire_k_fois_ligne_a_ligne 
		(struct linear_pgm* PL, mpq_t k, ba0_int_p src, ba0_int_p dst)
{   mpq_t tmp;
    ba0_int_p c;

    mpq_init (tmp);
    for (c = 0; c < PL->vars.size; c++)
    {	mpq_mul (tmp, k, PL->rels.tab [src]->lhs.tab [c]);
	mpq_sub (PL->rels.tab [dst]->lhs.tab [c],
		 PL->rels.tab [dst]->lhs.tab [c], tmp);
    }
    mpq_mul (tmp, k, PL->rels.tab [src]->rhs);
    mpq_sub (PL->rels.tab [dst]->rhs, PL->rels.tab [dst]->rhs, tmp);
}

/*
   Objective -= k * Constraint src
*/

static void soustraire_k_fois_ligne_a_objective
			(struct linear_pgm* PL, mpq_t k, ba0_int_p src)
{   mpq_t tmp;
    ba0_int_p c;

    mpq_init (tmp);
    for (c = 0; c < PL->vars.size; c++)
    {   mpq_mul (tmp, k, PL->rels.tab [src]->lhs.tab [c]);
	mpq_sub (PL->obj.couts.tab [c], PL->obj.couts.tab [c], tmp);
    }
    mpq_mul (tmp, k, PL->rels.tab [src]->rhs);
    mpq_sub (PL->obj.minus_z0, PL->obj.minus_z0, tmp);
}

/*
   A pivot stands at the col-th column of constraint number lig.
   Performs one step of a reduced row echelon form process.
*/

static void pivoter (struct linear_pgm* PL, ba0_int_p lig, ba0_int_p col)
{   mpq_t pivot, k;
    ba0_int_p l, c;
/*
   Make pivot monic
*/
    mpq_init (k);
    mpq_init (pivot);
    mpq_set (pivot, PL->rels.tab [lig]->lhs.tab [col]);
    for (c = 0; c < PL->vars.size; c++)
    {	mpq_div (PL->rels.tab [lig]->lhs.tab [c],
		 PL->rels.tab [lig]->lhs.tab [c], pivot);
    }
    mpq_div (PL->rels.tab [lig]->rhs, PL->rels.tab [lig]->rhs, pivot);
/*
   Pivote
*/
    for (l = 0; l < PL->rels.size; l++)
    {	if (l != lig)
	{   mpq_set (k, PL->rels.tab [l]->lhs.tab [col]);
	    soustraire_k_fois_ligne_a_ligne (PL, k, lig, l);
	}
    }
    mpq_set (k, PL->obj.couts.tab [col]);
    soustraire_k_fois_ligne_a_objective (PL, k, lig);
/*
   Update basis
*/
    PL->basis.tab [lig] = PL->vars.tab [col];
}

/*
   Looks for the next pivot column by scanning the objective (looks for a
   positive cost). Returns the column number or -1 if not found.
*/

static ba0_int_p pivot_column (struct linear_pgm* PL)
{   ba0_int_p c, col;

    col = 0;
    for (c = 1; c < PL->vars.size; c++)
    {	ba0_int_p status;

	status = mpq_cmp (PL->obj.couts.tab [c], PL->obj.couts.tab [col]);
	if (status > 0)
	    col = c;
    }
    if (mpq_sgn (PL->obj.couts.tab [col]) <= 0)
	return -1;
    else
	return col;
}

/*
   The column of the next pivot was found (it is c).
   Looks now for a row. Returns the row index or -1 if not found.
*/

static ba0_int_p pivot_row (struct linear_pgm* PL, ba0_int_p c)
{
    mpq_t min, quotient;
    ba0_int_p l, lmin;

    lmin = 0;
    while (lmin < PL->rels.size && 
			mpq_sgn (PL->rels.tab [lmin]->lhs.tab [c]) <= 0)
        lmin++;
/*
   If no positive entry on column c then there does not exist any pivot.
*/
    if (lmin == PL->rels.size)
	return -1;

    mpq_init (min);
    mpq_init (quotient);
/*
   Looks for the index of the smallest ratio
*/
    mpq_div (min, PL->rels.tab [lmin]->rhs, PL->rels.tab [lmin]->lhs.tab [c]);
    for (l = lmin + 1; l < PL->rels.size; l++)
    {   if (mpq_sgn (PL->rels.tab [l]->lhs.tab [c]) > 0)
        {   mpq_div (quotient, PL->rels.tab [l]->rhs, 
					PL->rels.tab [l]->lhs.tab [c]);
            if (mpq_cmp (quotient, min) < 0)
            {   mpq_set (min, quotient);
                lmin = l;
            }
        }
    }

    return lmin;
}

/*
   The Dantzig algorithm with the usual non guaranteed strategy.
   It is assumed that PL is a tableau. 
   It loop enumerates many different tableaux.
   It eventually proves that PL is unbounded or that PL is bounded.
   In that last case, the optimal solution of PL can be read from
   the last computed tableau (which is returned).

   The non guaranteed strategy may lead the algorithm into infinite loops.

		3/4*x1 - 20*x2 + 1/2*x3 - 6*x4 = z[max]
		1/4*x1 - 8*x2 - x3 + 9*x4 <= 0
		1/2 * x1 - 12*x2 - 1/2*x3 + 3*x4 <= 0
		x3 <= 1
		x1, x2, x3, x4 >= 0

   Even in the case of a finite number of loops, the number of loops may 
   reach 2**n where n denotes the number of constraints or of variables. 
   The following example provides an example for n=4. It is generic.

		8*x1 + 4*x2 + 2*x3 + x4 = z[max]
		x1 <= 5
		4*x1 + x2 <= 25
		8*x1 + 4*x2 + x3 <= 125
		16*x1 + 8*x2 + 4*x3 + x4 <= 625
		x1, x2, x3, x4 >= 0
*/

struct linear_pgm* Dantzig (enum typeof_sol* answer, struct linear_pgm* PL)
{   struct linear_pgm *PL2, *PL3;
    struct ba0_mark M;
    ba0_int_p l, c;

    ba0_push_another_stack ();
    ba0_record (&M);
    PL2 = new_linear_pgm ();
    copy_linear_pgm (PL2, PL);
    printf_tableau (PL2);
    for (;;)
    {	c = pivot_column (PL2);
	if (c == -1)
	{   *answer = bounded;
	    break;
	}
	l = pivot_row (PL2, c);
	if (l == -1)
	{   *answer = unbounded;
	    break;
	}
	pivoter (PL2, l, c);
	printf_tableau (PL2);
    }
    ba0_pull_stack ();
    PL3 = new_linear_pgm ();
    copy_linear_pgm (PL3, PL2);
    ba0_restore (&M);
    return PL3;
}

/**********************************************************************
 MAIN
 **********************************************************************/

static void usage ()
{
    fprintf (stderr, "usage: simplex [-dual] filename\n");
    exit (1);
}

int main (int argc, char **argv)
{   struct linear_pgm *PL, *PLD, *PL1, *PL2, *PL3, *PL4, *PL5, *PL6;
    struct ba0_tableof_mpq x, x1, x3, x6;
    enum typeof_sol answer;
    char* fname;
    FILE* f;
    bool dual;

    dual = false;
    if (argc == 2)
    {   fname = argv [1];
	if ((f = fopen (fname, "r")) == NULL)
	    usage ();
	fclose (f);
    } else if (argc == 3)
    {	fname = argv [2];
	if (strcmp (argv [1], "-dual") != 0 || ! (f = fopen (fname, "r")))
	    usage ();
	dual = true;
	fclose (f);
    } else
	usage ();
/*
   The following call activates the BLAD libraries
*/
    PL = scanf_linear_pgm (".", fname);
    printf ("INITIAL LINEAR PROGRAM\n\n");
    printf_linear_pgm (PL);
/*
   Computation of the dual program
*/
    if (dual)
    {	if (PL->type != canonical_form)
	{   fprintf (stderr, "The dual construction of non canonical linear programs is not implemented\n");
	    exit (1);
	}
	PLD = dual_pgm (PL);
	printf ("\n------------------------------------------------------------\n");
	printf ("\nDUAL PROGRAM\n\n");
	printf_linear_pgm (PLD);
	PL1 = PLD;
    } else
	PL1 = PL;
/*
   Computation of the standard form
*/
    PL2 = to_standard_form (PL1);
    printf ("\n------------------------------------------------------------\n");
    printf ("\nSAME PROGRAM UNDER STANDARD FORM\n\n");
    printf_linear_pgm (PL2);
    printf_correspondence (PL2, PL1->obj.type != PL2->obj.type);
    printf ("\n------------------------------------------------------------\n");
/*
   The tableau case. The Dantzig algorithm can be applied directly.
   Such linear programs are necessarily feasible. A bounded example

		4000*x1 + 5000*x2 + 100000 = z[max]
		2*x1 + x2 <= 8
		x1 + 2*x2 <= 7
		x2 <= 3
		x1, x2 >= 0

   An unbounded example

		-x1 -x2 = z[min]
		3*x1 + 4*x2 >= 12
		2*x1 + x2 >= 4
		x1 >= 0
*/
    if (PL2->type == tableau)
    {	
	printf ("\nAPPLICATION OF THE DANTZIG ALGORITHM\n\n");
	PL3 = Dantzig (&answer, PL2);
	if (answer == unbounded)
	{   printf ("Unbounded linear program\n");
	    goto fin;
	}
	ba0_init_table ((ba0_table)&x);
	ba0_init_table ((ba0_table)&x1);
	ba0_init_table ((ba0_table)&x3);
	basis_solution (&x3, PL3);
	printf_solution (&x3, PL3);
	printf ("\nSolution of the %s program\n", dual ? "dual" : "initial");
	corresponding_basis_solution (&x1, PL1, &x3, PL3);
	printf_solution (&x1, PL1);
        if (dual)
        {   printf ("\nSolution of the initial program\n");
            basis_solution_from_dual (&x, PL, PL3);
            printf_solution (&x, PL);
        }
 	goto fin;
    } 
/*
   The general case. We solve it by means of the two phases methods.
   A bounded example


		1000*x1 + 2000*x2 = z[min]
		8*x1 + 2*x2 >= 16
		x1 + x2 >= 5
		2*x1 + 7*x2 >= 20
		x1, x2 >= 0

   An unbounded example

		2*x1 - 3*x2 + 14 = z[max]
		x1 + x2 <= 5/3
		2*x1 - x2 + x3*1/7 = -4/2
		x1 + x4 >= -5
		x1, x4 >= 0
		x2 <= 0

   An infeasible example

		x1 - 2*x2 = z[min]
		x1 + x2 <= 3
		-x1 + 3*x2 <= -4
		x1, x2 >= 0
*/
    printf ("\nPHASE I. RESOLUTION OF THE ARTIFICIAL PROGRAM:\n\n");
/*
   The artificial linear program associated to PL
*/
    PL3 = phase_I_pgm (PL2);
    printf_linear_pgm (PL3);
    printf_correspondence (PL3, false);
    printf ("\n------------------------------------------------------------\n");
    printf ("\nPHASE I. APPLICATION OF THE DANTZIG ALGORITHM\n\n");
    PL4 = Dantzig (&answer, PL3);
/*
   Infeasible linear programs
*/
    if (mpq_sgn (PL4->obj.minus_z0) != 0)
    {   printf ("Infeasible linear pgm\n");
	goto fin;
    }
    printf ("Feasible linear pgm\n");
    printf ("\n------------------------------------------------------------\n");
    printf ("\nPHASE II. APPLICATION OF THE DANTZIG ALGORITHM\n\n");
/*
   PL5 is a tableau equivalent to PL.
   The process below is the same as in the tableau case above.
*/
    PL5 = phase_II_pgm (PL4, PL2);
    PL6 = Dantzig (&answer, PL5);
    if (answer == unbounded)
    {   printf ("Unbounded linear program\n");
	goto fin;
    }
    ba0_init_table ((ba0_table)&x);
    ba0_init_table ((ba0_table)&x1);
    ba0_init_table ((ba0_table)&x6);
    basis_solution (&x6, PL6);
    printf_solution (&x6, PL6);
    printf ("\nSolution of the %s program\n", dual ? "dual" : "initial");
    corresponding_basis_solution (&x1, PL1, &x6, PL6);
    printf_solution (&x1, PL1);
    if (dual)
    {	printf ("\nSolution of the initial program\n");
	basis_solution_from_dual (&x, PL, PL6);
	printf_solution (&x, PL);
    }
fin:
    bap_terminate (ba0_init_level);
    return 0;
}
