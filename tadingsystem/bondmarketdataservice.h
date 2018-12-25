#ifndef BOND_MARKET_DATA_SERVICE_HPP
#define BOND_MARKET_DATA_SERVICE_HPP

#include "soaservices/marketdataservice.hpp"
#include "soaservices/soa.hpp"
#include "globalfunctions.h"
#include "soaservices/products.hpp"


class BondMarketDataConnector;

// Bond market data service
class BondMarketDataService : public MarketDataService<Bond>
{
private:
	vector<ServiceListener<OrderBook<Bond>>*> listeners;
	map<string, OrderBook<Bond>> orderbooks;

public:
	
	BondMarketDataService() 
	{
		orderbooks = map<string, OrderBook<Bond>>();
		listeners = vector<ServiceListener<OrderBook<Bond>>*>();
	} 
	~BondMarketDataService(){}
	// default ctor
	
	// Get data on our service given a key
	virtual OrderBook<Bond> & GetData(string key) override
	{
		return orderbooks[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(OrderBook<Bond> &data)override
	{
		string productId = data.GetProduct().GetProductId();
		if (orderbooks.find(productId) == orderbooks.end()) // if not found then create one
			orderbooks.insert(std::make_pair(productId, data));
		else
			orderbooks[productId] = data;

		// call the listeners
		for (auto listener : listeners)
			listener->ProcessAdd(data);
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<OrderBook<Bond>> *listener)override
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<OrderBook<Bond>>* >& GetListeners() const override
	{
		return listeners;
	}

	// Get the best bid/offer order
	virtual const BidOffer& GetBestBidOffer(const string &productId) override 
	{
		return BidOffer(orderbooks[productId].GetBidStack().front(), orderbooks[productId].GetOfferStack().front());
	}

	// Aggregate the order book
	virtual const OrderBook<Bond>& AggregateDepth(const string &productId)override 
	{
		OrderBook<Bond> orderbook = orderbooks[productId];
		std::vector<Order> bidOrders = orderbook.GetBidStack();
		std::vector<Order> offerOrders = orderbook.GetOfferStack();

		// use map for aggregation
		map<double, long> bidMap;
		map<double, long> offerMap;

		double price; // temp price

		// for bid orders
		for (auto order :bidOrders)
		{
			price = order.GetPrice();
			if (bidMap.find(price) != bidMap.end()) // if found
				bidMap[price] += order.GetQuantity();
			else
				bidMap.insert(std::make_pair(price, order.GetQuantity()));
		}

		// for offer orders
		for (auto order: offerOrders)
		{
			price = order.GetPrice();
			if (offerMap.find(price) != offerMap.end()) // if found
				offerMap[price] += order.GetQuantity();
			else
				offerMap.insert(std::make_pair(price, order.GetQuantity()));
		}

		std::vector<Order> newbidOrders;
		std::vector<Order> newofferOrders;

		// re-construct the bid and offer orders
		for (auto iter = bidMap.begin(); iter != bidMap.end(); iter++)
		{
			Order bid(iter->first, iter->second, BID);
			newbidOrders.push_back(bid);
		}
		for (auto iter = offerMap.begin(); iter != offerMap.end(); iter++)
		{
			Order offer(iter->first, iter->second, OFFER);
			newofferOrders.push_back(offer);
		}

		//new order book
		OrderBook<Bond> neworderbook(orderbook.GetProduct(), newbidOrders, newofferOrders);

		orderbooks[productId] = neworderbook;
		return orderbooks[productId];
	}

	shared_ptr<BondMarketDataConnector> connector;

};



// Corresponding subscribe connector
class BondMarketDataConnector : public Connector<OrderBook<Bond>>
{
private:
	shared_ptr<BondMarketDataService> service;

public:
	BondMarketDataConnector(shared_ptr<BondMarketDataService> _bondMarketDataService) 
	{
		service = _bondMarketDataService;
	} // ctor

																																			 // Publish data to the Connector
	virtual void Publish(OrderBook <Bond> &data) override{}

	void subscribe() {
		int rowsforoneorder = 10;
		int count = 0;
		vector<Order> bidStack;
		vector<Order> offerStack;
		string productId;
		while (true)
		{
			io_service io;
			ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 666);
			ip::tcp::socket sock(io);
			boost::system::error_code ec;
			sock.connect(ep, ec);
			if (ec)
			{
				break;
				cout << "trades line finished!" << endl;
			}

			char str[1024];
			sock.read_some(buffer(str), ec);
			if (ec)
			{
				break;
				cout << "trades line finished!" << endl;
			}
			string line = str;

			stringstream lineStream(line);
			string cell;
			vector<string> row;
			while (getline(lineStream, cell, ','))
			{
				row.push_back(cell);
			}

			productId = row[0];
			double price = MakePrice(row[1]);
			long quantity = stol(row[2]);
			PricingSide side;
			if (row[3] == "BID") side = BID;
			else if (row[3] == "OFFER") side = OFFER;
			Order order(price, quantity, side);
			switch (side)
			{
			case BID:
				bidStack.push_back(order);
				break;
			case OFFER:
				offerStack.push_back(order);
				break;
			}

			count++;
			if (count == rowsforoneorder)
			{
				Bond product = GetBond(productId);
				OrderBook<Bond> orderBook(product, bidStack, offerStack);
				service->OnMessage(orderBook);

				bidStack = vector<Order>();
				offerStack = vector<Order>();
				count = 0;
			}
		}
	}

};



#endif
