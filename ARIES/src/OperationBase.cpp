#include "../include/OperationBase.h"


OperationBase::OperationBase(void *Object, ClassProc Proc)
{
		this->Proc=Proc;
		this->Object=(Sample*)Object;
}

std::string OperationBase::execute(){return (Object->*(Proc))();}
