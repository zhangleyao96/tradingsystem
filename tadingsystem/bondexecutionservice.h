#ifndef BOND_EXECUTION_SERVICE
#define BOND_EXECUTION_SERVICE

#include "bondalgoexcutionservice.h"
#include "soaservices/executionservice.hpp"

class BondExecutionService : public ExecutionService<Bond>
{
protected:
	std::vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;
	map<string, ExecutionOrder<Bond>> executions;
	long counter = 0; // counter to determine the side of the algo execution

public:
	BondExecutionService() {} // empty ctor

								  // Get data on our service given a key
	virtual ExecutionOrder<Bond> & GetData(string key)override
	{
		return executions[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(ExecutionOrder<Bond>& data)override
	{	
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)override {
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const override {
		return listeners;
	}

	virtual void ExecuteOrder(ExecutionOrder<Bond>& order, Market market) override
	{
		//cout << order.GetOrderId() << endl;
		for (auto listener : listeners)
		listener->ProcessAdd(order);
	}
};
class BondExecutionListener : public ServiceListener<AlgoExecution>
{
private:
	shared_ptr<BondExecutionService>  bondExecutionService;

public:
	BondExecutionListener(shared_ptr<BondExecutionService>  _bondExecutionService){
		bondExecutionService = _bondExecutionService;
	}// ctor

	 // Listener callback to process an add event to the Service
	virtual void ProcessAdd(AlgoExecution &algoexecution) override
	{
		bondExecutionService->ExecuteOrder(algoexecution.GetOrder(), Market(BROKERTEC));
	}
	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(AlgoExecution &data) override
	{
	}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(AlgoExecution &data) override
	{
	}


};

#endif
