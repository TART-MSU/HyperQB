#!/bin/sh

ltlfilt=../../bin/ltlfilt
ltl2tgba=../../bin/ltl2tgba
dstar2tgba=../../bin/dstar2tgba
timeout='timeout -sKILL 15m'
stats=--stats="%s, %e, %t, %a, %c, %d, %p, %r, %R"
empty='-, -, -, -, -, -, -, -, -'
tomany='!, !, !, !, !, !, !, !, !'
dbamin=${DBA_MINIMIZER}

get_stats()
{
  type=$1
  shift
  SPOT_SATLOG=$n.$type.satlog $timeout "$@" "$stats"> stdin.$$ 2>stderr.$$
  if grep -q 'INT_MAX' stderr.$$; then
    # Too many SAT-clause?
    echo "tomany"
  else
    tmp=`cat stdin.$$`
    echo ${tmp:-$empty}
  fi
  rm -f stdin.$$ stderr.$$
}

get_dbamin_stats()
{
  tmp=`./rundbamin.pl $timeout $dbamin "$@"`
  mye='-, -'
  echo ${tmp:-$mye}
}

n=$1
f=$2
type=$3
accmax=$4

case $type in
    *WDBA*)
  exit 0
  ;;
    *TCONG*|*trad*)  # Not in WDBA
  echo "$f, $accmax, $type..." 1>&2
  input=`get_stats TBA $ltl2tgba "$f" -D -x '!wdba-minimize,tba-det'`
  echo "$f, $accmax, $type, $input, DBA, ..." 1>&2

  dba=`get_stats BA $ltl2tgba "$f" -BD -x '!wdba-minimize,tba-det'`

  # Glucose (As before)
  export SPOT_SATSOLVER="glucose -verb=0 -model %I > %O"
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu..." 1>&2
  mindba_glu=`get_stats DBA.glu $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu..." 1>&2
  mindtgba_glu=`get_stats DTGBA.glu $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=4'`

  # PicoSAT (As before)
  export SPOT_SATSOLVER="picosat %I > %O"
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic..." 1>&2
  mindba_pic=`get_stats DBA.pic $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic..." 1>&2
  mindtgba_pic=`get_stats DTGBA.pic $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=4'`

  # PicoLibrary
  unset SPOT_SATSOLVER
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp..." 1>&2
  mindba_libp=`get_stats DBA.libp $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp..." 1>&2
  mindtgba_libp=`get_stats DTGBA.libp $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=4'`

  # Incr Naive
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1..." 1>&2
  mindba_incr1=`get_stats DBA.incr1 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=3,param=-1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1..." 1>&2
  mindtgba_incr1=`get_stats DTGBA.incr1 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=3,param=-1'`

  # Incr param=1
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1..." 1>&2
  mindba_incr2p1=`get_stats DBA.incr2p1 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=3,param=1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1..." 1>&2
  mindtgba_incr2p1=`get_stats DTGBA.incr2p1 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=3,param=1'`

  # Incr param=2
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2..." 1>&2
  mindba_incr2p2=`get_stats DBA.incr2p2 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=3,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2..." 1>&2
  mindtgba_incr2p2=`get_stats DTGBA.incr2p2 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=3,param=2'`

  # Incr param=4
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4..." 1>&2
  mindba_incr2p4=`get_stats DBA.incr2p4 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=3,param=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4..." 1>&2
  mindtgba_incr2p4=`get_stats DTGBA.incr2p4 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=3,param=4'`

  # Incr param=8
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8..." 1>&2
  mindba_incr2p8=`get_stats DBA.incr2p8 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=3,param=8'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8..." 1>&2
  mindtgba_incr2p8=`get_stats DTGBA.incr2p8 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=3,param=8'`

  # Assume param=1
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1..." 1>&2
  mindba_assp1=`get_stats DBA.assp1 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1..." 1>&2
  mindtgba_assp1=`get_stats DTGBA.assp1 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=1'`

  # Assume param=2
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2..." 1>&2
  mindba_assp2=`get_stats DBA.assp2 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2..." 1>&2
  mindtgba_assp2=`get_stats DTGBA.assp2 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=2'`

  # Assume param=3
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3..." 1>&2
  mindba_assp3=`get_stats DBA.assp3 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3..." 1>&2
  mindtgba_assp3=`get_stats DTGBA.assp3 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=2'`

  # Assume param=4
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4..." 1>&2
  mindba_assp4=`get_stats DBA.assp4 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4..." 1>&2
  mindtgba_assp4=`get_stats DTGBA.assp4 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=4'`

  # Assume param=5
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5..." 1>&2
  mindba_assp5=`get_stats DBA.assp5 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=5'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5..." 1>&2
  mindtgba_assp5=`get_stats DTGBA.assp5 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=5'`

  # Assume param=6
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6..." 1>&2
  mindba_assp6=`get_stats DBA.assp6 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=6'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6..." 1>&2
  mindtgba_assp6=`get_stats DTGBA.assp6 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=6'`

  # Assume param=7
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7..." 1>&2
  mindba_assp7=`get_stats DBA.assp7 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=7'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7..." 1>&2
  mindtgba_assp7=`get_stats DTGBA.assp7 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=7'`

  # Assume param=8
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8..." 1>&2
  mindba_assp8=`get_stats DBA.assp8 $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize=2,param=8'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8..." 1>&2
  mindtgba_assp8=`get_stats DTGBA.assp8 $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize=2,param=8'`

  # Dichotomy
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho..." 1>&2
  mindba_dicho=`get_stats DBA.dicho $ltl2tgba "$f" -BD -x '!wdba-minimize,sat-minimize'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho..." 1>&2
  mindtgba_dicho=`get_stats DTGBA.dicho $ltl2tgba "$f" -D -x '!wdba-minimize,sat-minimize'`

  # Dbaminimizer
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho, $mindtgba_dicho, dbaminimizer..." 1>&2
  case $type in
    *TCONG*) dbamin="n/a, n/a" dra="n/a";;
        *trad*)
      $ltlfilt --remove-wm -f "$f" -l |
      ltl2dstar --ltl2nba=spin:$ltl2tgba@-Ds - dra.$$
      dbamin=`get_dbamin_stats dra.$$`
      dra=`sed -n 's/States: \(.*\)/\1/p' dra.$$`
      rm dra.$$
      ;;
  esac
  ;;
    *DRA*)
  echo "$f, $accmax, $type..." 1>&2
  $ltlfilt --remove-wm -f "$f" -l |
  ltl2dstar --ltl2nba=spin:$ltl2tgba@-Ds - dra.$$
  input=`get_stats TBA $dstar2tgba dra.$$ -D -x '!wdba-minimize'`
  echo "$f, $accmax, $type, $input, DBA, ..." 1>&2
  dba=`get_stats BA $dstar2tgba dra.$$ -BD -x '!wdba-minimize'`

  # Glucose (As before)
  export SPOT_SATSOLVER="glucose -verb=0 -model %I > %O"
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu..." 1>&2
  mindba_glu=`get_stats DBA.glu $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu..." 1>&2
  mindtgba_glu=`get_stats DTGBA.glu $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=4'`

  # PicoSAT (As before)
  export SPOT_SATSOLVER="picosat %I > %O"
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic..." 1>&2
  mindba_pic=`get_stats DBA.pic $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic..." 1>&2
  mindtgba_pic=`get_stats DTGBA.pic $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=4'`

  # PicoLibrary
  unset SPOT_SATSOLVER
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp..." 1>&2
  mindba_libp=`get_stats DBA.libp $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp..." 1>&2
  mindtgba_libp=`get_stats DTGBA.libp $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=4'`

  # Incr Naive
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1..." 1>&2
  mindba_incr1=`get_stats DBA.incr1 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=3,param=-1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1..." 1>&2
  mindtgba_incr1=`get_stats DTGBA.incr1 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=3,param=-1'`

  # Incr param=1
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1..." 1>&2
  mindba_incr2p1=`get_stats DBA.incr2p1 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=3,param=1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1..." 1>&2
  mindtgba_incr2p1=`get_stats DTGBA.incr2p1 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=3,param=1'`

  # Incr param=2
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2..." 1>&2
  mindba_incr2p2=`get_stats DBA.incr2p2 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=3,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2..." 1>&2
  mindtgba_incr2p2=`get_stats DTGBA.incr2p2 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=3,param=2'`

  # Incr param=4
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4..." 1>&2
  mindba_incr2p4=`get_stats DBA.incr2p4 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=3,param=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4..." 1>&2
  mindtgba_incr2p4=`get_stats DTGBA.incr2p4 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=3,param=4'`

  # Incr param=8
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8..." 1>&2
  mindba_incr2p8=`get_stats DBA.incr2p8 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=3,param=8'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8..." 1>&2
  mindtgba_incr2p8=`get_stats DTGBA.incr2p8 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=3,param=8'`

  # Assume param=1
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1..." 1>&2
  mindba_assp1=`get_stats DBA.assp1 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=1'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1..." 1>&2
  mindtgba_assp1=`get_stats DTGBA.assp1 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=1'`

  # Assume param=2
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2..." 1>&2
  mindba_assp2=`get_stats DBA.assp2 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2..." 1>&2
  mindtgba_assp2=`get_stats DTGBA.assp2 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=2'`

  # Assume param=3
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3..." 1>&2
  mindba_assp3=`get_stats DBA.assp3 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=2'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3..." 1>&2
  mindtgba_assp3=`get_stats DTGBA.assp3 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=2'`

  # Assume param=4
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4..." 1>&2
  mindba_assp4=`get_stats DBA.assp4 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=4'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4..." 1>&2
  mindtgba_assp4=`get_stats DTGBA.assp4 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=4'`

  # Assume param=5
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5..." 1>&2
  mindba_assp5=`get_stats DBA.assp5 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=5'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5..." 1>&2
  mindtgba_assp5=`get_stats DTGBA.assp5 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=5'`

  # Assume param=6
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6..." 1>&2
  mindba_assp6=`get_stats DBA.assp6 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=6'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6..." 1>&2
  mindtgba_assp6=`get_stats DTGBA.assp6 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=6'`

  # Assume param=7
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7..." 1>&2
  mindba_assp7=`get_stats DBA.assp7 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=7'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7..." 1>&2
  mindtgba_assp7=`get_stats DTGBA.assp7 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=7'`

  # Assume param=8
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8..." 1>&2
  mindba_assp8=`get_stats DBA.assp8 $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize=2,param=8'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8..." 1>&2
  mindtgba_assp8=`get_stats DTGBA.assp8 $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize=2,param=8'`

  # Dichotomy
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho..." 1>&2
  mindba_dicho=`get_stats DBA.dicho $dstar2tgba dra.$$ -BD -x '!wdba-minimize,sat-minimize'`
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho..." 1>&2
  mindtgba_dicho=`get_stats DTGBA.dicho $dstar2tgba dra.$$ -D -x '!wdba-minimize,sat-minimize'`

  # Dbaminimizer
  echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho, $mindtgba_dicho, dbaminimizer..." 1>&2
  dbamin=`get_dbamin_stats dra.$$`
  dra=`sed -n 's/States: \(.*\)/\1/p' dra.$$`
  rm -f dra.$$
  ;;
    *not*)
  exit 0
  ;;
    *)
  echo "SHOULD NOT HAPPEND"
  exit 2
  ;;
