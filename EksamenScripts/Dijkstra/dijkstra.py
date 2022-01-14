import math
INFINITY = math.inf

#########################################################################
#  This program applies the link state algorithm (Dijkstra's) to a given
#  set of nodes and edges and prints the result out in a table.
#
#  It's possible to compute different result tables on new nodes if so
#  desired, and some user friendly features like undo last cost value are
#  also featured.
#########################################################################






#########################################################################
#   Helper functions
#########################################################################
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



#########################################################################
#   Output
#########################################################################
def print_result(subset, costs, n, node):
    """
    Print the forwarding table to std out
    """
    n_pad = len(subset)
    pad = " "*(n_pad-1)

    cost_values = []
    table_header = [f"| Step | N'{pad}|"]

    for i in range(0,n):
        N = "".join([get_char(s) for s in subset[0:i+1]]) + " "*(n_pad-i)
        cost = [f"| {i:<4} | {N}|"]
        for j in range(0, n):
            if j == node:
                continue
            cost.append(f"    {tuple_to_str(costs[i][j]):<6} |")
        cost_values.append("".join(cost))
        if i == node:
            continue
        c = get_char(i)
        table_header.append(f" D({c}),p({c}) |")

    row = "".join(table_header)
    line = "-"*len(row) + "\n"

    print("\n" + line + row + "\n" + line, end="")  # print header
    print("\n".join(cost_values))  # print values
    print(line)



#########################################################################
#   User input
#########################################################################
def get_number_of_nodes():
    """
    Get number of nodes from user input
    """
    while True:
        try:
            number_of_nodes = int(input("Input number of nodes: "))
        except:
            print("Invalid integer. Please try again.")
            continue
        if number_of_nodes < 2:
            print("Integer value must be at least 2. Please try again.")
        else:
            return number_of_nodes


def get_cost_table(number_of_nodes):
    """
    Get cost/distances from user input
    """
    cost_table = [[0] * number_of_nodes for i in range(number_of_nodes)]  # initialize cost table

    # input cost values
    print("Input costs/distances. (Leave blank if not connected - 'z' for undo)")
    i = 0
    j = 0
    while i < number_of_nodes:
        if j >= number_of_nodes:
            j = 0
            i += 1
        update_j = True
        if j > i:
            while True:
                x = input(f"{get_char(i)} to {get_char(j)} = ")
                if x == "":
                    x = INFINITY

                # cancel/repeat previous entering
                elif x == "z":
                    if j == 1 and i == 0:
                        print("No cost to undo!")                        
                    elif j-i > 1:
                        j -= 1
                    else:
                        j = number_of_nodes-1
                        i -= 1
                    update_j = False
                    break

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
        if update_j:
            j += 1
    return cost_table


def get_source_node(n):
    """
    Get the source node, on which to compute the forwarding table from, as integer
    """
    while True:
        available_nodes = [get_char(x) for x in range(0, n)]
        c = input("On which node do you wish to compute the forwarding table?\n(" + ", ".join(available_nodes) + "): " ).upper()
        try:
            char_value = ord(c)
        except:
            print("Invalid character. Please try again.")
            continue
        if c not in available_nodes:
             print("Character not represented in list. Please try again.")
        else:
            return char_value - 65



#########################################################################
#   Algorithms
#########################################################################
def compute_dijkstra(edges, number_of_nodes, source_node):
    """
    Calculate the forwarding table on the source node using the dijkstra link state routing algorithm
    """
    N = []     # N': subset of nodes; v is in N′ if the least-cost path from the source to v is definitively known.
    least_cost_paths = [[(INFINITY,0)] * number_of_nodes for i in range(number_of_nodes)]  # initialize with minimum distance

    # steps
    prev_node = source_node
    prev_step = 0
    prev_cost = 0
    for step in range(0, number_of_nodes):
        min_distance = INFINITY, 0
        N.append(prev_node)
        for j in range(0, number_of_nodes):
            # A (source_node) to node
            cost = edges[prev_node][j]
            if not cost == INFINITY and cost + prev_cost < least_cost_paths[prev_step][j][0]:
                lowest_cost = cost + prev_cost, prev_node
            else:
                lowest_cost = least_cost_paths[prev_step][j]
            least_cost_paths[step][j] = lowest_cost 
            # find best way on this step
            if lowest_cost[0] < min_distance[0] and j not in N:
                min_distance = lowest_cost[0], j
        prev_node = min_distance[1]
        prev_cost = min_distance[0]
        prev_step = step

    return (N, least_cost_paths)



#########################################################################
#   Main
#########################################################################
def do_work():

    number_of_nodes = get_number_of_nodes()
    edges = get_cost_table(number_of_nodes)  # load cost table

    while True:

        source_node = get_source_node(number_of_nodes)
        result = compute_dijkstra(edges, number_of_nodes, source_node)

        # print the forwarding table
        print_result(result[0], result[1], number_of_nodes, source_node)

        yes = input("Compute on new source node? (y/n): ")
        if not yes == 'y':
            break

    print("Goodbye!\n")



if __name__ == "__main__":

    do_work()
    