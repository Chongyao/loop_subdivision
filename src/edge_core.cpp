#include "edge_core.h"
#include <map>
#include <cmath>
#include <iostream>
using namespace std;
using namespace Eigen;
const float PI = 3.14159265359;
namespace marvel{

float edge_core::get_beta(const size_t& valence){
  return  (0.625 - pow((0.375 + 0.25 * cos(2 * PI / valence)), 2) ) / valence;
}

int edge_core::construct_core(const MatrixXi& tris, const MatrixXf& nods){
  //init
  num_faces_ = tris.cols();
  num_vertices_ = nods.cols();
  edges_.clear();
  valences_.clear();
  edges_ .resize(0);
  valences_.resize(num_vertices_);
  
  
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
        
        ++(valences_[one]);
        ++(valences_[another]);
        
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
  if_construct_ = true;
  return 0;
  
}



int edge_core::calculate_odd_points(C_MF_ptr& ori_verts, MF_ptr& new_verts){
#pragma omp parallel for 
  for(size_t i = 0; i < num_edges_; ++i){
    size_t v1 = edges_[i].v1, v2 = edges_[i].v2;
    if(edges_[i].f2 != -1){
      new_verts->col(num_vertices_ + i) = 0.375 * ( ori_verts->col(v1) + ori_verts->col(v2) )
          + 0.125 * ( ori_verts->col(edges_[i].v3) + ori_verts->col(edges_[i].v4) );
    }
    else
      new_verts->col(num_vertices_ + i) = 0.5 * ( ori_verts->col(v1) + ori_verts->col(v2) );
  }

  
  return 0;
}

int edge_core::calculate_even_points(C_MF_ptr& ori_verts, MF_ptr& new_verts){
  
#pragma omp parallel for
  for(size_t i = 0; i < num_vertices_; ++i){
    new_verts->col(i) = (1 - valences_[i] * get_beta(valences_[i])) * ori_verts->col(i);
  }

  for(size_t i = 0; i < num_edges_; ++i){
    
    size_t v1 = edges_[i].v1, v2 = edges_[i].v2;
    //add even weight // TODO::  handle boundary in even points

    new_verts->col(v1) += get_beta(valences_[v1]) * ori_verts->col(v2);
    new_verts->col(v2) += get_beta(valences_[v2]) * ori_verts->col(v1);
    
  }

  
  return 0;
}

int edge_core::set_odds_to_new_tris(C_MI_ptr& ori_tris, MI_ptr& new_tris){
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
  return 0;  
}
int edge_core::set_evens_to_new_tris(C_MI_ptr& ori_tris, MI_ptr& new_tris){
  #pragma omp parallel for
  for(size_t i = 0; i < num_faces_; ++i){
    new_tris->block(0, i*4, 1, 3) = ori_tris->col(i).transpose();
   } 
  return 0;
}

int edge_core::operator ()(C_MI_ptr& ori_tris, C_MF_ptr& ori_verts, MI_ptr& new_tris, MF_ptr& new_verts){
  if(!if_construct_ || !if_update_)  
    construct_core(*ori_tris, *ori_verts);  
  
  //construct new lookup table
  new_tris->resize(3, 4 * num_faces_);
  new_verts->resize(3, num_vertices_+ num_edges_);
  
  calculate_even_points(ori_verts, new_verts);  
  calculate_odd_points(ori_verts, new_verts);

  
  set_odds_to_new_tris(ori_tris, new_tris);
  set_evens_to_new_tris(ori_tris, new_tris);



  if(if_update_)
    update_edges_and_valence(ori_tris, new_tris);
  return 0;
  
}

//>>>>>>>>>>>>>>>>TODO : update edges after each loop<<<<<<<<<<<<<<<<<<<<<<<<<
int edge_core::update_edges_and_valence(C_MI_ptr& ori_tris, C_MI_ptr& new_tris){

  valences_.resize(num_vertices_ + num_edges_);
  fill(valences_.begin() + num_vertices_, valences_.begin() + num_vertices_ + num_edges_, 6);
  edges_.resize(2 * num_edges_ + 3 * num_faces_);
  #pragma omp parallel for 
  for(size_t i = 0; i < num_edges_; ++i){
      size_t order1 = get_order(edges_[i].v1, (*ori_tris).col(edges_[i].f1));
      size_t order2 = get_order(edges_[i].v2, (*ori_tris).col(edges_[i].f2));

      edges_[num_edges_ + i].f1 = edges_[i].f1 * 4 + (order1 + 1)%3;
      edges_[num_edges_ + i].v1 = num_vertices_ + i;
      edges_[num_edges_ + i].v2 = edges_[i].v2;
      edges_[num_edges_ + i].v3 = (*new_tris)(1, edges_[num_edges_ + i].f1);


      edges_[i].f1 = edges_[i].f1 * 4 + order1%3;
      edges_[i].v2 = num_vertices_ + i;
      edges_[i].v3 = (*new_tris)(2, edges_[i].f1);
    
    if(edges_[i].f2 != -1){
      edges_[num_edges_ + i].f2 = edges_[i].f2 * 4 + order2%3;
      edges_[num_edges_ + i].v4 = (*new_tris)(2, edges_[num_edges_ + i].f2);

      
      edges_[i].f2 = edges_[i].f2 * 4 + (order2 + 1)%3;
      edges_[i].v4 = (*new_tris)(1, edges_[i].f2);

    }
    else{
      edges_[num_edges_ + i].f2 = -1;
      edges_[num_edges_ + i].v4 = -1;
      
      edges_[i].f2 = -1;
      edges_[i].v4 = -1;      
      
    }
  }

  
  size_t begin = 2 * num_edges_;
  #pragma omp parallel for
  for(size_t i = 0; i < num_faces_; ++i){
    for(size_t j = 0; j < 3; ++j){
      edges_[begin + i * 3 + j].f1 = i*4 + 3;
      edges_[begin + i * 3 + j].f2 = i*4 + (j + 1) %3;
      edges_[begin + i * 3 + j].v1 = (*new_tris)(j%3, i*4 + 3);
      edges_[begin + i * 3 + j].v2 = (*new_tris)((j + 1)%3, i*4 + 3);
      edges_[begin + i * 3 + j].v3 = (*new_tris)((j + 2)%3, i*4 + 3);
      edges_[begin + i * 3 + j].v4 = (*new_tris)(0, i*4 + (j + 1) %3);
    }

  }

  num_faces_ *= 4;
  num_vertices_ += num_edges_;
  num_edges_ = edges_.size();
  return 0;
}

int edge_core::get_order(const size_t& v_id, const Vector3i& f){
  for(size_t i = 0; i < 3; ++i){
    if(v_id == f(i))
      return i;
  }
  assert(false);
}
                  



}//namespace


