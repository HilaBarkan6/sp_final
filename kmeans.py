import math
import sys
import argparse

def main():
    try:
        k= int(sys.argv[1])
    except:
        print("Invalid number of clusters!")
        exit()

    try:
        vectors_count = int(sys.argv[2])
    except:
        print("Invalid number of points!")
        exit()

    try:
        dimension = int(sys.argv[3])
    except:
        print("Invalid dimension of point!")
        exit()

    if len(sys.argv) == 6:
        try:
            iter = int(sys.argv[4])
        except:
            print("Invalid maximum iteration!")
            exit()

        if iter<=1 or iter>=1000:
            print("Invalid maximum iteration!")
            exit()
        input_data = sys.argv[5]

    else:
        iter = 200 
        input_data = sys.argv[4]
    
    if(vectors_count <= 1):
        print("Invalid number of points!")
        exit()

    if(k <= 1 or k>=vectors_count):
        print("Invalid number of clusters!")
        exit()
    
    if(dimension < 1):
        print("Invalid dimension of point!")
        exit()
 
    eps = 0.001
    vectors = create_vectors(input_data)
    
    print_vectors(calulate_k_means(vectors, k, iter, eps))
    
def create_vectors(input_file):
    vectors = []
    with open(input_file, "r") as data:
        for line in data:
            coordinates = line.split(',')
            num_coordinates = [float(coor) for coor in coordinates]
            vectors.append(tuple(num_coordinates))
    return vectors

def init_centroids(vectors, k):
    centroids = {}
    for i in range(k):
        centroids[vectors[i]] = []
    return centroids

def assign_vector_to_cluster(vector, centroids):
    min_distance = math.inf
    closest_centroid = None 
    for c in centroids.keys():
        current_dist = distance(vector, c)
        if current_dist < min_distance:
            min_distance = current_dist
            closest_centroid = c
    return closest_centroid

def distance(vector_a, vector_b):
    sum = 0
    for i in range(len(vector_a)):
        sum += math.pow((vector_a[i]-vector_b[i]), 2)
    return math.sqrt(sum)

def assign_all_vectors(vectors, centroids):
    for current_vector in vectors:
        closest_centroid = assign_vector_to_cluster(current_vector, centroids)
        centroids[closest_centroid].append(current_vector)
    return centroids 

def update_centroids(old_centroids):
    new_centroids = {}
    diffs = []
    for c in old_centroids:
        new_c_value = calculate_avg(old_centroids[c]) 
        new_centroids[new_c_value] = []
        diffs.append(distance(c, new_c_value))
    return new_centroids, diffs

def calculate_avg(vectors):
    avg_vector = []
    for i in range(len(vectors[0])):
        sum_i = 0
        for v in vectors:
            sum_i+=v[i]
        avg_vector.append(sum_i/len(vectors))
    return tuple(avg_vector)


def calulate_k_means(vectors, k, iter, eps):
    centroids = init_centroids(vectors,k)
    i = 0
    diff = math.inf
    while (i < iter and eps < diff):
        current_centroids = assign_all_vectors(vectors, centroids)
        centroids, diffs = update_centroids(current_centroids)
        diff = max(diffs)
        i += 1
    return centroids

def calulate_k_means_final_project(vectors, k, iter, eps):
    centroids_dict = calulate_k_means(vectors, k, iter, eps)
    centroids_index_dict = {}
    for idx, pair in enumerate(centroids_dict):
        centroids_index_dict[pair] = idx

    vectors_to_centroids = {}
    for vector in vectors:
        centroid = assign_vector_to_cluster(vector, centroids_dict)
        vectors_to_centroids[vector] = centroids_index_dict[centroid]
    
    return vectors_to_centroids


def print_vectors(vectors):
    for vector in vectors:
        vector = [f"{coor:.4f}" for coor in vector]
        print(','.join(vector))


if __name__ == "__main__":
    main()
