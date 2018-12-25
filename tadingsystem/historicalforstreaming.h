#ifndef HISTORY_BOND_STREAMING
#define HISTORY_BOND_STREAMING
#include "bondstreamingservice.h"
#include "soaservices/soa.hpp"
#include "soaservices/historicaldataservice.hpp"
class HistoricalStreamingDataServiceConnector : public Connector<PriceStream<Bond>>
{
public:
	// Publish data to streaming.txt
	virtual void Publish(PriceStream<Bond> &data) override
	{
		ofstream file;
		file.open("streaming.txt", ios::app);

		time_t now = time(0);
		string dt = ctime(&now);

		
		auto productid = data.GetProduct().GetProductId();
		string bidprice = MakePrice(data.GetBidOrder().GetPrice());
		auto bidvisible = data.GetBidOrder().GetVisibleQuantity();
		auto bidhid = data.GetBidOrder().GetHiddenQuantity();

		string offerprice = MakePrice(data.GetOfferOrder().GetPrice());
		auto offervisible = data.GetOfferOrder().GetVisibleQuantity();
		auto offerhid = data.GetOfferOrder().GetHiddenQuantity();

		// make the output
		file << systemtime() <<","<< productid <<","<< "BID"<<","<< bidprice<<","<< bidvisible <<","<< bidhid <<","<<"OFFER"<<","<< offerprice<<
			"," << offervisible << "," << offerhid <<endl;
	}
};


class HistoricalStreamingDataService : public HistoricalDataService<PriceStream<Bond>>
{
public:
	HistoricalStreamingDataService(){}

	// Get data on our service given a key
	virtual PriceStream<Bond>& GetData(string key)override
	{
		return data[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(PriceStream<Bond> &data) override
	{
		connector->Publish(data);
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

	// Persist data to a store
	virtual void PersistData(string persistKey, PriceStream<Bond>& _data) override
	{
		connector->Publish(_data);
	}

	shared_ptr<HistoricalStreamingDataServiceConnector> connector;

private:
	map<string, PriceStream<Bond>> data;
	vector<ServiceListener<PriceStream<Bond>>*> listeners;
};



class HistoricalStreamingDataServiceListener : public ServiceListener<PriceStream<Bond>>
{
private:
	shared_ptr<HistoricalStreamingDataService> service;
public:
	// Connector and Destructor
	HistoricalStreamingDataServiceListener(shared_ptr<HistoricalStreamingDataService> _service)
	{
		service = _service;
	}
	~HistoricalStreamingDataServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(PriceStream<Bond>& data) override
	{
			service->PersistData(data.GetProduct().GetProductId(),data);
	}

	// Listener callback to process a remove event to the Service
	void ProcessRemove(PriceStream<Bond>& data) override
	{
	}

	// Listener callback to process an update event to the Service
	void ProcessUpdate(PriceStream<Bond>& data) override
	{
	}
};




#endif