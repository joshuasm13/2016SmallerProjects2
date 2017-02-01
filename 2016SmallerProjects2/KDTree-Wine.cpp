#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <time.h>

// Josh Moore-jsm4 | HW4 - KD-Tree: K Nearest Neighbor Search/Classif. | CPSC2120-Prof. Brian Dean 
//------------------------------------------------------------------------------------------------
using namespace std;

int N, D, K;
double smallest = 999999999999999.0, err, **store;
struct Node {
  double *key;
  double real_rank;
  double knn_est;
  Node *left, *right;
  Node (double *k, double a) {real_rank = a; key = k; left = right = NULL;}
};
typedef pair<double,double> Pair;
typedef vector<Pair*> Vector;
Vector V;

//-------------------------------------------------------------------|
Node *insert(Node *T, double *val, int d, double qr) {
  if (T == NULL) return new Node(val, qr);
  if (val[d] < T->key[d]) T->left = insert(T->left, val, (d+1)%D, qr);
  else T->right = insert(T->right, val, (d+1)%D, qr);
  return T;
}
 
void standardize(double *stdev, double *mean){  
  for(int i=1; i<=D; i++)
    for(int j=0; j<N; j++)
      store[i][j] = (store[i][j] - mean[i-1]) / stdev[i-1];
}

void sdeviation(double *stdev, double *mean){
  for(int i=1; i<=D; i++){
    double temp = 0.0;
    for(int j=0; j<N; j++)
      temp += pow((store[i][j] - mean[i-1]),2.0);
    stdev[i-1] = sqrt(temp/(double)N);
  }
}
// Randomizes points. Allows for random splitting when later inserted into KD-tree
void randomize(){
  srand(time(NULL));
  for(int i=0; i<N; i++){
    int j = rand() % N;
    for(int k=0; k<=D; k++)
      swap(store[k][j], store[k][i]);
  }
}

void print_T(Node *T){
  if (T == NULL) return;
  print_T(T->left);
  // cout << T->real_rank << "  " << T->knn_est << endl;
  err += pow(T->real_rank - T->knn_est,2.0);
  print_T(T->right);
}
                  
// KNN Search
// ------------------------------------------------------------------|
double e_distance(double *p1, double *p2){                          
  double dist = 0.0;                                             
  for(int i=0; i<D; i++) dist += pow(p1[i] - p2[i], 2.0);  
  return sqrt(dist);
}

void search (Node *T, double *val, int d) {
  if(T == NULL) return;

  double q = e_distance(val, T->key);

  // Store k nearest points so far
  if(q != 0.0 && V.size() < K) {
    V.push_back(new Pair(q,T->real_rank));
    sort(V.begin(), V.end());
    smallest = V.back()->first;
  }
  else if (q != 0.0 &&  q < V.back()->first){
    delete V.back();
    V.pop_back();
    V.push_back(new Pair(q,T->real_rank));
    sort(V.begin(), V.end());
    smallest = V.back()->first;
  }

  // Search all the way down the tree. Prune search while walking back up.
  if(val[d] < T->key[d]){
    search(T->left, val, (d+1)%D);
    if(abs(T->key[d] - val[d]) < smallest)
      search(T->right, val, (d+1)%D);
  }
  else {
    search(T->right, val, (d+1)%D);
    if(abs(T->key[d] - val[d]) < smallest)
      search(T->left, val, (d+1)%D);
  }
}

// Determines weighted average of nearest neighbors, assigns approximated value
void determine(Node *T){
  double e, numerator = 0.0, denominator = 0.0;
  for(int i=0; i<V.size(); i++){
    e = exp(-V[i]->first*.999);
    numerator += V[i]->second * e;
    denominator += e;
  }
  T->knn_est = numerator/denominator;
  for(int i=0; i<V.size(); i++) delete V[i];
  V.erase(V.begin(), V.end());
}

// Loops entire KD-Tree to find K nearest neighbors for each point.
// Find KNN, then determine approximate label value
void classify (Node *T, Node *P) {
  if(T == NULL) return;
  classify(T->left, P);
  search(P, T->key, 0);
  determine(T);
  smallest = 99999999999999.0;
  classify(T->right,P);
}

// ------------------------------------------------------------------|
int main(int argc, char *argv[]){
  clock_t start = clock();
  if(argv[3] != NULL){cout << endl << "---\nIncorrect Command Line Arguments - Run Again"<< endl; return(0);}
  K = atoi(argv[2]);
  if( K<=0 || K > 10){cout << endl << "---\nIncorrect Command Line Arguments - Run Again" << endl; return(0);}
  ifstream in;
  in.open(argv[1]);
  in >> N >> D;

  double mean[D], stdev[D], *point, a, qr;
  int i, j;
  Node *T = NULL;
 
  store = new double *[D+1];
  for(i=0; i<=D; i++) store[i] = new double[N];
  for(i=0; i<D; i++) mean[i] = 0.0;

  for(j=0; in>>a; j++){
    store[0][j] = a;
    for(i=1; i<=D && in>>a; i++){
      store[i][j] = a;
      mean[i-1] += a;
    }
  } 
  in.close();

  for(i=0; i<D; i++) mean[i] /= N;

  randomize();
  sdeviation(stdev,mean);
  standardize(stdev,mean);

  // Insert labels along with its attributes (keys) into Tree
  for(j=0; j<N; j++){
    point = new double[D];
    for(i=1; i<=D; i++) point[i-1] = store[i][j];
    T = insert(T, point, 0, store[0][j]);
  }

  classify(T,T);
  print_T(T);  // Calculates error, epdates err variable
  cout << "---\n\n\n\nAverage Squared Error: " << err/(double) N << endl;
  cout << "Time: " << (clock()-start)*.000001 << "s\n\n";
  return 0;
}


























