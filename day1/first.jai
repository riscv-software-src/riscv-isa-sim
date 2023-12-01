#import "Basic";
#import "File";
#import "String";

get_input :: () -> string {
	file_path := "input.txt";
	file, read := read_entire_file(file_path);
	if !read {
		print("Cannot read file: %\n", file_path);
		exit(1);
	}

	return file;
}

part1 :: () {
	input := get_input();
	defer free(input);
	lines := split(input, "\n");
	s : u32 = 0;
	
	for l : lines {
		if l.count == 0 break;
		n : u8;

		for 0..l.count-1 {
			if (is_digit(l[it])) {
				n = 10 * (l[it] - #char "0");
				break;
			}
		}

		for < l.count-1..0 {
			if (is_digit(l[it])) {
				n += (l[it] - #char "0");
				break;
			}
		}

		print("l: % Digit: %\n", l, n);
		s += n;

	}
	print("part1: %\n", s);

}

part2 :: () {
	input := get_input();
	defer free(input);
	lines := split(input, "\n");
	s : u32 = 0;
	
	nums := string.["one", "two", "three", "four", "five", "six", "seven", "eight", "nine"];

	for l : lines {
		if l.count == 0 break;
		min_idx : s64 = l.count;
		min_val : u8 = 0;

		max_idx : s64 = 0;
		max_val : u8 = 0;
		for 0..l.count-1 {
			if (is_digit(l[it])) {
				min_idx = it;
				min_val = l[it] - #char "0";
				break;
			}
		}

		for < l.count-1..0 {
			if (is_digit(l[it])) {
				max_idx = it;
				max_val = l[it] - #char "0";
				break;
			}
		}

		for num_s: nums {
			val := cast(u8) it_index + 1; // +1, coz no zero in nums

			idx := find_index_from_left(l, num_s);
			if (idx == -1) continue;

			if (idx < min_idx) {
				min_idx = idx;
				min_val = val;
			}

			idx = find_index_from_right(l, num_s);
			if (idx == -1) continue;
			if (idx > max_idx) {
				max_idx = idx;
				max_val = val;
			}

		}

		n := (min_val * 10 + max_val);
		s += n;

	}
	print("part2: %\n", s);
}

main :: () {
	// 55447
	part1();
	// 54706
	part2();
}
