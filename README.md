Hercules - a unit testing framework for C
========

Features
--------
* Auto discovery of tests
* Fixtures
* Test filters
* new assert are easy to create
* a nice BSD license because testing is for everone

Installation
--------
    $ git clone https://github.com/svetlin-mladenov/hercules.git
    $ scons
    $ scons install

How do I use it?
--------
Here is how you write a Hercules test:
```c
#include <herc/herc.h>

HTEST(test_suite_name, test_case_name) {
   ...
   assert_true(<boolean-expression>);
   ...
   assert_eq(<expected-expression>, <actual-expression>);
}
```
Then compile and run:

    $ gcc -o tests my_first_test.c -rdynamic -lherc #note that herc provides its own main()
    $ ./tests
As a result you will see a nice green (or red if any of your tests have failed) bar telling you how many test have been auto discovered, how many passed, how many failed and if some tests were skipped or not.
NOTE!!! -rdynamic is very important and mandatory. if you forget -rdynamic the auto discovery will not find any tests.

Limitations
--------
* works only on systems that use ELF as the format for their executables. (ie. Linux, *BSD but not MacOS, Windows)
* currently the number of assertions is limited [in development]
* currently there is now XML output [in development]

How do I use fixtures?
--------

```c
#include <herc/herc.h>

//the code here will be run before any test that belongs to the test_suite_name suite is run.
HBEFORE_SUITE(test_suite_name) {
   //this is good place to create a fixture
   struct suite_fixture *fixture = malloc(...);
   ...
   return fixture;
}

//the code here will be run before each tests that belongs to the test_suite_name suite
HBEFORE_TEST(test_suite_name) {
   //_fixture gets automatically injected. It has the value returned from HBEFORE_SUITE or NULL
   struct suite_fixture* suite_fixture = _fixture;
   ...
   struct test_fixture *test_fixture = malloc(...);
   return test_fixture;
}

HTEST(test_suite_name, test_case_name) {
   //_fixture gets automatically injected. It has te value return from HBEFORE_TEST or if HBEFORE_TEST is not defined the value returned from HBEFORE_SUITE or NULL
   struct test_fixture *fixture = _fixture;
}

//the opposite of HBEFORE_TEST
HAFTER_TEST(test_suite_name) {
   //_fixture has the value return from HBEFORE_TEST or if it's not defined the value returned from HBEFORE_SUITE or NULL
   struct test_fixture *fixture = _fixture;
   ...
   free(fixture);
}

HAFTER_SUITE(test_suite_name) {
   //_fixture has the value returned from HBEFORE_SUITE or NULL
   struct suite_fixture *fixture = _fixture;
   ...
   free(fixture);
}
```

For more examples see the demo folder.

How can I write my own assert?
--------
```c
#include <herc/herc.h>

void myassert(...) {
    ....
    //test for some condition if it fails call herc_fail(message[, args]).
    //The arguments are the same arguments printf accepts
    //for example:
    herc_fail("myassert failed for %d", arg);
}

HTEST(suite, test) {
    ...
    myassert(...);
    ...
}
```

FAQ
--------
Q: How does Hercules find tests?
A: After star it inspects its own elf headers in order to find symbols that match a particular pattern. Depending on the pattern it matches a symbol is deemed to be a test, a fixture producing function and so one. That's why Hercules depends on libelf and that's why you must compile using -rdynamic.

Q: How are assertion failures handled?
A: Hercules uses non local returns to handle assertion failures.

Q: Can I use Hercules in C++ projects?
A: Yes. The macros and the assertions are the same. Hercules will take care of inserting extern "C" { } where it needs to.



