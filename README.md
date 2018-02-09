## Synopsis

Demo application computing Bitcoin price statistics. Data can be either
retrieved locally or fetched from Coindesk API.

## Building

### Requirements

 - cmake (>= 3.9)
 - C++17 compiler (g++ >= 7)
 - Boost (>= 1.66.0)

### Steps

```
# configure Boost 1.66.0 path
$ export BOOST_ROOT=<path to boost 1.66.0>
$ mkdir obj
$ cd obj
$ cmake ..
$ make
```

## Expected data format

Input shall conform to Coindesk closing API format.

Example:
```
{
  "bpi": {
    "2018-02-01": 9052.5763,
    "2018-02-02": 8827.63,
    "2018-02-03": 9224.3913,
    "2018-02-04": 8186.6488,
    "2018-02-05": 6914.26,
    "2018-02-06": 7700.3863,
  },
  "disclaimer": "This data was produced from the CoinDesk Bitcoin Price Index. BPI value data returned as USD.",
  "time": {
    "updated": "Feb 9, 2018 04:55:21 UTC",
    "updatedISO": "2018-02-09T04:55:21+00:00"
  }
}
```

## Output

The program returns multiple JSON string, one for each of the given input sets.
Each record contains informations about the source of the data, the timespan
covered. The statistics includes, over the requested timespan, the price's:

 - maximum (and associated timestamp)
 - minimum (and associated timestamp)
 - mean
 - median
 - variance
 - standard deviation

Example:
```
{
    "from": "2015-Jul-17 00:00:00",
    "to": "2018-Feb-08 00:00:00",
    "type": "file",
    "filename": "out.1",
    "statistics": {
        "lowest": {
            "price": "209.132200000000011642",
            "date": "2015-Aug-24 00:00:00"
        },
        "highest": {
            "price": "19343.0400000000008731",
            "date": "2017-Dec-16 00:00:00"
        },
        "stddev": "3657.07128283781484979",
        "average": "2325.58883038379530706",
        "median": "687.308150000000011914",
        "sample_size": "938"
    }

```

## Tested environment

 - OS X (with latest Xcode)
 - Fedora 27
 - Ubuntu 17.10

## Caveats

 - None of the asynchronous network I/O are being protected by timers.

## Future improvements

 - fix known caveats
 - implement a client/server interface
 - enable multithreaded operations
 - check support for visual studio
 - configurable coindesk endpoint
