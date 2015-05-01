# wooters
Semester long project for Parallel &amp; Distributed Systems : Twitter clone written in html and php, with networked back end in C++, then multi-threaded and later replicated.

**Goals for Future Improvement :**
- Make use of separate compilation for the functions in each module
- Offload the testing code into a separate module of its own
    - Improve rigor of testing code in general
- Improve overall robustness/error handling/worst case scenarios & race conditions
- Allow for a non-fixed # of RMs, to better represent arbitrary IP addresses of RMs
    - Do so by connecting to primary, and telling it and all known RMs to update their lists
    - Now wouldn't have fixed access time for list of RMs, but need to scan over the vector
- Would need to store pairs of connection file descriptors with IP addresses
-Instead of giving the system a command to copy local primary's file, implement a program that would accomplish this over the network. Either for the system() to call, or as part of the current .cpp. Interface would be the same, only instead of cp we'd have this.

