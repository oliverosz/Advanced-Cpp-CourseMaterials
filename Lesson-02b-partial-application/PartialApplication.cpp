// PartialApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <functional>

class Person {
	std::string name;
	int age;
public:
	Person(std::string n, int a) : name(n), age(a) {}
	int getAge() { return age; }
};

class Car {
	std::string maker;
	int year;
public:
	Car(std::string m, int y) : maker(m), year(y) {}
	int getAge() { return 2020 - year; }

};

class OlderThanXFunctor {
	int x;
public:
	OlderThanXFunctor(int x) : x(x) { }
	template<typename Object>
	bool operator()(Object* o) { return o->getAge() > x; }
};

// vagy: meg altalanosabban:
// (mivel az OlderThanFunc csak a getAge > x-re mukodott)
template <typename Function, typename SecondArg>
class PartialApply2ndArg {
	Function func;
	SecondArg secondarg;
public:
	PartialApply2ndArg(Function f, SecondArg sa) :
		func(f), secondarg(sa) {}
	
	template <typename FirstArg>
	decltype(auto) operator()(FirstArg&& fa) {
		return func(std::forward<FirstArg>(fa), secondarg);
	}
};

template <typename Object>
bool isAgeableObjectOlderThanY(Object* p, int y) {
	return p->getAge() > y;
}


int main()
{
	std::vector<Person*> people;
	Person adam{ "Adam", 34 };
	Person eve{ "Eva", 16 };
	Person dalma{ "Dalma", 22 };
	people.push_back(&adam);
	people.push_back(&eve);
	people.push_back(&dalma);
	
	auto num_adults = std::count_if(
		people.begin(),
		people.end(),
		//[](Person* p)->bool {return p->getAge() >= 18; }
		//OlderThanXFunctor(18)
		/*
		PartialApply2ndArg<
		  std::function<bool(Person*, int)>, int
		>(isAgeableObjectOlderThanY<Person>, 18)
		*/
		std::bind(isAgeableObjectOlderThanY<Person>, std::placeholders::_1, 18)
	);

	std::vector<Car*> cars;
	Car punto{ "Fiat", 2012 };
	Car mercVintage{ "Mercedes", 1998 };
	Car audiA6{ "Audi", 2018 };
	cars.push_back(&punto);
	cars.push_back(&mercVintage);
	cars.push_back(&audiA6);

	auto num_cars_older_than_10 = std::count_if(
		cars.begin(),
		cars.end(),
		//[](Car* c)->bool {return c->getAge() >= 10; }
		//OlderThanXFunctor(10)
		PartialApply2ndArg<
			std::function<bool(Car*, int)>, int
		>(isAgeableObjectOlderThanY<Car>, 10)
	);
	
	std::cout << "Number of grown-ups: " << num_adults << std::endl;
	std::cout << "Number of vintage cars: " << num_cars_older_than_10 << std::endl;

	auto is_6_gt_24 = std::bind(std::greater<double>(), 6, 24);
	if (!is_6_gt_24()) {
		std::cout << "no, 6 is NOT greater than 24" << std::endl;
	}

	using namespace std::placeholders;
	auto less_than = std::bind(std::greater<double>(), _2, _1);
	if (less_than(6, 24)) {
		std::cout << "yes, 6 is LESS than 24" << std::endl;
	}

	auto equal_to_itself = std::bind(std::equal_to<double>(), _1, _1);
	if (equal_to_itself(7)) {
		std::cout << "yes, 7 is equal to itself, duuuhh" << std::endl;
	}

	auto squares = std::bind(std::multiplies<double>(), _1, _1);
	std::cout << "square of 5 is " << squares(5) << std::endl;
	std::cout << "square of 7 is " << squares(7) << std::endl;

	std::cin.get();
    return 0;
}

