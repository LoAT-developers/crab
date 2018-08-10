#include "../program_options.hpp"
#include "../common.hpp"

using namespace crab::analyzer;
using namespace crab::cfg;
using namespace crab::cfg_impl;
using namespace crab::domain_impl;
using namespace ikos;

// to test array_expansion domain
z_cfg_t* prog1(variable_factory_t &vfac) 
{ // no overlapping, no joins
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 1 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("bb0","bb3",ARR);
  z_basic_block_t& bb0 = cfg->insert("bb0");
  z_basic_block_t& bb1 = cfg->insert("bb1");
  z_basic_block_t& bb2 = cfg->insert("bb2");
  z_basic_block_t& bb3 = cfg->insert("bb3");
  
  bb0 >> bb1;
  bb0 >> bb2;
  bb1 >> bb3;
  bb2 >> bb3;

  z_var x(vfac["x"], crab::INT_TYPE, 32);
  z_var y(vfac["y"], crab::INT_TYPE, 32);
  z_var z(vfac["z"], crab::INT_TYPE, 32);
  z_var val(vfac["val"], crab::INT_TYPE, 32);
  z_var tmp1(vfac["tmp1"], crab::INT_TYPE, 32);
  z_var tmp2(vfac["tmp2"], crab::INT_TYPE, 32);    
  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);

  bb0.assign(x, 2);  // index in mem
  bb0.assign(y, 6);  // index in mem
  bb0.assign(z, 10); // index in mem

  bb0.assign(val, 5);  
  bb0.array_store(mem, x, val, 4);
  bb0.assign(val, 6);  
  bb0.array_store(mem, y, val, 4);
  bb0.assign(val, 7);    
  bb0.array_store(mem, z, val, 4);  

  bb3.array_load(tmp1, mem, x, 4);
  bb3.array_load(tmp2, mem, z, 4);
  bb3.assertion(tmp1 + 2 == tmp2);
  
  return cfg;
}

z_cfg_t* prog2(variable_factory_t &vfac) 
{ // overlapping, no joins
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 2 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("bb0","bb3",ARR);
  z_basic_block_t& bb0 = cfg->insert("bb0");
  z_basic_block_t& bb1 = cfg->insert("bb1");
  z_basic_block_t& bb2 = cfg->insert("bb2");
  z_basic_block_t& bb3 = cfg->insert("bb3");
  
  bb0 >> bb1;
  bb0 >> bb2;
  bb1 >> bb3;
  bb2 >> bb3;

  z_var x(vfac["x"], crab::INT_TYPE, 32);
  z_var y(vfac["y"], crab::INT_TYPE, 32);
  z_var z(vfac["z"], crab::INT_TYPE, 32);
  z_var val(vfac["val"], crab::INT_TYPE, 32);
  z_var tmp1(vfac["tmp1"], crab::INT_TYPE, 32);
  z_var tmp2(vfac["tmp2"], crab::INT_TYPE, 32);
  z_var tmp3(vfac["tmp3"], crab::INT_TYPE, 32);      
  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);

  bb0.assign(x, 4);  // index in mem
  bb0.assign(y, 6);  // index in mem
  bb0.assign(z, 10); // index in mem

  bb0.assign(val, 5);  
  bb0.array_store(mem, x, val, 4);
  bb0.assign(val, 6);  
  bb0.array_store(mem, y, val, 4); // destroy x
  bb0.assign(val, 7);    
  bb0.array_store(mem, z, val, 4);  

  bb3.array_load(tmp1, mem, x, 4); // tmp1 should be top
  bb3.array_load(tmp2, mem, y, 4); // tmp2 = 6
  bb3.array_load(tmp3, mem, z, 4); // tmp3 = 7
  bb3.assertion(tmp1 + 1 == tmp2); // WARNING
  bb3.assertion(tmp2 + 1 == tmp3); // OK  
  
  return cfg;
}

