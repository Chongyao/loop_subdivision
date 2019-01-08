#include "edge_core.h"
using namespace std;
using namespace Eigen;

namespace marvel{



edge_core::edge_core(const MatrixXi& tris, const MatrixXf& nods){
  
  edges_ = set<pair<size_t, size_t>>;
  for(size_t i = 0; i < tris.cols(); ++i){
   
  }
}

}
