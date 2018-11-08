#pragma once 

#include "boost/range/algorithm/set_algorithm.hpp"

#include <crab/config.h>
#include <crab/common/debug.hpp>
#include <crab/common/stats.hpp>
#include <crab/common/types.hpp>
#include <crab/domains/operators_api.hpp>
#include <crab/domains/domain_traits.hpp>
#include <crab/domains/intervals.hpp>

namespace crab {
   namespace domains {
      typedef enum {
	ELINA_ZONES,
	ELINA_OCT, 
	ELINA_PK } elina_domain_id_t;
   }
}

#ifndef HAVE_ELINA
/*
 * Dummy implementation if Elina not found 
 */
#define ELINA_NOT_FOUND "No Elina. Run cmake with -DUSE_ELINA=ON"

namespace crab {
   namespace domains {
      template<typename Number, typename VariableName, elina_domain_id_t ElinaDom>
      class elina_domain: 
	public abstract_domain<Number,VariableName,elina_domain<Number,VariableName,ElinaDom> > {
              
       public:
        typedef elina_domain<Number, VariableName, ElinaDom> elina_domain_t;
	typedef abstract_domain<Number, VariableName, elina_domain_t> abstract_domain_t;
        using typename abstract_domain_t::linear_expression_t;
        using typename abstract_domain_t::linear_constraint_t;
        using typename abstract_domain_t::linear_constraint_system_t;
	using typename abstract_domain_t::disjunctive_linear_constraint_system_t;
        using typename abstract_domain_t::variable_t;
        using typename abstract_domain_t::number_t;
        using typename abstract_domain_t::varname_t;
        typedef interval <Number> interval_t;

        elina_domain() {}    

        static elina_domain_t top() { CRAB_ERROR(ELINA_NOT_FOUND); }

        static elina_domain_t bottom() { CRAB_ERROR(ELINA_NOT_FOUND); }

        elina_domain (const elina_domain_t& other) {}
        
        bool is_bottom() { CRAB_ERROR(ELINA_NOT_FOUND); }

        bool is_top() { CRAB_ERROR(ELINA_NOT_FOUND); }

        bool operator<=(elina_domain_t other) { CRAB_ERROR(ELINA_NOT_FOUND); }

        void operator|=(elina_domain_t other)
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        elina_domain_t operator|(elina_domain_t other)
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        elina_domain_t operator&(elina_domain_t other) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        elina_domain_t operator||(elina_domain_t other)
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        template<typename Thresholds>
        elina_domain_t widening_thresholds (elina_domain_t e, const Thresholds &ts) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        elina_domain_t operator&& (elina_domain_t other) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void operator-=(variable_t var) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        interval_t operator[](variable_t v) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        void set(variable_t v, interval_t ival) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        void operator += (linear_constraint_system_t csts) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void assign (variable_t x, linear_expression_t e) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
          
        void apply (operation_t op, variable_t x, variable_t y, Number z) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void apply(operation_t op, variable_t x, variable_t y, variable_t z) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void apply(operation_t op, variable_t x, Number k) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        void apply(int_conv_operation_t op, variable_t dst, variable_t src) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
	
        void apply(bitwise_operation_t op, variable_t x, variable_t y, variable_t z) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void apply(bitwise_operation_t op, variable_t x, variable_t y, Number k) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void apply(div_operation_t op, variable_t x, variable_t y, variable_t z) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void apply(div_operation_t op, variable_t x, variable_t y, Number k) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void backward_assign (variable_t x, linear_expression_t e,
			      elina_domain_t invariant) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
          
        void backward_apply (operation_t op,
			     variable_t x, variable_t y, Number z,
			     elina_domain_t invariant) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        void backward_apply(operation_t op,
			    variable_t x, variable_t y, variable_t z,
			    elina_domain_t invariant) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
        
        linear_constraint_system_t to_linear_constraint_system ()
        { CRAB_ERROR(ELINA_NOT_FOUND); }

        disjunctive_linear_constraint_system_t to_disjunctive_linear_constraint_system ()
        { CRAB_ERROR(ELINA_NOT_FOUND); }
	
        void write(crab_os& o) 
        { CRAB_ERROR(ELINA_NOT_FOUND); }
          
        static std::string getDomainName () {
          return "Dummy Elina";
        }  
      }; 
   } // namespace domains
}// namespace crab
#else

/* 
 * Real implementation starts here.
 * The code is a copy-and-paste version of apron_domains.hpp.
 * Thanks to Gagandeep Singh. 
 */

#include <crab/domains/elina/elina.hpp>
#include <boost/bimap.hpp>
#include <crab/domains/linear_interval_solver.hpp>

namespace crab {
namespace domains {
  
  using namespace elina;

