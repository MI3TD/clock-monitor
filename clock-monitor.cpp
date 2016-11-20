// Copyright: Xuefer <xuefer@gmail.com>

#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

enum {
	NANO_PER_SECOND = 1000000000
};

double clock(clockid_t clk_id)
{
	timespec tv;
	clock_gettime(clk_id, &tv);
	return tv.tv_sec + static_cast<double>(tv.tv_nsec) / NANO_PER_SECOND;
}

std::string readLine(const char *path) {
	std::string s;
	std::ifstream(path) >> s;
	return s;
}

class comma_numpunct : public std::numpunct<char>
{
protected:
	virtual char do_thousands_sep() const
	{
		return ',';
	}

	virtual std::string do_grouping() const
	{
		return "\03";
	}
};

int main() {
	using std::cout;
	using std::setw;
	using std::right;
	using std::left;
	using std::endl;

	cout.imbue(std::locale(std::locale(), new comma_numpunct()));
	cout << std::fixed;

	auto referenceTime = clock(CLOCK_REALTIME);
	auto monotonic     = clock(CLOCK_MONOTONIC);
	auto boottime      = clock(CLOCK_BOOTTIME);

	cout << "E=elpased, M=monotonic, F=freq" << endl;
	for (;;) {
		auto currentTime  = clock(CLOCK_REALTIME);
		auto monotonicNow = clock(CLOCK_MONOTONIC);
		auto boottimeNow  = clock(CLOCK_BOOTTIME);
		auto elapsed = currentTime - referenceTime;

		cout
			<< right
			<< "E:" << setw(13) << elapsed
			<< "|M:" << right << setw(12) << (monotonicNow - monotonic) << ' ' << left << setw(12) << (monotonicNow - monotonic - elapsed)
			<< "|B:"  << right << setw(12) << (boottimeNow - boottime)   << ' ' << left << setw(12) << (boottimeNow - boottime - elapsed)
			<< right
			<< "|F:"
			<< setw(12) << readLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
			<< "|"
			<< readLine("/sys/kernel/cluster/active")
			<< endl;

		sleep(10);
	}
	return 0;
}
