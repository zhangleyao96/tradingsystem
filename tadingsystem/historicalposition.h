#ifndef HISTORY_POSITION
#define HISTORY_POSITION
#include "bondpositionservice.h"
#include "soaservices/soa.hpp"
#include "soaservices/historicaldataservice.hpp"

class HistoricalPositionServiceConnector : public Connector<Position<Bond>>
{
public:
	// Publish data to position.txt
	virtual void Publish(Position<Bond> &data) override
	{
		ofstream file;
		file.open("position.txt", ios::app);

		time_t now = time(0);
		string dt = ctime(&now);


		auto productid = data.GetProduct().GetProductId();
		string book1 = string("TRSY1");
		string book2 = string("TRSY2");
		string book3 = string("TRSY3");
		// make the output
		file << systemtime() << "," << productid << ","<<"TRSY1:"<<data.GetPosition(book1) << ","<<"TRSY2:"<< data.GetPosition(book2)<<","<<"TRSY3:"<< data.GetPosition(book3)  <<endl;

	}
};


class HistoricalPositionService : public HistoricalDataService<Position<Bond>>
{
public:
	HistoricalPositionService() {}

	// Get data on our service given a key
	virtual Position<Bond>& GetData(string key)override
	{
		return data[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Position<Bond> &data) override
	{
		connector->Publish(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Position<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector<ServiceListener<Position<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Persist data to a store
	virtual void PersistData(string persistKey, Position<Bond>& _data) override
	{
		connector->Publish(_data);
	}

	shared_ptr<HistoricalPositionServiceConnector> connector;

private:
	map<string, Position<Bond>> data;
	vector<ServiceListener<Position<Bond>>*> listeners;
};



class HistoricalPositionServiceListener : public ServiceListener<Position<Bond>>
{
private:
	shared_ptr<HistoricalPositionService> service;
public:
	// Connector and Destructor
	HistoricalPositionServiceListener(shared_ptr<HistoricalPositionService> _service)
	{
		service = _service;
	}
	~HistoricalPositionServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Position<Bond>& data) override
	{
		service->PersistData(data.GetProduct().GetProductId(), data);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Position<Bond>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Position<Bond>& data) override
	{
	}
};




#endif