  template<typename Number, typename VariableName, elina_domain_id_t ElinaDom>
  class elina_domain_:
    public abstract_domain<Number,VariableName,
			   elina_domain_<Number,VariableName,ElinaDom> > {
    
    typedef elina_domain_<Number, VariableName, ElinaDom> elina_domain_t;
    typedef abstract_domain<Number,VariableName,elina_domain_t> abstract_domain_t;
  public:
    using typename abstract_domain_t::linear_expression_t;
    using typename abstract_domain_t::linear_constraint_t;
    using typename abstract_domain_t::linear_constraint_system_t;
    using typename abstract_domain_t::disjunctive_linear_constraint_system_t;    
    using typename abstract_domain_t::variable_t;
    using typename abstract_domain_t::number_t;
    using typename abstract_domain_t::varname_t;
    using typename abstract_domain_t::variable_vector_t;	
    typedef interval <Number> interval_t;
    
  private:
    typedef interval_domain <Number, VariableName> interval_domain_t;
    typedef bound <Number> bound_t;
    typedef boost::bimap<variable_t, elina_dim_t > var_map_t;
    typedef typename var_map_t::value_type binding_t;
    
    static elina_manager_t* m_apman;
    
    elina_state_ptr m_apstate; 
    var_map_t m_var_map;
    
    static elina_manager_t* get_man () {
      if (!m_apman) {
	switch (ElinaDom) {
	case ELINA_ZONES: m_apman = opt_zones_manager_alloc(); break;
	case ELINA_OCT: m_apman = opt_oct_manager_alloc(); break;
	case ELINA_PK : m_apman = opt_pk_manager_alloc(false); break;
	default: CRAB_ERROR("unknown elina domain");
	}
      }
      return m_apman;
    }
    
    size_t get_dims (elina_state_ptr s) const {
      elina_dimension_t dims = _elina_abstract0_dimension (&*s);
      return dims.intdim;
    }
    
    size_t get_dims () const { return get_dims (m_apstate); }
    
    // If v is in the map then it maps v to a dimension, otherwise null
    boost::optional<elina_dim_t> get_var_dim (const var_map_t& m, variable_t v) const {
      auto it = m.left.find (v);
      if (it != m.left.end ())
	return it->second;
      else
	return boost::optional<elina_dim_t> ();
    }
    
    boost::optional<elina_dim_t> get_var_dim (variable_t v) const {
      return get_var_dim (m_var_map, v);
    }
    
    elina_dim_t get_var_dim_insert (variable_t v) {
      assert (m_var_map.size () == get_dims ());
      if (auto dim = get_var_dim (v))
	return *dim;
      else {
	elina_dim_t i = m_var_map.size ();
	m_var_map.insert (binding_t (v, i));
	add_dimensions (m_apstate, 1);
	assert (m_var_map.size () == get_dims ());
	return i;
      }
    }
    
    bool has_variable (const var_map_t& m, elina_dim_t i) const {
      return m.right.find (i) != m.right.end ();
    }
    
    bool has_variable (elina_dim_t i) const {
      return has_variable (m_var_map, i);
    }
    
    variable_t get_variable (const var_map_t& m, elina_dim_t i) const {
      auto it = m.right.find (i);
      if (it != m.right.end ())
	return it->second;            
      CRAB_ERROR("Elina dimension ", i, " is not used!");
    }
    
    variable_t get_variable (elina_dim_t i) const {
      return get_variable (m_var_map, i);
    }
    
    void add_dimensions (elina_state_ptr& s, size_t dims) const {
      if (dims <= 0) return;
      
      elina_dimchange_t* dimchange =  elina_dimchange_alloc (dims, 0);
      for (unsigned i=0 ; i<dims ; i++)
	dimchange->dim[i] = get_dims (s); // add dimension at the end
      
      s = elinaPtr (get_man (), 
		    elina_abstract0_add_dimensions(get_man (), false, 
						   &*s, dimchange, false));
      elina_dimchange_free (dimchange);
      
    }
    
    void remove_dimensions (elina_state_ptr& s, std::vector<elina_dim_t> dims) const {
      if (dims.empty ()) return;
      
      // Elina assumption: make sure that the removing dimensions
      //                   are in ascending order.
      std::sort(dims.begin(), dims.end());
      
      elina_dimchange_t* dimchange =  elina_dimchange_alloc (dims.size (), 0);
      for (unsigned i=0; i<dims.size () ; i++) {
	// remove dimension dims[i] and shift to the left all the
	// dimensions greater than dims[i]
	dimchange->dim[i] = dims[i]; 
      }
      
      s = elinaPtr (get_man (), 
		    elina_abstract0_remove_dimensions(get_man (), false, 
						      &*s, dimchange));
      elina_dimchange_free (dimchange);
      
      #if 0
      crab::outs() << "Removed " << dims.size () << " dimensions\n";
      crab::outs() << "Size = " << get_dims (s) << "\n";
      #endif           
    }

    bool check_perm (elina_dimperm_t* perm, size_t size){
      // it does not check injectivity
      if (perm->size != size) return false;
      for (unsigned i=0; i<perm->size; i++){
	if (perm->dim[i]>=size){
	  return false;
	}
      }
          return true;
    }
    
    var_map_t merge_var_map (const var_map_t& m_x, elina_state_ptr& s_x,
			     const var_map_t& m_y, elina_state_ptr& s_y) {
	  
      assert (m_x.size () == get_dims (s_x));
      assert (m_y.size () == get_dims (s_y));
      
      // -- collect all vars from the two maps
      std::set<variable_t> vars;
      for (auto const& px: m_x.left)
	vars.insert (px.first);
      for (auto const& py: m_y.left)
	vars.insert (py.first);
      
      assert (vars.size () >= get_dims (s_x));
      assert (vars.size () >= get_dims (s_y));
      
      add_dimensions (s_x, vars.size () - get_dims (s_x));
      add_dimensions (s_y, vars.size () - get_dims (s_y));
      
      assert (get_dims (s_x) == get_dims (s_y));
      
      // -- create a fresh map 
      var_map_t res;
      for (auto v: vars) {
	elina_dim_t i = res.size ();
	assert (i < get_dims (s_x));
	res.insert (binding_t (v, i));
      }
      
      // build the permutations maps
      elina_dimperm_t* perm_x = elina_dimperm_alloc (get_dims (s_x));
      elina_dimperm_t* perm_y = elina_dimperm_alloc (get_dims (s_x));
      char * xmap1 = (char *)calloc (get_dims (s_x), sizeof(char));
      if (!xmap1)
	CRAB_ERROR("calloc does not have more available memory");
      char * xmap2 = (char *)calloc (get_dims (s_x), sizeof(char));
      if (!xmap2)
	CRAB_ERROR("calloc does not have more available memory");
      for (auto const &px: m_x.left) {
	elina_dim_t ind = res.left.at (px.first);
	perm_x->dim [px.second] = ind;
	// This sets 1 if the index that has been assigned
	assert (px.second < get_dims(s_x));
	xmap1[px.second] = 1;
	// This sets 1 if the value has been assigned
	assert (ind < get_dims(s_x));
	xmap2[ind] = 1;
      }
      elina_dim_t i, counter = 0;
      for(i=0; i < get_dims (s_x); i++){
	// If the index has been assigned, skip
	if(xmap1[i]) continue;
	// Find the next available element that has not been assigned
	while(xmap2[counter])
	  counter++;
	perm_x->dim[i] = counter;
	counter++;
      }
      free (xmap1);
      free (xmap2);
      
      char * ymap1 = (char *)calloc (get_dims (s_x), sizeof(char));
      if (!ymap1) CRAB_ERROR("calloc does not have more available memory");
      char * ymap2 = (char *)calloc (get_dims (s_x), sizeof(char));
      if (!ymap2) CRAB_ERROR("calloc does not have more available memory");
      for (auto const &py: m_y.left) {
	elina_dim_t ind = res.left.at (py.first);
	perm_y->dim [py.second] = ind;
	assert (py.second < get_dims(s_x));
	ymap1[py.second] = 1;
	assert (ind < get_dims(s_x));
	ymap2[ind] = 1; 
      }
      
      counter = 0;
      for(i=0; i < get_dims (s_x); i++){
	if(ymap1[i]) continue;
	while(ymap2[counter])
	  counter++;
	perm_y->dim[i] = counter;
	counter++;
      }
      
      free (ymap1);
      free (ymap2);
      
      #if 0          
      crab::outs() << "Permutations \n";
      elina_dimperm_fprint(stdout, perm_x);          
      crab::outs() << "Permutations \n";
      elina_dimperm_fprint(stdout, perm_y);          
      #endif 

      assert (check_perm (perm_x, get_dims (s_x)));
      assert (check_perm (perm_y, get_dims (s_x)));
      
      // apply the permutations
      s_x = elinaPtr (get_man (), 
		      elina_abstract0_permute_dimensions(get_man (), false, &*s_x, perm_x));
      s_y = elinaPtr (get_man (), 
		      elina_abstract0_permute_dimensions(get_man (), false, &*s_y, perm_y));
      
      elina_dimperm_free (perm_x);
      elina_dimperm_free (perm_y);
      
      assert (res.size () == get_dims (s_x));
      assert (res.size () == get_dims (s_y));
      
      return res;
    }
    
    // FIXME: dispatch based on Number ?
    #if 0
    static elina_texpr0_t* ADD(elina_texpr0_t* a, elina_texpr0_t*b)
    { /// XXX: should not be the rounding direction ELINA_RDIR_ZERO 
      ///      (i.e., truncation for integers) ?
      return elina_texpr0_binop(ELINA_TEXPR_ADD,a,b,ELINA_RTYPE_INT,ELINA_RDIR_NEAREST);
    }
    static elina_texpr0_t* SUB(elina_texpr0_t* a, elina_texpr0_t*b)
    { return elina_texpr0_binop(ELINA_TEXPR_SUB,a,b,ELINA_RTYPE_INT,ELINA_RDIR_NEAREST);}	  
    static elina_texpr0_t* MUL(elina_texpr0_t* a, elina_texpr0_t*b)
    { return elina_texpr0_binop(ELINA_TEXPR_MUL,a,b,ELINA_RTYPE_INT,ELINA_RDIR_NEAREST); }	  
    static elina_texpr0_t* DIV(elina_texpr0_t* a, elina_texpr0_t*b)
    { return elina_texpr0_binop(ELINA_TEXPR_DIV,a,b,ELINA_RTYPE_INT,ELINA_RDIR_NEAREST);}
    #else
    ///////////
    // XXX: we approximate integers and rationals using reals
    //////////
    
    static elina_texpr0_t* ADD(elina_texpr0_t* a, elina_texpr0_t*b)
    { /// XXX: ELINA_RTYPE_REAL does not have rounding so we choose
      ///      an arbitrary one
      return elina_texpr0_binop(ELINA_TEXPR_ADD,a,b,ELINA_RTYPE_REAL,ELINA_RDIR_UP);
    }
    static elina_texpr0_t* SUB(elina_texpr0_t* a, elina_texpr0_t*b)	  
    { return elina_texpr0_binop(ELINA_TEXPR_SUB,a,b,ELINA_RTYPE_REAL,ELINA_RDIR_UP);}
    static elina_texpr0_t* MUL(elina_texpr0_t* a, elina_texpr0_t*b)	  
    { return elina_texpr0_binop(ELINA_TEXPR_MUL,a,b,ELINA_RTYPE_REAL,ELINA_RDIR_UP);}
    static elina_texpr0_t* DIV(elina_texpr0_t* a, elina_texpr0_t*b)	  
    { return elina_texpr0_binop(ELINA_TEXPR_DIV,a,b,ELINA_RTYPE_REAL,ELINA_RDIR_UP);}
    #endif
	       	
    // --- from crab to elina
    
    inline elina_texpr0_t* var2texpr (variable_t v) { 
      return elina_texpr0_dim (get_var_dim_insert (v));
    }
    
    inline void convert_crab_number (ikos::z_number n, mpq_class &res) const
    { res = mpq_class ((mpz_class) n); }
    
    inline void convert_crab_number (ikos::q_number n, mpq_class &res) const 
    { res = ((mpq_class) n); }
    
    inline elina_texpr0_t* num2texpr (Number i) const {  
      mpq_class n(0);
      convert_crab_number(i, n);
      return elina_texpr0_cst_scalar_mpq (n.get_mpq_t ());
    }
    
    inline elina_texpr0_t* intv2texpr (Number a, Number b) const {
      mpq_class n1(0), n2(0);
      convert_crab_number(a, n1);
      convert_crab_number(b, n2);
      return elina_texpr0_cst_interval_mpq (n1.get_mpq_t (), n2.get_mpq_t ());
    }
    
    inline elina_texpr0_t* expr2texpr (linear_expression_t e)  {
      Number cst = e.constant ();
      elina_texpr0_t* res = num2texpr (cst);
      for (auto p: e) {
	elina_texpr0_t* term = MUL (num2texpr(p.first), var2texpr(p.second));
	res = ADD (res, term); 
      }
      return res;
    }
    
    inline elina_tcons0_t const2tconst (linear_constraint_t cst)  {
      linear_expression_t exp = cst.expression();
      if (cst.is_equality ()) {
	return elina_tcons0_make (ELINA_CONS_EQ, expr2texpr (exp), NULL);            
      } else if (cst.is_inequality ()) {
	return elina_tcons0_make (ELINA_CONS_SUPEQ, expr2texpr (-exp), NULL);
      } else if (cst.is_strict_inequality()) {
	return elina_tcons0_make (ELINA_CONS_SUP, expr2texpr (-exp), NULL);
      } else  {
	assert(cst.is_disequation ());
	return elina_tcons0_make (ELINA_CONS_DISEQ, expr2texpr (exp), NULL);
      } 
    }
    
    // --- from elina to crab 
    
    inline void convert_elina_number (double n, ikos::z_number &res) const
    { res = ikos::z_number ((long) n); }
    
    inline void convert_elina_number (double n, ikos::q_number &res) const
    { res = ikos::q_number (n); }
    
    inline void convert_elina_number (mpq_ptr n, ikos::z_number &res) const
    { // FIXME: find a better way
      res = ikos::z_number (mpz_class(mpq_class(n)));
    }
    inline void convert_elina_number (mpq_ptr n, ikos::q_number &res) const
    { res = ikos::q_number (mpq_class(n)); }	  
    
    Number coeff2Num (elina_coeff_t* coeff) {
      assert (coeff->discr == ELINA_COEFF_SCALAR);
      
      elina_scalar_t* scalar = coeff->val.scalar;	  
      if (scalar->discr == ELINA_SCALAR_DOUBLE) { // elina uses double
	Number res;
	convert_elina_number(scalar->val.dbl, res);
	return res;
      }
      else if (scalar->discr == ELINA_SCALAR_MPQ) {
	Number res;
	convert_elina_number(scalar->val.mpq, res);
	return res;
      }
      else
	CRAB_ERROR("elina translation only covers double or mpq scalars");
    }
    
    linear_expression_t term2expr (elina_coeff_t* coeff, elina_dim_t i) {
      return variable_t (get_variable (i)) * coeff2Num(coeff) ;
    }
    
    linear_constraint_t tconst2const (elina_lincons0_t cons) {
      assert (cons.scalar == NULL); // Not modulo form
      elina_linexpr0_t* linexp = cons.linexpr0;
      assert (elina_linexpr0_is_linear (linexp));
      
      linear_expression_t e (0);
      for (unsigned i=0; i < get_dims (); ++i) {
	elina_coeff_t* coeff = elina_linexpr0_coeffref (linexp, i);
	if (elina_coeff_zero (coeff)) continue;
	
	if (!has_variable (i)) continue; // unused dimension
	
	e = e + term2expr ( coeff, i);
      }
      
      // add possible constant
      elina_coeff_t* cst = elina_linexpr0_cstref (linexp);
      if (!elina_coeff_zero (cst)) 
	e = e + coeff2Num(cst);
      linear_constraint_t res;
      switch (cons.constyp) {
      case ELINA_CONS_EQ:
	// e == k 
	res =  linear_constraint_t (e, linear_constraint_t::kind_t::EQUALITY);
	break;
      case ELINA_CONS_SUPEQ:
	// e >= k 
	e = -e;
	res =  linear_constraint_t (e, linear_constraint_t::kind_t::INEQUALITY);
	break;
      case ELINA_CONS_SUP:
	// e > k 
	e = -e;
	res =  linear_constraint_t (e, linear_constraint_t::kind_t::STRICT_INEQUALITY);
	break;
      case ELINA_CONS_EQMOD:
	res = linear_constraint_t::get_true();
	break;
      case ELINA_CONS_DISEQ:
	// e != k
	res =  linear_constraint_t (e, linear_constraint_t::kind_t::DISEQUATION);
	break;
      }
      return res;
    }
        
    void dump (const var_map_t& m, elina_state_ptr apstate ) {  
      crab::outs() << "\nNumber of dimensions=" << get_dims (apstate) << "\n";
      crab::outs() << "variable map ["; 
      std::vector<char*> names;
      for (unsigned i=0; i < get_dims (apstate) ; i++){
	std::string varname;
	if (has_variable (m, i))
	  varname = get_variable (m, i).str ();
	else // unused dimension
	  varname = std::string ("_x") + std::to_string (i);
	crab::outs() << i << " -> " << varname << ";";
	char* name = new char [varname.length () + 1];
	strcpy (name, varname.c_str ());
	names.push_back (name);
      }
      crab::outs() << "]\n";
      elina_abstract0_fprint (stdout, get_man (), &*apstate, &names[0]);
      for (auto n : names) { delete n; }
    }
    
    void dump () { dump (m_var_map, m_apstate); }

    // x != n
    void inequalities_from_disequation(variable_t x, number_t n, linear_constraint_system_t& out) {
      interval_t i = this->operator[](x);
      interval_t new_i =
	linear_interval_solver_impl::trim_interval<interval_t>(i, interval_t(n));
      if (new_i.is_bottom()) {
	out += linear_constraint_t::get_false();
      } else if (!new_i.is_top() && (new_i <= i)) {
	if(new_i.lb().is_finite()) {
	  // strenghten lb
	  out += linear_constraint_t(x >= *(new_i.lb().number()));
	}
	if(new_i.ub().is_finite()) {
	  // strenghten ub
	  out += linear_constraint_t(x <= *(new_i.ub().number()));	  
	}
      }
    } 

    interval_t compute_residual(linear_expression_t e, variable_t pivot) {
      interval_t residual(-e.constant());
      for (typename linear_expression_t::iterator it = e.begin(); it != e.end(); ++it) {
	variable_t v = it->second;
	if (v.index() != pivot.index()) {
	  residual = residual - (interval_t (it->first) * this->operator[](v));
	}
      }
      return residual;
    }
    
    void inequalities_from_disequation(linear_expression_t e, linear_constraint_system_t& o) {
      for (typename linear_expression_t::iterator it = e.begin(); it != e.end(); ++it) {
	variable_t pivot = it->second;
	interval_t i = compute_residual(e, pivot) / interval_t(it->first);
	if (auto k = i.singleton()) {
	  inequalities_from_disequation(pivot, *k, o);
	}
      }
    }
    
  public:
    
    void print_stats () { elina_abstract0_fprint (stdout, get_man (), &*m_apstate, NULL); }
    
  private:
    
    elina_domain_ (elina_state_ptr apState, var_map_t varMap):
      m_apstate (apState), m_var_map (varMap) {
      
      std::vector<elina_dim_t> dims;
      var_map_t res;
      /// XXX: we must iterate on the dimension id's to preserve
      /// order between them
      for (auto const& p: m_var_map.right) {  
	if (elina_abstract0_is_dimension_unconstrained (get_man (),
							&*m_apstate, 
							p.first)) {
	  dims.push_back (p.first);
	}
	else {
	  elina_dim_t i = res.size ();
	  res.insert (binding_t (p.second, i));
	}
      }
      remove_dimensions (m_apstate, dims);
      std::swap (m_var_map, res);
      
      assert (m_var_map.size () == get_dims ());
    }
    
    
    elina_domain_ (elina_state_ptr&& apState, var_map_t&& varMap):
      m_apstate (std::move (apState)), 
      m_var_map (std::move (varMap)) { 
      
      std::vector<elina_dim_t> dims;
      var_map_t res;
      /// XXX: we must iterate on the dimension id's to preserve
      /// order between them
      for (auto const& p: m_var_map.right) {  
	if (elina_abstract0_is_dimension_unconstrained (get_man (),
							&*m_apstate, 
							p.first)) {
	  dims.push_back (p.first);
	}
	else {
	  elina_dim_t i = res.size ();
	  res.insert (binding_t (p.second, i));
	}
      }
      remove_dimensions (m_apstate, dims);
      std::swap (m_var_map, res);
      
      assert (m_var_map.size () == get_dims ());
    }
    
    
  public:
    
    elina_domain_ (bool isBot = false):
      m_apstate (elinaPtr (get_man(), 
			   (isBot ? 
			    elina_abstract0_bottom (get_man(), 0, 0) : 
			    elina_abstract0_top (get_man(), 0, 0)))) {}
    
    ~elina_domain_ () {}
    
    elina_domain_ (const elina_domain_t& o): 
      m_apstate (elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*(o.m_apstate)))),
      m_var_map (o.m_var_map) {
      crab::CrabStats::count (getDomainName() + ".count.copy");
      crab::ScopedCrabStats __st__(getDomainName() + ".copy");
    }
    
