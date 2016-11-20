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

	auto started = clock(CLOCK_REALTIME);
	auto oldDiffMonotonic = started - clock(CLOCK_MONOTONIC);
	auto oldDiffBoottime  = started - clock(CLOCK_BOOTTIME);

	for (;;) {
		auto now = clock(CLOCK_REALTIME);
		auto newDiffMonotonic = now - clock(CLOCK_MONOTONIC);
		auto newDiffBoottime  = now - clock(CLOCK_BOOTTIME);

		cout
			<< right
			<< setw(13) << now - started
			<< left
			<< " freq="
			<< setw(12) << readLine("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq")
			<< "|"
			<< readLine("/sys/kernel/cluster/active")
			<< endl;
		cout
			<< left
			<< "Mnew:"    << setw(22) << newDiffMonotonic
			<< " - Mold:" << setw(22) << oldDiffMonotonic
			<< " = "      << setw(12) << newDiffMonotonic - oldDiffMonotonic
			<< endl;

		cout
			<< left
			<< "Bnew:"    << setw(22) << newDiffBoottime
			<< " - Bold:" << setw(22) << oldDiffBoottime
			<< " = "      << setw(12) << newDiffBoottime - oldDiffBoottime
			<< endl;

		sleep(10);
	}
	return 0;
}
