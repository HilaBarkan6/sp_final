from sklearn.metrics import silhouette_score
import sys
import kmeans as hw1
import symnmf 


def main():
    if len(sys.argv) != 3:
        print("An Error Has Occurred") 
        exit()
    k=0
    try:
        k = int(sys.argv[1])
    except:
        print("An Error Has Occurred\n")
        exit()
    file_path = sys.argv[2]

    vectors = hw1.create_vectors(file_path)
    final_centroids = hw1.calulate_k_means_final_project(vectors, k, 200, 0.01)
   
    print(f"kmeans: {silhouette_score(vectors, list(final_centroids.values()))}")

    y = symnmf.run_for_analysis(k, file_path)
    print(f"symnmf: {silhouette_score(vectors, y)}")
    pass


if __name__ == "__main__":
    main()