    elina_domain_ (elina_domain_t&& o): 
      m_apstate (std::move (o.m_apstate)), 
      m_var_map (std::move (o.m_var_map)) { }
    
    elina_domain_t& operator=(const elina_domain_t& o) {
      crab::CrabStats::count (getDomainName() + ".count.copy");
      crab::ScopedCrabStats __st__(getDomainName() + ".copy");
      if (this != &o) {
	m_apstate = elinaPtr(get_man(), elina_abstract0_copy (get_man (), &*(o.m_apstate)));
	m_var_map = o.m_var_map;
      }
      return *this;
    }
    
    elina_domain_t& operator=(elina_domain_t&& o) {
      if (this != &o) {
	m_apstate = std::move (o.m_apstate);
	m_var_map = std::move (o.m_var_map);
      }
      return *this;
    }
    
    static elina_domain_t top() { 
      return elina_domain_t (false);
    }
    
    static elina_domain_t bottom() { 
      return elina_domain_t (true);
    }

    bool is_bottom() { 
      return elina_abstract0_is_bottom (get_man(), &*m_apstate);
    }
    
    bool is_top() { 
      return elina_abstract0_is_top (get_man(), &*m_apstate);
    }
    
    bool operator<=(elina_domain_t o) { 
      crab::CrabStats::count (getDomainName() + ".count.leq");
      crab::ScopedCrabStats __st__(getDomainName() + ".leq");
      
      if (is_bottom()) 
	return true;
      else if(o.is_bottom())
	return false;
      else if (o.is_top ())
	return true;
      else if (is_top () && !o.is_top ())
	return false;
      else if (is_top () && o.is_top ())
	return true;
      else { 
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	return elina_abstract0_is_leq (get_man(), &*x, &*o.m_apstate);
      }
    }
    
