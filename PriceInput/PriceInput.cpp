


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

void PriceGenerator()
{
	std::fstream file("prices.txt", std::ios::out | std::ios::trunc); // open the file

	if (file.is_open())
	{
		std::cout << "Simulation started\n";


		// each product has 1000000 prices
		for (long i = 0; i <  1000000*6; i++)
		{
			// price (ocsillate from 99 to 101 to 99 with 1/256 as increments/decrements for each product)
			int temp = (i / 6) % 1024;
			std::string bondcusip;
			if (i % 6 == 0) bondcusip = string("9128285Q9");
			if (i % 6 == 1) bondcusip = string("9128285R7");
			if (i % 6 == 2) bondcusip = string("9128285P1");
			if (i % 6 == 3) bondcusip = string("9128285N6");
			if (i % 6 == 4) bondcusip = string("9128285M8");
			if (i % 6 == 5) bondcusip = string("912810SE9");

			double midprice = 99.0 + ((temp < 512) ? temp / 256.0 : (1024 - temp) / 256.0);
			double spread = ((i / 6) % 2 == 0) ? 1.0 / 128 : 1.0 / 256;
			std::string bidprice = MakePrice(midprice-spread);
			std::string offerprice = MakePrice(midprice+spread);

			// make the output
			file << bondcusip << "," << bidprice << ","<< offerprice <<"\n";
		}
		std::cout << "Simulation finished!\n";
	}


}

int main()
{
	//PriceGenerator();
	io_service io;
	ip::tcp::endpoint ep(ip::tcp::v4(), 6688);
	ip::tcp::acceptor acceptor(io, ep);

	ifstream PriceData("prices.txt");
	string line;
	while (getline(PriceData, line))
	{


		ip::tcp::socket sock(io);
		acceptor.accept(sock);
		boost::system::error_code ec;
		char str[1024];
		strncpy(str, line.c_str(), line.length() + 1);
		sock.write_some(buffer(str), ec);
		cout << str << endl;
	}
}