#include <assert.h>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include "AtomicResultList.h"

namespace Interpret{
    enum InterpreterStatus{
            OK = 0,
            FUNCTION_NOT_FOUND = -1,
            MODULE_NOT_FOUND = -2,
            RETURNED_NULL = -3,
            NOT_CALLABLE_OBJECT = -4,
            UNKNOWN_ERROR = -5,
            ARGUMENT_ERROR = -6,
            UNKNOWN_INTEGER_ERROR = -7
    };

    class PythonIntepreter 
    {
    private:
    	std::vector<PyObject*> m_Globals;
        PyObject * m_CurrentModule = nullptr;
        PyObject * m_CurrentTable = nullptr;
        PyObject * m_CurrentModuleName = nullptr;

        bool m_Initialized = false;

        std::map<std::string, std::pair<PyObject*, PyObject*>> m_LoadedModules;



        std::map<ReturnValue::ReturnValueType,std::function<bool(PyObject*)>> m_DataTypeCheckers;
        ReturnValue *m_Value = nullptr;

    public:

        PythonIntepreter()
        {

        }

        ~PythonIntepreter()
        {
        	cleanUp();

        	// Finish the Python Interpreter
            if(m_Initialized) {
            	std::cout << "PythonInterpreter destroyed!" << std::endl;
            	Py_Finalize();
            }
        }

        bool isInitialized(){return m_Initialized;}

        void assertNone(){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::NULL_OBJECT && m_Value->getValue() == Py_None);
        }
        
