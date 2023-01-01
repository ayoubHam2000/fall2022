/*******************************************/
/*     created by aben-ham 1/1/23         */
/*******************************************/

#include "utils.h"

time_t get_time()
{
	struct timeval time_now;

	gettimeofday(&time_now, nullptr);
	time_t msecs_time = (time_now.tv_sec * 1000) + (time_now.tv_usec / 1000);
	return (msecs_time);
}

double distance(const double a, const double b){
	return std::sqrt((a)*(a) + (b)*(b));
}

double distance(const APos& a, const APos& b){
	return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
