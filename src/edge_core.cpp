#include "edge_core.h"
#include <map>
#include <cmath>
using namespace std;
using namespace Eigen;
const float PI = 3.14159265359;
namespace marvel{



int edge_core::construct_core(const MatrixXi& tris, const MatrixXf& nods){
  //init
  num_faces_ = tris.cols();
  num_vertices_ = nods.cols();
  edges_.resize(0);
  vertices_.resize(num_vertices_, vector<size_t>(0));
  
  
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
  num_edges_ = edges_.size();

  return 0;
  
}






int edge_core::operator ()(C_MI_ptr& ori_tris, C_MF_ptr& ori_verts, MI_ptr& new_tris, MF_ptr new_verts){
  

  construct_core(*ori_tris, *ori_verts);

  
  //construct new lookup table
  new_tris->resize(3, 4 * num_faces_);
  new_verts->resize(3, num_vertices_+ num_edges_);
  
  
  //calculate odd points
  #pragma omp parallel for 
  for(size_t i = 0; i < num_edges_; ++i){
    if(edges_[i].f2 != -1){
      new_verts->col(num_vertices_ + i) = 0.375 * ( ori_verts->col(edges_[i].v1) + ori_verts->col(edges_[i].v2) )
          + 0.125 * ( ori_verts->col(edges_[i].v3) + ori_verts->col(edges_[i].v4) );
    }

    else
      new_verts->col(num_vertices_ + i) = 0.5 * ( ori_verts->col(edges_[i].v1) + ori_verts->col(edges_[i].v2) );
  }

  //calculate even points
  #pragma omp parallel for
  for(size_t i = 0; i < num_vertices_; ++i){
    size_t valence = vertices_[i].size();
    float beta = 1.0 / valence * (0.625 - pow((0.375 + 0.25 * cos(2 * PI / valence)), 2) );

    new_verts->col(i) = (1 - valence * beta) * ori_verts->col(i);
    for(size_t j = 0; j < valence; ++j){
      new_verts->col(i) += beta * ori_verts->col( vertices_[i][j]  );
    }

  }

  //set new tris odd points:
  #pragma omp parallel for
  for(size_t i = 0; i < num_edges_; ++i){
    //get order
    size_t order1, order2;{
      for(size_t j = 0; j < 3; ++j){

        if(edges_[i].v1 == (*ori_tris)(j, edges_[i].f1)){
          order1 = j;
          break;
        }
      }
      for(size_t j = 0; j < 3; ++j){
        if(edges_[i].v2 == (*ori_tris)(j, edges_[i].f2)){
          order2 = j;
          break;
        }
      }      
    }

    size_t vert_id = i + num_vertices_;
    assert(vert_id >= num_vertices_);
    size_t f1_1st = edges_[i].f1 * 4 ;
    assert(vert_id < num_vertices_ + num_edges_);
    (*new_tris)(1, f1_1st + order1 % 3) = vert_id;
    (*new_tris)(2, f1_1st + (order1 + 1) %3) = vert_id;
    (*new_tris)(order1, f1_1st + 3) = vert_id;


    size_t f2_1st = edges_[i].f2 * 4;
    (*new_tris)(1, f2_1st + order2 % 3) = vert_id;
    (*new_tris)(2, f2_1st + (order2 + 1) %3) = vert_id;
    (*new_tris)(order2, f2_1st + 3) = vert_id;
  }
  //set new tris odd points:

  #pragma omp parallel for
  for(size_t i = 0; i < num_faces_; ++i){

    new_tris->block(0, i*4, 1, 3) = ori_tris->col(i).transpose();
  }
  return 0;
  
}




}//namespace


