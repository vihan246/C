#include "Hashtable.h"
#include <iostream>
#include <string>
using namespace std;
int main()
{
    Hashtable<int> test(true, 12);
    // added to index 1
    test.add("a", 1);
    // added to index 2
    test.add("a", 2);
    // added to index 5
    test.add("a", 3);
    // added to index 10
    test.add("a", 4);
    // added to index 5; then index 2; then 1 etc. This addition takes 46,341 probes.
    test.add("a", 5);
    // hangs on this input. 
    cout << test.add("a", 6) << endl;
    //breaks the quadratic probing because the hashtable does not have the opportunity to resize at the appropriate time.
    return 0;
}