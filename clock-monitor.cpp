// Copyright: Xuefer <xuefer@gmail.com>

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

timespec clock(clockid_t clk_id)
{
	timespec tv;
	clock_gettime(clk_id, &tv);
	return tv;
}

timespec timespecSub(const timespec &time1, const timespec &time2)
{
	timespec result;

	/* Subtract the second time from the first. */
	/* TIME1 < TIME2? */
	if (time1.tv_sec - time2.tv_sec < 0 ||
		(time1.tv_sec == time2.tv_sec && time1.tv_nsec - time2.tv_nsec < 0)) {
		result = timespecSub(time2, time1);
		result.tv_sec = -result.tv_sec;
		result.tv_nsec = -result.tv_nsec;
	} else {
		result.tv_sec = time1.tv_sec - time2.tv_sec;
		if (time1.tv_nsec - time2.tv_nsec < 0) {
			result.tv_nsec = time1.tv_nsec + 1000000000L - time2.tv_nsec;
			result.tv_sec--;
		} else {
			result.tv_nsec = time1.tv_nsec - time2.tv_nsec;
		}
	}

	return result;
}

timespec monotonicDiff() {
	return timespecSub(clock(CLOCK_REALTIME), clock(CLOCK_BOOTTIME));
}

std::ostream &operator <<(std::ostream &os, const timespec &tv) {
	std::ostringstream str;
	if (tv.tv_nsec < 0 && tv.tv_sec == 0) {
		str << '-';
	} else {
		str << '+';
	}
	str << tv.tv_sec << '.' << abs(tv.tv_nsec);

	os << str.str();
	return os;
}

std::string readLine(const char *path) {
	std::string s;
	std::ifstream(path) >> s;
	return s;
}

int main() {
	timespec started = clock(CLOCK_REALTIME);
	timespec oldDiff = monotonicDiff();

	using std::setw;
	for (;;) {
		timespec now = clock(CLOCK_REALTIME);
		timespec newDiff = monotonicDiff();

		std::cout
			<< std::right
			<< setw(13) << timespecSub(now, started)
			<< std::left
			<< ": newDiff:"
			<< setw(22) << newDiff
			<< " - oldDiff:"
			<< setw(22) << oldDiff
			<< " = "
			<< setw(12) << timespecSub(newDiff, oldDiff)
			<< " freq="
			<< setw(12) << readLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
			<< "|"
			<< readLine("/sys/kernel/cluster/active")
			<< std::endl;

		sleep(10);
	}
	return 0;
}
