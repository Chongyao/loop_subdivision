#include "edge_core.h"
#include <map>
using namespace std;
using namespace Eigen;

namespace marvel{



edge_core::edge_core(const MatrixXi& tris, const MatrixXf& nods){

  edges_.resize(0);
  vertices_.resize(nods.cols(), vector<size_t>(0));
  
  
  map<pair<size_t, size_t>, size_t> edge_pair2id;
  size_t edge_id = 0;
  for(size_t i = 0; i < tris.cols(); ++i){
    for(size_t j = 0; j < 3; ++j){
      // one <another
      size_t one, another;{
        if (tris(j%3, i) < tris((j + 1)%3, i)){
          one = tris(j%3, i);
          another = tris((j + 1)%3, i);
        }
        else{
          one = tris((j + 1)%3, i);
          another = tris(j%3, i);
        }
          
      }

      auto res_ins = edge_pair2id.insert({{one, another}, edge_id});
      if( res_ins.second ){
        assert(edges_.size() == edge_id);
        
        edges_.push_back({static_cast<int>(i), -1, tris(j%3, i), tris((j + 1)%3, i), tris((j + 2)%3, i), -1});
        
        vertices_[one].push_back(another);
        vertices_[another].push_back(one);
        
        ++edge_id;
      }
      else{
        assert(edges_[res_ins.first->second].f2 == -1);
        edges_[res_ins.first->second].f2 = i;
        edges_[res_ins.first->second].v4 = tris((j + 2)%3, i);
      }
    }

  }
  num_edges_ = edge_id;
  
}

}//namespace


