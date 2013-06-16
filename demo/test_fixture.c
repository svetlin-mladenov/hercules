#include <herc/herc.h>
#include <stdio.h>

HBEFORE_SUITE(test_fixture) {
	printf("before suite\n");
	return "before suite fixture";
}

HBEFORE_TEST(test_fixture) {
	char *fixture = _fixture;
	printf("before test, fixture = %s\n", fixture);
	return "before test fixture";
}

HTEST(test_fixture, test1) {
	char *fixture = _fixture;
	printf("test1 fixture = %s\n", fixture);
}

HTEST(test_fixture, test2) {
	char *fixture = _fixture;
	printf("test2 fixture = %s\n", fixture);
}

HTEST(test_fixture, test3) {
	char *fixture = _fixture;
	printf("test3 fixture = %s\n", fixture);
}

HAFTER_TEST(test_fixture) {
	char *fixture = _fixture;
	printf("after test. This is a good place to deallocate the before test fixture. Fixture = %s\n", fixture);
}

HAFTER_SUITE(test_fixture) {
	char *fixture = _fixture;
	printf("after suite. This is a good place to deallocate the before suite fixture. Fixture = %s\n", fixture);
}
