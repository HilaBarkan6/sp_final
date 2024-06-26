import sys
import mysymnmfsp
import math
import numpy as np

GOAL_SYM = "sym"
GOAL_DDG = "ddg"
GOAL_NORM = "norm"
GOAL_SYMNMF = "symnmf"


def run_for_analysis(k, file_path):
    # read file
    vectors = read_file(file_path)
    vectors_count = len(vectors)
    if k >= vectors_count:
        print("An Error Has Occurred") 
        return
    dimension = len(vectors[0])

    # create association matrix
    normal_similarity_matrix = mysymnmfsp.calc_norm(vectors, vectors_count, dimension)
    initial_H = calculate_initial_H(normal_similarity_matrix, k, vectors_count)
    association_matrix = mysymnmfsp.calc_symnmf(normal_similarity_matrix, initial_H, k, vectors_count)

    # assign vectors to clusters
    vectors_cluster_assignment = []
    for row in association_matrix:
        # find cluster with max score for vector
        max_entry = 0
        max_entry_index = None
        for i in range(len(row)):
            if row[i] > max_entry:
                max_entry_index = i
                max_entry = row[i]
        vectors_cluster_assignment.append(max_entry_index)

    return vectors_cluster_assignment


def run_goal(goal, k, file_path):
    # read file
    vectors = read_file(file_path)
    vectors_count = len(vectors)
    if k >= vectors_count:
        print("An Error Has Occurred") 
        return
    dimension = len(vectors[0])
    result = None

    if goal == GOAL_SYM:
        result = mysymnmfsp.calc_sym(vectors, vectors_count, dimension)
    if goal == GOAL_DDG:
        result = mysymnmfsp.calc_ddg(vectors, vectors_count, dimension)
    if goal == GOAL_NORM:
        result = mysymnmfsp.calc_norm(vectors, vectors_count, dimension)
    if goal == GOAL_SYMNMF:
        normal_similarity_matrix = mysymnmfsp.calc_norm(vectors, vectors_count, dimension)
        initial_H = calculate_initial_H(normal_similarity_matrix, k, vectors_count)
        result = mysymnmfsp.calc_symnmf(normal_similarity_matrix, initial_H, k, vectors_count)

    print_matrix(result)


def print_list_float(lst):
    for i in range(len(lst) - 1):
        print(f"{lst[i]:.4f}", end=",")
    print(f"{lst[len(lst) - 1]:.4f}")


def print_matrix(matrix):
    for i in range(len(matrix)):
        print_list_float(matrix[i])

    
def calculate_initial_H(normal_similarity_matrix, k, vector_count):
    m = 0
    counter = 0
    for i in range(len(normal_similarity_matrix)):
        for j in range(len(normal_similarity_matrix[i])):
            m += normal_similarity_matrix[i][j]
            counter += 1
    m = m / counter
    np.random.seed(0)
    initial_H = np.random.uniform(0, 2 * math.sqrt(m / k), (vector_count, k)).tolist()
    return initial_H


def read_file(file_path):
    vectors = []
    with open(file_path, "r") as data:
        for line in data:
            coordinates = line.split(',')
            num_coordinates = [float(coor) for coor in coordinates]
            vectors.append(num_coordinates)
    return vectors


def main():
    if len(sys.argv) != 4:
        print("An Error Has Occurred") 
        exit()
    k = 0
    try:
        k = int(sys.argv[1])
    except:
        print("An Error Has Occurred\n")
        exit()
    goal = sys.argv[2]
    file_path = sys.argv[3]
    run_goal(goal, k, file_path)


if __name__ == "__main__":
    main()