z_cfg_t* prog3(variable_factory_t &vfac) 
{ // overlapping, no joins
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 3 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("bb0","bb3",ARR);
  z_basic_block_t& bb0 = cfg->insert("bb0");
  z_basic_block_t& bb1 = cfg->insert("bb1");
  z_basic_block_t& bb2 = cfg->insert("bb2");
  z_basic_block_t& bb3 = cfg->insert("bb3");
  
  bb0 >> bb1;
  bb0 >> bb2;
  bb1 >> bb3;
  bb2 >> bb3;

  z_var x(vfac["x"]  , crab::INT_TYPE, 32);
  z_var x1(vfac["x1"], crab::INT_TYPE, 32);
  z_var x2(vfac["x2"], crab::INT_TYPE, 32);
  z_var x3(vfac["x3"], crab::INT_TYPE, 32);
  z_var x4(vfac["x4"], crab::INT_TYPE, 32);
  
  z_var tmp1(vfac["tmp1"], crab::INT_TYPE, 16);
  z_var tmp2(vfac["tmp2"], crab::INT_TYPE, 16);
  z_var tmp3(vfac["tmp3"], crab::INT_TYPE, 16);
  z_var tmp4(vfac["tmp4"], crab::INT_TYPE, 16);  
  
  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);

  z_var val(vfac["val"], crab::INT_TYPE, 64);  
  

  // indexes in mem
  bb0.assign(x, 4);   
  bb0.assign(x1, 4);  
  bb0.assign(x2, 6);  
  bb0.assign(x3, 8);  
  bb0.assign(x4, 10); 

  bb0.assign(val, 0);  
  bb0.array_store(mem, x, val, 8);
  bb3.array_load(tmp1, mem, x1, 2);
  bb3.array_load(tmp2, mem, x2, 2);  
  bb3.array_load(tmp3, mem, x3, 2);
  bb3.array_load(tmp4, mem, x4, 2);  
  
  bb3.assertion(tmp1 == 0); // WARNING
  bb3.assertion(tmp2 == 0); // WARNING
  bb3.assertion(tmp3 == 0); // WARNING
  bb3.assertion(tmp4 == 0); // WARNING  
  
  return cfg;
}

z_cfg_t* prog4(variable_factory_t &vfac) {
  // i  and j are stored in an array rather than being scalar variables.
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 4 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("loop1_entry","ret",ARR);
  z_basic_block_t& loop1_entry = cfg->insert ("loop1_entry");
  z_basic_block_t& loop1_bb1   = cfg->insert ("loop1_bb1");
  z_basic_block_t& loop1_bb1_t = cfg->insert ("loop1_bb1_t");
  z_basic_block_t& loop1_bb1_f = cfg->insert ("loop1_bb1_f");
  z_basic_block_t& loop1_bb2   = cfg->insert ("loop1_bb2");
  z_basic_block_t& loop2_entry = cfg->insert ("loop2_entry");
  z_basic_block_t& loop2_bb1   = cfg->insert ("loop2_bb1");
  z_basic_block_t& loop2_bb1_t = cfg->insert ("loop2_bb1_t");
  z_basic_block_t& loop2_bb1_f = cfg->insert ("loop2_bb1_f");
  z_basic_block_t& loop2_bb2   = cfg->insert ("loop2_bb2");
  z_basic_block_t& ret         = cfg->insert ("ret");

  loop1_entry >> loop1_bb1;
  loop1_bb1 >> loop1_bb1_t; loop1_bb1 >> loop1_bb1_f;
  loop1_bb1_t >> loop1_bb2; loop1_bb2 >> loop1_bb1; loop1_bb1_f >> loop2_entry;

  loop2_entry >> loop2_bb1;
  loop2_bb1 >> loop2_bb1_t; loop2_bb1 >> loop2_bb1_f;
  loop2_bb1_t >> loop2_bb2; loop2_bb2 >> loop2_bb1; loop2_bb1_f >> ret;

  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);
  z_var base(vfac["Mem_base"], crab::INT_TYPE, 32);  
  z_var i(vfac["i"], crab::INT_TYPE, 32);
  z_var j(vfac["j"], crab::INT_TYPE, 32);
  z_var mem_i(vfac["*i"], crab::INT_TYPE, 32);
  z_var mem_j(vfac["*j"], crab::INT_TYPE, 32);    
  z_var val0(vfac["val0"], crab::INT_TYPE, 32);

  // set register addresses
  loop1_entry.assign(base, 100 /* some constant value here*/);    
  loop1_entry.add(i, base, 8);
  loop1_entry.add(j, base, 12);
  // i := 0
  loop1_entry.assign(val0, 0);    
  loop1_entry.array_store(mem, i, val0, 4);
  // assume(i <= 9)
  loop1_bb1_t.array_load(mem_i, mem, i, 4);  
  loop1_bb1_t.assume (mem_i <= 9);
  // assume(i >= 10)  
  loop1_bb1_f.array_load(mem_i, mem, i, 4);  
  loop1_bb1_f.assume (mem_i >= 10);
  // i := i + 1 
  loop1_bb2.array_load(mem_i, mem, i, 4);
  loop1_bb2.add (mem_i, mem_i, 1);
  loop1_bb2.array_store(mem, i, mem_i, 4);
  // j := 0
  loop2_entry.array_store(mem, j, val0, 4);
  // assume(j <= 9)
  loop2_bb1_t.array_load(mem_j, mem, j, 4);  
  loop2_bb1_t.assume (mem_j <= 9);
  // assume(j >= 10)
  loop2_bb1_f.array_load(mem_j, mem, j, 4);  
  loop2_bb1_f.assume (mem_j >= 10);
  // j := j + 1
  loop2_bb2.array_load(mem_j, mem, j, 4);
  loop2_bb2.add (mem_j, mem_j, 1);
  loop2_bb2.array_store(mem, j, mem_j, 4);

  ret.array_load(mem_i, mem, i, 4);  
  ret.array_load(mem_j, mem, j, 4);
  ret.assertion(mem_i == 10);
  ret.assertion(mem_j == 10);  
  return cfg;
}

