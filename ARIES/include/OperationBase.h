#ifndef OPERATIONBASE_H
#define OPERATIONBASE_H


#include <iostream>
#include <string>


class Sample{};
typedef std::string (Sample::*ClassProc)();	

class OperationBase
{
	public:
		OperationBase(void *Object,ClassProc Proc);

		std::string execute();

	private:
		ClassProc Proc;
		Sample * Object;
	
};


#endif