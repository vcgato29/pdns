cd ../regression-tests.dnsdist
DNSDISTBIN=../pdns/dnsdistdist/dnsdist ./runtests test_Protobuf.py


echo "-----------------------------------------------------------"
echo "-----------------------------------------------------------"
echo "-----------------------------------------------------------"
echo "-----------------------------------------------------------"
echo "-----------------------------------------------------------"
echo "-----------------------------------------------------------"

cat nosetests.xml

