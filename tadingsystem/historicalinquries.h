#ifndef HISTORICAL_INQUIRIES
#define HISTORICAL_INQUIRIES


#include "bondinquiryservice.h"
#include "soaservices/soa.hpp"
#include "soaservices/historicaldataservice.hpp"
#include "soaservices/inquiryservice.hpp"
#include "globalfunctions.h"

class HistoricalInquiryServiceConnector : public Connector<Inquiry<Bond>>
{
public:
	// Publish data to allinquiries.txt
	virtual void Publish(Inquiry<Bond> &data) override
	{
		ofstream file;
		file.open("allinquiries.txt", ios::app);

		auto productid = data.GetProduct().GetProductId();
		auto id= data.GetInquiryId();
		auto price = data.GetPrice();
		auto quantity = data.GetQuantity();
		auto side = data.GetSide();
		auto state = data.GetState();
		string s_state;
		if (state = RECEIVED) s_state = "RECEIVED";
		if (state == QUOTED) s_state = "QUOTED";
		if (state == DONE) s_state = "DONE";
		string s_price = MakePrice(price);
		// make the output
		file << systemtime() << "," << productid << "," << id << "," << state << "," << s_price << "," << side <<
			"," << quantity  << endl;
	}
};



class HistoricalInquiryService:public HistoricalDataService<Inquiry<Bond>>
{
public:
	HistoricalInquiryService() {}

	// Get data on our service given a key
	virtual Inquiry<Bond>& GetData(string key)override
	{
		return data[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Inquiry<Bond> &data) override
	{
		connector->Publish(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Inquiry<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Persist data to a store
	virtual void PersistData(string persistKey, Inquiry<Bond>& data) override
	{
		connector->Publish(data);
	}

	shared_ptr<HistoricalInquiryServiceConnector> connector;
private:
	map<string, Inquiry<Bond>> data;
	vector<ServiceListener<Inquiry<Bond>>*> listeners;
};


// corresponding service listener
class HistoricalInquiryListener : public ServiceListener<Inquiry<Bond>>
{
protected:
	shared_ptr<HistoricalInquiryService>  historicalinquiryservice;

public:
	HistoricalInquiryListener(shared_ptr<HistoricalInquiryService>  _historicalinquiryservice) {
		historicalinquiryservice = _historicalinquiryservice;
	}// Ctor

   // Listener callback to process an add event to the Service
	virtual void ProcessAdd(Inquiry<Bond> &data)
	{
		Inquiry<Bond> in(data);
		historicalinquiryservice->PersistData(data.GetInquiryId(),in);
	}

	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(Inquiry<Bond> &data){}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(Inquiry<Bond> &data){}
};






#endif