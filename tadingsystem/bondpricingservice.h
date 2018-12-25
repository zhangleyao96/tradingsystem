//
// bondpricingservice.hpp
// Defines the data types and Service for internal prices.
//
// @author Leo
//

#ifndef BOND_PRICING_SERVICE_HPP
#define BOND_PRICING_SERVICE_HPP

#include <string>
#include "soaservices/soa.hpp"
#include "soaservices/products.hpp"
#include "soaservices/pricingservice.hpp"
#include "soaservices/products.hpp"
#include "globalfunctions.h"
using namespace boost::asio;


class BondPricingServiceConnector;

class BondPricingService : public PricingService<Bond>
{
public:
	// Get data on our service given a key
	BondPricingService(){ 
		prices = map<string, Price<Bond>>();
		listeners = vector<ServiceListener<Price<Bond>>*>(); 
	}
	~BondPricingService(){}
	virtual Price<Bond>& GetData(string key) override
	{
		return prices[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Price<Bond> &data) override
	{
		string productId = data.GetProduct().GetProductId();
		prices[productId] = data;
		for (auto lis : listeners) 
		{
			lis->ProcessAdd(data);
		}
	};

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.

	virtual void AddListener(ServiceListener<Price<Bond>> *listener) override
	{
		listeners.push_back(listener);
	};
	
	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Price<Bond>>* >& GetListeners() const override
	{
		return listeners;
	};

	shared_ptr<BondPricingServiceConnector> connector;
private:
	map<string, Price<Bond>> prices;
	vector<ServiceListener<Price<Bond>>*> listeners;
};






class BondPricingServiceConnector :public Connector<Bond>
{
public:
	//initialize
	BondPricingServiceConnector(shared_ptr<BondPricingService> ser) {
		service = ser;
	}

	// Publish data to the Connector
	void Publish(Bond &data) override{};
	//get data from the socket and push to the service.
	//void operator()() {}
	void subscribe() {

		//ifstream PriceData("prices.txt");
		//string line;

		while (true)
		{
			io_service io;
			ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 6688);
			ip::tcp::socket sock(io);
			boost::system::error_code ec;
			sock.connect(ep, ec);

			if (ec)
			{
				cout << "pricelinefinished!" << endl;
				break;
			}

			char str[1024];
			sock.read_some(buffer(str),ec);
			if (ec)
			{
				cout << "pricelinefinished!" << endl;
				break;
			
			}
			string line = str;
		
			stringstream lineStream(line);
			string cell;
			vector<string> cells;
			while (getline(lineStream, cell, ','))
			{
				cells.push_back(cell);
			}

			string productId = cells[0];
			double bidPrice = MakePrice(cells[1]);
			double offerPrice = MakePrice(cells[2]);
			double midPrice = (bidPrice + offerPrice) / 2.0;
			double spread = offerPrice - bidPrice;
			Bond thisbond = GetBond(productId);
			Price<Bond> price(thisbond, midPrice, spread);
			service->OnMessage(price);
		}
	}

private:
	shared_ptr<BondPricingService>  service;
};


#endif
