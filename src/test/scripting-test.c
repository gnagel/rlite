// eval 'return {"1","2"}' 0'
// eval 'return 1' 0
#include <string.h>
#include <unistd.h>
#include "hirlite.h"
#include "test/test_hirlite.h"

static int populateArgvlen(char *argv[], size_t argvlen[]) {
	int i;
	for (i = 0; argv[i] != NULL; i++) {
		argvlen[i] = strlen(argv[i]);
	}
	return i;
}

static int test_types()
{
	rliteContext *context = rliteConnect(":memory:", 0);

	rliteReply* reply;
	size_t argvlen[100];

	{
		char* argv[100] = {"eval", "return 1", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_INTEGER(reply, 1);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return '1'", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STR(reply, "1", 1);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return true", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_INTEGER(reply, 1);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return false", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_NIL(reply);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return {err='bad'}", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_ERROR_STR(reply, "bad", 3);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return {ok='good'}", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STATUS(reply, "good", 4);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return {'ok','err'}", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_LEN(reply, 2);
		EXPECT_STR(reply->element[0], "ok", 2);
		EXPECT_STR(reply->element[1], "err", 3);
		rliteFreeReplyObject(reply);
	}

	rliteFree(context);
	return 0;
}

static int test_call_ok()
{
	rliteContext *context = rliteConnect(":memory:", 0);

	rliteReply* reply;
	size_t argvlen[100];

	{
		char* argv[100] = {"set", "key", "value", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STATUS(reply, "OK", 2);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return redis.call('get', 'key')", "0", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STR(reply, "value", 5);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return redis.call('get', KEYS[1])", "1", "key", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STR(reply, "value", 5);
		rliteFreeReplyObject(reply);
	}
	{
		char* argv[100] = {"eval", "return redis.call('ping', ARGV[1])", "0", "value", NULL};
		reply = rliteCommandArgv(context, populateArgvlen(argv, argvlen), argv, argvlen);
		EXPECT_STATUS(reply, "value", 5);
		rliteFreeReplyObject(reply);
	}

	rliteFree(context);
	return 0;
}

int run_scripting_test()
{
	if (test_types()) {
		return 1;
	}
	if (test_call_ok()) {
		return 1;
	}
	return 0;
}
