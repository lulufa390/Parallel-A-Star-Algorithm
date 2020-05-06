import csv  

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


def write_to_csv(file, metrics):

    fields = ['test_name', 'thread_count', 'total_time', 'shortest', 'total_explore', 'avg_explore', 'explore_rate', 'avg_explore_rate']  

    rows = [list(metric) for metric in metrics]

    with open(file, 'w') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(fields)
        csvwriter.writerows(rows) 

if __name__ == "__main__":
    exp_list = [
        "100_100",
        "1000_1000",
        "2000_2000",
        "4000_4000",
        "100_100_no_goal",
        "1000_1000_no_goal",
        "2000_2000_no_goal",
        "4000_4000_no_goal",
    ]

    for exp in exp_list:
        metric = read_file(exp + ".txt")
        write_to_csv(exp + ".csv", metric)