z_cfg_t* prog5(variable_factory_t &vfac) {
  // similar to prog4 but with an extra level of indirection for i and j
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 5 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("loop1_entry","ret",ARR);
  z_basic_block_t& loop1_entry = cfg->insert ("loop1_entry");
  z_basic_block_t& loop1_bb1   = cfg->insert ("loop1_bb1");
  z_basic_block_t& loop1_bb1_t = cfg->insert ("loop1_bb1_t");
  z_basic_block_t& loop1_bb1_f = cfg->insert ("loop1_bb1_f");
  z_basic_block_t& loop1_bb2   = cfg->insert ("loop1_bb2");
  z_basic_block_t& loop2_entry = cfg->insert ("loop2_entry");
  z_basic_block_t& loop2_bb1   = cfg->insert ("loop2_bb1");
  z_basic_block_t& loop2_bb1_t = cfg->insert ("loop2_bb1_t");
  z_basic_block_t& loop2_bb1_f = cfg->insert ("loop2_bb1_f");
  z_basic_block_t& loop2_bb2   = cfg->insert ("loop2_bb2");
  z_basic_block_t& ret         = cfg->insert ("ret");

  loop1_entry >> loop1_bb1;
  loop1_bb1 >> loop1_bb1_t; loop1_bb1 >> loop1_bb1_f;
  loop1_bb1_t >> loop1_bb2; loop1_bb2 >> loop1_bb1; loop1_bb1_f >> loop2_entry;

  loop2_entry >> loop2_bb1;
  loop2_bb1 >> loop2_bb1_t; loop2_bb1 >> loop2_bb1_f;
  loop2_bb1_t >> loop2_bb2; loop2_bb2 >> loop2_bb1; loop2_bb1_f >> ret;

  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);
  z_var base(vfac["Mem_base"], crab::INT_TYPE, 32);  
  z_var i(vfac["i"], crab::INT_TYPE, 32);
  z_var j(vfac["j"], crab::INT_TYPE, 32);
  z_var ii(vfac["ii"], crab::INT_TYPE, 32);
  z_var jj(vfac["jj"], crab::INT_TYPE, 32);
  z_var mem_i(vfac["*i"], crab::INT_TYPE, 32);
  z_var tmp(vfac["tmp"], crab::INT_TYPE, 32);  
  z_var mem_j(vfac["*j"], crab::INT_TYPE, 32);    
  z_var val0(vfac["val0"], crab::INT_TYPE, 32);

  // set register addresses
  loop1_entry.assign(base, 100 /* some constant value here*/);    
  loop1_entry.add(i, base, 24);
  loop1_entry.add(j, base, 28);
  loop1_entry.add(ii, base, 124);
  loop1_entry.add(jj, base, 128);
  loop1_entry.array_store(mem, i, ii, 4);
  loop1_entry.array_store(mem, j, jj, 4);
  loop1_entry.assign(val0, 0);
  
  // i := 0
  loop1_entry.array_load(mem_i, mem, i, 4);    
  loop1_entry.array_store(mem, mem_i, val0, 4);
  // assume(i <= 9)
  loop1_bb1_t.array_load(mem_i, mem, i, 4);
  loop1_bb1_t.array_load(mem_i, mem, mem_i, 4);    
  loop1_bb1_t.assume (mem_i <= 9);
  // assume(i >= 10)
  loop1_bb1_f.array_load(mem_i, mem, i, 4);    
  loop1_bb1_f.array_load(mem_i, mem, mem_i, 4);  
  loop1_bb1_f.assume (mem_i >= 10);
  // i := i + 1
  loop1_bb2.array_load(mem_i, mem, i, 4);  
  loop1_bb2.array_load(mem_i, mem, mem_i, 4);
  loop1_bb2.add (mem_i, mem_i, 1);
  loop1_bb2.array_load(tmp, mem, i, 4);    
  loop1_bb2.array_store(mem, tmp, mem_i, 4);
  // j := 0
  loop2_entry.array_load(mem_j, mem, j, 4);      
  loop2_entry.array_store(mem, mem_j, val0, 4);
  // assume(j <= 9)
  loop2_bb1_t.array_load(mem_j, mem, j, 4);  
  loop2_bb1_t.array_load(mem_j, mem, mem_j, 4);  
  loop2_bb1_t.assume (mem_j <= 9);
  // assume(j >= 10)
  loop2_bb1_f.array_load(mem_j, mem, j, 4);    
  loop2_bb1_f.array_load(mem_j, mem, mem_j, 4);  
  loop2_bb1_f.assume (mem_j >= 10);
  // j := j + 1
  loop2_bb2.array_load(mem_j, mem, j, 4);  
  loop2_bb2.array_load(mem_j, mem, mem_j, 4);
  loop2_bb2.add (mem_j, mem_j, 1);
  loop2_bb2.array_load(tmp, mem, j, 4);      
  loop2_bb2.array_store(mem, tmp, mem_j, 4);

  ret.array_load(mem_i, mem, i, 4);    
  ret.array_load(mem_i, mem, mem_i, 4);  
  ret.array_load(mem_j, mem, j, 4);
  ret.array_load(mem_j, mem, mem_j, 4);  
  ret.assertion(mem_i == 10);
  ret.assertion(mem_j == 10);  
  return cfg;
}

