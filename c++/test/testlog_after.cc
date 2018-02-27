#include <btree.h>
#include <iostream>
#include <memory>
// #include "src/btree.cc"
#include <cstdlib>   // rand()
#include <vector>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

double cclock(){

  struct timeval tmp;
  double sec;
  gettimeofday( &tmp, (struct timezone *)0 );
  sec = tmp.tv_sec + ((double)tmp.tv_usec)/1000000.0;
  return sec;
}


void random_fill(BTree<int,int,std::less<int>>& t, unsigned int size){
  int r;
  unsigned int j;
  std::pair<int,int> p;
  std::vector<std::pair<int,int>> v;
	
  t.clear();

  r = size;
  for( j=0; j<size; j++){
    //std::cout<<r<<std::endl;
    p = {r, r};
    v.push_back(p);
    r--;
  }

  /*while(v.size()<size){
    r = rand() % (size*100);
    for( j=0; j<v.size(); j++){
      if( r == v[j].first )
	break;
    }
    if( j == v.size() ){
      p = {r, r};
      v.push_back(p);
    }
    }*/


  //std::cout << "v has size " << v.size() << std::endl;
  for(j=0; j<v.size(); j++)
    t.insert( v[j] );

}

int main(int argc, char * argv []){

  double t_start, t_end, max;
  double  max_sum_after=0;
  unsigned int size;
  int omp_size;
  unsigned int k;
  
   if( argc < 2 ){
    std::cerr<< "Error. I need the size of the tree.\nProgram exit."<<std::endl;
    exit(EXIT_FAILURE);
    }
  
  size = atoi(argv[1]);
  //std::cout<<"tree size "<<size<<std::endl;
  
  if( size < 1 ){
    std::cerr<<"Error. Inconsistent tree size.\nProgram exit.\n"<<std::endl;
    exit(EXIT_FAILURE);
    }
  
  BTree<int,int,std::less<int>> t;
  
  random_fill(t, size);
  unsigned int r, j;
  std::pair<unsigned int,unsigned int> p;
  std::vector<std::pair< unsigned int, unsigned int>> v_find;
  
  while(v_find.size()<size){
    r = rand() % (size);
    for( j=0; j<v_find.size(); j++){
      if( r == v_find[j].first )
	break;
    }
    if( j == v_find.size() ){
      p = {r, r};
      v_find.push_back(p);
    }
  }
  
  t.balance();
  
  max=0;
#pragma omp parallel private(k, t_start, t_end, max) reduction(+:max_sum_after)
  {
    //std::cout<<"inside before "<<std::endl;
    omp_size = omp_get_num_threads();
    //std::cout<<"omp_size "<<omp_size<<std::endl;
    for (k=0; k<v_find.size(); k++){
      t_start=cclock();
      t.find(v_find[k].first);
      t_end = cclock();
      if ((t_end - t_start)>max){
	max = t_end - t_start;
	//std::cout<<"partial max before "<<max<<std::endl;
      }
      //std::cout<<"time after balancing: "<<t_end - t_start<<std::endl;
    }
    max_sum_after=max/omp_size;

  }
  std::cout<<"max_sum_after "<<max_sum_after<<std::endl;
  return 0;
}