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

timespec operator -(const timespec &time1, const timespec &time2)
{
	timespec result;

	/* Subtract the second time from the first. */
	/* TIME1 < TIME2? */
	if (time1.tv_sec - time2.tv_sec < 0 ||
		(time1.tv_sec == time2.tv_sec && time1.tv_nsec - time2.tv_nsec < 0)) {
		result = time2 - time1;
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
	timespec oldDiffMonotonic = started - clock(CLOCK_MONOTONIC);
	timespec oldDiffBoottime  = started - clock(CLOCK_BOOTTIME);

	using std::setw;
	for (;;) {
		timespec now = clock(CLOCK_REALTIME);
		timespec newDiffMonotonic = now - clock(CLOCK_MONOTONIC);
		timespec newDiffBoottime  = now - clock(CLOCK_BOOTTIME);

		std::cout
			<< std::right
			<< setw(13) << now - started
			<< std::left
			<< " freq="
			<< setw(12) << readLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
			<< "|"
			<< readLine("/sys/kernel/cluster/active")
			<< std::endl;
		std::cout
			<< std::left
			<< "Mnew:"    << setw(22) << newDiffMonotonic
			<< " - Mold:" << setw(22) << oldDiffMonotonic
			<< " = "      << setw(12) << newDiffMonotonic - oldDiffMonotonic
			<< std::endl;

		std::cout
			<< std::left
			<< "Bnew:"    << setw(22) << newDiffBoottime
			<< " - Bold:" << setw(22) << oldDiffBoottime
			<< " = "      << setw(12) << newDiffBoottime - oldDiffBoottime
			<< std::endl;

		sleep(10);
	}
	return 0;
}
