#include <stdio.h>

#define N 4

bool used[N];
int perm[N];

bool orbits[N][N];

char adj[N][N+1] = {
  "0100",
  "1011",
  "0101",
  "0110"
};

void go(int pos) {
  bool ok;
  
  if (pos==N) {

    /*ok = true;
    for (int i=0; i<N; i++)
      for (int j=0; j<N; j++)
	if (adj[perm[i]][perm[j]] != adj[i][j]) {
	  ok = false;
	    break;
	    }*/

    
    for (int i=0; i<N; i++)
      printf("%d", perm[i]);
    putchar('\n');

    for (int i=0; i<N; i++)
      orbits[i][perm[i]] = true;
    
  } else {
    for (int i=0; i<N; i++)
      if (!used[i]) {
	perm[pos] = i;

	ok = true;
	// para ser mais eficiente quebrar logo aqui
	for (int j=0; j<pos; j++)
	  if (adj[perm[pos]][perm[j]] != adj[pos][j]) {
	    ok = false;
	    break;
	  }
	    	
	if (ok) {	  
	  used[i] = true;
	  go(pos+1);
	  used[i] = false;
	}
      }
  }
}


int main() {

  for (int i=0; i<N; i++)
    for (int j=0; j<N; j++)
      orbits[i][j] = false;
  
  for (int i=0; i<N; i++) used[i] = false;
  go(0);

  for (int i=0; i<N; i++) {
    printf("Orbit %d:", i);
    for (int j=0; j<N; j++)
      if (orbits[i][j]) printf(" %d", j);
    printf("\n");
  }

  return 0;
}
