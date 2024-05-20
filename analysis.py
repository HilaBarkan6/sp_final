from sklearn.metrics import silhouette_score
import sys
import kmeans as hw1
import symnmf 

MAX_ITER = 300
EPSILON = 0.0001


def run_analysis(k, file_path):
    vector_assignment_symnmf = symnmf.run_for_analysis(k, file_path)

    vectors = hw1.create_vectors(file_path)
    final_centroids = hw1.calulate_k_means_final_project(vectors, k, MAX_ITER, EPSILON)
    vector_assignment_kmeans = list(final_centroids.values())

    print(f"nmf: {silhouette_score(vectors, vector_assignment_symnmf):.4f}")
    print(f"kmeans: {silhouette_score(vectors, vector_assignment_kmeans):.4f}")


def main():
    if len(sys.argv) != 3:
        print("An Error Has Occurred") 
        exit()
    k = 0
    try:
        k = int(sys.argv[1])
    except:
        print("An Error Has Occurred\n")
        exit()
    file_path = sys.argv[2]

    run_analysis(k, file_path)


if __name__ == "__main__":
    main()
