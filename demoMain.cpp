#include "demo.h"
#include <assert.h>

int main(int argc, char** argv)
{
    auto py = new Interpret::PythonInterpreter();
    //import spam
    auto res = py->loadModule("spam");
    assert (res == 0 );

    //spa.foo2(1,3)
    res = py->callFunction("foo2", {"1","3"});
    assert (res == 0);

    //spa.foo()
    res = py->callFunction("foo", {});

    assert (res == 0);
    return 1;
}
