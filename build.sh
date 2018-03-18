WHAT=$1

BUILD="g++ -std=c++11 -ggdb $WHAT.cpp -lboost_coroutine -lboost_chrono -lboost_thread -lboost_system -o app"

echo $BUILD
$BUILD
