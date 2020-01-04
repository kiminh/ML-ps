#ifndef _MATRIX_CONTEXT_
#define _MATRIX_CONTEXT_
 
#include<string>


class MatrixContext{


  /**
   * Matrix readable name
   */
  private:
  
  std::string name;

  /**
   * Number of rows for this matrix
   */
  int rowNum;

  /**
   * Number of cols for this matrix
   */
  long colNum;

  /**
   * Index range start
   */
  long indexStart;

  /**
   * Index range end
   */
  long indexEnd;

  /**
   * Number of valid indexes
   */
  long validIndexNum;

  /**
   * Number of rows for one block
   */
  int maxRowNumInBlock;

  /**
   * Number of cols for one block
   */
  long maxColNumInBlock;


  /**
   * Partitioner for this matrix
   */
  private Class<? extends Partitioner> partitionerClass;


  /**
   * Matrix partitions
   */
  std::vector<PartContext> parts;



  /**
   * Row type
   */
//  RowType rowType;




  /**
   * Others key value attributes for this matrix.
   */
   std::unordered_map<std::string, std::tring> attributes;

  /**
   * Matrix id
   */
  int matrixId;

  /**
   * PS Matrix initialization function
   */
 // PSMatrixInit initFunc;


  /**
   * Creates a new MatrixContext by default.
   */
  public MatrixContext() {
    this->name ("", -1, -1);
  }

  /**
   * Create a new MatrixContext
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param colNum matrix column number
   */
  public MatrixContext(String name, int rowNum, long colNum) {
    this(name, rowNum, colNum, -1, -1);
  }

  /**
   * reate a new MatrixContext use column range
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param start column index range start
   * @param end column index range end
   */
  public MatrixContext(String name, int rowNum, long start, long end) {
    this(name, rowNum, -1, start, end, -1, -1, -1, new ArrayList<>(), RowType.T_DOUBLE_DENSE);
  }


  /**
   * Create a new MatrixContext
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param colNum matrix column number
   * @param maxRowNumInBlock matrix block row number
   * @param maxColNumInBlock matrix block column number
   */
  public MatrixContext(String name, int rowNum, long colNum, int maxRowNumInBlock,
      long maxColNumInBlock) {
    this(name, rowNum, colNum, -1, -1, -1, maxRowNumInBlock, maxColNumInBlock, new ArrayList<>(),
        RowType.T_DOUBLE_DENSE);
  }


  /**
   * Create a new MatrixContext
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param colNum matrix column number
   * @param validIndexNum number of valid indexes
   * @param maxRowNumInBlock matrix block row number
   * @param maxColNumInBlock matrix block column number
   */
  public MatrixContext(String name, int rowNum, long colNum, long validIndexNum,
      int maxRowNumInBlock, long maxColNumInBlock) {
    this(name, rowNum, colNum, -1, -1, validIndexNum, maxRowNumInBlock, maxColNumInBlock,
        new ArrayList<>(), RowType.T_DOUBLE_DENSE);
  }

  /**
   * Create a new MatrixContext use column size and partitioner parameters
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param colNum matrix column number
   * @param validIndexNum number of valid indexes
   * @param maxRowNumInBlock matrix block row number
   * @param maxColNumInBlock matrix block column number
   * @param rowType matrix row type
   */
  public MatrixContext(String name, int rowNum, long colNum, long validIndexNum,
      int maxRowNumInBlock, long maxColNumInBlock, RowType rowType) {
    this(name, rowNum, colNum, -1, -1, validIndexNum, maxRowNumInBlock, maxColNumInBlock,
        new ArrayList<>(), rowType);
  }

  /**
   * Create a new MatrixContext use column range and partitioner parameters
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param indexStart column index range start
   * @param indexEnd column index range end
   * @param validIndexNum number of valid indexes
   * @param maxRowNumInBlock matrix block row number
   * @param maxColNumInBlock matrix block column number
   * @param rowType matrix row type
   */
  public MatrixContext(String name, int rowNum, long indexStart, long indexEnd,
      long validIndexNum, int maxRowNumInBlock, long maxColNumInBlock, RowType rowType) {
    this(name, rowNum, -1, indexStart, indexEnd, validIndexNum, maxRowNumInBlock, maxColNumInBlock,
        new ArrayList<>(), rowType);
  }

  /**
   * Create a matrix context use column size and partitions
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param colNum matrix column number
   * @param validIndexNum number of column valid indexes
   * @param parts matrix partitions
   * @param rowType matrix row type
   */
  public MatrixContext(String name, int rowNum, long colNum,
      long validIndexNum, List<PartContext> parts, RowType rowType) {
    this(name, rowNum, colNum, -1, -1, validIndexNum, -1, -1, parts, rowType);
  }

