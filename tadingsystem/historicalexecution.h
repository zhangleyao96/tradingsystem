#ifndef HISTORICAL_EXECUTION
#define HISTORICAL_EXECUTION


#include "bondexecutionservice.h"
#include "soaservices/soa.hpp"
#include "soaservices/historicaldataservice.hpp"

class HistoricalExecutionServiceConnector : public Connector<ExecutionOrder<Bond>>
{
public:
	// Publish data to execution.txt
	virtual void Publish(ExecutionOrder<Bond> &data) override
	{
		ofstream file;
		file.open("execution.txt", ios::app);

		auto productid = data.GetProduct().GetProductId();
		auto ordertype = data.GetOrderType();
		auto parentid = data.GetParentOrderId();
		string price= MakePrice(data.GetPrice());
		auto side= data.GetSide();
		auto quantity = data.GetVisibleQuantity();
		auto ischild = data.IsChildOrder();

		// make the output
		file << systemtime() << "," << productid  << "," << ordertype << "," << parentid << "," << price << "," << side <<
			"," << quantity << "," << ischild << endl;
	}
};


class HistoricalExecutionService : public HistoricalDataService<ExecutionOrder<Bond>>
{
public:
	HistoricalExecutionService() {}

	// Get data on our service given a key
	virtual ExecutionOrder<Bond>& GetData(string key)override
	{
		return data[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(ExecutionOrder<Bond> &data) override
	{
		connector->Publish(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<ExecutionOrder<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<ExecutionOrder<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Persist data to a store
	virtual void PersistData(string persistKey, ExecutionOrder<Bond>& _data) override
	{
		connector->Publish(_data);
	}

	shared_ptr<HistoricalExecutionServiceConnector> connector;
private:
	map<string, ExecutionOrder<Bond>> data;
	vector<ServiceListener<ExecutionOrder<Bond>>*> listeners;
};




class HistoricalExecutionServiceListener : public ServiceListener<ExecutionOrder<Bond>>
{
private:
	shared_ptr<HistoricalExecutionService>  historicalexecutionservice;

public:
	HistoricalExecutionServiceListener(shared_ptr<HistoricalExecutionService>  _historicalexecutionservice) {
		historicalexecutionservice = _historicalexecutionservice;
	}// ctor

	 // Listener callback to process an add event to the Service
	virtual void ProcessAdd(ExecutionOrder<Bond> &algoexecution) override
	{
		historicalexecutionservice->PersistData(algoexecution.GetOrderId(),algoexecution);
	}
	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(ExecutionOrder<Bond> &data) override
	{
	}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(ExecutionOrder<Bond> &data) override
	{
	}
};





#endif // !HISTORICAL_EXECUTION
