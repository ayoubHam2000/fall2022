/*******************************************/
/*     created by aben-ham 1/1/23         */
/*******************************************/

#ifndef MYCODE_UTILS_H
#define MYCODE_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <sys/time.h>
#include <ctime>
#include <queue>
#include <stack>
#include <cmath>

#define LEFT (1)
#define RIGHT (-1)
#define PRINT_ERR(x) cerr << x << endl

using namespace std;

template <class Tp>
ostream& operator<<(ostream& os, const vector<Tp>& v){
	os << '[';
	for (auto item = v.begin(); item != v.end(); ++item){
		os << *item;
		if (item != v.end() - 1)
			os << ", ";
	}
	os << ']';
	return (os);
}

time_t get_time();

#endif //MYCODE_UTILS_H
