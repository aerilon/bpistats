#include <limits.h>

#include <cmath>

#include <statistics.hpp>

namespace bpi::statistics
{

engine::engine(const bpi::records::map_t& map) :
	map(map),
	printer("%Y-%m-%d")
{
}

boost::property_tree::ptree
engine::run()
{
	auto size = this->map.size();

	size_t i = 1;
	size_t median_index = size / 2;
	if (size % 2)
	{
		median_index += 1;
	}
	median_index += 1;

	boost::posix_time::ptime lowest_pt, highest_pt;
	long double lowest, highest, last_x, sum_x, sum_x2, median;

	lowest = LDBL_MAX;
	highest = 0;
	last_x = 0;
	sum_x = 0;
	sum_x2 = 0;

	// Welford's method, requires only 1 pass
	for (const auto& [pt, price] : this->map)
	{
		auto x = static_cast<long double>(price);

		sum_x += x;

		sum_x2 += (x * x);

		// Highest price
		if (x > highest)
		{
			highest = x;
			highest_pt = pt;
		}

		if (x < lowest)
		{
			lowest = x;
			lowest_pt = pt;
		}

		if (i > median_index)
		{
			continue;
		}

		if (i == median_index)
		{
			median = last_x;
			if ((static_cast<int>(size) % 2) == 0)
			{
				median += x;
				median /= 2.0;
			}
		}

		last_x = x;

		i++;
	}

	auto mean = sum_x / size;
	auto variance = (sum_x2 - (sum_x * sum_x) / size) / (size - 1);
	auto stddev = std::sqrt(variance);

	boost::property_tree::ptree statistics;

	statistics.put("lowest.price", lowest);
	statistics.put("lowest.date", this->printer(lowest_pt));
	statistics.put("highest.price", highest);
	statistics.put("highest.date", this->printer(highest_pt));
	statistics.put("stddev", stddev);
	statistics.put("average", mean);
	statistics.put("median", median);
	statistics.put("sample_size", size);

	return statistics;
}

}
