// SwapProfiling2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <functional>

struct FunctionTimer {
	std::map<std::string, float> functionTimes;
	std::map<std::string, std::function<void(void)>> functions;
	int timesToRun;

	FunctionTimer(int timesToRun) : timesToRun(timesToRun) {}

	void addFunction(std::function<void(void)> f, std::string name) {
		functions.insert(std::make_pair(name, f));
	}

	void profileAllFunctions() {
		for (auto funcAndName : functions) {
			clock_t start, end;
			start = clock();
			for (int i = 0; i < timesToRun; i++) {
				funcAndName.second();
			}
			end = clock();
			float time_in_seconds = (end - start) / static_cast<float>(CLOCKS_PER_SEC);
			functionTimes[funcAndName.first] = time_in_seconds;
		}
	}

	
	template <typename Function>
	void profileFunction(Function f, std::string name) {
		clock_t start, end;
		start = clock();
		for (int i = 0; i < timesToRun; i++) {
			f();
		}
		end = clock();
		float time_in_seconds = (end - start) / static_cast<float>(CLOCKS_PER_SEC);
		//std::cout << "This took " << (end - start) / float(CLOCKS_PER_SEC) << " seconds" << std::endl;
		functionTimes[name] = time_in_seconds;
	}
	

	void report() {
		for (const auto& fTime : functionTimes) {
			std::cout << fTime.first << " took " << fTime.second << " seconds" << std::endl;
		}
	}

};

int globalx = 10;
int globaly = 11;

void swap1(int& a, int& b) {
	int temp{ b };
	b = a;
	a = temp;
}


class FunctorSwap1 {
public:
	void operator()() {
		swap1(globalx, globaly);
	}
};



void swap2(int& a, int& b) {
	int temp{ std::move(b) };
	b = std::move(a);
	a = std::move(temp);
}

void swap1NoParams() {
	swap1(globalx, globaly);
}

void swap2NoParams() {
	swap2(globalx, globaly);
}


class FunctorSwap2 {
public:
	void operator()() {
		swap2(globalx, globaly);
	}
};


int main()
{
	int x = 6; int y = 5;
	swap2(x, y);
	std::cout << "x = " << x << " , y = " << y << std::endl;


	FunctorSwap1 fs1;
	FunctorSwap2 fs2;
	FunctionTimer ft(1000 * 1000 * 1000);
	ft.profileFunction(fs1, "swap1-not-stdfunction");
	ft.profileFunction(fs2, "swap2-not-stdfunction");
	ft.profileFunction([&x, &y]()->void {swap1(x, y); }, "swap1-not-stdfunction");
	ft.profileFunction([&x, &y]()->void {swap2(x, y); }, "swap2-not-stdfunction");
	ft.addFunction([&x, &y]()->void {swap1(x, y); }, "swap1");
	ft.addFunction([&x, &y]()->void {swap2(x, y); }, "swap2");

	// std::function igazabol nagyon altalanos dolog!
	// mert fv pointerekre is mukodik!
	typedef void(*fnptr)(void);
	fnptr swap1b = swap1NoParams; // func pointer csak fuggvenyt fogad, lambdat es functort nem!
	fnptr swap2b = swap2NoParams;
	ft.addFunction(swap1b, "swap1b");
	ft.addFunction(swap2b, "swap2b");

	ft.profileAllFunctions();
	ft.report();

	auto lambda1 = []() { std::cout << "I am lambda1" << std::endl; };
	lambda1();



	std::cin.get();
    return 0;
}

