#ifndef BOND_RISK_SERVICE
#define BOND_RISK_SERVICE


#include "bondpositionservice.h"
#include "soaservices/riskservice.hpp"


class BondRiskService : public RiskService<BucketedSector<Bond>>
{
public:
	BondRiskService()
	{
		bucketrisks = map<string, PV01<BucketedSector<Bond>>>();
		listeners = vector<ServiceListener<PV01<BucketedSector<Bond>>>*>();
		bucketrisks["9128285Q9"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("9128285Q9")), 0.018932, 0);
		bucketrisks["9128285R7"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("9128285R7")), 0.0213311, 0);
		bucketrisks["9128285P1"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("9128285P1")), 0.0312111, 0);
		bucketrisks["9128285N6"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("9128285N6")), 0.0498714, 0);
		bucketrisks["9128285M8"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("9128285M8")), 0.0682494, 0);
		bucketrisks["912810SE9"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("912810SE9")), 0.0804811, 0);
		bucketrisks["FrontEnd"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("FrontEnd")), 0.093765, 0);
		bucketrisks["Belly"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("Belly")), 0.1333765, 0);
		bucketrisks["LongEnd"] = PV01<BucketedSector<Bond>>(BucketedSector<Bond>(vector<Bond>(), string("LongEnd")), 0.213317, 0);
	}

	// Get data on our service given a key
	virtual PV01<BucketedSector<Bond>> & GetData(string key)override
	{
		return bucketrisks[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PV01<BucketedSector<Bond>> &data) override {}

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

	// Add a position that the service will risk
	virtual void AddPosition(Position<Bond>& position) 
	{
		auto productID = position.GetProduct().GetProductId();
		auto quantity = position.GetAggregatePosition();
		bucketrisks[productID].ChangeQuantity(quantity);
		for (auto listener : listeners) listener->ProcessAdd(bucketrisks[productID]);
	}

	// Get the bucketed risk for the bucket sector
	virtual const PV01< BucketedSector<Bond> >& GetBucketedRisk(BucketedSector<Bond> &sector)
	{
		return bucketrisks[sector.GetName()];
	}

private:

	map<string, PV01<BucketedSector<Bond>>> bucketrisks;
	vector<ServiceListener<PV01<BucketedSector<Bond>>>*> listeners;

};



//listener
class BondRiskServiceListener : public ServiceListener<Position<Bond>>
{
private:

	shared_ptr<BondRiskService> service;

public:
	// Connector and Destructor
	BondRiskServiceListener(shared_ptr<BondRiskService> _service)
	{
		service = _service;
	}
	~BondRiskServiceListener() {}

	// Listener callback to process an add event to the Service
	virtual void ProcessAdd(Position<Bond>& data) override
	{
		service->AddPosition(data);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Position<Bond>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Position<Bond>& data) override
	{
	}
};


#endif // !BOND_RISK_SERVICE

