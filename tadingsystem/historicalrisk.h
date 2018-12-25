
#ifndef HISTORY_RISK
#define HISTORY_RISK
#include "bondriskservice.h"
#include "soaservices/soa.hpp"
#include "soaservices/historicaldataservice.hpp"


class HistoricalRiskServiceConnector : public Connector<PV01<BucketedSector<Bond>>>
{
public:
	// Publish data
	virtual void Publish(PV01<BucketedSector<Bond>> &data) override
	{
		ofstream file;
		file.open("risk.txt", ios::app);

		string productname = data.GetProduct().GetName();

		// make the output
		file << systemtime() <<","<< productname <<","<< data.GetPV01()<<","<<data.GetQuantity()<< endl;
	}
};


class HistoricalRiskService : public HistoricalDataService<PV01<BucketedSector<Bond>>>
{
public:
	HistoricalRiskService() {}

	// Get data on our service given a key
	virtual PV01<BucketedSector<Bond>>& GetData(string key)override
	{
		return data[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PV01<BucketedSector<Bond>> &data) override
	{
		connector->Publish(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PV01<BucketedSector<Bond>>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PV01<BucketedSector<Bond>>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Persist data to a store
	virtual void PersistData(string persistKey, PV01<BucketedSector<Bond>>& data) override
	{
		connector->Publish(data);
	}

	shared_ptr<HistoricalRiskServiceConnector> connector;

private:
	map<string, PV01<BucketedSector<Bond>>> data;
	vector<ServiceListener<PV01<BucketedSector<Bond>>>*> listeners;
};



class HistoricalRiskServiceListener : public ServiceListener<PV01<BucketedSector<Bond>>>
{
private:
	shared_ptr<HistoricalRiskService> service;
public:
	// Connector and Destructor
	HistoricalRiskServiceListener(shared_ptr<HistoricalRiskService> _service)
	{
		service = _service;
	}
	~HistoricalRiskServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(PV01<BucketedSector<Bond>>& data) override
	{
		service->PersistData(string("a"),data);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(PV01<BucketedSector<Bond>>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(PV01<BucketedSector<Bond>>& data) override
	{
	}
};




#endif

