#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <queue>
#include <algorithm>
#define MCADAMS 17731931
#define INF 9999999

// Josh Moore - jsm4 | Lab12 - Single-Source Paths: USA Diameter | CPSC2120 - Prof. Brian Dean 
//--------------------------------------------------------------------------------------------

using namespace std;
typedef pair<double,int> Pair;

struct Edge {
  int src, dst;
  float travel_time;
};
  // The node right in front of McAdams Hall is number 17731931.
int N, M, *first_edge;;
Edge *E;
double *Dists;
// compare two edges based on their source nodes 
// (break ties using destination nodes)
bool comp(Edge e1, Edge e2)
{
  if (e1.src == e2.src) return e1.dst < e2.dst;
  return e1.src < e2.src;
}

double dijkstra(){
  priority_queue<Pair> Q;
  double Max = 0.0;

  for(int i=0; i<M; i++) Dists[i] = INF;
  Dists[MCADAMS] = 0.0;

  Q.push(Pair(0.0, MCADAMS));
  while(!Q.empty()){
    int P = Q.top().second;
    Q.pop();
    for(int i = first_edge[P]; i<M && E[i].src==P; i++){
      if(Dists[E[i].dst] > Dists[P] + E[i].travel_time){
        Dists[E[i].dst] = Dists[P] + E[i].travel_time;
        Q.push(Pair(-Dists[E[i].dst], E[i].dst));
      } 
    }
  }

  for(int i=0; i<M; i++) 
    if(Dists[i] > Max && Dists[i] != INF)
      Max = Dists[i];
  
  return Max;
}
int main(void)
{
  FILE *fp = popen ("zcat /tmp/usa_network.txt.gz", "r");
  fscanf (fp, "%d %d", &N, &M);
  E = new Edge[M];
  first_edge = new int[N];
  Dists = new double[M];


  for(int i=0; i<N; i++) first_edge[i] = -1;

  printf ("Reading input");
  for (int i=0; i<M; i++) {
    fscanf (fp, "%d %d %f", &E[i].src, &E[i].dst, &E[i].travel_time);
    if (i % 1000000 == 0) { printf ("."); fflush(stdout); }
  }
  fclose(fp);
  printf (" Done.\n" ); fflush(stdout); 

  sort(E, E+M, comp);

  for(int i=0; i<M; i++)
    if(first_edge[E[i].src] == -1)
      first_edge[E[i].src] = i;

  double pass1 = dijkstra();
  for(int i=0; i<M; i++) swap(E[i].src, E[i].dst);
  sort(E, E+M, comp);
  double pass2 = dijkstra();

  cout << pass1 << endl;
  cout << "\n\n\nSum: " << pass1 + pass2 << endl;
  cout << "Average: " << (pass1 + pass2) / 2.0 << endl;

  return 0;
}