    void operator|=(elina_domain_t o) {
      crab::CrabStats::count (getDomainName() + ".count.join");
      crab::ScopedCrabStats __st__(getDomainName() + ".join");
      
      if (is_bottom() || o.is_top ())
	*this = o;
      else if (is_top () || o.is_bottom())
	return ;
      else {
	m_var_map = merge_var_map (m_var_map, m_apstate, o.m_var_map, o.m_apstate);
	m_apstate = elinaPtr (get_man(), 
			      elina_abstract0_join (get_man(), false, 
						    &*m_apstate, &*o.m_apstate));
      }
    }
    
    elina_domain_t operator|(elina_domain_t o) {
      crab::CrabStats::count (getDomainName() + ".count.join");
      crab::ScopedCrabStats __st__(getDomainName() + ".join");
      
      if (is_bottom() || o.is_top ())
	return o;
      else if (is_top () || o.is_bottom())
	return *this;
      else {
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	var_map_t  m = merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	return elina_domain_t (elinaPtr (get_man(), 
					 elina_abstract0_join (get_man(), false, 
							       &*x, &*o.m_apstate)), m);
      }
    }        
    
    elina_domain_t operator&(elina_domain_t o) {
      crab::CrabStats::count (getDomainName() + ".count.meet");
      crab::ScopedCrabStats __st__(getDomainName() + ".meet");
      
      if (is_bottom() || o.is_bottom())
	return bottom();
      else if (is_top())
	return o;
      else if (o.is_top())
	return *this;
      else{
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	var_map_t  m = merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	return elina_domain_t (elinaPtr (get_man(), 
					 elina_abstract0_meet (get_man(), false, 
							       &*x, &*o.m_apstate)), m);
      }
    }        
    
    elina_domain_t operator||(elina_domain_t o) {
      crab::CrabStats::count (getDomainName() + ".count.widening");
      crab::ScopedCrabStats __st__(getDomainName() + ".widening");
      
      if (is_bottom())
	return o;
      else if (o.is_bottom())
	return *this;
      else {
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	var_map_t  m = merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	return elina_domain_t (elinaPtr (get_man(), 
					 elina_abstract0_widening (get_man(), 
								   &*x, &*o.m_apstate)), m);
      }
    }        
    
    template<typename Thresholds>	
    elina_lincons0_array_t make_thresholds (elina_domain_t o, const Thresholds &ts) {
      // TODO: make some constraints using the constants from ts
      elina_lincons0_array_t csts = elina_lincons0_array_make(0);
      return csts;
    }
    
    template<typename Thresholds>
    elina_domain_t widening_thresholds (elina_domain_t o, const Thresholds &ts) {
      crab::CrabStats::count (getDomainName() + ".count.widening");
      crab::ScopedCrabStats __st__(getDomainName() + ".widening");
      
      if (is_bottom())
	return o;
      else if (o.is_bottom())
	return *this;
      else {
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	var_map_t  m = merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	//////
	// We cannot refine the result of widening with
	// widening w/ thresholds over intervals because it might
	// cause non-termination.
	///// 
	// This causes a loss of precision in a couple of tests:
	// - tests/domains/test2-rat.cc
	// - tests/domains/test3-rat.cc
	/////
        #if 0
	// widening w/o thresholds in the elina domain
	elina_domain_t res (elinaPtr (get_man(), 
				      elina_abstract0_widening(get_man(), 
	    						       &*x, &*o.m_apstate)), m);
	// widening w/ thresholds in the interval domain
	auto intv_this  = this->to_interval_domain ();
	auto intv_o     = o.to_interval_domain ();
	auto intv_widen = intv_this.widening_thresholds (intv_o, ts);	    
	// refine the elina domain using the widen intervals
	elina_domain_t elina_intv_widen;
	elina_intv_widen += intv_widen.to_linear_constraint_system ();
	return res & elina_intv_widen;
        #else
	elina_lincons0_array_t csts = make_thresholds (o, ts);
	elina_domain_t res (elinaPtr (get_man(), 
				      elina_abstract0_widening_threshold
				      (get_man(), &*x, &*o.m_apstate, &csts)), m);
	elina_lincons0_array_clear (&csts);
	return res;
        #endif 
      }
    }
    
