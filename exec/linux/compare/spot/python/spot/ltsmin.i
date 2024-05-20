// -*- coding: utf-8 -*-
// Copyright (C) by the Spot authors, see the AUTHORS file for details.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

%module(package="spot", director="1") ltsmin

%include "std_string.i"
%include "exception.i"
%import(module="spot.impl") "std_set.i"
%include "std_shared_ptr.i"

%shared_ptr(spot::bdd_dict)
%shared_ptr(spot::twa)
%shared_ptr(spot::kripke)
%shared_ptr(spot::fair_kripke)

%{
#include <spot/ltsmin/ltsmin.hh>
using namespace spot;
%}

%import(module="spot.impl") <spot/misc/common.hh>
%import(module="spot.impl") <spot/twa/bdddict.hh>
%import(module="spot.impl") <spot/twa/twa.hh>
%import(module="spot.impl") <spot/tl/formula.hh>
%import(module="spot.impl") <spot/tl/apcollect.hh>
%import(module="spot.impl") <spot/kripke/fairkripke.hh>
%import(module="spot.impl") <spot/kripke/kripke.hh>

%exception {
  try {
    $action
  }
  catch (const std::runtime_error& e)
  {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%rename(model) spot::ltsmin_model;
%rename(kripke_raw) spot::ltsmin_model::kripke;
%include <spot/ltsmin/ltsmin.hh>

%pythoncode %{
import spot
import spot.aux
import sys
import subprocess
import os.path
import re

def load(filename):
  # Compile promela model with Spins by end, even if it would be done in model.load,
  # so we can capture the output of the compilation regardless of the Jupyter version.
  # (Older Jupyter version to not send the output to the notebook, and newer versions
  # do it asynchronously in a way that make testing quite hard.)
  if filename.endswith('.pm') or filename.endswith('.pml') or filename.endswith('.prom'):
    dst = os.path.basename(filename) + '.spins'
    if not os.path.exists(dst) or (os.path.getmtime(dst) < os.path.getmtime(filename)):
      p = subprocess.run(['spins', filename], stdout=subprocess.PIPE,
                         stderr=subprocess.STDOUT,
                         universal_newlines=True)
      if p.stdout: print(re.sub(r'^\s*\[\.*\s*\]\n', '', p.stdout,
                                flags=re.MULTILINE), file=sys.stderr)
      if p.stderr: print(p.stderr, file=sys.stderr)
      p.check_returncode()
      filename = dst
  return model.load(filename)

@spot._extend(model)
class model:
  def kripke(self, ap_set, dict=spot._bdd_dict,
	      dead=spot.formula_ap('dead'),
	      compress=2):
    s = spot.atomic_prop_set()
    for ap in ap_set:
      s.insert(spot.formula_ap(ap))
    return self.kripke_raw(s, dict, dead, compress)

  def info(self):
    res = {}
    ss = self.state_size()
    res['state_size'] = ss
    res['variables'] = [(self.state_variable_name(i),
			 self.state_variable_type(i)) for i in range(ss)]
    tc = self.type_count()
    res['types'] = [(self.type_name(i),
		     [self.type_value_name(i, j)
		      for j in range(self.type_value_count(i))])
		     for i in range(tc)]
    return res

  def __repr__(self):
    res = "ltsmin model with the following variables:\n";
    info = self.info()
    for (var, t) in info['variables']:
      res += '  ' + var + ': ';
      type, vals = info['types'][t]
      if vals:
        res += str(vals)
      else:
        res += type
      res += '\n';
    return res

def require(*tools):
    """
    Exit with status code 77 if the required tool is not installed.

    This function is mostly useful in Spot test suite, where 77 is a
    code used to indicate that some test should be skipped.
    """
    import shutil
    for tool in tools:
        if tool == "divine":
            if shutil.which("divine") == None:
                print("divine not available", file=sys.stderr)
                sys.exit(77)
            try:
                out = subprocess.check_output(['divine', 'compile', '--help'],
                                              stderr=subprocess.STDOUT)
            except (subprocess.CalledProcessError):
                print("divine does not understand 'compile --help'",
                       file=sys.stderr)
                sys.exit(77)
            if b'LTSmin' not in out:
                print("divine available but no support for LTSmin",
                       file=sys.stderr)
                sys.exit(77)
        elif tool == "spins":
            if shutil.which("spins") == None:
                print("spins not available", file=sys.stderr)
                sys.exit(77)
        else:
            raise ValueError("unsupported argument for require(): " + tool)


# Load IPython specific support if we can.
try:
    # Load only if we are running IPython.
    __IPYTHON__

    from IPython.core.magic import Magics, magics_class, cell_magic
    import os
    import tempfile

    @magics_class
    class EditDVE(Magics):

        @cell_magic
        def dve(self, line, cell):
            if not line:
               raise ValueError("missing variable name for %%dve")
            with spot.aux.tmpdir():
               with tempfile.NamedTemporaryFile(dir='.', suffix='.dve') as t:
                   t.write(cell.encode('utf-8'))
                   t.flush()

                   try:
                       p = subprocess.run(['divine', 'compile',
                                             '--ltsmin', t.name],
                                            stdout=subprocess.PIPE,
                                            stderr=subprocess.PIPE,
                                            universal_newlines=True)
                       if p.stdout: print(p.stdout)
                       if p.stderr: print(p.stderr, file=sys.stderr)
                       p.check_returncode()
                       self.shell.user_ns[line] = load(t.name + '2C')
                   finally:
                       spot.aux.rm_f(t.name + '.cpp')
                       spot.aux.rm_f(t.name + '2C')

    @magics_class
    class EditPML(Magics):

        @cell_magic
        def pml(self, line, cell):
            if not line:
               raise ValueError("missing variable name for %%pml")
            with spot.aux.tmpdir():
               with tempfile.NamedTemporaryFile(dir='.', suffix='.pml') as t:
                   t.write(cell.encode('utf-8'))
                   t.flush()
                   try:
                       self.shell.user_ns[line] = load(t.name)
                   finally:
                       spot.aux.rm_f(t.name + '.spins.c')
                       spot.aux.rm_f(t.name + '.spins')

    ip = get_ipython()
    ip.register_magics(EditDVE)
    ip.register_magics(EditPML)

except (ImportError, NameError):
    pass
%}
