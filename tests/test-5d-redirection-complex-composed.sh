TEST=$(basename "${0%.sh}")
OUTPUT=$(realpath $(dirname $0))/output
OUT=$OUTPUT/$TEST.out
EOUT=$OUTPUT/$TEST.eout
LOG=$OUTPUT/$TEST.log
LOBO="./lobo_shell.x"
BASH=`which bash`

test "tests" != "$(basename $(dirname $(realpath $0)))" && { echo "FAIL: $0 not in 'tests'"; exit 1; }
! test -x $LOBO && { echo "FAIL: $LOBO must exist"; exit 2; }
mkdir -p $OUTPUT
rm -f $OUT $EOUT

#
# Test Case
#
cat >$LOG << 'EOF'
Commands:
---------
wc -l < /usr/share/dict/words
whoami > tests/output/tmp
wc -w < /usr/share/dict/words >> tests/output/tmp
cat tests/output/tmp
---------
EOF

#
# Test with LOBO
#
$LOBO >$OUT 2>&1 << 'EOF'
wc -l < /usr/share/dict/words
whoami > tests/output/tmp
wc -w < /usr/share/dict/words >> tests/output/tmp
cat tests/output/tmp
EOF

#
# Test with BASH
#
$BASH >$EOUT 2>&1 << 'EOF'
wc -l < /usr/share/dict/words
whoami > tests/output/etmp
wc -w < /usr/share/dict/words >> tests/output/etmp
cat tests/output/etmp
EOF

#
# Analyze results
#
rm -f testsss/output/tmp testsss/output/etmp

diff $EOUT $OUT >> $LOG
echo "---------" >> $LOG

diff $EOUT $OUT >/dev/null && echo "PASS $TEST" || echo "FAIL $TEST"