    elina_domain_t operator&&(elina_domain_t o) {
      crab::CrabStats::count (getDomainName() + ".count.narrowing");
      crab::ScopedCrabStats __st__(getDomainName() + ".narrowing");
      
      if (is_bottom() || o.is_bottom())
	return bottom();
      else if (is_top())
	return o;
      else if (o.is_top())
	return *this;
      else{
	elina_state_ptr x = elinaPtr (get_man (), elina_abstract0_copy (get_man (), &*m_apstate));
	var_map_t  m = merge_var_map (m_var_map, x, o.m_var_map, o.m_apstate);
	switch (ElinaDom) {
	case ELINA_OCT:
	  return elina_domain_t(elinaPtr(get_man(), 
				      elina_abstract0_opt_oct_narrowing(get_man(),
									&*x, &*o.m_apstate)), m);
	case ELINA_ZONES:  
	case ELINA_PK:
	default:
	  //CRAB_WARN ("used meet instead of narrowing: \n",
	  //           "make sure only a finite number of descending iterations are run.");
	  return elina_domain_t(elinaPtr(get_man(), 
					 elina_abstract0_meet(get_man(), false,
							      &*x, &*o.m_apstate)), m);
	}
      }
    }        
    
    template<typename Range>
    void forget (const Range &vars) {
      crab::CrabStats::count (getDomainName() + ".count.forget");
      crab::ScopedCrabStats __st__(getDomainName() + ".forget");
      
      std::vector<elina_dim_t> vector_dims;
      std::set<elina_dim_t> set_dims;
      
      for (auto v: vars)  {
	if (auto dim = get_var_dim (v)) {
	  vector_dims.push_back (*dim);
	  set_dims.insert (*dim);
	}
      }
      
      if (vector_dims.empty ()) return;
      
      m_apstate = elinaPtr (get_man (), 
			    elina_abstract0_forget_array (get_man (), 
							  false, 
							  &*m_apstate, 
							  &vector_dims[0], vector_dims.size(), 
							  false));
      
      // -- Remove forgotten dimensions while compacting
      var_map_t res;
      /// XXX: we must iterate on the dimension id's to preserve
      /// order between them
      for (auto const& p: m_var_map.right) {  
	if (set_dims.count (p.first) <= 0) {
	  elina_dim_t i = res.size ();
	  res.insert (binding_t (p.second, i));
	}
      }
      
      remove_dimensions (m_apstate, vector_dims);
      std::swap (m_var_map, res);
    }
    
    void operator-=(variable_t var) {
      crab::CrabStats::count (getDomainName() + ".count.forget");
      crab::ScopedCrabStats __st__(getDomainName() + ".forget");
      
      if (is_bottom() || is_top())
	return;
      
      std::vector<elina_dim_t> vector_dims;
      if (auto dim = get_var_dim (var)) {
	vector_dims.push_back (*dim);
	m_apstate = elinaPtr (get_man (), 
			      elina_abstract0_forget_array (get_man (), 
							    false, 
							    &*m_apstate, 
							    &vector_dims[0], vector_dims.size(), 
							    false));
	// -- Remove forgotten dimensions while compacting
	var_map_t res;
	/// XXX: we must iterate on the dimension id's to preserve
	/// order between them
	for (auto const& p: m_var_map.right) {  
	  if (p.first != *dim) {
	    elina_dim_t i = res.size ();
	    res.insert (binding_t (p.second, i));
	  }
	}
	remove_dimensions (m_apstate, vector_dims);
	std::swap (m_var_map, res);
      }
    }
    
    // remove all variables except vars
    template<typename Range>
    void project (const Range& vars) {
      crab::CrabStats::count (getDomainName() + ".count.project");
      crab::ScopedCrabStats __st__(getDomainName() + ".project");
      
      if (is_bottom ()) return;
      std::set<variable_t> s1,s2,s3;
      for (auto p: m_var_map.left) s1.insert (p.first);
      s2.insert (vars.begin (), vars.end ());
      boost::set_difference (s1,s2,std::inserter (s3, s3.end ()));
      forget (s3);
    }
    
    interval_t operator[](variable_t v) {
      crab::CrabStats::count (getDomainName() + ".count.to_intervals");
      crab::ScopedCrabStats __st__(getDomainName() + ".to_intervals");
      
      if (is_bottom ()) 
	return interval_t::bottom ();
      
      if (auto dim = get_var_dim (v)) {
	
	elina_interval_t* intv = elina_abstract0_bound_dimension (get_man (),
								  &*m_apstate, 
								  *dim);
	if (elina_interval_is_top (intv)) {
	  elina_interval_free (intv);
	  return interval_t::top ();
	}
	
	elina_scalar_t* lb = intv->inf;
	elina_scalar_t* ub = intv->sup;
        
	if (lb->discr == ELINA_SCALAR_DOUBLE && ub->discr == ELINA_SCALAR_DOUBLE) { 
	  
	  if (elina_scalar_infty(lb) == -1) {     // [-oo, k]
	    Number sup;
	    convert_elina_number (ub->val.dbl, sup);
	    elina_interval_free (intv);
	    return interval_t (bound_t::minus_infinity (), sup);
	  }
	  else if (elina_scalar_infty(ub) == 1) { // [k, +oo]
	    Number inf;
	    convert_elina_number (lb->val.dbl, inf);
	    elina_interval_free (intv);
	    return interval_t (inf, bound_t::plus_infinity ());
            
	  }
	  else { 
	    assert (elina_scalar_infty(lb) == 0); // lb is finite
	    assert (elina_scalar_infty(ub) == 0); // ub is finite
	    Number inf, sup;
	    convert_elina_number(lb->val.dbl, inf);
	    convert_elina_number(ub->val.dbl, sup);
	    elina_interval_free (intv);
	    return interval_t (inf, sup);
	  }
	  
	}
	else if (lb->discr == ELINA_SCALAR_MPQ && ub->discr == ELINA_SCALAR_MPQ ) {
	  
	  if (elina_scalar_infty(lb) == -1) {     // [-oo, k]
	    Number sup;
	    convert_elina_number (ub->val.mpq, sup);
	    elina_interval_free (intv);
	    return interval_t (bound_t::minus_infinity (), sup);
	    
	  }
	  else if (elina_scalar_infty(ub) == 1) { // [k, +oo]
	    Number inf;
	    convert_elina_number (lb->val.mpq, inf);
	    elina_interval_free (intv);
	    return interval_t (inf, bound_t::plus_infinity ());
	  }
	  else {
	    assert (elina_scalar_infty(lb) == 0); // lb is finite
	    assert (elina_scalar_infty(ub) == 0); // ub is finite
	    
	    Number inf, sup;
	    convert_elina_number (lb->val.mpq, inf);
	    convert_elina_number (ub->val.mpq, sup);		
	    elina_interval_free (intv);
	    return interval_t (inf, sup);
	  }
	}
	else 
	  CRAB_ERROR("elina translation only covers double or mpq scalars");
      }
      else 
	return interval_t::top ();
    }
    