z_cfg_t* prog6(variable_factory_t &vfac) {
  // similar to prog5 but all array offsets are negative
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 6 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("loop1_entry","ret",ARR);
  z_basic_block_t& loop1_entry = cfg->insert ("loop1_entry");
  z_basic_block_t& loop1_bb1   = cfg->insert ("loop1_bb1");
  z_basic_block_t& loop1_bb1_t = cfg->insert ("loop1_bb1_t");
  z_basic_block_t& loop1_bb1_f = cfg->insert ("loop1_bb1_f");
  z_basic_block_t& loop1_bb2   = cfg->insert ("loop1_bb2");
  z_basic_block_t& loop2_entry = cfg->insert ("loop2_entry");
  z_basic_block_t& loop2_bb1   = cfg->insert ("loop2_bb1");
  z_basic_block_t& loop2_bb1_t = cfg->insert ("loop2_bb1_t");
  z_basic_block_t& loop2_bb1_f = cfg->insert ("loop2_bb1_f");
  z_basic_block_t& loop2_bb2   = cfg->insert ("loop2_bb2");
  z_basic_block_t& ret         = cfg->insert ("ret");

  loop1_entry >> loop1_bb1;
  loop1_bb1 >> loop1_bb1_t; loop1_bb1 >> loop1_bb1_f;
  loop1_bb1_t >> loop1_bb2; loop1_bb2 >> loop1_bb1; loop1_bb1_f >> loop2_entry;

  loop2_entry >> loop2_bb1;
  loop2_bb1 >> loop2_bb1_t; loop2_bb1 >> loop2_bb1_f;
  loop2_bb1_t >> loop2_bb2; loop2_bb2 >> loop2_bb1; loop2_bb1_f >> ret;

  z_var mem(vfac["Mem"], crab::ARR_INT_TYPE);
  z_var base(vfac["Mem_base"], crab::INT_TYPE, 32);  
  z_var i(vfac["i"], crab::INT_TYPE, 32);
  z_var j(vfac["j"], crab::INT_TYPE, 32);
  z_var ii(vfac["ii"], crab::INT_TYPE, 32);
  z_var jj(vfac["jj"], crab::INT_TYPE, 32);
  z_var mem_i(vfac["*i"], crab::INT_TYPE, 32);
  z_var tmp(vfac["tmp"], crab::INT_TYPE, 32);  
  z_var mem_j(vfac["*j"], crab::INT_TYPE, 32);    
  z_var val0(vfac["val0"], crab::INT_TYPE, 32);

  // set register addresses
  loop1_entry.assign(base, 0 /* some constant value here*/);    
  loop1_entry.sub(i, base, 24);
  loop1_entry.sub(j, base, 28);
  loop1_entry.sub(ii, base, 124);
  loop1_entry.sub(jj, base, 128);
  loop1_entry.array_store(mem, i, ii, 4);
  loop1_entry.array_store(mem, j, jj, 4);
  loop1_entry.assign(val0, 0);
  
  // i := 0
  loop1_entry.array_load(mem_i, mem, i, 4);    
  loop1_entry.array_store(mem, mem_i, val0, 4);
  // assume(i <= 9)
  loop1_bb1_t.array_load(mem_i, mem, i, 4);
  loop1_bb1_t.array_load(mem_i, mem, mem_i, 4);    
  loop1_bb1_t.assume (mem_i <= 9);
  // assume(i >= 10)
  loop1_bb1_f.array_load(mem_i, mem, i, 4);    
  loop1_bb1_f.array_load(mem_i, mem, mem_i, 4);  
  loop1_bb1_f.assume (mem_i >= 10);
  // i := i + 1
  loop1_bb2.array_load(mem_i, mem, i, 4);  
  loop1_bb2.array_load(mem_i, mem, mem_i, 4);
  loop1_bb2.add (mem_i, mem_i, 1);
  loop1_bb2.array_load(tmp, mem, i, 4);    
  loop1_bb2.array_store(mem, tmp, mem_i, 4);
  // j := 0
  loop2_entry.array_load(mem_j, mem, j, 4);      
  loop2_entry.array_store(mem, mem_j, val0, 4);
  // assume(j <= 9)
  loop2_bb1_t.array_load(mem_j, mem, j, 4);  
  loop2_bb1_t.array_load(mem_j, mem, mem_j, 4);  
  loop2_bb1_t.assume (mem_j <= 9);
  // assume(j >= 10)
  loop2_bb1_f.array_load(mem_j, mem, j, 4);    
  loop2_bb1_f.array_load(mem_j, mem, mem_j, 4);  
  loop2_bb1_f.assume (mem_j >= 10);
  // j := j + 1
  loop2_bb2.array_load(mem_j, mem, j, 4);  
  loop2_bb2.array_load(mem_j, mem, mem_j, 4);
  loop2_bb2.add (mem_j, mem_j, 1);
  loop2_bb2.array_load(tmp, mem, j, 4);      
  loop2_bb2.array_store(mem, tmp, mem_j, 4);

  ret.array_load(mem_i, mem, i, 4);    
  ret.array_load(mem_i, mem, mem_i, 4);  
  ret.array_load(mem_j, mem, j, 4);
  ret.array_load(mem_j, mem, mem_j, 4);  
  ret.assertion(mem_i == 10);
  ret.assertion(mem_j == 10);  
  return cfg;
}


