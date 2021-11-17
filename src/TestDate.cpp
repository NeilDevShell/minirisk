#include "Date.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>-

using namespace minirisk;

bool failflag = false;

const std::array<unsigned, 12> days_in_month = { {31,28,31,30,31,30,31,31,30,31,30,31} };

bool is_leap_year(int year) {
	return ((year % 4 != 0) ? false : (year % 100 != 0) ? true : (year % 400 != 0) ? false : true);
}

void gnerate_invalid_date(int type,int &year,int &month,int &day) {

	switch (type % 6)
	{
		//year before 1900
	case 0:
		year = -rand() % 2000 + 1899;
		month = rand() % 12 + 1;
		day = rand() % (days_in_month[month - 1]) + 1;
		break;
		//year after 2199
	case 1:
		year = rand() % 2000 + 2200;
		month = rand() % 12 + 1;
		day = rand() % (days_in_month[month - 1]) + 1;
		break;
		//month less than 1
	case 2:
		year = rand() % 300 + 1900;
		month = -rand() % 12;
		day = 1;
		break;
		//month larger than 12
	case 3:
		year = rand() % 300 + 1900;
		month = rand() % 12 + 13;
		day = 1;
		break;
		//day less than 0
	case 4:
		year = rand() % 300 + 1900;
		month = rand() % 12 + 1;
		day = -rand() % 31;
		break;
		//day larger than days in month
	case 5:
		year = rand() % 300 + 1900;
		month = rand() % 12 + 1;
		day = days_in_month[month - 1] + 1 + ((is_leap_year(year) && month == 2) ? 1 : 0);
		break;
	}

}

bool differ_by_1(int year,int month,int day,int year2,int month2,int day2) {
	Date testdate(year, month, day);
	Date testdate2(year2, month2, day2);
	return ((testdate2 - testdate) == 1) ? true : false;
}



void test1()
{
	int year = 0;
	int month = 0;
	int day = 0;
	for (int i = 0; i < 1000; i++) {

		//generate invalid date
		gnerate_invalid_date(i, year, month, day);
		
		try
		{
			Date testdate(year, month, day);
			failflag = true;
			std::cout << "Invalid Date Test fail in case"<<i << std::endl;
			std::cout << testdate.to_string()<<std::endl;
		}
		catch (...)
		{
		}
	}


    
}

void test2()
{
	for (int year = 1900; year <= 2199; year++) {
		for (int month = 1; month <= 12; month++) {
			int d_max = days_in_month[month - 1] + ((is_leap_year(year) && month == 2) ? 1 : 0);
			for (int day = 1; day <= d_max; day++) {
				Date testdate(year, month, day); 
				std::string teststr = std::to_string((int)day) + "-" + std::to_string((int)month) + "-" + std::to_string((int)year);
				int testdate_serial = testdate.m_serial();
				Date testdate2(testdate_serial);
				std::string teststr2 = testdate2.to_string();
				if ((teststr==teststr2)==false) {
					std::cout << "Convert test failed" << std::endl << "the date is " << teststr << std::endl;
					failflag = true;
				}
				
			}
		}
	}

}

void test3()
{
	//test 1900-2198
	for (int year = 1900; year <= 2198; year++) {
		//test 12-31
		if (differ_by_1(year, 12, 31, year + 1, 1, 1) == false) {
			std::cout << "countiguous test fail" << std::endl << "the date is " << year << "-12-31" << std::endl;
			failflag = true;
		}
		//test the rest month
		for (int month = 1; month <= 11; month++) {
			int d_max = days_in_month[month - 1] + ((is_leap_year(year) && month == 2) ? 1 : 0);
			if (differ_by_1(year, month,d_max, year, month+1, 1) == false) {
				std::cout << "countiguous test fail" << std::endl << "the date is " << year << "-"<<month<<"-"<<d_max << std::endl;
				failflag = true;
			}
		}
	}
	//test 2199
	for (int month = 1; month <= 11; month++) {
		int d_max = days_in_month[month - 1];
		if (differ_by_1(2199, month, d_max, 2199, month + 1, 1) == false){
			std::cout << "countiguous test fail" << std::endl << "the date is " << "2199" << "-" << month << "-" << d_max << std::endl;
			failflag = true;
		}
	}
	
}

int main()
{
    test1();
    test2();
    test3();
	if (failflag == false){
		std::cout << "success" << std::endl;
	}
    return 0;
}

