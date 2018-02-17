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

## Input

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

 - count
 - maximum (and associated timestamp)
 - minimum (and associated timestamp)
 - average
 - median
 - standard deviation

## Demo

```
% ./bpistats --help
Allowed options:
  --help                this help message
  --minify              minify output
  --range arg           date range to check. Multiple ranges can be given.
                        format expected: YYYY-MM-DD,YYYY-MM-DD
  --file arg            JSON file to parse, must match coindesk historical
                        close API. Multiple file can be given.

% curl 'https://api.coindesk.com//v1/bpi/historical/close.json?start=2018-01-12&end=2018-02-21' > sample.0

# Single file passed:
% ./bpistats --file sample.0
{
    "from": "2018-01-12",
    "to": "2018-02-09",
    "type": "file",
    "filename": "sample.0",
    "statistics": {
        "lowest": {
            "price": "6914.26000000000021828",
            "date": "2018-02-05"
        },
        "highest": {
            "price": "14188.7849999999998545",
            "date": "2018-01-13"
        },
        "stddev": "1948.40109589134032586",
        "average": "10650.3055034482756973",
        "median": "11090.063799999999901",
        "sample_size": "29"
    }
}

# Multiple files can be passed on the command line
# [output shortened for clarity purpose]
% curl 'https://api.coindesk.com//v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-21' > sample.1
% ./bpistats --file sample.0 sample.1
{
    "from": "2018-01-12",
    "to": "2018-02-09",
    "type": "file",
    "filename": "sample.0",
    "statistics": {
[...]
    }
}

{
    "from": "2018-01-01",
    "to": "2018-01-21",
    "type": "file",
    "filename": "sample.1",
    "statistics": {
[...]
    }
}

# On top of that, one or multiple range to be fetched from CoinDesk API can be appended:
# [output shortened for clarity purpose]
% ./bpistats --file sample.0 sample.1 --range 2011-01-12,2012-02-21
[...]

{
    "from": "2018-01-01",
    "to": "2018-01-21",
    "type": "file",
    "filename": "sample.1",
    "statistics": {
[...]
    }
}

{
    "from": "2011-01-12",
    "to": "2012-02-21",
    "type": "online",
    "host": "api.coindesk.com",
    "target": "\/v1\/bpi\/historical\/close.json?start=2011-01-12&end=2012-02-21",
    "statistics": {
        "lowest": {
            "price": "0.313000000000000000444",
            "date": "2011-01-18"
        },
        "highest": {
            "price": "29.6000000000000014211",
            "date": "2011-06-08"
        },
        "stddev": "5.26287442331568498018",
        "average": "5.80055147783251234209",
        "median": "10.6550000000000002487",
        "sample_size": "406"
    }
}
```

## Tested environment

 - OS X (with latest Xcode)
 - Fedora 27
 - Ubuntu 17.10

If your build environment is not listed in the above or experience any
difficulty, a complete docker image is available in the `build_env` root directory.

## Docker build

```
% cd build_env/
% sudo docker build --no-cache --tag bpistats_environment
% sudo docker run -it --privileged bpistats_environment /bin/bash
root@93b85156ae62% git clone https://github.com/aerilon/bpistats.git
root@93b85156ae62% cd bpistats
root@93b85156ae62% mkdir obj
root@93b85156ae62% cd obj
root@93b85156ae62% cmake ..
root@93b85156ae62% make
```

## Unit tests

Unit tests can be run by executing the `bpitests` binary once the application is
build;

## Caveats

 - None of the asynchronous network I/O are being protected by timers.

## Future improvements

 - fix known caveats
 - implement a client/server interface
 - enable multithreaded operations
 - check support for visual studio
 - configurable coindesk endpoint
 - truncate price results to a meaningful decimal