  /**
   * Create a matrix context use column range and partitions
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param indexStart matrix column index start
   * @param indexEnd matrix column index end
   * @param validIndexNum valid index number in range
   * @param parts matrix partitions
   * @param rowType matrix row type
   */
  public MatrixContext(String name, int rowNum, long indexStart, long indexEnd,
      long validIndexNum, List<PartContext> parts, RowType rowType) {
    this(name, rowNum, -1, indexStart, indexEnd, validIndexNum, -1, -1, parts, rowType);
  }

  /**
   * Create a matrix context use column range and partitions
   *
   * @param name matrix name
   * @param rowNum matrix row number
   * @param indexStart matrix column index start
   * @param indexEnd matrix column index end
   * @param validIndexNum valid index number in range
   * @param parts matrix partitions
   * @param rowType matrix row type
   */
  public MatrixContext(String name, int rowNum, long colNum, long indexStart, long indexEnd,
      long validIndexNum, int maxRowNumInBlock, long maxColNumInBlock, List<PartContext> parts,
      RowType rowType) {
    this.name = name;
    this.rowNum = rowNum;
    this.colNum = colNum;
    this.indexStart = indexStart;
    this.indexEnd = indexEnd;
    this.validIndexNum = validIndexNum;
    this.maxRowNumInBlock = maxRowNumInBlock;
    this.maxColNumInBlock = maxColNumInBlock;
    this.parts = parts;
    this.rowType = rowType;
    this.attributes = new HashMap<>();
    this.matrixId = -1;
  }






//////////////////////////////////////////////////////
  /**
   * Gets name.
   *
   * @return the name
   */
  public :
  
  std::string getName() {
    return name;
  }

  /**
   * Gets row num.
   *
   * @return the row num
   */
  
  int getRowNum() {
    return rowNum;
  }

  /**
   * Gets col num.
   *
   * @return the col num
   */
  long getColNum() {
    return colNum;
  }

  /**
   * Get number of valid indexes
   *
   * @return number of valid indexes
   */
  long getValidIndexNum() {
    return validIndexNum;
  }

  /**
   * Set number of valid indexes
   *
   * @param validIndexNum number of valid indexes
   */
  void setValidIndexNum(long validIndexNum) {
    this->validIndexNum = validIndexNum;
  }




  /**
   * Gets max row num in block.
   *
   * @return the max row num in block
   */
  int getMaxRowNumInBlock() {
    return maxRowNumInBlock;
  }

  /**
   * Gets max col num in block.
   *
   * @return the max col num in block
   */
  long getMaxColNumInBlock() {
    return maxColNumInBlock;
  }

  /**
   * Gets partitioner.
   *
   * @return the partitioner
   */
//  public Class<? extends Partitioner> getPartitionerClass() {
//    return partitionerClass;
//  }

  /**
   * Gets row type.
   *
   * @return the row type
   */
//  RowType getRowType() {
//    return rowType;
//  }

  /**
   * Gets attributes.
   *
   * @return the attributes
   */

 std::unordered_map<std::string, std::string> getAttributes() {
    return attributes;
  }



  /**
   * Set matrix id
   */
  void setMatrixId(int matrixId) {
    this->matrixId = matrixId;
  }

  /**
   * Sets name.
   *
   * @param name the name
   */
   void setName(std::tring name) {
    this->name = name;
  }

  /**
   * Sets row num.
   *
   * @param rowNum the row num
   */
  void setRowNum(int rowNum) {
    this->rowNum = rowNum;
  }

  /**
   * Sets col num.
   *
   * @param colNum the col num
   */
  void setColNum(long colNum) {
    this->colNum = colNum;
  }

  /**
   * Sets max row num in block.
   *
   * @param maxRowNumInBlock the max row num in block
   */
  void setMaxRowNumInBlock(int maxRowNumInBlock) {
    this->maxRowNumInBlock = maxRowNumInBlock;
  }

  /**
   * Sets max col num in block.
   *
   * @param maxColNumInBlock the max col num in block
   */
  void setMaxColNumInBlock(long maxColNumInBlock) {
    this->maxColNumInBlock = maxColNumInBlock;
  }

  /**
   * Sets partitioner.
   *
   * @param partitionerClass the partitioner class
   */
//  public void setPartitionerClass(Class<? extends Partitioner> partitionerClass) {
//    this.partitionerClass = partitionerClass;
//  }

  /**
   * Sets row type.
   *
   * @param rowType the row type
   */
//  void setRowType(RowType rowType) {
//    this->rowType = rowType;
//  }

  /**
   * Set matrix op log type
   *
   * @param type op log type
   */
 // public void setMatrixOpLogType(MatrixOpLogType type) {
 //   attributes.put(MatrixConf.MATRIX_OPLOG_TYPE, type.name());
 // }

