#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/asio.hpp>
using namespace std;
using namespace boost::asio;
string MakePrice(double doublePrice)
{
	int doublePrice100 = floor(doublePrice);
	int doublePrice256 = floor((doublePrice - doublePrice100) * 256.0);
	int doublePrice32 = floor(doublePrice256 / 8.0);
	int doublePrice8 = doublePrice256 % 8;

	string stringPrice100 = to_string(doublePrice100);
	string stringPrice32 = to_string(doublePrice32);
	string stringPrice8 = to_string(doublePrice8);

	if (doublePrice32 < 10) stringPrice32 = "0" + stringPrice32;
	if (doublePrice8 == 4) stringPrice8 = "+";

	string stringPrice = stringPrice100 + "-" + stringPrice32 + stringPrice8;
	return stringPrice;
}


void MarketDataGenerator()
{
	std::fstream file("marketdata.txt", std::ios::out | std::ios::trunc); // open the file

	if (file.is_open())
	{
		std::cout << "Simulation started\n";


		for (int k = 0; k < 6; k++) {
			std::string bondcusip;
			if (k % 6 == 0) bondcusip = string("9128285Q9");
			if (k % 6 == 1) bondcusip = string("9128285R7");
			if (k % 6 == 2) bondcusip = string("9128285P1");
			if (k % 6 == 3) bondcusip = string("9128285N6");
			if (k % 6 == 4) bondcusip = string("9128285M8");
			if (k % 6 == 5) bondcusip = string("912810SE9");
		for (int i = 0; i <1000; i++)
		{
			// trades
			int temp = (i / 6) % 1024;
			double midprice = 99.0 + ((temp < 512) ? temp / 256.0 : (1024 - temp) / 256.0);

			for (int j = 0; j < 5; j++)
			{
				string quantity = to_string((j % 5 + 1) * 10000000);
				string bidside("BID");
				string offerside("OFFER");
				string bid = MakePrice(midprice - (i % 5 + 1) / 256.0-(j)/128.0);
				string offer = MakePrice( midprice + (i % 5 + 1) / 256.0+j/128.0);

				// make the output
				file << bondcusip << "," << bid << "," << quantity << "," << bidside << "\n";
				file << bondcusip << "," << offer << "," << quantity << "," << offerside << "\n";
			}			
		}
		}
		std::cout << "Simulation finished!\n";
	}


}

int main()
{
//	MarketDataGenerator();
	io_service io;
	ip::tcp::endpoint ep(ip::tcp::v4(), 666);
	ip::tcp::acceptor acceptor(io, ep);

	ifstream PriceData("marketdata.txt");
	string line;
	while (getline(PriceData, line))
	{
		ip::tcp::socket sock(io);
		acceptor.accept(sock);
		boost::system::error_code ec;
		//char *a = new char[line.length() + 1]
		char str[1024];
		strncpy(str, line.c_str(), line.length() + 1);
		sock.write_some(buffer(str), ec);
		cout << str << endl;
	}



}