        void assertInt(const int value){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::NUMBER && (int)PyInt_AsLong(m_Value->getValue() ) == value);
        }

        void assertString(const std::string& value){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::STRING && strcmp ( (char*) PyString_AsString(m_Value->getValue() ) , value.c_str() ) == 0  );
        }

        void assertDict(){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::DICTIONARY);

        }
        void assertList(){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::LIST);
        }
        void assertFloat(const float value){
            assert(m_Value && m_Value->getType() == ReturnValue::ReturnValueType::NUMBER);
        }

        InterpreterStatus loadModule(const std::string& module)
        {
     	    if(!m_Initialized) {
        		Py_Initialize();
                addCwdInPythonPath();
                setupDataTypeCheckers();
        		m_Initialized = true;
        		std::cout << "PythonInterpreter initialized!" << std::endl;
        	}

            //If the module isnt here yet, or the same module has being loaded earlier, load it
            if(m_LoadedModules.find(module.c_str()) == m_LoadedModules.end() ) {

    	        //std::cout << "Module init" << std::endl;
     	        m_CurrentModuleName = PyString_FromString(module.c_str());


    	        // Load the module object
    	        m_CurrentModule = PyImport_Import(m_CurrentModuleName);
                if(!m_CurrentModule) return MODULE_NOT_FOUND;
    	
    	        // pDict is a borrowed reference 
    	        m_CurrentTable = PyModule_GetDict(m_CurrentModule);

                m_LoadedModules[module] = std::make_pair(m_CurrentModule,m_CurrentModuleName);

            }
            else {

            	m_CurrentModuleName = m_LoadedModules[module.c_str()].second;
            	m_CurrentModule = m_LoadedModules[module.c_str()].first; 
    	        m_CurrentTable = PyModule_GetDict(m_CurrentModule);
            }


            return OK;
        }

        bool handleReturnValue(PyObject& value)
        {
            for(auto &ch : m_DataTypeCheckers) {
            	if(ch.second(&value)) {
            	    if(m_Value) delete m_Value;
                    m_Value = new ReturnValue(&value, ch.first);
            		return true;
            	}
            }
            
            assert(nullptr);
        	return false;
        }

        PyObject* setFunctionArgs(const std::vector<std::string>& args)
        {
            int argsN = (int)args.size();
            auto pArgs = PyTuple_New(argsN );
            PyObject *pValue = nullptr;

            for (auto i = 0; i < argsN ; i++) {
                //std::cout << args[i + 0] << std::endl;
                pValue = PyInt_FromLong(atoi(args[i].c_str()));
                if (!pValue) {
                    PyErr_Print();
                    return nullptr;//UNKNOWN_INTEGER_ERROR;
                }
                PyTuple_SetItem(pArgs, i, pValue);  
            }
            return pArgs;
        }

        InterpreterStatus callFunction(const std::string& function, const std::vector<std::string> args)
        {
    	    PyObject* currentFunction = PyDict_GetItemString(m_CurrentTable, function.c_str());
            if(!currentFunction) {std::cout<<"function not found"<<std::endl;return FUNCTION_NOT_FOUND;}

        	if (PyCallable_Check(currentFunction)) {
    	    	PyObject *pValue = nullptr;
    		    // Prepare the argument list for the call
    		    int argsN = (int)args.size();
    		    if( argsN > 0 ) {
    			    auto pArgs = setFunctionArgs(args);   
    			    pValue = PyObject_CallObject(currentFunction, pArgs);
                    if (pArgs != nullptr) {
                    	Py_DECREF(pArgs);
                    }
                } 
                else {
                    pValue = PyObject_CallObject(currentFunction, nullptr);
                }

    		    if (pValue != nullptr)  {
    		    	handleReturnValue(*pValue);
    			    printf("Return of call : %d\n", (int)PyInt_AsLong(pValue));
    			    //Py_DECREF(pValue);
    		    }
    		    else {
    		    	PyErr_Print();
    		    	return ARGUMENT_ERROR;
    		    }
    		} 
    		else {
    			//std::cout << "Cant call it" << std::endl;
    		    PyErr_Print();
    		    return NOT_CALLABLE_OBJECT;
    	    }
            return OK;
        }

        InterpreterStatus callFunctions(const std::map<std::string,std::vector<std::string>> &activationRecords)
        {
            return NOT_CALLABLE_OBJECT;
        }

        InterpreterStatus callFunction(const std::string& function)
        {
           PyObject *currentFunction = PyDict_GetItemString(m_CurrentTable, function.c_str());
            if(!currentFunction) {std::cout<<"function not found"<<std::endl;return FUNCTION_NOT_FOUND;}

        	if (PyCallable_Check(currentFunction)) {
             	PyObject *pValue = nullptr;
                pValue = PyObject_CallObject(currentFunction, nullptr);

    		    if (pValue != nullptr)  {
    		    	/*if(PyDict_Check(pValue)) std::cout << "it is a dict" << std::endl;
    		    	if(PyString_Check(pValue)) std::cout << "it is a string" << std::endl;*/
    		    	handleReturnValue(*pValue);
    			    printf("Return of call : %d\n", (int)PyInt_AsLong(pValue));
    			    //Py_DECREF(pValue);
    		    }
    		    else {
    		    	PyErr_Print();
    		    	return ARGUMENT_ERROR;
    		    }
    		} 
    		else {
    			//std::cout << "Cant call it" << std::endl;
    		    PyErr_Print();
    		    return NOT_CALLABLE_OBJECT;
    	    }
            return OK;
        }

    private:
        void setupDataTypeCheckers()
        {

        	m_DataTypeCheckers[ReturnValue::STRING] = [](PyObject* value){ return PyString_Check(value) ? true : false;};
        	m_DataTypeCheckers[ReturnValue::NUMBER] = [](PyObject* value){ return PyInt_Check(value) || PyFloat_Check(value) ? true : false;};
       	    m_DataTypeCheckers[ReturnValue::DICTIONARY] = [](PyObject* value){ return PyDict_Check(value) ? true : false;};
       	    m_DataTypeCheckers[ReturnValue::LIST] = [](PyObject* value){ return PyList_Check(value) ? true : false;};
       	    m_DataTypeCheckers[ReturnValue::NULL_OBJECT] = [](PyObject* value){ return Py_None == value ? true : false;};

        };

        void addCwdInPythonPath()
        {
            PyObject *sys = PyImport_ImportModule("sys");
            PyObject *path = PyObject_GetAttrString(sys, "path");
            PyList_Append(path, PyUnicode_FromString("."));

            m_Globals.push_back(sys);
            m_Globals.push_back(path);
        }

        void cleanUp()
        {
        	// Clean up
        	//std::cout << "Destroying " << m_Locals.size() << " locals" << std::endl;
            for(auto var : m_LoadedModules) {
                Py_DECREF((var).second.first);
                Py_DECREF((var).second.second);
            }

            m_LoadedModules.clear();

        	//std::cout << "Destroying " << m_Globals.size() << " globals" << std::endl;
            for(auto &var : m_Globals) {
                Py_DECREF(var);
            }
        }
    };
};