  /**
   * Set matrix value type class, this parameter should be set if you use
   * T_ANY_INTKEY_DENSE,T_ANY_INTKEY_SPARSE and T_ANY_LONGKEY_SPARSE
   *
   * @param valueClass matrix value type class
   */
//  public void setValueType(Class<? extends IElement> valueClass) {
//    attributes.put(MatrixConf.VALUE_TYPE_CLASSNANE, valueClass.getName());
//  }

  /**
   * Get matrix value type class
   *
   * @return null if this parameter is not set
   * @throws ClassNotFoundException if value class is not found
   */
//  public Class<? extends IElement> getValueType() throws ClassNotFoundException {
//    String className = attributes.get(MatrixConf.VALUE_TYPE_CLASSNANE);
//    if (className == null) {
//      return null;
//    } else {
//      return (Class<? extends IElement>) Class.forName(className);
//    }
//  }

  /**
   * Get matrix server partition class
   *
   * @return matrix server partition class
   * @throws ClassNotFoundException if server partition class is not found
   */
//  public Class<? extends IServerPartition> getPartitionClass() throws ClassNotFoundException {
//    String className = attributes.get(MatrixConf.SERVER_PARTITION_CLASS);
//    if (className == null) {
//      return MatrixConf.DEFAULT_SERVER_PARTITION_CLASS;
//    } else {
//      return (Class<? extends ServerPartition>) Class.forName(className);
//    }
//  }

  /**
   * Set matrix server partition class
   *
   * @param partClass server partition class
   */
//  public void setPartitionClass(Class<? extends IServerPartition> partClass) {
//    attributes.put(MatrixConf.SERVER_PARTITION_CLASS, partClass.getName());
//  }

  /**
   * Get matrix server partition storage class
   *
   * @return matrix server partition storage class, null means not set by user
   * @throws ClassNotFoundException if server partition storage class is not found
   */
//  public Class<? extends IServerPartitionStorage> getPartitionStorageClass()
//      throws ClassNotFoundException {
//    String className = attributes.get(MatrixConf.SERVER_PARTITION_STORAGE_CLASS);
//    if (className == null) {
//      return null;
//    } else {
//      return (Class<? extends IServerPartitionStorage>) Class.forName(className);
//    }
//  }

  /**
   * Set matrix server partition storage class
   *
   * @param partStorageClass matrix server partition storage class
   */
//  public void setPartitionStorageClass(Class<? extends IServerPartitionStorage> partStorageClass) {
//    attributes.put(MatrixConf.SERVER_PARTITION_STORAGE_CLASS, partStorageClass.getName());
//  }

  /**
   * Set matrix context.
   *
   * @param key the key
   * @param value the value
   * @return the matrix context
   */
   MatrixContext& set(std::string key, std::string value) {
    attributes[key]= value;
    return *this;
  }

  private void initPartitioner() {

    if (partitionerClass != null) {
      return;
    }

    partitionerClass = RangePartitioner.class;
  }

  /**
   * Set index range start
   *
   * @param indexStart index range start
   */
 
  void setIndexStart(long indexStart) {
    this->indexStart = indexStart;
  }

  /**
   * get index range start
   */
  
  long getIndexStart() {
    return indexStart;
  }

  /**
   * Get index range end
   *
   * @return index range end
   */
 
  long getIndexEnd() {
    return indexEnd;
  }

  /**
   * Set index range end
   *
   * @param indexEnd index range end
   */
  
  void setIndexEnd(long indexEnd) {
    this->indexEnd = indexEnd;
  }

  /**
   * Get matrix partitions
   *
   * @return matrix partitions
   */
  std::vector<PartContext> getParts() {
    return parts;
  }

  /**
   * Set matrix partitions
   *
   * @param parts matrix partitions
   */
 
  void setParts(List<PartContext> parts) {
    this->parts = parts;
  }

  /**
   * Add a partition context
   *
   * @param part partition context
   */
  void addPart(PartContext part) {
    parts.push_back(part);
  }


  /**
   * Get matrix id
   *
   * @return matrix id
   */
  int getMatrixId() {
    return matrixId;
  }

  /**
   * Get PS matrix init function
   * @return PS matrix init function
   */
  
  PSMatrixInit getInitFunc() {
    return initFunc;
  }

  /**
   * Set PS matrix init function
   * @param initFunc PS matrix init function
   */
 
  void setInitFunc(PSMatrixInit initFunc) {
    this->initFunc = initFunc;
  }

  /**
   * Init matrix
   */
  
