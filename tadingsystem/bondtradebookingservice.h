#ifndef BOND_TRADE_BOOKING_SERVICE
#define BOND_TRADE_BOOKING_SERVICE

#include "soaservices/soa.hpp"
#include "soaservices/products.hpp"
#include "globalfunctions.h"
#include <string>
#include "soaservices/tradebookingservice.hpp"
#include "bondexecutionservice.h"
class BondTradeBookingServiceConnector;

class BondTradeBookingService: public TradeBookingService<Bond>
{
public:
	// Get data on our service given a key
	BondTradeBookingService() {
		trades = map<string, Trade<Bond>>();
		listeners = vector<ServiceListener<Trade<Bond>>*>();
	}
	~BondTradeBookingService() {}
	virtual Trade<Bond>& GetData(string key) override
	{
		return trades[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Trade<Bond> &data) override
	{
		string productId = data.GetProduct().GetProductId();
		trades[productId] = data;

		for (auto lis : listeners)
		{
			lis->ProcessAdd(data);
		}

	};

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.

	virtual void AddListener(ServiceListener<Trade<Bond>> *listener) override
	{
		listeners.push_back(listener);
	};

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Trade<Bond>>* >& GetListeners() const override
	{
		return listeners;
	};
	virtual  void BookTrade(const Trade<Bond> &trade) override
	{
		string productId = trade.GetProduct().GetProductId();
		trades[productId] = trade;
	}

	shared_ptr<BondTradeBookingServiceConnector> connector;
private:
	map<string, Trade<Bond>> trades;
	vector<ServiceListener<Trade<Bond>>*> listeners;
};






class BondTradeBookingServiceConnector :public Connector<Trade<Bond>>
{
public:
	//initialize
	BondTradeBookingServiceConnector(shared_ptr<BondTradeBookingService> ser)
	{
		service = ser;
	}

	// Publish data to the Connector
	void Publish(Trade<Bond> &data) override {};
	void subscribe() 
	{
		while (true)
		{
			io_service io;
			ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 80);
			ip::tcp::socket sock(io);
			boost::system::error_code ec;
			sock.connect(ep, ec);
			if (ec)
			{
				cout << "trades line finished!" << endl;
				break;
			}

			char str[1024];
			sock.read_some(buffer(str),ec);
			if (ec)
			{
				cout << "trades line finished!" << endl;
				break;
				
			}
			string line = str;
		
		
			stringstream lineStream(line);
			string element;
			vector<string> row;
			while (getline(lineStream, element, ','))
			{
				row.push_back(element);
			}

			string productId = row[0];
			string tradeId = row[1];
			double price = MakePrice(row[2]);
			string book = row[3];
			long quantity = stol(row[4]);
			Side side;
			if (row[5] == "BUY") side = BUY;
			else if (row[5] == "SELL") side = SELL;
			Bond thisbond = GetBond(productId);

			Trade<Bond> trade(thisbond, tradeId, price, book, quantity, side);
			


			service->OnMessage(trade);
		}
	}

private:
	shared_ptr<BondTradeBookingService>  service;
};


class BondExecutionBookingListener : public ServiceListener<ExecutionOrder<Bond>>
{
private:
	shared_ptr<BondTradeBookingService>  bondtradebookingservice;

public:
	BondExecutionBookingListener(shared_ptr<BondTradeBookingService>  _bondtradebookingservice) {
		bondtradebookingservice = _bondtradebookingservice;
	}// ctor

	 // Listener callback to process an add event to the Service
	virtual void ProcessAdd(ExecutionOrder<Bond> &algoexecution) override
	{
		Bond product = algoexecution.GetProduct();
		string Id = algoexecution.GetOrderId();
		PricingSide side= algoexecution.GetSide();
		Side tradeside;
		if (side == BID) { tradeside = SELL; }
		else tradeside = BUY;

		Trade<Bond> trade(product, Id, algoexecution.GetPrice(), string("TRSY1"), algoexecution.GetVisibleQuantity(),tradeside);
		bondtradebookingservice->OnMessage(trade);
	}
	// Listener callback to process a remove event to the Service
	virtual void ProcessRemove(ExecutionOrder<Bond> &data) override
	{
	}

	// Listener callback to process an update event to the Service
	virtual void ProcessUpdate(ExecutionOrder<Bond> &data) override
	{
	}
};







#endif // !BOND_TRADE_BOOKING_SERVICE
