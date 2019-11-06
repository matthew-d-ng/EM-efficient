#include<vector>
#include<string>
#include<iostream>
#include<iomanip>
#include<cmath>

using namespace std;

// want to represents vocab items by integers because then various tables 
// need by the IBM model and EM training can just be represented as 2-dim 
// tables indexed by integers

// the following #defines, defs of VS, VO, S, O, and create_vocab_and_data()
// are set up to deal with the specific case of the two pair corpus
// (la maison/the house)
// (la fleur/the flower)

// S VOCAB
#define LA 0
#define MAISON 1
#define FLEUR 2
// O VOCAB
#define THE 0
#define HOUSE 1
#define FLOWER 2

#define VS_SIZE 3
#define VO_SIZE 3
#define D_SIZE 2

vector<string> VS(VS_SIZE); // S vocab: VS[x] gives Src word coded by x 
vector<string> VO(VO_SIZE); // O vocab: VO[x] gives Obs word coded by x

vector<vector<int> > S(D_SIZE); // all S sequences; in this case 2
vector<vector<int> > O(D_SIZE); // all O sequences; in this case 2

vector<vector<double> > tr(VS_SIZE);
vector<vector<double> > count_os(VO_SIZE);

// sets S[0] and S[1] to be the int vecs representing the S sequences
// sets O[0] and O[1] to be the int vecs representing the O sequences
void create_vocab_and_data();
void init_tr();

// functions which use VS and VO to 'decode' the int vecs representing the 
// Src and Obs sequences
void show_pair(int d);
void show_O(int d); 
void show_S(int d);
void show_tr();
void show_counts();

double prob(int j, int i, int pair);
int ITER_COUNT = 50;

main() {

  create_vocab_and_data();
  // guts of it to go here
  // you may well though want to set up further global data structures
  // and functions which access them 

  // init tr(o|s) uniformly
  init_tr();
  // VS.push_back("NULL");
  cout << endl << "Initial tr: " << endl << endl;
  show_tr();

  // repeat [E]; [M] until convergence
  for(int run = 0; run < ITER_COUNT; run ++) {
    cout << "Run: " << run << endl << endl;
    //  [E]

    //  for o in Vo
    //    for s in Vs
    //      #(o, s) = 0
    for (int o = 0; o < VO_SIZE; o++) {
      count_os[o].resize(VS_SIZE);
      for (int s = 0; s < VS_SIZE; s++)
        count_os[o][s] = 0;
    }

    //  for each sentence pair (o, s)
    //    for j in 1:lo
    //      for i in 0:ls
    //        #(o_j, s_i) += p((j, i)|o, s)
    for (int pair = 0; pair < D_SIZE; pair++)
      for (int j = 0; j < 2; j++)
        for (int i = 0; i < 2; i++) {
          int o = O[pair][j];
          int s = S[pair][i];
          count_os[o][s] += prob(j, i, pair);
        }

    //  [M]

    //  for o in Vo
    //    for s in Vs
    //      tr(o|s) = #(o, s) / sum(#(o, s))
    for (int o = 0; o < VO_SIZE; o++)
      for(int s = 0; s < VS_SIZE; s++) {
        double sum = 0;
        for (int o_pr = 0; o_pr < VO_SIZE; o_pr++)
          sum += count_os[o_pr][s];
        tr[o][s] = count_os[o][s] / sum;
      }

    show_counts();
    show_tr();
  }
}

void init_tr() {

  for (int o = 0; o <  VO_SIZE; o++) {
    tr[o].resize(VS_SIZE);
    for (int s = 0; s < VS_SIZE; s++)
      tr[o][s] = 1.0 / VS_SIZE;
  }
}

void show_counts() {

  cout << "UNNORMALISED COUNTS: " << endl;
  for (int o = 0; o < VO_SIZE; o++) {
    for(int s = 0; s < VS_SIZE; s++)
      cout << VO[o] << " \t" << VS[s] << " \t" << count_os[o][s] << endl;
  }
  cout << endl;
}

void show_tr() {

  cout << "PROBABILITY ESTIMATES: " << endl;
  for (int o = 0; o < VO_SIZE; o++) {
    for(int s = 0; s < VS_SIZE; s++)
      cout << VO[o] << " \t" << VS[s] << " \t" << tr[o][s] << endl;
  }
  cout << endl;
}

double prob(int j, int i, int pair) {

  double sum = 0;
  int o = O[pair][j];
  int s = S[pair][i];
  for (int i_pr = 0; i_pr < 2; i_pr++) {
    int s_pr = S[pair][i_pr];
    sum += tr[o][s_pr];
  }
  return tr[o][s] / sum;
}

void create_vocab_and_data() {

  VS[LA] = "la";
  VS[MAISON] = "maison";
  VS[FLEUR] = "fleur";

  VO[THE] = "the";
  VO[HOUSE] = "house";
  VO[FLOWER] = "flower";

  cout << "source vocab\n";
  for(int vi=0; vi < VS.size(); vi++) {
    cout << VS[vi] << " ";
  }
  cout << endl;
  cout << "observed vocab\n";
  for(int vj=0; vj < VO.size(); vj++) {
    cout << VO[vj] << " ";
  }
  cout << endl;

  // make S[0] be {LA,MAISON}
  //      O[0] be {THE,HOUSE}
  S[0].resize(2);   O[0].resize(2);
  S[0] = {LA,MAISON};
  O[0] = {THE,HOUSE};

  // make S[1] be {LA,FLEUR}
  //      O[1] be {THE,FLOWER}
  S[1].resize(2);   O[1].resize(2);
  S[1] = {LA,FLEUR};
  O[1] = {THE,FLOWER};

  for(int d = 0; d < S.size(); d++) {
    show_pair(d);
  }
}

void show_O(int d) {
  for(int i=0; i < O[d].size(); i++) {
    cout << VO[O[d][i]] << " ";
  }
}

void show_S(int d) {
  for(int i=0; i < S[d].size(); i++) {
    cout << VS[S[d][i]] << " ";
  }
}

void show_pair(int d) {
  cout << "S" << d << ": ";
  show_S(d);
  cout << endl;
  cout << "O" << d << ": ";
  show_O(d);
  cout << endl;
}

 
