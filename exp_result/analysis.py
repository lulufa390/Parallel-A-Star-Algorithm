def get_element(line, prefix, suffix=""):
    if suffix == "":
        return line[len(prefix):]
    else:
        return line[len(prefix):-len(suffix)]

def read_case(lines):
    assert len(lines) == 7, len(lines)
    test_name = " ".join(lines[0].split()[:-1])
    thread_count = lines[0].split()[-1]
    total_time = int(get_element(lines[1], "Total Time : ", "ms"))
    shortest = int(get_element(lines[2], "shortest path length : "))
    total_explore = int(get_element(lines[3], "Explore ", " nodes in total"))
    avg_explore = float(get_element(lines[4], "Explore ", " nodes in avg"))
    explore_rate = float(get_element(lines[5], "Overal explore rate: "))
    avg_explore_rate = float(get_element(lines[6], "Avg explore rate: "))
    return test_name, thread_count, total_time, shortest, total_explore, avg_explore, explore_rate, avg_explore_rate

def read_file(file_name):
    lines = open(file_name).read().strip().split("\n")[4:]
    assert len(lines) % 7 == 0
    cases = [lines[i*7:i*7+7] for i in range(len(lines)//7)]
    return [read_case(case) for case in cases]

metric = read_file("exp_result/100_100.txt")
test_name, thread_count, total_time, shortest, total_explore, avg_explore, explore_rate, avg_explore_rate = metric[0]