#include <chrono>
#include <mdbx.h++>
#include <thread>

using namespace mdbx;
using namespace std;

int main() {
	auto env = new env_managed("/mnt/nvme/mdbx/1-main-min", env::operate_parameters(5, 0, env::mode::readonly));

	txn_managed txn = env->start_read();
	map_handle map = txn.open_map("email");
	cursor_managed cursor = txn.open_cursor(map);

	// cursor::move_result result = cursor.to_first();
	// unsigned int i = 1;

	// while (result.done) {
	// 	// printf("[%s -> %s]: %d\n", result.key.as_string().c_str(), result.value.as_string().c_str(), i);

	// 	if (i % 100000 == 0) {
	// 		printf("[%s]: %d\n", result.key.as_string().c_str(), i);
	// 	}

	// 	// printf("Found (%d): %s:%s\n", i, result.key.as_string().c_str(), result.value.as_string().c_str());
	// 	i += 1;
	// 	result = cursor.to_next_first_multi(false);
	// }

	// printf("Total: %d", i);

	cursor_managed cursorStart = txn.open_cursor(map);
	cursor_managed cursorEnd = txn.open_cursor(map);

	auto stat = txn.get_map_stat(map);

	cursorStart.to_first();
	cursorEnd.to_last();

	ptrdiff_t result = mdbx::estimate(cursorStart, cursorEnd);
	printf("Total entries: %ld\n"
		   "Estimate: %ld\n",
		stat.ms_entries, result);

	mdbx::cursor::move_result moveResult = cursor.lower_bound("-");
	ptrdiff_t estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	ptrdiff_t estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("0");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("79");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("79166931657@maail.com");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("a");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("h");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("i");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("ilonaperner@alice.it");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.lower_bound("k");
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	moveResult = cursor.to_last();
	estimateResult1 = mdbx::estimate(cursor, cursorEnd);
	estimateResult2 = mdbx::estimate(cursor, cursorStart);
	printf("Key [%s]: %ld, %ld\n", moveResult.key.as_string().c_str(), estimateResult1, estimateResult2);

	// slice search = "keller@";
	// short limit = 1000;

	// auto time1 = chrono::high_resolution_clock::now();

	// cursor::move_result result = cursor.move(cursor::move_operation::key_lowerbound, search, false);
	// unsigned short i = 0;

	// while (i < limit && result.done && result.key.starts_with(search)) {
	// 	// printf("Found (%d): %s:%s\n", i, result.key.as_string().c_str(), result.value.as_string().c_str());
	// 	i += 1;
	// 	result = cursor.to_next();
	// }
	// // this_thread::sleep_for(chrono::seconds(1));

	// auto time2 = chrono::high_resolution_clock::now();

	// auto delta = chrono::duration_cast<chrono::milliseconds>(time2 - time1);
	// printf("Elapsed: %ld\n", delta.count());

	// if (result.done) {
	// 	printf("Found (%s): %s:%s\n", result.done ? "done" : "fail", result.key.as_string().c_str(), result.value.as_string().c_str());
	// }

	// cursor.fullscan

	// cursor.scan_from(
	// 	[](const mdbx::pair pair) -> bool {
	// 		// aaa

	// 		printf("Found: %s:%s\n", pair.key.as_string().c_str(), pair.value.as_string().c_str());

	// 		return pair.key.starts_with("keller18306");
	// 	},
	// 	result);
	// cursor.scan([](mdbx::pair pair) -> bool { return false; });

	// cursor::move_result res = cursor.lower_bound("keller18306");

	// for (int i = 0; i < 20; i++) {
	// 	cursor::move_result res = cursor.to_next();

	// 	printf("Found [%d]: %s:%s\n", i, res.key.as_string().c_str(), res.value.as_string().c_str());
	// }

	// cursor.find
	// cursor.move(mdbx::cursor::move_operation::)

	// auto ver = mdbx::get_version();

	// printf(ver.sourcery);
	// printf("\n");
}