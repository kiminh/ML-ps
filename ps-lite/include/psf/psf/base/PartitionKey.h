#ifndef _PARTITION_KE
#define _PARTITION_KEY

/**
 * The type Partition key,represent a part of matrix
 */

class PartitionKey{


 private:
 
  int partitionId = 0;
  int matrixId = 0;


  /**
   * Elements in this partition row number are in [startRow, endRow) and column number are in
   * [StartCol, endCol);
   */
  int startRow = -1;
  long startCol = -1;
  int endRow = -1;
  long endCol = -1;
  int indexNum = -1;
  


  public:

  PartitionKey() {}

  PartitionKey(int partitionId, int matrixId, int startRow, long startCol, int endRow,
      long endCol) {
		  
    this->partitionId = partitionId;
    this->matrixId = matrixId;
    this->startRow = startRow;
    this->startCol = startCol;
    this->endRow = endRow;
    this->endCol = endCol;
    this->indexNum =-1;

 
  }

  PartitionKey(int partitionId, int matrixId, int startRow, long startCol, int endRow,
      long endCol, int indexNum) {
	
    this->partitionId = partitionId;
    this->matrixId = matrixId;
    this->startRow = startRow;
    this->startCol = startCol;
    this->endRow = endRow;
    this->endCol = endCol;
    this->indexNum = indexNum;
  
  }

  PartitionKey(int matrixId, int partId) {

    this->partitionId = partId;
    this->matrixId = matrixId;
    this->startRow = -1;
    this->startCol = -1;
    this->endRow = -1;
    this->endCol = -1;
    this->indexNum = -1;
  
}

/*
  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("PartitionKey(");
    builder.append("matrixId=").append(matrixId).append(", ");
    builder.append("partitionId=").append(partitionId).append(", ");
    builder.append("startRow=").append(startRow).append(", ");
    builder.append("startCol=").append(startCol).append(", ");
    builder.append("endRow=").append(endRow).append(", ");
    builder.append("endCol=").append(endCol).append(")");
    builder.append("indexNum=").append(indexNum).append(")");
    return builder.toString();
  }
*/

   int getPartitionId() {
    return partitionId;
  }

   int getMatrixId() {
    return matrixId;
  }

   void setMatrixId(int matrixId) {
    this->matrixId = matrixId;
  }

  int getStartRow() {
    return startRow;
  }

  long getStartCol() {
    return startCol;
  }

  void setStartCol(int startCol) {
    this->startCol = startCol;
  }

  int getEndRow() {
    return endRow;
  }

  long getEndCol() {
    return endCol;
  }

  void setEndCol(int endCol) {
    this->endCol = endCol;
  }

  void setStartCol(long startCol) {
    this->startCol = startCol;
  }

  void setEndCol(long endCol) {
    this->endCol = endCol;
  }

  int getIndexNum() {
    return indexNum;
  }

  void setIndexNum(int indexNum) {
    this->indexNum = indexNum;
  }

  void setPartitionId(int partitionId) {
    this->partitionId = partitionId;
  }

  void setStartRow(int startRow) {
    this->startRow = startRow;
  }

 void setEndRow(int endRow) {
    this->endRow = endRow;
  }

/*
  @Override
  public int compareTo(PartitionKey other) {
    if (this.startRow > other.startRow) {
      return 1;
    }

    if (this.startRow < other.startRow) {
      return -1;
    }

    if (this.startCol > other.startCol) {
      return 1;
    }

    if (this.startCol < other.startCol) {
      return -1;
    }

    return 0;
  }
 */
 
  /*

  @Override
  public void serialize(ByteBuf buf) {
    buf.writeInt(matrixId);
    buf.writeInt(partitionId);
    buf.writeInt(startRow);
    buf.writeInt(endRow);
    buf.writeLong(startCol);
    buf.writeLong(endCol);
    buf.writeInt(indexNum);
  }

  @Override
  public void deserialize(ByteBuf buf) {
    matrixId = buf.readInt();
    partitionId = buf.readInt();
    startRow = buf.readInt();
    endRow = buf.readInt();
    startCol = buf.readLong();
    endCol = buf.readLong();
    indexNum = buf.readInt();
  }

  @Override
  public int bufferLen() {
    return 8;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + matrixId;
    result = prime * result + partitionId;
    return result;
  }
  */
  
/*
  @Override
  public boolean equals(Object obj) {
    if (this == obj) {
      return true;
    }
    if (obj == null) {
      return false;
    }
    if (getClass() != obj.getClass()) {
      return false;
    }
    PartitionKey other = (PartitionKey) obj;
    if (matrixId != other.matrixId) {
      return false;
    }
    return partitionId == other.partitionId;
  }
  */
  
  
  
  
};

#endif

