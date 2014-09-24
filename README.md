algorithm_sorting
=================

proposed boost/sort library using a hybrid radix sort that is faster than O(n*log(n))

To install, download boost, run bootstrap, and copy this library into <your boost root>/libs/sort.  Then run ./b2 headers from your boost root to copy the headers into the boost tree.  Replace "/" with "\" in all commands if running on windows.
Then run the unit tests from your boost root:
./b2 libs/sort/test

Then go to <your boost root>/libs/sort and run tune.pl:
Then from the same directory verify correctness and speed on small data sets:
perl tune.pl -small [-windows]
(it needs the windows option to build for windows)
This tests sorting on many different distributions and data types, making sure the results are identical to std::sort and showing a speed comparison that is a weighted average across multiple data distributions.
If you're interested in more accurate speed comparisons, run the same command either without the -small option, or with the -large option instead.  This will take substantially longer.

Documentation is available from the index.html in this same directory, including a description of the algorithm, how to use it, and why it's faster.

Finally, if you have an unusual computing system, you may want to use the -tune option to tune.pl, to tune the constants used by the library for your specific system.  Note that it doesn't tune MAX_SPLITS, the most important parameter, because that should only be tuned with the -large option and it can overfit to the specific amount of data passed in.

Feel free to contact spreadsort@gmail.com with any questions about this library.