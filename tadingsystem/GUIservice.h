#ifndef GUISERVICE
#define GUISERVICE

#include "soaservices/soa.hpp"
#include "bondpricingservice.h"
#include "globalfunctions.h"
class GUIServiceConnector : public Connector<Price<Bond>>
{
public:
	// Publish data to gui.txt
	virtual void Publish(Price<Bond> &data) override
	{
		ofstream file;
		file.open("gui.txt", ios::app);
		std::string priceStr = MakePrice(data.GetMid());
		auto productid = data.GetProduct().GetProductId();
		// make the output
		file << systemtime()<<"," << productid << "," << priceStr << endl;
	}
};


class GUIService: public Service<string,Price<Bond>>
{
public:
	GUIService() { interval = std::chrono::milliseconds(300); }

	// Get data on our service given a key
	virtual Price<Bond>& GetData(string key)override
	{
		return prices[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Price<Bond> &data) override
	{
		connector->Publish(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Price<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Price<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}
	shared_ptr<GUIServiceConnector> connector;
	// throttles
	std::chrono::milliseconds interval;
private:
	map<string, Price<Bond>> prices;
	vector<ServiceListener<Price<Bond>>*> listeners;
};



class PricingGUIServiceListener : public ServiceListener<Price<Bond>>
{
private:
	shared_ptr<GUIService> service;
	std::chrono::system_clock::time_point lasttime;
	std::chrono::milliseconds interval;
	int counter = 0; //to count 100 times
public:
	// Connector and Destructor
	PricingGUIServiceListener(shared_ptr<GUIService> _service)
	{
		service = _service;
		interval = service->interval;
		lasttime = std::chrono::system_clock::now();
	}
	~PricingGUIServiceListener() {}

	// Listener callback to process an add event to the Service
	void ProcessAdd(Price<Bond>& data) override
	{
		//every 300ms and 100times in all.
		if (std::chrono::system_clock::now() - lasttime >= interval && counter < 100)
		{
			service->OnMessage(data);
			lasttime = std::chrono::system_clock::now(); // restart the clock
			counter++;
		}
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
#endif // ! GUISERVICE
