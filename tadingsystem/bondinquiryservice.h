#ifndef BOND_INQUIRY
#define BOND_INQUIRY

#include "soaservices/inquiryservice.hpp"
#include "soaservices/soa.hpp"
#include "soaservices/products.hpp"
class BondInquiryService : public InquiryService<Bond>
{
private:
	std::vector<ServiceListener<Inquiry<Bond>>*> listeners;
	map<string, Inquiry<Bond>> inquirys; // key on inquiry identifier

public:
	shared_ptr<Connector<Inquiry<Bond>>> connector;

	BondInquiryService() {}; // empty ctor

	// Get data on our service given a key
	virtual Inquiry<Bond> & GetData(string key) {
		return inquirys[key];
	}

	// The callback that a Connector should invoke for any new or updated data
	virtual void OnMessage(Inquiry<Bond> &data)
	{
		data.ChangeState("DONE");
		for (auto lis : listeners)
		{
			lis->ProcessAdd(data);
		}
	}

	// Add a listener to the Service for callbacks on add, remove, and update events
	// for data to the Service.
	virtual void AddListener(ServiceListener<Inquiry<Bond>> *listener) 
	{
		listeners.push_back(listener);
	}

	// Get all listeners on the Service.
	virtual const vector< ServiceListener<Inquiry<Bond>>* >& GetListeners() const
	{
		return listeners;
	}

	// Send a quote back to the client
	virtual void SendQuote(const string &inquiryId, double price) override
	{
		Inquiry<Bond> inquiry = inquirys[inquiryId]; // retrieve the corresponding inquiry
		inquiry.ChangeState("QUOTED");
		Inquiry<Bond> newInquiry(inquiryId, inquiry.GetProduct(), inquiry.GetSide(), inquiry.GetQuantity(),
			100, inquiry.GetState()); // create a new inquiry with the new price
		connector->Publish(newInquiry);
	}

	// Reject an inquiry from the client
	virtual void RejectInquiry(const string &inquiryId) 
	{}

};


// corresponding subscribe and publish connector
class BondInquiryConnector : public Connector<Inquiry<Bond>>
{
private:
	shared_ptr<BondInquiryService> bondInquiryService;

public:
	BondInquiryConnector(shared_ptr<BondInquiryService> _bondInquiryService) {
		bondInquiryService = _bondInquiryService;
	} // ctor

	// Publish data to the Connector
	virtual void Publish(Inquiry <Bond> &data) {}

	void subscribe()
	{
		while (true)
		{
			io_service io;
			ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 688);
			ip::tcp::socket sock(io);
			boost::system::error_code ec;
			sock.connect(ep, ec);
			if (ec)
			{
				cout << "inquiry line finished!" << endl;
				break;
			}

			char str[1024];
			sock.read_some(buffer(str), ec);
			if (ec)
			{
				cout << "inquiry line finished!" << endl;
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
			string inquiryId = row[0];
			string productId = row[1];
			Side side;
			if (row[2] == "BUY") side = BUY;
			else if (row[2] == "SELL") side = SELL;
			long quantity = stol(row[3]);
			double price = MakePrice(row[4]);
			InquiryState state;
			if (row[5] == "RECEIVED") state = RECEIVED;
			if (row[5] == "QUOTED") state = QUOTED;
			if (row[5] == "DONE") state = DONE;
			if (row[5] == "REJECTED") state = REJECTED;
			if (row[5] == "CUSTOMER_REJECTED") state = CUSTOMER_REJECTED;
			Bond bond = GetBond(productId);
			Inquiry<Bond> inquiry(inquiryId, bond, side, quantity, price, state);
			bondInquiryService->OnMessage(inquiry);


		}
	}
};




#endif // !BOND_INQUIRY
