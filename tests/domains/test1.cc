#include "../program_options.hpp"
#include "../common.hpp"

using namespace std;
using namespace crab::analyzer;
using namespace crab::cfg;
using namespace crab::cfg_impl;
using namespace crab::domain_impl;

/* Example of how to build a CFG */
z_cfg_t* prog (variable_factory_t &vfac)  {

  // Defining program variables
  z_var i (vfac ["i"]);
  z_var k (vfac ["k"]);
  z_var nd (vfac ["nd"]);
  z_var inc (vfac ["inc"]);
  // entry and exit block
  auto cfg = new z_cfg_t("x0","ret");
  // adding blocks
  z_basic_block_t& x0 = cfg->insert ("x0");
  z_basic_block_t& x1 = cfg->insert ("x1");
  z_basic_block_t& x2 = cfg->insert ("x2");
  z_basic_block_t& x3 = cfg->insert ("x3");
  z_basic_block_t& entry = cfg->insert ("entry");
  z_basic_block_t& bb1   = cfg->insert ("bb1");
  z_basic_block_t& bb1_t = cfg->insert ("bb1_t");
  z_basic_block_t& bb1_f = cfg->insert ("bb1_f");
  z_basic_block_t& bb2   = cfg->insert ("bb2");
  z_basic_block_t& ret   = cfg->insert ("ret");
  // adding control flow
  x0 >> x1; x1 >> x2; x2 >> x3; x3 >> entry;
  entry >> bb1;
  bb1 >> bb1_t; bb1 >> bb1_f;
  bb1_t >> bb2; bb2 >> bb1; bb1_f >> ret;
  // adding statements
  x0.assign (k, 2147483648);
  entry.assign (i, 0);
  bb1_t.assume (i <= 99);
  bb1_f.assume (i >= 100);
  bb2.havoc(nd);
  bb2.select(inc,nd,1,2);
  bb2.add(i, i, inc);

  return cfg;
}

/* Example of how to infer invariants from the above CFG */
int main (int argc, char** argv )
{
  SET_TEST_OPTIONS(argc,argv)

  variable_factory_t vfac;
  z_cfg_t* cfg = prog(vfac);
  cfg->simplify (); // this is optional
  crab::outs() << *cfg << "\n";

  run<z_interval_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);
  run<z_dbm_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);
  run<z_sdbm_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);
  run<z_ric_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);
  run<z_term_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);
  run<z_dis_interval_domain_t>(cfg,  false, 1, 2, 20, stats_enabled);

  // free the CFG
  delete cfg;

  return 0;
}
