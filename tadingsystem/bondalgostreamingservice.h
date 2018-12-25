#ifndef BOND_ALGO_STREAMING_SERVICE
#define BOND_ALGO_STREAMING_SERVICE

#include "bondpricingservice.h"
#include "soaservices/streamingservice.hpp"
#include "soaservices/soa.hpp"


class AlgoStream
{
public:
	// ctor for an order
	AlgoStream(){}
	AlgoStream(shared_ptr<PriceStream<Bond>> _priceStream)
	{
		priceStream = _priceStream;
	}
	shared_ptr<PriceStream<Bond>> getdata() { return priceStream; }
private:
	shared_ptr<PriceStream<Bond>> priceStream;
};

class BondAlgoStreamingService: public Service<string, AlgoStream>
{
public:
	BondAlgoStreamingService() 
	{ 
		count = 0;
		streams = map<string, AlgoStream>();
		listeners = vector<ServiceListener<AlgoStream>*>();
	}

	// Get data on our service given a key
	virtual AlgoStream& GetData(string key)override
	{
		return streams[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(AlgoStream &data) override{}

	void algodata(Price<Bond> &data) 
	{
		// mid price and bid-offer spread from price object
		double mid = data.GetMid();
		double spread = data.GetBidOfferSpread();

		// create bid/offer price stream order with hard-coded visible and hidden quantity 
		long visibleQuantity;
		if (count % 2 == 0) visibleQuantity = 1000000;
		else if (count % 2 == 1) visibleQuantity = 2000000;
		long hiddenQuantity = 2 * visibleQuantity;
		PriceStreamOrder bidOrder(mid - spread / 2, visibleQuantity, hiddenQuantity, BID);
		PriceStreamOrder offerOrder(mid + spread / 2, visibleQuantity, hiddenQuantity, OFFER);

		// Generate a price stream based on the data
		shared_ptr<PriceStream<Bond>> stream(new PriceStream<Bond>(data.GetProduct(), bidOrder, offerOrder));

		// Add an algo stream related to the price stream to the stored data
		string productId = data.GetProduct().GetProductId();
		AlgoStream algostream(stream);
		count++;
		//streams[productId] = algostream;
		//cout << productId;
		//pass data to the next level.
		for (auto listener : listeners) listener->ProcessAdd(algostream);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<AlgoStream> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<AlgoStream>* >& GetListeners() const override
	{
		return listeners;
	}
private:
	map<string, AlgoStream> streams;
	vector<ServiceListener<AlgoStream>*> listeners;
	int count;
};

class PricingBondalgoServiceListener : public ServiceListener<Price<Bond>>
{
private:
	shared_ptr<BondAlgoStreamingService> service;


public:
	// Connector and Destructor
	PricingBondalgoServiceListener(shared_ptr<BondAlgoStreamingService> _service)
	{
		service = _service;

	}
	~PricingBondalgoServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Price<Bond>& data) override
	{
			service->algodata(data);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(Price<Bond>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(Price<Bond>& data) override
	{
	}
};

#endif // !BOND_ALGO_STREAMING_SERVICE