    void set(variable_t v, interval_t ival) {
      crab::CrabStats::count (getDomainName() + ".count.assign");
      crab::ScopedCrabStats __st__(getDomainName() + ".assign");
      
      variable_t vv(v);
      
      // -- forget v
      *this -= v;
      
      // -- add constraints v >= lb and v <= ub
      linear_constraint_system_t csts;
      auto lb = ival.lb ();
      if (lb.is_finite ())  {
	// v >= lb <--> -v + lb <= 0
	assert (lb.number ());
	linear_expression_t e = (Number(-1) * vv) + *(lb.number ());
	csts += (linear_constraint_t (e, linear_constraint_t::kind_t::INEQUALITY));
      }
      auto ub = ival.ub ();
      if (ub.is_finite ()) {
	// v <= ub <--> v - ub <= 0
	assert (ub.number ());
	linear_expression_t e = (vv - *(ub.number ()));
	csts += (linear_constraint_t (e, linear_constraint_t::kind_t::INEQUALITY));
      }
      
      if (csts.size () > 0)
	*this += csts;
    }
    
    void operator+=(linear_constraint_system_t _csts) {
      crab::CrabStats::count (getDomainName() + ".count.add_constraints");
      crab::ScopedCrabStats __st__(getDomainName() + ".add_constraints");
      
      if(is_bottom()) return;
      
      if (_csts.is_false()) {
	*this = bottom ();
	return;
      }
      
      // XXX: filter out unsigned linear inequalities, and analyze
      //      separately disequalities because elina does not support
      //      them.
      linear_constraint_system_t csts;
      for (auto const& c: _csts) {
	if (c.is_inequality() && c.is_unsigned()) {
	  CRAB_WARN("unsigned inequality skipped");
	  continue;
	}
	if (c.is_disequation()) {
	  inequalities_from_disequation(c.expression(), csts);
	} else {
	  csts += c;
	}
      }

      if (csts.is_false()) {
	// csts can be false after breaking disequalities into
	// inequalities
	*this = bottom();
	return;
      }
      
      elina_tcons0_array_t array = elina_tcons0_array_make (csts.size ());
      unsigned i=0;
      
      for (auto cst : csts) {
	elina_tcons0_t tcons = const2tconst (cst);
	array.p[i] = tcons;
	++i;
      }
      
      #if 0
      ///// debugging
      std::vector<char*> names;
      for (unsigned i=0; i < get_dims (m_apstate) ; i++){
	std::string varname;
	if (has_variable (m_var_map, i))
	  varname = get_variable (m_var_map, i).str ();
	else // unused dimension
	  varname = std::string ("_x") + std::to_string (i);
	char* name = new char [varname.length () + 1];
	strcpy (name, varname.c_str ());
	names.push_back (name);
      }
      elina_tcons0_array_fprint (stdout, &array, &names[0]);
      for (auto n : names) { delete n; }
      #endif 
      
      m_apstate = elinaPtr (get_man (), 
			    elina_abstract0_meet_tcons_array (get_man (), false, 
							      &*m_apstate, &array));
      
      elina_tcons0_array_clear(&array);
      CRAB_LOG("elina", 
	       crab::outs() << "--- " << "Assume " << csts << " --> "
	                    << *this << "\n";);
    }
    
