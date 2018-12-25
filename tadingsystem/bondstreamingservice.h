#ifndef BOND_STREAMING_SERVICE
#define BOND_STREAMING_SERVICE

#include "bondpricingservice.h"
#include "soaservices/streamingservice.hpp"
#include "soaservices/soa.hpp"
#include "bondalgostreamingservice.h"
#include "soaservices\marketdataservice.hpp"


class BondStreamingService : public StreamingService<Bond>
{
public:

	BondStreamingService() {
		prices = map<string, PriceStream<Bond>>();
		listeners = vector<ServiceListener<PriceStream<Bond>>*>();
	}

	// Get data on our service given a key
	virtual PriceStream<Bond>& GetData(string key)override
	{
		return prices[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PriceStream<Bond> &data) override
	{
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<PriceStream<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<PriceStream<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Publish two-way prices
	virtual void PublishPrice( PriceStream<Bond>& priceStream) override
	{
		for (auto listener : listeners) listener->ProcessAdd(priceStream);
	}

private:
	map<string, PriceStream<Bond>> prices;
	vector<ServiceListener<PriceStream<Bond>>*> listeners;
};

class BondStreamingServiceListener : public ServiceListener<AlgoStream>
{
private:
	shared_ptr<BondStreamingService> service;


public:
	// Connector and Destructor
	BondStreamingServiceListener(shared_ptr<BondStreamingService> _service)
	{
		service = _service;

	}
	~BondStreamingServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(AlgoStream& data) override
	{
		service->PublishPrice(*data.getdata());
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(AlgoStream& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(AlgoStream& data) override
	{
	}
};

#endif