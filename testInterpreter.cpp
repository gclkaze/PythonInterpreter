#include "PythonInterpreter.h"
void loadModule();
void callIntFunction();
void callStringFunction();
void callDictFunction();
void callListFunction();
void callNoneFunction();
void callMultiArgumentFunction();
void callNoReturnFunction();

int main(int argc, char **argv)
{
	loadModule();
	callIntFunction();
	callStringFunction();
	callNoneFunction();
	return 1;
}
void loadModule()
{
    auto py = new Interpret::PythonIntepreter();
    assert (!py->isInitialized());
    assert ( py->loadModule("lala") == Interpret::InterpreterStatus::MODULE_NOT_FOUND);
    assert (py->isInitialized());
    assert ( py->loadModule("spam") == Interpret::InterpreterStatus::OK);
    assert (py->isInitialized());
    delete py;
}

void callIntFunction()
{
    auto py = new Interpret::PythonIntepreter();
    assert ( py->loadModule("spam") == Interpret::InterpreterStatus::OK);
    assert (py->callFunction("foo") == Interpret::InterpreterStatus::OK );
    py->assertInt(6);
    delete py;
}

void callStringFunction()
{
    auto py = new Interpret::PythonIntepreter();
    assert ( py->loadModule("spam") == Interpret::InterpreterStatus::OK);
    assert (py->callFunction("string_return") == Interpret::InterpreterStatus::OK );
    py->assertString("a string");
    delete py;

}

void callDictFunction()
{

}

void callListFunction()
{

}

void callNoneFunction()
{
    auto py = new Interpret::PythonIntepreter();
    assert ( py->loadModule("spam") == Interpret::InterpreterStatus::OK);
    assert (py->callFunction("none_return") == Interpret::InterpreterStatus::OK );
    py->assertNone();
    delete py;
}

void callMultiArgumentFunction()
{

}

void callNoReturnFunction()
{

}
