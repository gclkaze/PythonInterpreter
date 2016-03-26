// 
#include "PythonInterpreter.h"


void add_cwd_in_python_path();
int initial(int argc, char** argv);
bool callPythonInterpreter(int argc, char** argv);

int main(int argc, char *argv[])
{
	
	if (argc < 3) 
	{
		printf("Usage: exe_name python_source function_name\n");
		return 1;
	}
    //initial(argc, argv);
    int i = 0;
    while(i++ < 30)
    callPythonInterpreter(argc, argv);

}

bool callPythonInterpreter(int argc, char** argv)
{
	auto py = new Interpret::PythonIntepreter();
    auto res = py->loadModule(argv[1]);
    if(res < 0) return false;
	

    res = py->callFunction(argv[2], [&]{
    	std::vector<std::string> toString;
    	for(int i = 3; i<argc;i++) toString.push_back(argv[i]);

    	return toString;
    }());

    if(res < 0) return false;
    res = py->callFunction(argv[2], [&]{
    	std::vector<std::string> toString;
    	for(int i = 3; i<argc;i++) toString.push_back(argv[i]);

    	return toString;
    }());


    delete py;
	return res;
}


void add_cwd_in_python_path()
{
    PyObject *sys = PyImport_ImportModule("sys");
    PyObject *path = PyObject_GetAttrString(sys, "path");
    PyList_Append(path, PyUnicode_FromString("."));
}

int initial(int argc, char** argv)
{
	int i;
	PyObject *pName, *pModule, *pDict, *pFunc, *pArgs, *pValue;
		// Initialize the Python Interpreter
	Py_Initialize();

    add_cwd_in_python_path();

	// Build the name object
	pName = PyString_FromString(argv[1]);


	// Load the module object
	pModule = PyImport_Import(pName);
    assert(pModule);
	
	// pDict is a borrowed reference 
	pDict = PyModule_GetDict(pModule);

	// pFunc is also a borrowed reference 
	pFunc = PyDict_GetItemString(pDict, argv[2]);

	if (PyCallable_Check(pFunc)) 
	{
		// Prepare the argument list for the call
		if( argc > 3 )
		{
    			pArgs = PyTuple_New(argc - 3);
    			for (i = 0; i < argc - 3; i++)
    			{
					pValue = PyInt_FromLong(atoi(argv[i + 3]));
        			if (!pValue)
        			{
						PyErr_Print();
             			return 1;
        			}
        			PyTuple_SetItem(pArgs, i, pValue);	
    			}
			
				pValue = PyObject_CallObject(pFunc, pArgs);

				if (pArgs != NULL)
				{
					Py_DECREF(pArgs);
				}
		} else
		{
				pValue = PyObject_CallObject(pFunc, NULL);
		}

		if (pValue != NULL) 
		{
			printf("Return of call : %d\n", (int)PyInt_AsLong(pValue));
			Py_DECREF(pValue);
		}
		else 
		{
			PyErr_Print();
		}
	} else 
	{
		PyErr_Print();
	}

	// Clean up
	Py_DECREF(pModule);
	Py_DECREF(pName);

	// Finish the Python Interpreter
	Py_Finalize();

	return 0;
}