z_cfg_t* prog7(variable_factory_t &vfac) {
  // same sequence of bytes in different arrays 
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 7 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("entry","ret",ARR);
  z_basic_block_t& entry = cfg->insert ("entry");
  z_basic_block_t& ret = cfg->insert ("ret");
  z_var m1(vfac["Mem1"], crab::ARR_INT_TYPE);
  z_var m2(vfac["Mem2"], crab::ARR_INT_TYPE);
  z_var m3(vfac["Mem3"], crab::ARR_INT_TYPE);
  z_var x(vfac["x"], crab::INT_TYPE, 32);
  z_var y(vfac["y"], crab::INT_TYPE, 32);
  z_var z(vfac["z"], crab::INT_TYPE, 32);
  z_var val(vfac["val"], crab::INT_TYPE, 32);
  z_var tmp1(vfac["tmp1"], crab::INT_TYPE, 32);
  z_var tmp2(vfac["tmp2"], crab::INT_TYPE, 32);
  z_var tmp3(vfac["tmp3"], crab::INT_TYPE, 32);

  entry >> ret;
  
  entry.assign(x, 4); // index in mem1
  entry.assign(y, 4); // index in mem2
  entry.assign(z, 4); // index in mem3
  
  entry.assign(val, 42);
  entry.array_store(m1, x, val, 4);
  entry.assign(val, 43);  
  entry.array_store(m2, y, val, 4);
  entry.assign(val, 44);  
  entry.array_store(m3, z, val, 4);

  entry.array_load(tmp1, m1, x, 4);
  entry.array_load(tmp2, m2, y, 4);
  entry.array_load(tmp3, m3, z, 4);
  
  ret.assertion(z_lin_t(tmp1) < tmp2);
  ret.assertion(z_lin_t(tmp2) < tmp3);  
  return cfg;
}