  void init(Configuration conf) {

    initPartitioner();

    String loadPath = attributes.get(MatrixConf.MATRIX_LOAD_PATH);
    if (loadPath == null) {
      loadPath = conf.get(AngelConf.ANGEL_LOAD_MODEL_PATH);
      if (loadPath != null) {
        if (matrixPathExist(loadPath, name, conf)) {
          attributes.put(MatrixConf.MATRIX_LOAD_PATH, loadPath);
          loadMatrixMetaFromFile(name, loadPath, conf);
        }
      }
    } else {
      loadMatrixMetaFromFile(name, loadPath, conf);
    }

    adaptParams();
    check();
  }





/*
  private boolean matrixPathExist(String loadPath, String name, Configuration conf)
      throws IOException {
    Path matrixPath = new Path(loadPath, name);
    FileSystem fs = matrixPath.getFileSystem(conf);
    return fs.exists(matrixPath);
  }

  private void adaptParams() {
    // If col == -1 and start/end not set
    if (colNum <= 0 && indexEnd <= indexStart) {
      if (rowType.isIntKey()) {
        indexStart = Integer.MIN_VALUE;
        indexEnd = Integer.MAX_VALUE;
        colNum = indexEnd - indexStart;
      } else {
        indexStart = Long.MIN_VALUE;
        indexEnd = Long.MAX_VALUE;
      }
    } else if (colNum <= 0 && indexEnd > indexStart) {
      // start/end set
      // for dense type, we need to set the colNum to set dim for vectors
      if (rowType.isIntKey()) {
        colNum = indexEnd - indexStart;
      }
    } else if (colNum > 0 && indexEnd <= indexStart) {
      // colNum set, start/end not set
      indexStart = 0;
      indexEnd = colNum;
    }

    LOG.info("Matrix context " + name + " row=" + rowNum +
        " col=" + colNum + " start=" + indexStart + " end=" + indexEnd);
  }

  private:

   void check() {


    // Row number must > 0
    if (rowNum <= 0) {
      throw new AngelException(
          "matrix " + name + " parameter is invalid, row number must > 0, now is " + rowNum);
    }

    if (colNum > 0 && indexEnd > indexStart && (colNum != (indexEnd - indexStart))) {
      // both set, check its valid
      throw new AngelException("matrix " + name
          + " parameter is invalid, column number must = (indexEnd - indexStart), now colNum = "
          + colNum
          + ", indexEnd = " + indexEnd + ", indexStart = " + indexStart);
    }

*/


/*
    if (colNum <= 0 && rowType.isLongKey() && rowType.isDense()) {
      throw new AngelException(
          "matrix " + name + " is dense and with longkey, might cost a lot of memory. " +
              "Please considering to configure with sparse row type, like (T_FLOAT_SPARSE_LONGKEY)");
    }

    if (indexStart != 0 && (rowType.isDense() || rowType.isComp())) {
      throw new AngelException("matrix " + name
          + " parameter is invalid, nonzero index range start can only be use sparse model type now"
          + ", but model type now is " + rowType + " with index range start value = " + indexStart);
    }
*/



  }

/*
  private void loadMatrixMetaFromFile(String name, String path, Configuration conf)
      throws IOException {
    Path meteFilePath = new Path(new Path(path, name), ModelFilesConstent.modelMetaFileName);
    MatrixFilesMeta meta = new MatrixFilesMeta();

    FileSystem fs = meteFilePath.getFileSystem(conf);
    LOG.info("Load matrix meta for matrix " + name + " from " + meteFilePath);

    if (!fs.exists(meteFilePath)) {
      throw new IOException("matrix meta file does not exist ");
    }

    FSDataInputStream input = fs.open(meteFilePath);
    try {
      meta.read(input);
    } catch (Throwable e) {
      throw new IOException("Read meta failed ", e);
    } finally {
      input.close();
    }

    rowNum = meta.getRow();
    colNum = meta.getCol();
    maxRowNumInBlock = meta.getBlockRow();
    maxColNumInBlock = meta.getBlockCol();
    indexStart = meta.getFeatureIndexStart();
    indexEnd = meta.getFeatureIndexEnd();
    rowType = RowType.valueOf(meta.getRowType());
    Map<String, String> oldAttributes = meta.getOptions();
    if (oldAttributes != null && !oldAttributes.isEmpty()) {
      for (Map.Entry<String, String> kv : oldAttributes.entrySet()) {
        attributes.put(kv.getKey(), kv.getValue());
      }
    }
  }

*/


  /**
   * Get estimate sparsity
   *
   * @return estimate sparsity
   */
/* 
 public double getEstSparsity() {
    if (validIndexNum <= 0) {
      return 0.0;
    } else {
      if (colNum <= 0) {
        if (rowType == RowType.T_DOUBLE_SPARSE || rowType == RowType.T_FLOAT_SPARSE
            || rowType == RowType.T_LONG_SPARSE || rowType == RowType.T_INT_SPARSE) {
          return (double) validIndexNum / 2 / Integer.MAX_VALUE;
        } else {
          return (double) validIndexNum / 2 / Long.MAX_VALUE;
        }
      } else {
        return (double) validIndexNum / colNum;
      }
    }
  }
*/

};
#endif