    void assign (variable_t x, linear_expression_t e) {
      crab::CrabStats::count (getDomainName() + ".count.assign");
      crab::ScopedCrabStats __st__(getDomainName() + ".assign");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* t = expr2texpr (e);
      assert (t);
      auto dim_x = get_var_dim_insert (x);
      m_apstate = elinaPtr (get_man (), 
			    elina_abstract0_assign_texpr(get_man (), false, 
							 &*m_apstate, 
							 dim_x, t, NULL));
      
      elina_texpr0_free (t);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x<< ":="<< e << " --> "<< *this<<"\n";);
    }
    
    void apply (operation_t op, variable_t x, variable_t y, Number z) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* a = var2texpr (y);
      elina_texpr0_t* b = num2texpr (z);
      elina_texpr0_t* res = nullptr;
      
      switch (op){
      case OP_ADDITION: res = ADD (a, b); break;
      case OP_SUBTRACTION: res = SUB (a, b); break;
      case OP_MULTIPLICATION: res = MUL (a, b); break;
      case OP_DIVISION: res = DIV (a, b); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      assert (res);
      
      auto dim_x = get_var_dim_insert (x);
      m_apstate = elinaPtr (get_man (), elina_abstract0_assign_texpr(get_man (), false, 
								     &*m_apstate, 
								     dim_x, res, NULL));
      
      elina_texpr0_free (res);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x<< ":="<< y<< op<< z<< " --> "<< *this<<"\n";);
    }
    
    void apply(operation_t op, variable_t x, variable_t y, variable_t z) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* a = var2texpr (y);
      elina_texpr0_t* b = var2texpr (z);
      elina_texpr0_t* res = nullptr;
      
      switch (op){
      case OP_ADDITION: res = ADD (a, b); break;
      case OP_SUBTRACTION: res = SUB (a, b); break;
      case OP_MULTIPLICATION: res = MUL (a, b); break;
      case OP_DIVISION: res = DIV (a, b); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      assert (res);
      
      auto dim_x = get_var_dim_insert (x);
      m_apstate = elinaPtr (get_man (),
			    elina_abstract0_assign_texpr(get_man (), false, 
							 &*m_apstate, 
							 dim_x, res, NULL));
      
      elina_texpr0_free (res);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x<< ":="<< y<< op<< z<< " --> "<< *this<<"\n";);
    }
    
    void apply(operation_t op, variable_t x, Number k) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* a = var2texpr (x);
      elina_texpr0_t* b = num2texpr (k);
      elina_texpr0_t* res = nullptr;
      
      switch (op){
      case OP_ADDITION: res = ADD (a, b); break;
      case OP_SUBTRACTION: res = SUB (a, b); break;
      case OP_MULTIPLICATION: res = MUL (a, b); break;
      case OP_DIVISION: res = DIV (a, b); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      assert (res);

      auto dim_x = get_var_dim_insert (x);
      m_apstate = elinaPtr (get_man (),
			    elina_abstract0_assign_texpr(get_man (), false, 
							 &*m_apstate, 
							 dim_x, res, NULL));
      
      elina_texpr0_free (res);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x<< ":="<< x<< op<< k<< " --> "<< *this<<"\n";);
    }
    
    void apply(int_conv_operation_t op, variable_t dst, variable_t src) {
      // since reasoning about infinite precision we simply assign and
      // ignore the widths.
      assign(dst, src);
    }
    
    void apply(bitwise_operation_t op, variable_t x, variable_t y, variable_t z) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      // Convert to intervals and perform the operation
      interval_t yi = operator[](y);
      interval_t zi = operator[](z);
      interval_t xi = interval_t::top();
      switch (op) {
      case OP_AND: xi = yi.And(zi); break;
      case OP_OR: xi = yi.Or(zi); break;
      case OP_XOR: xi = yi.Xor(zi); break; 
      case OP_SHL: xi = yi.Shl(zi); break; 
      case OP_LSHR: xi = yi.LShr(zi); break;
      case OP_ASHR: xi = yi.AShr(zi); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      set(x, xi);
    }
    
    void apply(bitwise_operation_t op, variable_t x, variable_t y, Number k) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      // Convert to intervals and perform the operation
      interval_t yi = operator[](y);
      interval_t zi(k);
      interval_t xi = interval_t::top();
      switch (op) {
      case OP_AND: xi = yi.And(zi); break;
      case OP_OR: xi = yi.Or(zi); break;
      case OP_XOR: xi = yi.Xor(zi); break; 
      case OP_SHL: xi = yi.Shl(zi); break; 
      case OP_LSHR: xi = yi.LShr(zi); break;
      case OP_ASHR: xi = yi.AShr(zi); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      set(x, xi);
    }
    
    void apply(div_operation_t op, variable_t x, variable_t y, variable_t z) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      if (op == OP_SDIV){
	apply(OP_DIVISION, x, y, z);
      }
      else {
	// Convert to intervals and perform the operation
	interval_t yi = operator[](y);
	interval_t zi = operator[](z);
	interval_t xi = interval_t::top ();
        
	switch (op) {
	case OP_UDIV: xi = yi.UDiv(zi); break;
	case OP_SREM: xi = yi.SRem(zi); break;
	case OP_UREM: xi = yi.URem(zi); break;
	default: CRAB_ERROR("elina operation not supported");
	}
	set(x, xi);
      }
    }
    
    void apply(div_operation_t op,
	       variable_t x, variable_t y, Number k) {
      crab::CrabStats::count (getDomainName() + ".count.apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".apply");
      
      if (op == OP_SDIV){
	apply(OP_DIVISION, x, y, k);
      }
      else {
	// Convert to intervals and perform the operation
	interval_t yi = operator[](y);
	interval_t zi(k);
	interval_t xi = interval_t::top ();
	switch (op) {
	case OP_UDIV: xi = yi.UDiv(zi); break;
	case OP_SREM: xi = yi.SRem(zi); break;
	case OP_UREM: xi = yi.URem(zi); break;
	default: CRAB_ERROR("elina operation not supported");
	}
	set(x, xi);
      }
    }
    
    void backward_assign (variable_t x, linear_expression_t e,
			  elina_domain_t invariant) {
      crab::CrabStats::count (getDomainName() + ".count.backward_assign");
      crab::ScopedCrabStats __st__(getDomainName() + ".backward_assign");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* t = expr2texpr (e);
      assert (t);
      auto dim_x = get_var_dim_insert (x);
      
      m_var_map = merge_var_map (m_var_map, m_apstate,
				     invariant.m_var_map,
				     invariant.m_apstate);
      
      m_apstate = elinaPtr (get_man (), 
			    elina_abstract0_substitute_texpr(get_man (), false, 
							     &*m_apstate, 
							     dim_x, t,
							     &*invariant.m_apstate));
      
      elina_texpr0_free (t);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x<< " :=_bwd " << e << " --> "
	                    << *this<<"\n";);
    }
    
    void backward_apply (operation_t op,
			 variable_t x, variable_t y, Number z,
			 elina_domain_t invariant) {
      crab::CrabStats::count (getDomainName() + ".count.backward_apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".backward_apply");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* a = var2texpr (y);
      elina_texpr0_t* b = num2texpr (z);
      elina_texpr0_t* res = nullptr;
      
      switch (op){
      case OP_ADDITION: res = ADD (a, b); break;
      case OP_SUBTRACTION: res = SUB (a, b); break;
      case OP_MULTIPLICATION: res = MUL (a, b); break;
      case OP_DIVISION: res = DIV (a, b); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      assert (res);
      
      auto dim_x = get_var_dim_insert (x);
      
      m_var_map = merge_var_map (m_var_map, m_apstate,
				 invariant.m_var_map,
				 invariant.m_apstate);
      
      m_apstate = elinaPtr (get_man (),
			    elina_abstract0_substitute_texpr(get_man (), false, 
							     &*m_apstate, 
							     dim_x, res,
							     &*invariant.m_apstate));
      
      elina_texpr0_free (res);
      CRAB_LOG("elina",
	       crab::outs() << "--- "<< x << " :=_bwd " << y << op << z
	                    << " --> " << *this<<"\n";);
    }
    
    void backward_apply(operation_t op,
			variable_t x, variable_t y, variable_t z,
			elina_domain_t invariant)  {
      crab::CrabStats::count (getDomainName() + ".count.backward_apply");
      crab::ScopedCrabStats __st__(getDomainName() + ".backward_apply");
      
      if(is_bottom()) return;
      
      elina_texpr0_t* a = var2texpr (y);
      elina_texpr0_t* b = var2texpr (z);
      elina_texpr0_t* res = nullptr;
      
      switch (op){
      case OP_ADDITION: res = ADD (a, b); break;
      case OP_SUBTRACTION: res = SUB (a, b); break;
      case OP_MULTIPLICATION: res = MUL (a, b); break;
      case OP_DIVISION: res = DIV (a, b); break;
      default: CRAB_ERROR("elina operation not supported");
      }
      assert (res);
      
      auto dim_x = get_var_dim_insert (x);
      
      m_var_map = merge_var_map (m_var_map, m_apstate,
				 invariant.m_var_map,
				 invariant.m_apstate);
      
      m_apstate = elinaPtr (get_man (),
			    elina_abstract0_substitute_texpr(get_man (), false, 
							     &*m_apstate, 
							     dim_x, res,
							     &*invariant.m_apstate));
      
      elina_texpr0_free (res);
      CRAB_LOG("elina",
	       crab::outs() << "--- " << x << ":=_bwd " << y << op << z
	                    << " --> "<< *this <<"\n";);
    }
    
    interval_domain_t to_interval_domain () {
      if (is_bottom ()) return interval_domain_t::bottom();
      if (is_top ())    return interval_domain_t::top();	  
      
      interval_domain_t res;
      for (auto &px: m_var_map.left)
	res.set (px.first, this->operator[](px.first));
      return res;
    }
    
    linear_constraint_system_t to_linear_constraint_system () {
      linear_constraint_system_t csts;
      if(is_bottom ())  {
	csts += linear_constraint_t::get_false();
      }
      else if(is_top ()) {
	csts += linear_constraint_t::get_true();
      }
      else {
	normalize ();
	
	elina_lincons0_array_t lcons_arr =
	  elina_abstract0_to_lincons_array (get_man (), &*m_apstate);
	for (unsigned i=0 ; i < lcons_arr.size; i++)
	  csts += tconst2const (lcons_arr.p[i]);
	
	elina_lincons0_array_clear (&lcons_arr);
      }
      return csts;
    }

    disjunctive_linear_constraint_system_t to_disjunctive_linear_constraint_system() {
      auto lin_csts = to_linear_constraint_system();
      if (lin_csts.is_false()) {
	return disjunctive_linear_constraint_system_t(true /*is_false*/); 
      } else if (lin_csts.is_true()) {
	return disjunctive_linear_constraint_system_t(false /*is_false*/);
      } else {
	return disjunctive_linear_constraint_system_t(lin_csts);
      }
    }
    
    void rename(const variable_vector_t &from, const variable_vector_t &to) {
      if (is_top () || is_bottom()) return;
      
      // renaming m_var_map by creating a new map 
      CRAB_LOG("elina",
	       crab::outs() << "Replacing {";
	       for (auto v: from) crab::outs() << v << ";";
	       crab::outs() << "} with ";
	       for (auto v: to) crab::outs() << v << ";";
	       crab::outs() << "}:\n";
	       crab::outs() << *this << "\n";);
      
      var_map_t new_var_map;
      for (auto kv: m_var_map.left) {
	ptrdiff_t pos = std::distance(from.begin(),
				      std::find(from.begin(), from.end(), kv.first));
	if (pos < (int) from.size()) {
	  new_var_map.insert(binding_t(to[pos], kv.second));
	} else {
	  new_var_map.insert(binding_t(kv.first, kv.second));
	}
      }
      std::swap(m_var_map, new_var_map);
      
      CRAB_LOG("elina",
	       crab::outs () << "RESULT=" << *this << "\n");
    }
    
    void expand (variable_t x, variable_t dup) {
      if (is_bottom() || is_top()) return;
      
      if (get_var_dim(dup)) {
	CRAB_ERROR("expand second parameter ", dup,
		   " cannot be already a variable in the elina domain ", *this);
      }
      
      // --- increases number of dimensions by one
      auto dim_x = get_var_dim_insert (x);
      m_apstate = elinaPtr (get_man(),
			    elina_abstract0_expand(get_man (), false, &* m_apstate, 
						   dim_x, 1));
      
      // --- the additional dimension is put at the end of integer
      //     dimensions.
      m_var_map.insert (binding_t (dup, get_dims () - 1));
    }
    
    void normalize () {
      elina_abstract0_canonicalize (get_man (), &*m_apstate);
    }
    
    void write(crab_os& o) {
      if(is_bottom()){
	o << "_|_";
	return;
      }
      else if (is_top()){
	o << "{}";
	return;
      }
      else {
	// dump ();
	linear_constraint_system_t inv = to_linear_constraint_system ();
	o << inv;
      }
    }          
    
    static std::string getDomainName () {
      switch (ElinaDom) {
      case ELINA_ZONES:   return "ElinaZones";		
      case ELINA_OCT:     return "ElinaOctagon"; 
      case ELINA_PK:      return "ElinaPolyhedra";
      default: CRAB_ERROR("Unknown elina domain");
      }
    }
  }; 
  
  #if 1
  // Without copy-on-write
  template<class Number, class VariableName, elina_domain_id_t ElinaDom>
  using elina_domain = elina_domain_<Number,VariableName,ElinaDom>;     
  #else 
  // Quick wrapper which uses shared references with copy-on-write.
  template<class Number, class VariableName, elina_domain_id_t ElinaDom>
  class elina_domain :
    public abstract_domain<Number, VariableName,
			   elina_domain<Number, VariableName, ElinaDom> > {
    
    typedef elina_domain<Number, VariableName, ElinaDom> elina_domain_t;
    typedef abstract_domain<Number, VariableName, elina_domain_t> abstract_domain_t;
    
  public:
    
    using typename abstract_domain_t::linear_expression_t;
    using typename abstract_domain_t::linear_constraint_t;
    using typename abstract_domain_t::linear_constraint_system_t;
    using typename abstract_domain_t::variable_t;
    using typename abstract_domain_t::number_t;
    using typename abstract_domain_t::varname_t;
    using typename abstract_domain_t::variable_vector_t;	
    typedef typename linear_constraint_t::kind_t constraint_kind_t;
    typedef interval<Number>  interval_t;
    
  private:
    
    typedef elina_domain_<Number, VariableName, ElinaDom> elina_domain_impl_t;
    typedef std::shared_ptr<elina_domain_impl_t> elina_domain_ref_t;
    
    elina_domain_ref_t _ref;  
    
    elina_domain(elina_domain_ref_t ref) : _ref(ref) { }
    
    elina_domain_t create(elina_domain_impl_t&& t) {
      return std::make_shared<elina_domain_impl_t>(std::move(t));
    }
    
    void detach(void) {
      if(!_ref.unique())
	_ref = std::make_shared<elina_domain_impl_t>(*_ref);
    }
    
  public:
    
    static elina_domain_t top() { return elina_domain(false); }
    
    static elina_domain_t bottom() { return elina_domain(true); }
    
    elina_domain(bool is_bottom = false)
      : _ref(std::make_shared<elina_domain_impl_t>(is_bottom)) { }
        
    elina_domain(const elina_domain_t& o)
      : _ref(o._ref) { }
    
    elina_domain& operator=(const elina_domain_t& o) {
      if (this != &o) {
	_ref = o._ref;
      }
      return *this;
    }
    
    elina_domain_impl_t& ref(void) { return *_ref; }
    const elina_domain_impl_t& ref(void) const { return *_ref; }
    
    bool is_bottom() { return ref().is_bottom(); }
    bool is_top() { return ref().is_top(); }
    bool operator<=(elina_domain_t& o) { return ref() <= o.ref(); }
    void operator|=(elina_domain_t o) { detach(); ref() |= o.ref(); }
    elina_domain_t operator|(elina_domain_t o)
    { return create(ref() | o.ref()); }
    elina_domain_t operator||(elina_domain_t o)
    { return create(ref() || o.ref()); }
    elina_domain_t operator&(elina_domain_t o)
    { return create(ref() & o.ref()); }
    elina_domain_t operator&&(elina_domain_t o)
    { return create(ref() && o.ref()); }
    
    template<typename Thresholds>
    elina_domain_t widening_thresholds(elina_domain_t o, const Thresholds &ts) {
          return create(ref().template widening_thresholds<Thresholds>(o.ref(), ts));
    }
    
    void normalize() { detach (); ref().normalize(); }
    
    void operator+=(linear_constraint_system_t csts) {
      detach(); ref() += csts;
    }
    void operator-=(variable_t v) {
      detach(); ref() -= v;
    }
    interval_t operator[](variable_t x) {
      return ref()[x];
    }
    void set(variable_t x, interval_t intv) {
      detach(); ref().set(x, intv);
    }
    
    template<typename Range>
    void forget (Range vs) { detach(); ref().forget(vs); }
    
    void assign(variable_t x, linear_expression_t e) {
      detach(); ref().assign(x, e);
    }
    void apply(operation_t op,
	       variable_t x, variable_t y, Number k) {
      detach(); ref().apply(op, x, y, k);
    }
    void apply(operation_t op,
	       variable_t x, variable_t y, variable_t z) {
      detach(); ref().apply(op, x, y, z);
    }	
    void apply(int_conv_operation_t op, variable_t dst, variable_t src) {
      detach(); ref().apply(op, dst, src);
    }
    void apply(bitwise_operation_t op,
	       variable_t x, variable_t y, Number k) {
      detach(); ref().apply(op, x, y, k);
    }
    void apply(bitwise_operation_t op,
	       variable_t x, variable_t y, variable_t z) {
      detach(); ref().apply(op, x, y, z);
    }
    void apply(div_operation_t op,
	       variable_t x, variable_t y, variable_t z) {
      detach(); ref().apply(op, x, y, z);
    }
    void apply(div_operation_t op,
	       variable_t x, variable_t y, Number k) {
      detach(); ref().apply(op, x, y, k);
    }
    void backward_assign(variable_t x, linear_expression_t e,
			 elina_domain_t invariant) {
      detach(); ref().backward_assign(x, e, invariant.ref());
    }
    void backward_apply(operation_t op,
			variable_t x, variable_t y, Number k,
			elina_domain_t invariant) {
      detach(); ref().backward_apply(op, x, y, k, invariant.ref());
    }
    void backward_apply(operation_t op,
			variable_t x, variable_t y, variable_t z,
			elina_domain_t invariant) {
      detach(); ref().backward_apply(op, x, y, z, invariant.ref());
    }	
    void expand (variable_t x, variable_t y) {
      detach(); ref().expand(x, y);
    }
    
    void rename(const variable_vector_t &from, const variable_vector_t &to)
    { detach(); ref().rename(from, to); }
    
    template<typename Range>
    void project (Range vs) { detach(); ref().project(vs); }
    
    template <typename NumDomain>
    void push (const variable_t& x, NumDomain&inv){
      detach(); ref().push(x, inv);
    }
    
    void write(crab_os& o) { ref().write(o); }
        
    linear_constraint_system_t to_linear_constraint_system () {
      return ref().to_linear_constraint_system();
    }
    static std::string getDomainName () {
      return elina_domain_impl_t::getDomainName();
    }
    
  };
  #endif

  // -- domain traits
  template<typename Number, typename VariableName, elina_domain_id_t ElinaDom>
  class domain_traits <elina_domain<Number, VariableName, ElinaDom> > {
  public:
    
    typedef elina_domain<Number, VariableName, ElinaDom> elina_domain_t;
    typedef ikos::variable<Number, VariableName> variable_t;
    
    template<class CFG>
    static void do_initialization(CFG cfg) { }
    
    static void normalize(elina_domain_t& inv) { 
      inv.normalize ();
    }
    
    template <typename Iter>
    static void forget(elina_domain_t& inv, Iter it, Iter end) {
      inv.forget (boost::make_iterator_range (it, end));
    }
    
    template <typename Iter >
    static void project(elina_domain_t& inv, Iter it, Iter end) {
      inv.project (boost::make_iterator_range (it, end));
    }
    
    static void expand(elina_domain_t& inv, variable_t x, variable_t new_x) {
      inv.expand (x, new_x);
    }		
  };
     
  // --- global datastructures
  template<typename N, typename V, elina_domain_id_t D>
  elina_manager_t* elina_domain_<N,V,D>::m_apman = nullptr;

   
} // namespace domains
}// namespace crab
#endif
