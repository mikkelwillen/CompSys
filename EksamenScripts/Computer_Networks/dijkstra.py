import math

INFINITY = math.inf


def get_char(x:int):
    """
    return chars 'A', 'B' or 'C' for integer 0, 1 or 2 ... etc. 
    """
    c = chr(x+65)
    return c


def tuple_to_str(tup):
    if tup[0] == math.inf:
        return "∞"
    a = str(tup[0])
    b = get_char(tup[1])
    return a+","+b


def load_cost_table():

    while True:
        try:
            number_of_nodes = int(input("Input number of nodes: "))
        except:
            print("Invalid integer. Please try again.")
            continue
        if number_of_nodes < 2:
            print("Integer value must be at least 2. Please try again.")
        else:
            break

    # number_of_nodes = int(input("Input number of nodes: "))
    cost_table = [[0] * number_of_nodes for i in range(number_of_nodes)]

    print("Input costs/distances. (Leave blank if not connected)")
    # input edge values
    for i in range(0, number_of_nodes):
        for j in range(0, number_of_nodes):
            if j > i:
                while True:
                    x = input(f"{get_char(i)} to {get_char(j)} = ")
                    if x == "":
                        x = INFINITY
                    else:
                        try:
                            x = int(x)
                        except:
                            print("Invalid input! Please try again.")
                            continue
                    if x == 0:
                        continue
                    cost_table[i][j] = x
                    cost_table[j][i] = cost_table[i][j]
                    break
    return cost_table


def print_result(subset, costs, n, node):
    n_pad = len(subset)
    pad = " "*(n_pad-1)
    header = f"| Step | N'{pad}|"
    table = [header]
    for i in range(0, n):
        if i == node:
            continue
        c = get_char(i)
        table.append(f" D({c}),p({c}) |")
    print()
    row = "".join(table)
    print("-"*len(row))
    print(row)
    print("-"*len(row))

    for i in range(0,n):
        N = "".join([get_char(s) for s in subset[0:i+1]]) + " "*(n_pad-i)
        cost = [f"| {i:<4} | {N}|"]
        for j in range(0, n):
            if j == node:
                continue
            cost.append(f"    {tuple_to_str(costs[i][j]):<6} |")
        print("".join(cost))

    print("-"*len(row))
    print()



def char_to_int(n):
    while True:
        available_nodes = [get_char(x) for x in range(0, n)]
        c = input("On which node do you wish to compute the forwarding table?\n(" + ", ".join(available_nodes) + "): " )
        try:
            char_value = ord(c)
        except:
            print("Invalid character. Please try again.")
            continue
        if char_value < 65 or char_value > 64+n:
             print("Character not represented in list. Please try again.")
        else:
            return char_value - 65



def do_work():
    edges = load_cost_table()
    number_of_nodes = len(edges[0])

    while True:
        source_node = char_to_int(number_of_nodes)

        least_cost_paths = [[(INFINITY,0)] * number_of_nodes for i in range(number_of_nodes)]
        
        fst = 0
        snd = 1
        min_distance = INFINITY, 0
        N = []     # N': subset of nodes; v is in N′ if the least-cost path from the source to v is definitively known.

        # steps
        prev_node = source_node
        prev_step = 0
        prev_cost = 0
        for step in range(0, number_of_nodes):
            min_distance = INFINITY, 0
            N.append(prev_node)
            for j in range(0, number_of_nodes):
                # A to node
                cost = edges[prev_node][j]
                if not cost == INFINITY and cost + prev_cost < least_cost_paths[prev_step][j][fst]:
                    lowest_cost = cost + prev_cost, prev_node
                else:
                    lowest_cost = least_cost_paths[prev_step][j]
                least_cost_paths[step][j] = lowest_cost 
                # find best way on this step
                if lowest_cost[fst] < min_distance[fst] and j not in N:
                    min_distance = lowest_cost[fst], j
            prev_node = min_distance[snd]
            prev_cost = min_distance[fst]
            prev_step = step

        print_result(N, least_cost_paths, number_of_nodes, source_node)
        
        yes = input("Compute on new source node? (y/n): ")
        if not yes == 'y':
            break

    print("Goodbye!\n")



if __name__ == "__main__":

    do_work()
    