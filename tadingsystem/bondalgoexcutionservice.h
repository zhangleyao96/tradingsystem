#ifndef BOND_ALGO_EXCUTION_SERVICE
#define BOND_ALGO_EXCUTION_SERVICE

#include "bondmarketdataservice.h"
#include "soaservices/executionservice.hpp"

// Algo Execution with an execution order object

class AlgoExecution
{
private:
	ExecutionOrder<Bond> order;

public:
	AlgoExecution(const ExecutionOrder<Bond>& _order) : order(_order){}// ctor
	AlgoExecution(){} // empty default ctor

					   // Get the execution order
	ExecutionOrder<Bond>& GetOrder() 
	{
		return order;
	}
};


// Bond algo-execution service to determine the execution order
// key on the product identifier
// value on an AlgoExecution object
class BondAlgoExecutionService : public Service<string, AlgoExecution>
{
protected:
	std::vector<ServiceListener<AlgoExecution>*> listeners;
	map<string, AlgoExecution> algoexecutions; 
	long counter = 0; // counter to determine the side of the algo execution

public:
	BondAlgoExecutionService() {} // empty ctor

								  // Get data on our service given a key
	virtual AlgoExecution & GetData(string key)override
	{
		return algoexecutions[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(AlgoExecution& data)override
	{

	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<AlgoExecution> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<AlgoExecution>* >& GetListeners() const override{
		return listeners;
	}

	void ExecuteOrder(AlgoExecution& data)
	{
		for (auto listener : listeners)
			listener->ProcessAdd(data);
	}


};

// Bond algo-execution service listener
// registered into the bond market data service to process the orderbook data
class BondAlgoExecutionListener : public ServiceListener<OrderBook<Bond>>
{
private:
	shared_ptr<BondAlgoExecutionService>  bondAlgoExecutionService;

public:
	BondAlgoExecutionListener(shared_ptr<BondAlgoExecutionService>  _bondAlgoExecutionService) 
	{
		bondAlgoExecutionService = _bondAlgoExecutionService;
	}// ctor

	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(OrderBook<Bond> &orderbook) override
	{
		Bond product = orderbook.GetProduct();
		string productId = product.GetProductId();
		PricingSide side;
		string orderId = CreateId();
		double price;
		long quantity;

		Order bidOrder = orderbook.GetBidStack().front();
		double bidPrice = bidOrder.GetPrice();
		long bidQuantity = bidOrder.GetQuantity();
		Order offerOrder = orderbook.GetOfferStack().front();
		double offerPrice = offerOrder.GetPrice();
		long offerQuantity = offerOrder.GetQuantity();

		long count = 0;
		if (offerPrice - bidPrice <= 1.0 / 128.0)
		{
			switch (count % 2)
			{
			case 0:
				price = bidPrice;
				quantity = bidQuantity;
				side = BID;
				break;
			case 1:
				price = offerPrice;
				quantity = offerQuantity;
				side = OFFER;
				break;
			}
			count++;
			ExecutionOrder<Bond> execution(product, side, orderId, MARKET, price, quantity, 0, "", false);
			AlgoExecution algoexecution(execution);
			bondAlgoExecutionService->ExecuteOrder(algoexecution);
		}
	}
	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(OrderBook<Bond> &data) override
	{
	}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(OrderBook<Bond> &data) override
	{
	}


};




#endif