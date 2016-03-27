#include <Python.h>
#include <utility>
#include <iostream>
#include <atomic>

namespace Interpret{

    class ReturnValue{
    public:
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
            //std::cout << "Added" << PyInt_AsLong(m_Value) << "=>" << m_Value << std::endl;
        }

        ~ReturnValue()
        {
        	if(m_Value) { 
        		//std::cout << "Going to delete => " << m_Value << std::endl;
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

	            ReturnValueEntry m_Value;
	        
	            Node():m_Next(nullptr){}
	            Node(const Node& another):m_Value(another.m_Value){
	            	std::cout << "cpy ctor" << std::endl;
	            }
	            Node(const ReturnValueEntry& another){
	            	m_Value = std::make_pair<int, ReturnValue*> (0, nullptr);
                    m_Value.first = another.first;
	            	m_Value.second = another.second;
	            	std::cout << "Node->" << m_Value.second->m_Value << std::endl;
	            }
	            ~Node(){
	            	std::cout << "Delete the node" << std::endl;
	            	assert(m_Value.second);
	            	delete m_Value.second;
	            }
	    };

	    std::atomic<Node*> m_Head{nullptr};
        public:
    	~AtomicResultList()
    	{
    		//std::cout << "AtomicResultList~" << std::endl;
    		clear();
    	}
        void clear()
        {
            Node* newHead = nullptr;
            newHead = m_Head.load();

            while(!m_Head.compare_exchange_weak(newHead,nullptr ));

            Node* current = nullptr;
            int i = 0;

            while(1){
            	current = newHead;
            	if(!current->m_Next) break;
            	newHead = current->m_Next;
                std::cout << i++ << std::endl;
            	delete current;
            }

        }

        void dump()
        {
        	Node* current = m_Head.load();
        	int size = 0;
        	while(!m_Head.compare_exchange_weak(current,m_Head));
        	while(current) {
        		std::cout << size++  << ". => " << current->m_Value.second->m_Value << std::endl;
        		current = current->m_Next;
        	}
        }

        void push(ReturnValueEntry& entry){
    
            Node* n  = new Node(entry);
            n->m_Next = m_Head.load();
            
            while(!m_Head.compare_exchange_weak(n->m_Next,n )){ 
                      //if head == n->next, then head = n, return true finally
                std::cout<<n->m_Next<<"  "<<m_Head<<std::endl;
            
            };
                
        }

	};
};