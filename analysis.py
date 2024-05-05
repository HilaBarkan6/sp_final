from sklearn.datasets import make_blobs
from sklearn.cluster import KMeans
from sklearn.metrics import silhouette_score
import sys
sys.path.insert(1, 'C:\Projects\Software Project\sp_hw1\Python')
import kmeans as hw1

def main():
    X, y = make_blobs(random_state=42)
    kmeans = KMeans(n_clusters=2, random_state=42)
    silhouette_score(X, kmeans.fit_predict(X))


if __name__ == "__main__":
    main()