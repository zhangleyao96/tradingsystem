#pragma once


#include "soaservices/products.hpp"
#include "bondpricingservice.h"
#include "soaservices/soa.hpp"
#include <cmath>
#include <string>
#include <iostream>
#include <chrono>

using namespace std;

// Get Bond object for US Treasury 2Y, 3Y, 5Y, 7Y, 10Y, and 30Y.
Bond GetBond(string Cusip)
{
	if (Cusip == "9128285Q9") return Bond("9128285Q9", CUSIP, "T", 0.02500, from_string("2020/12/31"));
	if (Cusip == "9128285R7") return Bond("9128285R7", CUSIP, "T", 0.02625, from_string("2021/12/17"));
	if (Cusip == "9128285P1") return Bond("9128285P1", CUSIP, "T", 0.02875, from_string("2023/11/30"));
	if (Cusip == "9128285N6") return Bond("9128285N6", CUSIP, "T", 0.02875, from_string("2025/11/30"));
	if (Cusip == "9128285M8") return Bond("9128285M8", CUSIP, "T", 0.03125, from_string("2028/12/17"));
	if (Cusip == "912810SE9") return Bond("912810SE9", CUSIP, "T", 0.03375, from_string("2048/12/16"));
}


//have time stamp
string systemtime() {
	std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
	std::time_t timestamp = tmp.count();
	INT64 milli = timestamp + (INT64)(-5) * 60 * 60 * 1000;
	auto mTime = std::chrono::milliseconds(milli);
	auto tp1 = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(mTime);
	auto tt1 = std::chrono::system_clock::to_time_t(tp1);
	tm* now = std::gmtime(&tt1);
	string res;
	int year = now->tm_year + 1900;
	int mon = now->tm_mon + 1;
	int day = now->tm_mday;
	int hour = now->tm_hour;
	int min = now->tm_min;
	int sec = now->tm_sec;
	int millisec = milli % 1000;
	res = to_string(year) + string("/") + to_string(mon) + string("/") + to_string(day) + string(",") + to_string(hour) + string(":") + to_string(min) + string(":") + to_string(sec) + string(":") + to_string(millisec);

	return res;

}


// convert price from string to double
double MakePrice(string stringPrice)
{
	string stringPrice100 = "";
	string stringPrice32 = "";
	string stringPrice8 = "";

	int count = 0;
	for (int i = 0; i < stringPrice.size(); i++)
	{
		if (stringPrice[i] == '-')
		{
			count++;
			continue;
		}
		if (count == 0)
		{
			stringPrice100.push_back(stringPrice[i]);
		}
		if (count == 1)
		{
			stringPrice32.push_back(stringPrice[i]);
			count++;
		}

		if (count == 2)
		{
			stringPrice32.push_back(stringPrice[i]);
			count++;
		}
	
		if (count == 3)
		{
			if (stringPrice[i] == '+') stringPrice[i] = '4';
			stringPrice8.push_back(stringPrice[i]);
		}
	}

	double doublePrice100 = stod(stringPrice100);
	double doublePrice32 = stod(stringPrice32);
	double doublePrice8 = stod(stringPrice8);
	double doublePrice = doublePrice100 + doublePrice32 * 1.0 / 32.0 + doublePrice8 * 1.0 / 256.0;
	return doublePrice;
}

// Convert double price to string price.
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


//create tradeId and inquiryId.
string CreateId()
{
	string id;
	int random_variable = std::rand();
	long i = abs(random_variable * 17333 % 100000000);
	id = std::to_string(i);
	return id;
}