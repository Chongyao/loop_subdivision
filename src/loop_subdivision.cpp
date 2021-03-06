#include <libigl/readOBJ.h>
#include <libigl/writeOBJ.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include "edge_core.h"
using namespace std;
using namespace Eigen;
using namespace marvel;
using namespace chrono;
int main(int argc, char**argv){
  Eigen::initParallel();
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>Eigen parallel<<<<<<<<<<<<<<<<<<" << endl;
  cout << "enable parallel in Eigen in " << nbThreads() << " threads" << endl;
  
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>load obj<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
  boost::property_tree::ptree pt;{
    const string jsonfile_path = "../../input.json";
    
    cout << jsonfile_path << endl;
    const size_t ext = jsonfile_path.rfind(".json");
    if (ext != std::string::npos){
      read_json(jsonfile_path, pt);
      cout << "read json successful" <<endl;
    }
    else{
      cout << "json file extension error" << endl;
      return 0;
    }
  }
  
  cout << "[INFO]>>>>>>>>>>>>>>>>>>>IMPORT MESH<<<<<<<<<<<<<<<<<<" << endl;
  const string mesh_name = pt.get<string>("surf");
  const string indir = pt.get<string>("indir");
  const string outdir = pt.get<string>("outdir");
  const size_t times = pt.get<size_t>("times");
  MatrixXi surf;
  MatrixXf nods;
  
  igl::readOBJ((indir+mesh_name+".obj").c_str(), nods, surf);
  cout << "surf: " << surf.rows() << " " << surf.cols() << endl << "nods: " << nods.rows() << " " << nods.cols() << endl;
  
  surf.transposeInPlace();
  nods.transposeInPlace();
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>load obj<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//

  
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>construct edge_core
  bool if_update = times > 1? true : false;
  edge_core EC(if_update);
  shared_ptr<MatrixXi> ori_tris = make_shared<MatrixXi>(surf);
  shared_ptr<MatrixXf> ori_verts = make_shared<MatrixXf>(nods);
  shared_ptr<MatrixXi> new_tris = make_shared<MatrixXi>(0,0);
  shared_ptr<MatrixXf> new_verts = make_shared<MatrixXf>(0,0);
  auto start = system_clock::now();

  for(size_t i = 0; i < times; ++i){
    EC(ori_tris, ori_verts, new_tris, new_verts);

    igl::writeOBJ((outdir+mesh_name+ to_string(i) + ".obj").c_str(), new_verts->transpose(), new_tris->transpose());
    

    
    ori_tris = new_tris;
    ori_verts = new_verts;
    new_tris = make_shared<MatrixXi>(0,0);
    new_verts = make_shared<MatrixXf>(0,0);
  }
  auto end = system_clock::now();
  auto duration = duration_cast<microseconds>(end - start);
  cout <<  "updated alg paralellel花费了" 
       << double(duration.count()) * microseconds::period::num / microseconds::period::den 
       << "秒" << endl;

  return 0;

  
}