esac

echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho, $mindtgba_dicho, dbaminimizer, $dbamin, DRA, $dra, $n" 1>&2
echo "$f, $accmax, $type, $input, DBA, $dba, minDBA.glu, $mindba_glu, minDTGBA.glu, $mindtgba_glu, minDBA.pic, $mindba_pic, minDTGBA.pic, $mindtgba_pic, minDBA.libp, $mindba_libp, minDTGBA.libp, $mindtgba_libp, minDBA.incr1, $mindba_incr1, minDTGBA.incr1, $mindtgba_incr1, minDBA.incr2p1, $mindba_incr2p1, minDTGBA.incr2p1, $mindtgba_incr2p1, minDBA.incr2p2, $mindba_incr2p2, minDTGBA.incr2p2, $mindtgba_incr2p2, minDBA.incr2p4, $mindba_incr2p4, minDTGBA.incr2p4, $mindtgba_incr2p4, minDBA.incr2p8, $mindba_incr2p8, minDTGBA.incr2p8, $mindtgba_incr2p8, minDBA.assp1, $mindba_assp1, minDTGBA.assp1, $mindtgba_assp1, minDBA.assp2, $mindba_assp2, minDTGBA.assp2, $mindtgba_assp2, minDBA.assp3, $mindba_assp3, minDTGBA.assp3, $mindtgba_assp3, minDBA.assp4, $mindba_assp4, minDTGBA.assp4, $mindtgba_assp4, minDBA.assp5, $mindba_assp5, minDTGBA.assp5, $mindtgba_assp5, minDBA.assp6, $mindba_assp6, minDTGBA.assp6, $mindtgba_assp6, minDBA.assp7, $mindba_assp7, minDTGBA.assp7, $mindtgba_assp7, minDBA.assp8, $mindba_assp8, minDTGBA.assp8, $mindtgba_assp8, minDBA.dicho, $mindba_dicho, minDTGBA.dicho, $mindtgba_dicho, dbaminimizer, $dbamin, DRA, $dra, $n"
