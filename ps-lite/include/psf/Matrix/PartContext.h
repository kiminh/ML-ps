public class PartContext {
  private int startRow;
  private int endRow;
  private long startCol;
  private long endCol;
  private int indexNum;

  public PartContext(int startRow, int endRow, long startCol, long endCol, int indexNum) {
    this.startRow = startRow;
    this.endRow = endRow;
    this.startCol = startCol;
    this.endCol = endCol;
    this.indexNum = indexNum;
  }

  public int getStartRow() {
    return startRow;
  }

  public void setStartRow(int startRow) {
    this.startRow = startRow;
 }

  public int getEndRow() {
    return endRow;
  }

  public void setEndRow(int endRow) {
    this.endRow = endRow;
  }

  public long getStartCol() {
    return startCol;
  }

  public void setStartCol(long startCol) {
    this.startCol = startCol;
  }

  public long getEndCol() {
    return endCol;
  }

  public void setEndCol(long endCol) {
    this.endCol = endCol;
  }

  public int getIndexNum() {
    return indexNum;
  }

  public void setIndexNum(int indexNum) {
    this.indexNum = indexNum;
  }

  @Override
  public String toString() {
    return "PartContext{" +
        "startRow=" + startRow +
        ", endRow=" + endRow +
        ", startCol=" + startCol +
        ", endCol=" + endCol +
        ", indexNum=" + indexNum +
        '}';
  }
}
