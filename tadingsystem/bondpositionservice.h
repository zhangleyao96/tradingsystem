#ifndef BOND_POSITION_SERVICE
#define BOND_POSITION_SERVICE
#include "bondtradebookingservice.h"
#include "soaservices/positionservice.hpp"

class BondPositionService : public PositionService<Bond>
{
public:
	BondPositionService()
	{
		positions = map<string, Position<Bond>>();
		listeners = vector<ServiceListener<Position<Bond>>*>();
	}

	// Get data on our service given a key
	virtual Position<Bond>& GetData(string key)override
	{
		return positions[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Position<Bond> &data) override {}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Position<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Position<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	virtual void AddTrade(Trade<Bond>& trade) override
	{
		auto side = trade.GetSide();
		auto product = trade.GetProduct();
		string productId = product.GetProductId();
		double price = trade.GetPrice();
		string book = trade.GetBook();
		long quantity = trade.GetQuantity();
		Position<Bond> newposition(product);
		
		if (positions.find(productId) == positions.end()) // if not found then create one
		{
			positions.insert(std::make_pair(productId, newposition));
			if (side == SELL)
				positions[productId].SubPosition(book,quantity);
			else
				positions[productId].AddPosition(book,quantity);
		}
		else
		{
			if (side == SELL)
				positions[productId].SubPosition(book,quantity);
			else
				positions[productId].AddPosition(book,quantity);
		}
		
		//cout << positions[productId].GetPosition(book) <<","<< positions[productId].GetAggregatePosition() << endl;
		for (auto listener : listeners) listener->ProcessAdd(positions[productId]);
	}	

	
private:
	map<string, Position<Bond>> positions;
	vector<ServiceListener<Position<Bond>>*> listeners;
};





class BondPositionServiceListener : public ServiceListener<Trade<Bond>>
{
private:
	shared_ptr<BondPositionService> service;


public:
	// Connector and Destructor
	BondPositionServiceListener(shared_ptr<BondPositionService> _service)
	{
		service = _service;
	}
	~BondPositionServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Trade<Bond>& data) override
	{
		service->AddTrade(data);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Trade<Bond>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Trade<Bond>& data) override
	{
	}
};




#endif