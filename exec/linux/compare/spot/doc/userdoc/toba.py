#!/usr/bin/env python3
import spot, sys

# Read the input automaton from standard input, or from a supplied filename.
argc = len(sys.argv)
if argc < 2:
  filename = "-"
elif argc == 2:
  filename = sys.argv[1]
else:
   print("pass a single filename, or pipe to stdin", file=sys.stderr)
   exit(1)

aut = spot.automaton(filename)

# Make sure the acceptance condition is Büchi.  Alternatively,
# allow "t" acceptance (where every state is accepting), since we
# can interpret this as a Büchi automaton in which all states are
# marked as accepting.
acc = aut.acc()
if not (acc.is_buchi() or acc.is_t()):
   print(f"unsupported acceptance: {acc.get_acceptance()}", file=sys.stderr)
   exit(1)

# Transition-based acceptance is not supported by this format;
# convert to state-based if it isn't already.
aut = spot.sbacc(aut)

# We want one minterm per edge, as those will become letters
aut = spot.split_edges(aut)

# Now simply output the automaton in the BA format
print(aut.get_init_state_number())
for e in aut.edges():
    print(f"{e.cond.id()},{e.src}->{e.dst}")
for s in range(aut.num_states()):
    if acc.accepting(aut.state_acc_sets(s)):
       print(s)
