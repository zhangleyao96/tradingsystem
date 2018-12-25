

//In the entire project this system plays a role as a client.All inputs will connect to clients by sockets(TCP).
//The four lines can play simultaneously.

#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp> 
#include "bondpricingservice.h"
#include "GUIservice.h"
#include "bondalgostreamingservice.h"
#include "bondstreamingservice.h"
#include "historicalforstreaming.h"
#include "bondtradebookingservice.h"
#include "bondpositionservice.h"
#include "bondriskservice.h"
#include "historicalposition.h"
#include "historicalrisk.h"
#include "bondmarketdataservice.h"
#include "bondalgoexcutionservice.h"
#include "bondexecutionservice.h"
#include "historicalexecution.h"
#include "bondinquiryservice.h"
#include "historicalinquries.h"
void PricingServiceLine()
{

}

void run2()
{
	boost::this_thread::sleep_for(boost::chrono::seconds(10));
	std::cout << "r2 done!" << std::endl;
}

int main()
{
	//initialize the pricingservice line.
	shared_ptr<BondPricingService> bondpricingservice(new BondPricingService());
	shared_ptr<BondPricingServiceConnector> bondpricingserviceconnector(new BondPricingServiceConnector(bondpricingservice));
	bondpricingservice->connector = bondpricingserviceconnector;

	shared_ptr<GUIService> guiservice(new GUIService());
	PricingGUIServiceListener pricingguiservicelistener(guiservice);
	PricingGUIServiceListener* pricingguiservicelistener_ptr = &pricingguiservicelistener;
	bondpricingservice->AddListener(pricingguiservicelistener_ptr);
	shared_ptr<GUIServiceConnector> guiserviceconnector(new GUIServiceConnector());
	guiservice->connector = guiserviceconnector;

	shared_ptr<BondAlgoStreamingService> bondalgostreamingservice(new BondAlgoStreamingService());
	PricingBondalgoServiceListener pricingbondalgoservicelistener(bondalgostreamingservice);
	PricingBondalgoServiceListener* pricingbondalgoservicelistener_ptr = &pricingbondalgoservicelistener;
	bondpricingservice->AddListener(pricingbondalgoservicelistener_ptr);

	shared_ptr<BondStreamingService> bondstreamingservice(new BondStreamingService());
	BondStreamingServiceListener bondstreamingservicelistener(bondstreamingservice);
	BondStreamingServiceListener* bondstreamingservicelistener_ptr = &bondstreamingservicelistener;
	bondalgostreamingservice->AddListener(bondstreamingservicelistener_ptr);

	shared_ptr<HistoricalStreamingDataService> historicalstreamingdataservice(new HistoricalStreamingDataService());
	HistoricalStreamingDataServiceListener historicalstreamingdataservicelistener(historicalstreamingdataservice);
	HistoricalStreamingDataServiceListener* historicalstreamingdataservicelistener_ptr = &historicalstreamingdataservicelistener;
	bondstreamingservice->AddListener(historicalstreamingdataservicelistener_ptr);
	shared_ptr<HistoricalStreamingDataServiceConnector> historicalstreamingdataserviceconnector(new HistoricalStreamingDataServiceConnector());
	historicalstreamingdataservice->connector = historicalstreamingdataserviceconnector;


	
	//initialize the tradebookingservice line.
	shared_ptr<BondTradeBookingService> bondtradebookingservice(new BondTradeBookingService());
	shared_ptr<BondTradeBookingServiceConnector> bondtradebookingserviceconnector(new BondTradeBookingServiceConnector(bondtradebookingservice));
	bondtradebookingservice->connector = bondtradebookingserviceconnector;

	shared_ptr<BondPositionService> bondpositionservice(new BondPositionService());
	BondPositionServiceListener bondpositionservicelistener(bondpositionservice);
	BondPositionServiceListener* bondpositionservicelistener_ptr = &bondpositionservicelistener;
	bondtradebookingservice->AddListener(bondpositionservicelistener_ptr);

	shared_ptr<BondRiskService> bondriskservice(new BondRiskService());
	BondRiskServiceListener bondriskservicelistener(bondriskservice);
	BondRiskServiceListener* bondriskservicelistener_ptr = &bondriskservicelistener;
	bondpositionservice->AddListener(bondriskservicelistener_ptr);

	shared_ptr<HistoricalPositionService> historicalpositionservice(new HistoricalPositionService());
	HistoricalPositionServiceListener historicalpositionservicelistener(historicalpositionservice);
	HistoricalPositionServiceListener* historicalpositionservicelistener_ptr = &historicalpositionservicelistener;
	bondpositionservice->AddListener(historicalpositionservicelistener_ptr);
	shared_ptr<HistoricalPositionServiceConnector> historicalpositionserviceconnector(new HistoricalPositionServiceConnector());
	historicalpositionservice->connector = historicalpositionserviceconnector;

	shared_ptr<HistoricalRiskService> historicalriskservice(new HistoricalRiskService());
	HistoricalRiskServiceListener historicalriskservicelistener(historicalriskservice);
	HistoricalRiskServiceListener* historicalriskservicelistener_ptr = &historicalriskservicelistener;
	bondriskservice->AddListener(historicalriskservicelistener_ptr);
	shared_ptr<HistoricalRiskServiceConnector> historicalriskserviceconnector(new HistoricalRiskServiceConnector());
	historicalriskservice->connector = historicalriskserviceconnector;




	//initialize the marketdataline
	shared_ptr<BondMarketDataService> bondmarketdataservice(new BondMarketDataService());
	shared_ptr<BondMarketDataConnector> bondmarketdataserviceconnector(new BondMarketDataConnector(bondmarketdataservice));
	bondmarketdataservice->connector = bondmarketdataserviceconnector;

	shared_ptr<BondAlgoExecutionService> bondalgoexecutionservice(new BondAlgoExecutionService());
	BondAlgoExecutionListener bondalgoexecutionservicelistener(bondalgoexecutionservice);
	BondAlgoExecutionListener* bondalgoexecutionservicelistener_ptr = &bondalgoexecutionservicelistener;
	bondmarketdataservice->AddListener(bondalgoexecutionservicelistener_ptr);

	shared_ptr<BondExecutionService> bondexecutionservice(new BondExecutionService());
	BondExecutionListener bondexecutionservicelistener(bondexecutionservice);
	BondExecutionListener* bondexecutionservicelistener_ptr = &bondexecutionservicelistener;
	bondalgoexecutionservice->AddListener(bondexecutionservicelistener_ptr);

	BondExecutionBookingListener bondexecutionbookinglistener(bondtradebookingservice);
	BondExecutionBookingListener* bondexecutionbookinglistener_ptr = &bondexecutionbookinglistener;
	bondexecutionservice->AddListener(bondexecutionbookinglistener_ptr);

	shared_ptr<HistoricalExecutionService> historicalexecutionservice(new HistoricalExecutionService());
	HistoricalExecutionServiceListener historicalexecutionservicelistener(historicalexecutionservice);
	HistoricalExecutionServiceListener* historicalexecutionservicelistener_ptr = &historicalexecutionservicelistener;
	bondexecutionservice->AddListener(historicalexecutionservicelistener_ptr);
	shared_ptr<HistoricalExecutionServiceConnector> historicalexecutionserviceconnector(new HistoricalExecutionServiceConnector());
	historicalexecutionservice->connector = historicalexecutionserviceconnector;





	//initialize the marketdataline
	shared_ptr<BondInquiryService> bondinquiryservice(new BondInquiryService());
	shared_ptr<BondInquiryConnector> bondinquiryserviceconnector(new BondInquiryConnector(bondinquiryservice));
	bondinquiryservice->connector = bondinquiryserviceconnector;

	shared_ptr<HistoricalInquiryService> historicalinquiryservice(new HistoricalInquiryService());
	HistoricalInquiryListener historicalinquiryservicelistener(historicalinquiryservice);
	HistoricalInquiryListener* historicalinquiryservicelistener_ptr = &historicalinquiryservicelistener;
	bondinquiryservice->AddListener(historicalinquiryservicelistener_ptr);
	shared_ptr<HistoricalInquiryServiceConnector> historicalinquiryserviceconnector(new HistoricalInquiryServiceConnector());
	historicalinquiryservice->connector = historicalinquiryserviceconnector;






	std::cout << "system started!" << std::endl;


	auto priceline = [&]() {bondpricingserviceconnector->subscribe(); };
	auto tradesline = [&]() {bondtradebookingserviceconnector->subscribe(); };
	auto marketline = [&]() {bondmarketdataserviceconnector->subscribe(); };
	auto inquiryline = [&]() {bondinquiryserviceconnector->subscribe(); };

	boost::thread thread1(priceline);
	boost::thread thread2(tradesline);
	boost::thread thread3(marketline);
	boost::thread thread4(inquiryline);

	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();

	std::cout << "finished!" << std::endl;


	

	








	return 0;
}


