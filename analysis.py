from sklearn.datasets import make_blobs
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
import sys
sys.path.insert(1, 'C:\Projects\Software Project\sp_hw1\Python')
import kmeans as hw1

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
   


    X, y = make_blobs(random_state=42)
    kmeans = KMeans(n_clusters=2, random_state=42)
    silhouette_score(X, kmeans.fit_predict(X))


if __name__ == "__main__":
    main()