z_cfg_t* prog8(variable_factory_t &vfac) {
  // same sequence of bytes in different arrays 
  crab::outs () << "===================================\n";  
  crab::outs () << " Test 8 for array expansion domain \n";
  crab::outs () << "===================================\n";    
  z_cfg_t* cfg = new z_cfg_t("entry","ret",ARR);
  z_basic_block_t& entry = cfg->insert ("entry");
  z_basic_block_t& ret = cfg->insert ("ret");
  z_var m1(vfac["Mem1"], crab::ARR_INT_TYPE);
  z_var m2(vfac["Mem2"], crab::ARR_INT_TYPE);
  z_var m3(vfac["Mem3"], crab::ARR_INT_TYPE);
  z_var x(vfac["x"], crab::INT_TYPE, 32);
  z_var y(vfac["y"], crab::INT_TYPE, 32);
  z_var z(vfac["z"], crab::INT_TYPE, 32);
  z_var val(vfac["val"], crab::INT_TYPE, 32);
  z_var tmp1(vfac["tmp1"], crab::INT_TYPE, 32);
  z_var tmp2(vfac["tmp2"], crab::INT_TYPE, 32);
  z_var tmp3(vfac["tmp3"], crab::INT_TYPE, 32);

  entry >> ret;
  
  entry.assign(x, 4); // index in mem1
  entry.assign(y, 5); // index in mem2
  entry.assign(z, 6); // index in mem3
  
  entry.assign(val, 42);
  entry.array_store(m1, x, val, 4);
  entry.assign(val, 43);  
  entry.array_store(m2, y, val, 4);
  entry.assign(val, 44);  
  entry.array_store(m3, z, val, 4);

  entry.array_load(tmp1, m1, x, 4);
  entry.array_load(tmp2, m2, y, 4);
  entry.array_load(tmp3, m3, z, 4);
  
  ret.assertion(z_lin_t(tmp1) < tmp2);
  ret.assertion(z_lin_t(tmp2) < tmp3);  
  return cfg;
}

void test_array_expansion(int test) {
  variable_factory_t vfac;
  z_cfg_t* cfg = nullptr;
  switch(test) {
  case 1: cfg = prog1(vfac); break;
  case 2: cfg = prog2(vfac); break;
  case 3: cfg = prog3(vfac); break;
  case 4: cfg = prog4(vfac); break;
  case 5: cfg = prog5(vfac); break;
  case 6: cfg = prog6(vfac); break;
  case 7: cfg = prog7(vfac); break;
  case 8: cfg = prog8(vfac); break;                    
  default:
    crab::outs() << "No test selected\n";
  }
  
  if (cfg) {
    run_and_check<z_ae_term_int_t>(cfg,cfg->entry(),false,1,2,20,stats_enabled);
    delete cfg;
  }
}

int main (int argc, char** argv) {
  SET_TEST_OPTIONS(argc,argv)

  test_array_expansion(1);
  test_array_expansion(2);
  test_array_expansion(3);
  test_array_expansion(4);
  test_array_expansion(5);
  test_array_expansion(6);
  test_array_expansion(7);
  test_array_expansion(8);          
  return 0;
}