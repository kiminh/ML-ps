#include<string>
#include"psf/comm/MLClientFactory.h"
#include"psf/comm/MLClient.h"


class PSModel{

private:

 

std::string modelName;
int row;
long col;
int  blockRow;
long  blockCol;
long validIndexNum;
bool needSave;
MatrixContext* getContext;
MLClient* getClient;


public:


       PSModel(std::string modelName,
               int row,
               long col,
               int  blockRow= -1,
               long  blockCol = -1,
               long validIndexNum  = -1,
               bool needSave = true):modelName(modelName),row(row),col(col),blockRow(blockRow),blockCol(blockCol),validIndexNum(validIndexNum),needSave(needSave){


  /** Matrix configuration */
   getContext  = new MatrixContext(modelName, row, col, validIndexNum, blockRow, blockCol)

  /** Get ps matrix client */
  getClient = MLClientFactory::getMatrix(modelName);


 }



  // =======================================================================
  // Get and Set Area
  // =======================================================================


  /**
    * Get matrix id
    *
    * @return matrix id
    */

  
  int  getMatrixId(){

    return getClient->getMatrixId();

  }



  /**
    * Set model need to be saved
    *
    * @param _needSave
    * @return
    */
  void  setNeedSave(bool _needSave) {
    this->needSave = _needSave;
  }


 
  /**
    * Set model load path
    *
    * @param path load path
    */
//  void  setLoadPath(std::string path) {

//    getContext->set(MatrixConf.MATRIX_LOAD_PATH, path);

   // LOG.info("Before training, matrix " + this.matrixCtx.getName + " will be loaded from " + path)
    
// }

  /**
    * Set model save path
    *
    * @param path
    */
// void  setSavePath(std::string) {

//    getContext->set(MatrixConf.MATRIX_SAVE_PATH, path);

//  }



// =======================================================================
  // Remote Model Area
  // =======================================================================


  /**
    * Increment the matrix row vector use a same dimension vector. The update will be cache in local
    * and send to ps until flush or clock is called
    *
    * @param delta update row vector
    * @throws com.tencent.angel.exception.AngelException
    */
  void  increment(std::vector<float> delta) {
    
      getClient->increment(delta);
    
   
  }

  /**
    * Increment the matrix row vector use a same dimension vector. The update will be cache in local
    * and send to ps until flush or clock is called
    *
    * @param rowIndex row index
    * @param delta    update row vector
    * @throws com.tencent.angel.exception.AngelException
    */
  void increment( int rowIndex,  std::vector<float> delta ) {

       getClient->increment(rowIndex, delta);
  }

  /**
    * Increment the matrix row vectors use same dimension vectors. The update will be cache in local
    * and send to ps until flush or clock is called
    *
    * @param deltas update row vectors
    * @throws com.tencent.angel.exception.AngelException
    */
  void increment(std::vector<std::vector<float>> deltas) {
    for (auto &delta :deltas) increment(delta);
  }



  /**
    * Get any result you want about the matrix use a psf get function
    *
    * @param func psf get function
    * @throws com.tencent.angel.exception.AngelException
    * @return psf get function result
    */
  GetResult  get(func: GetFunc) {

      return getClient->get(func);

  }


  /**
    * Get a matrix row use row index
    *
    * @param rowIndex row index
    * @throws com.tencent.angel.exception.AngelException
    * @return
    */
  std::vector<float> getRow(int rowIndex){

      return getClient->getRow(rowIndex);
  }


  /**
    * Update the matrix use a update psf
    *
    * @param func update psf
    * @throws com.tencent.angel.exception.AngelException
    * @return a future result
    */

  void  update( UpdateFunc func ) {

      return getClient->asyncUpdate(func);
  }


};

