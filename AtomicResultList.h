#include <Python.h>
#include <utility>
#include <iostream>
#include <atomic>

namespace Interpret{

    class ReturnValue{
        PyObject* m_Value;
        
        public:
        enum ReturnValueType{
            STRING = 0,
            NUMBER = 1,
            DICTIONARY = 2,
            LIST = 3,
            NULL_OBJECT = 4
        } ;
        
        private:
        ReturnValueType m_Type;
        
        public:
        ReturnValue(PyObject* theValue, const ReturnValueType& t)
        :  m_Value(theValue), m_Type(t)
        {

        }

        ~ReturnValue()
        {
        	if(m_Value) { 
        		Py_DECREF(m_Value);
        	}
        }

        ReturnValueType getType(){return m_Type;}
        PyObject* getValue(){return m_Value;}
    };
	
	typedef std::pair<int, ReturnValue*> ReturnValueEntry;
	
	class AtomicResultList
	{
	    struct Node {
	            Node* m_Next;

	            ReturnValueEntry* m_Value;
	        
	            Node():m_Next(nullptr){}
	            ~Node(){
	            	delete m_Value->second;
	            }
	    };

	    std::atomic<Node*> m_Head{nullptr};
        
        void push(ReturnValueEntry* entry){
    
            Node* n  = new Node();
            n->m_Value = entry;
            n->m_Next = m_Head.load();
            
            while(!m_Head.compare_exchange_weak(n->m_Next,n )){ 
                      //if head == n->next, then head = n, return true finally
                std::cout<<n->m_Next<<"  "<<m_Head<<std::endl;
            
            };
            //Outside of the cmp_xchng atomic loop, head == n->next 
            //for a slight moment, afterwards head gets value == n
            //std::cout<<head<<"=="<<n<<std::endl;
            
            if(m_Head != n){
                //Head changed again by another thread that called 
               //compare_exchange_weak after the successful cmp_xch 
               //call of the current thread!!
                //Shit!!!
                std::cout<<m_Head<<"   WHAT   "<<n<<std::endl;//exit(1);
            }
    
        }

	};
};