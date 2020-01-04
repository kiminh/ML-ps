/**
 *  Copyright (c) 2015 by Contributors
 */
#ifndef PS_KV_APP_H_
#define PS_KV_APP_H_
#include <algorithm>
#include <utility>
#include <vector>
#include <unordered_map>
#include "ps/base.h"
#include "ps/simple_app.h"
#include "psf/server/serverMatrixMeta.h"
#include "psf/client/ReqMatrixMeta.h"
#include "psf/psf/PSFunc.h"

namespace ps {

/**
 * \brief the structure for a list of key-value pairs
 *
 * The keys must be unique and sorted in an increasing order.  The length of a
 * value can be more than one. If \a lens is empty, then the length
 * of a value is determined by `k=vals.size()/keys.size()`.  The \a i-th KV pair
 * is then
 *
 * \verbatim {keys[i], (vals[i*k], ..., vals[(i+1)*k-1])} \endverbatim
 *
 * If \a lens is given, then `lens[i]` is the length of the \a i-th
 * value. Let
 *
 * \verbatim n = lens[0] + .. + lens[i-1]  \endverbatim
 *
 * then the \a i-th KV pair is presented as
 *
 * \verbatim {keys[i], (vals[n], ..., vals[lens[i]+n-1])} \endverbatim
 */
template <typename Val>
struct KVPairs {
  // /** \brief empty constructor */
  // KVPairs() {}
  /** \brief the list of keys */
  SArray<Key> keys;
  /** \brief the according values */
  SArray<Val> vals;
  /** \brief the according value lengths (could be empty) */
  SArray<int> lens;


  ///////////

  SArray<ServerMatrixMeta> matrixmeta;
  SArray<ReqMatrixMeta> reqmatrixmeta;
  ///////////


  /** \brief priority */
  int priority = 0;

};








/**
 * \brief A worker node that can \ref Push (\ref Pull) key-value pairs to (from) server
 * nodes
 *
 * \tparam Val the type of value, which should be primitive types such as
 * int32_t and float
 */
template<typename Val>
class KVWorker : public SimpleApp {
 public:

  /** avoid too many this-> */
  using SimpleApp::obj_;
  /**
   * \brief callback function for \ref Push and \ref Pull
   *
   * It is called by the data receiving thread of this instance when the push or
   * pull is actually finished. Namely the kv pairs have already written into
   * servers' data structure or the kv pairs have already pulled back.
   */
  using Callback = std::function<void()>;

  /**
   * \brief constructor
   *
   * \param app_id the app id, should match with \ref KVServer's id
   * \param customer_id the customer id which is unique locally
   */
  explicit KVWorker(int app_id, int customer_id) : SimpleApp() {
    using namespace std::placeholders;
    slicer_ = std::bind(&KVWorker<Val>::DefaultSlicer, this, _1, _2, _3);
    obj_ = new Customer(app_id, customer_id, std::bind(&KVWorker<Val>::Process, this, _1));
  }

  /** \brief deconstructor */
  virtual ~KVWorker() { delete obj_; obj_ = nullptr; }

  /**
   * \brief Pushes a list of key-value pairs to all server nodes.
   *
   * This function pushes a KV list specified by \a keys and \a vals to all
   * server nodes.
   *
   * Sample usage: the following codes push two KV pairs `{1, (1.1, 1.2)}` and `{3,
   * (3.1,3.2)}` to server nodes, where the value is a length-2 float vector
   * \code
   *   KVWorker<float> w;
   *   std::vector<Key> keys = {1, 3};
   *   std::vector<float> vals = {1.1, 1.2, 3.1, 3.2};
   *   w.Push(keys, vals);
   * \endcode
   *
   * If \a lens is given, then the value can be various length. See
   * \ref KVPairs for more information.
   *
   * The KV list is partitioned and sent based on the key range each server
   * maintaining. This function returns without waiting the data are sent
   * actually. Instead, use either \ref Wait or the callback to know when
   * finished. This function is thread-safe.
   *
   * @param keys a list of keys, must be unique and sorted in increasing order
   * @param vals the according values
   * @param lens optional, lens[i] stores the value length of the \a
   * i-th KV pair
   * @param cmd an optional command sent to the servers
   * @param cb the callback which is called when the push is finished.
   * @return the timestamp of this request
   */
  int Push(const std::vector<Key>& keys,
           const std::vector<Val>& vals,
           const std::vector<int>& lens = {},
           const std::vector<ServerMatrixMeta>& matrixmeta={},
           int cmd = 0,
           const Callback& cb = nullptr,
           int priority = 0) {
    return ZPush(
        SArray<Key>(keys), SArray<Val>(vals), SArray<int>(lens), SArray<ServerMatrixMeta>(matrixmeta), cmd, cb,
        priority);
  }




  /**
   * \brief Pulls the values associated with the keys from the server nodes
   *
   * This function pulls the values of the keys specified in \a keys from the
   * server nodes. The format is same to \ref KVPairs
   *
   * Sample usage: the following codes pull the values of keys \a 1 and \a 3
   * from the server nodes.
   * \code
   *   KVWorker<float> w;
   *   std::vector<Key> keys = {1, 3};
   *   std::vector<float> vals;
   *   w.Pull(keys, &vals);
   * \endcode
   *
   * It's a non-blocking call. The actual pulling is finished,
   * namely \a vals (and \a lens) is filled with pulled values, only
   * if \ref Wait returns or the callback is called.
   *
   * @param keys a list of keys, must be unique and sorted in increasing order
   * @param vals the buffer for the pulled values. It can be 0 size.
   * @param lens optional buffer for the value length. If set, it can be 0 size.
   * @param cmd an optional command sent to the servers
   * @param cb the callback which is called when the pull is finished.
   * @return the timestamp of this request
   */
  int Pull(const std::vector<Key>& keys,
           std::vector<Val>* vals,
           std::vector<ReqMatrixMeta>& reqmatrixmeta,
           std::vector<int>* lens = nullptr,
           int cmd = 0,
           const Callback& cb = nullptr,
           int priority = 0) {

    SArray<Key> skeys(keys);
    SArray<ReqMatrixMeta> smetas(reqmatrixmeta); 
    int ts = AddPullMLCB(skeys, vals, smetas,lens, cmd, cb);

    KVPairs<Val> kvs;
    kvs.keys = skeys;
    kvs.reqmatrixmeta = smetas; 
    kvs.priority = priority;
    Send(ts, false, true, cmd, kvs);

    return ts;

  }

  /**
   * \brief Pushes and Pulls a list of key-value pairs to and from the server
   * nodes.
   *
   * This function pushes the values of the keys specified in \a keys to the
   * server nodes and subsequently pulls and updates the values in \a vals.
   *
   * Sample usage: the following code pushes and pulls the values of keys
   * \a 1 and \a 3 to and from the server nodes.
   * \code
   *   KVWorker<float> w;
   *   std::vector<Key> keys = {1, 3};
   *   std::vector<float> vals;
   *   w.PushPull(keys, &vals);
   * \endcode
   *
   * It's a non-blocking call. The actual pulling is finished,
   * namely \a vals (and \a lens) is filled with pulled values, only
   * if \ref Wait returns or the callback is called.
   *
   * @param keys a list of keys, must be unique and sorted in increasing order
   * @param vals the according values
   * @param outs the buffer for the pulled values. It can be 0 size.
   * @param lens optional buffer for the value length. If set, it can be 0 size.
   * @param cmd an optional command sent to the servers
   * @param cb the callback which is called when the pull is finished.
   * @return the timestamp of this request
   */
  int PushPull(const std::vector<Key>& keys,
               const std::vector<Val>& vals,
               std::vector<Val>* outs,
               std::vector<int>* lens = nullptr,
               int cmd = 0,
               const Callback& cb = nullptr,
               int priority = 0) {
    CHECK_NOTNULL(outs);
    if (outs->empty())
      outs->resize(vals.size());
    else
      CHECK_EQ(vals.size(), outs->size());

    SArray<Key> skeys(keys);
    SArray<Val> svals(vals);
    auto souts = new SArray<Val>(outs->data(), outs->size());
    SArray<int>* slens = lens ?
        new SArray<int>(lens->data(), lens->size()) : nullptr;
    int ts = ZPushPull(skeys, svals, souts, slens, cmd,
        [this, cb, souts, slens]() {
          delete souts;
          delete slens;
          if (cb) cb();
        }, priority);
    return ts;
  }

  /**
   * \brief Waits until a push or pull has been finished
   *
   * Sample usage:
   * \code
   *   int ts = w.Pull(keys, &vals);
   *   Wait(ts);
   *   // now vals is ready for use
   * \endcode
   *
   * \param timestamp the timestamp returned by the push or pull
   */
  void Wait(int timestamp) { obj_->WaitRequest(timestamp); }

  /**
   * \brief zero-copy Push
   *
   * This function is similar to \ref Push except that all data
   * will not be copied into system for better performance. It is the caller's
   * responsibility to keep the content to be not changed before actually
   * finished.
   */
  int ZPush(const SArray<Key>& keys,
            const SArray<Val>& vals,
            const SArray<int>& lens = {},
            const SArray<ServerMatrixMeta>& matrixmeta={},
            int cmd = 0,
            const Callback& cb = nullptr,
            int priority = 0) {

    int ts = obj_->NewRequest(kServerGroup);
    AddCallback(ts, cb);
    KVPairs<Val> kvs;
    kvs.keys = keys;
    kvs.vals = vals;
    kvs.lens = lens;
    kvs.matrixmeta = matrixmeta;
    kvs.priority = priority;
    Send(ts, true, false, cmd, kvs); 
    return ts;
}

  /**
   * \brief zero-copy Pull
   *
   * This function is similar to \ref Pull except that all data
   * will not be copied into system for better performance. It is the caller's
   * responsibility to keep the content to be not changed before actually
   * finished.
   */
  int ZPull(const SArray<Key>& keys,
            SArray<Val>* vals,
            SArray<int>* lens = nullptr,
            int cmd = 0,
            const Callback& cb = nullptr,
            int priority = 0) {
    int ts = AddPullCB(keys, vals, lens, cmd, cb);
    KVPairs<Val> kvs;
    kvs.keys = keys;
    kvs.priority = priority;
    Send(ts, false, true, cmd, kvs);
    return ts;
  }

  /**
   * \brief zero-copy PushPull
   *
   * This function is similar to \ref PushPull except that all data
   * will not be copied into system for better performance. It is the caller's
   * responsibility to keep the content to be not changed before actually
   * finished.
   */
  int ZPushPull(const SArray<Key>& keys,
                const SArray<Val>& vals,
                SArray<Val>* outs,
                SArray<int>* lens = nullptr,
                int cmd = 0,
                const Callback& cb = nullptr,
                int priority = 0) {
    int ts = AddPullCB(keys, outs, lens, cmd, cb);
    KVPairs<Val> kvs;
    kvs.keys = keys;
    kvs.vals = vals;
    kvs.priority = priority;
    if (lens)
      kvs.lens = *lens;
    Send(ts, true, true, cmd, kvs);
    return ts;
  }
  using SlicedKVs = std::vector<std::pair<bool, KVPairs<Val>>>;
  /**
   * \brief a slicer partitions a key-value list according to the key ranges
   * \param send the kv list for partitioning
   * \param ranges the key ranges, ranges[i] is the key range of server i
   * \param sliced the sliced lists. slices[i] should only contains keys in
   * ranges[i] and the according values
   */
  using Slicer = std::function<void(
      const KVPairs<Val>& send, const std::vector<Range>& ranges,
      SlicedKVs* sliced)>;

  /**
   * \brief set a user-defined slicer
   */
  void set_slicer(const Slicer& slicer) {
    CHECK(slicer); slicer_ = slicer;
  }

 private:
  /**
   * \brief internal pull, C/D can be either SArray or std::vector
   */
  template <typename C, typename D>
  int AddPullCB(const SArray<Key>& keys, C* vals,const SArray<ReqMatrixMeta>& reqmeta, D* lens,
            int cmd, const Callback& cb);
  
  template <typename C, typename D>
  int AddPullMLCB(const SArray<Key>& keys, C* vals,const SArray<ReqMatrixMeta>& reqmeta, D* lens,
            int cmd, const Callback& cb);
  /**
   * \brief add a callback for a request. threadsafe.
   * @param cb callback
   * @param timestamp the timestamp of the request
   */
  
  void AddCallback(int timestamp, const Callback& cb) {

    if (!cb) return;

    std::lock_guard<std::mutex> lk(mu_);

    callbacks_[timestamp] = cb;

  }


  /**
   * \brief run and delete the callback
   * \param timestamp the timestamp of the callback
   */
  void RunCallback(int timestamp);
 

 
 /**
   * \brief send the kv list to all servers
   * @param timestamp the timestamp of the request
   * @param push whether or not it is a push request
   * @param push whether or not it is a pull request
   * @param cmd command
   */
  void Send(int timestamp, bool push, bool pull, int cmd, const KVPairs<Val>& kvs);


  
  /** \brief internal receive handle */
  void Process(const Message& msg);
  /** \brief default kv slicer */
  void DefaultSlicer(const KVPairs<Val>& send,
                     const std::vector<Range>& ranges,
                     SlicedKVs* sliced);

  /** \brief data buffer for received kvs for each timestamp */
  std::unordered_map<int, std::vector<KVPairs<Val>>> recv_kvs_;

  /** \brief callbacks for each timestamp */
  std::unordered_map<int, Callback> callbacks_;

  /** \brief lock */
  std::mutex mu_;
  /** \brief kv list slicer */
  Slicer slicer_;
};






/** \brief meta information about a kv request */
struct KVMeta {
  /** \brief the int cmd */
  int cmd;
  /** \brief whether or not this is a push request */
  bool push;
  /** \brief whether or not this is a pull request */
  bool pull;
  /** \brief sender's node id */
  int sender;
  /** \brief the associated timestamp */
  int timestamp;
  /** \brief the customer id of worker */
  int customer_id;
};


/**
 * \brief A server node for maintaining key-value pairs
 */
template <typename Val>
class KVServer : public SimpleApp {
 public:
  /**
   * \brief constructor
   * \param app_id the app id, should match with \ref KVWorker's id
   */
  explicit KVServer(int app_id) : SimpleApp() {
    using namespace std::placeholders;
    obj_ = new Customer(app_id, app_id, std::bind(&KVServer<Val>::Process, this, _1));
  }

  /** \brief deconstructor */
  virtual ~KVServer() { delete obj_; obj_ = nullptr; }

  /**
   * \brief the handle to process a push/pull request from a worker
   * \param req_meta meta-info of this request
   * \param req_data kv pairs of this request
   * \param server this pointer
   */
  using ReqHandle = std::function<void(const KVMeta& req_meta,
                                       const KVPairs<Val>& req_data,
                                       KVServer* server)>;
  void set_request_handle(const ReqHandle& request_handle) {
    CHECK(request_handle) << "invalid request handle";
    request_handle_ = request_handle;
  }

  /**
   * \brief response to the push/pull request
   * \param req the meta-info of the request
   * \param res the kv pairs that will send back to the worker
   */
  void Response(const KVMeta& req, const KVPairs<Val>& res = KVPairs<Val>());

 
  
 private:
  /** \brief internal receive handle */
  void Process(const Message& msg);
  /** \brief request handle */
  ReqHandle request_handle_;

  
};


/**
 * \brief an example handle adding pushed kv into store
 */

////////////////////////////////// store matrix value and meta


// when push , req_data.key is the key to store the partitioned matrix, when pull , the key is always 0 for ps0, xx for ps2 ...
// because we save the key in the req_data.reqmatrix, key for matrix1, key2 for matrix , rowIndex for row1, rowIndex2 for row2
// we use column partition to ensure the same position of elements for row1 ,row2 are on the same machine.


template <typename Val>
struct KVServerMLHandle {
  void operator()(
      const KVMeta& req_meta, const KVPairs<Val>& req_data, KVServer<Val>* server) {

    size_t n = req_data.keys.size();


    //CHECK_EQ(n,req_data.matrixmeta.size());
   // std::cout<< req_data.matrixmeta[0].matrixId<<std::endl;


    KVPairs<Val> res;

    if (!req_meta.pull) {

      CHECK_EQ(n, req_data.lens.size());

      CHECK_EQ(n,req_data.matrixmeta.size());

      //LOG(INFO)<<"matrix meta:"<<req_data.matrixmeta[0].endRow;

     // one key, one matrix, one matrixmeta

    } else {


      res.keys = req_data.keys;

     // res.reqmatrixmeta = req_data.reqmatrixmeta;

    //  LOG(INFO)<<"reqmatrixmeta.size()" <<res.reqmatrixmeta.size();

      CHECK_EQ(n,req_data.reqmatrixmeta.size());

     
    }

    int t = 0;

    for (size_t i = 0; i < n; ++i) {

      //Key key = req_data.keys[i];


///////////////////////////////////////////////////////////////////////

      if (req_meta.push) { // push may inc a exit row , so the

 
        handlePush(i,req_data,t);


      }

      if (req_meta.pull) {

       handlePull(req_data.reqmatrixmeta[i],res);

      }

///////////////////////////////////////////////////////////////////////////

    }

    server->Response(req_meta, res);

  }



 void handlePush(const int index, const KVPairs<Val>& req_data, int & accumulate){
      
         Key key = req_data.keys[index];

         size_t len_ = req_data.lens[index];

         psfType type = req_data.matrixmeta[index].type; // pushRow, pushAll

         switch(type){

           case psfType::PushAll:
            {

//             std::cout<<key<<std::endl; 
             if(MatrixValue_[key].size()==0){

             MatrixMeta_[key]=req_data.matrixmeta[index];

             for(int j=0;j<req_data.lens[index];j++)
                 MatrixValue_[key].push_back(req_data.vals[accumulate++]);
             } else{

             CHECK_EQ(len_,MatrixValue_[key].size()); // 
            
             for(int j=0;j<req_data.lens[index];j++)
                 MatrixValue_[key][j]+=req_data.vals[accumulate++];
               
                
             }



             }
              
             

             break;

           case psfType::PushRow:
               {

               CHECK_NE(MatrixValue_[key].size(),0);
               CHECK_GE(req_data.matrixmeta[index].rowIndex,MatrixMeta_[key].startRow);
               CHECK_LT(req_data.matrixmeta[index].rowIndex,MatrixMeta_[key].endRow);
               CHECK_EQ(len_,MatrixValue_[key].size());
               for(size_t j = 0 ; j<len_;j++) MatrixValue_[key][j]+=req_data.vals[accumulate++];

               }
               break;

           default:
             break;
        }

}

 void handlePull(ReqMatrixMeta req, KVPairs<Val>& res){

    //check matrixId

    Key  key = req.key;    

    //int key2 = req.key2;

    
   // std::cout<< "handle pull" << key<<" "<< MatrixMeta_[key].matrixId<<" "<< req.matrixId;

    CHECK_EQ(MatrixMeta_[key].matrixId,req.matrixId);

    //CHECK_EQ(MatrixMeta_[key2].matrixId,req.matrixId2);
    //Key key2 = req.key2; 


    switch(req.type){

    case psfType::PullAll:
       {
      
        req.setStartRow(MatrixMeta_[key].startRow);
        req.setEndRow(MatrixMeta_[key].endRow);
        req.setStartCol( MatrixMeta_[key].startCol);
        req.setEndCol(MatrixMeta_[key].endCol);
        res.reqmatrixmeta.push_back(req);
        pullall(key,req,res);
       
       }
       break;

    case psfType::GetRow:

        //check row range
        //LOG(INFO)<<"GetRow";

        {

        CHECK_GE(req.rowIndex,MatrixMeta_[key].startRow);
        CHECK_LT(req.rowIndex,MatrixMeta_[key].endRow);

        req.setStartCol( MatrixMeta_[key].startCol);
        req.setEndCol(MatrixMeta_[key].endCol);
        res.reqmatrixmeta.push_back(req);
        getrow(key,req,res);
 
        }

        break;

    case psfType::RowSum:
        {
        
        
        CHECK_GE(req.rowIndex,MatrixMeta_[key].startRow);
        CHECK_LT(req.rowIndex,MatrixMeta_[key].endRow);

        req.setStartCol(MatrixMeta_[key].startCol);
        req.setEndCol(MatrixMeta_[key].endCol);

        res.reqmatrixmeta.push_back(req);

        rowsum(key,req,res);

         }
        break;

    case psfType::GetCol:
        {
        CHECK_GE(req.colIndex,MatrixMeta_[key].startCol);
        CHECK_LT(req.colIndex,MatrixMeta_[key].endCol);
        
        req.setStartRow( MatrixMeta_[key].startRow);
        req.setEndRow(MatrixMeta_[key].endRow);
        res.reqmatrixmeta.push_back(req);
        getcol(key,req,res);
        }
        break;
    case psfType::ColSum:
         {
        CHECK_GE(req.colIndex,MatrixMeta_[key].startCol);
        CHECK_LT(req.colIndex,MatrixMeta_[key].endCol);
        
        req.setStartRow( MatrixMeta_[key].startRow);
        req.setEndRow(MatrixMeta_[key].endRow);
        res.reqmatrixmeta.push_back(req);
        colsum(key,req,res);
         }  
        break;

    case psfType::RowDot:
        {
        Key  key2 = req.key2;
        CHECK_EQ(MatrixMeta_[key2].matrixId,req.matrixId2);

        CHECK_GE(req.rowIndex,MatrixMeta_[key].startRow);
        CHECK_LT(req.rowIndex,MatrixMeta_[key].endRow);
        CHECK_GE(req.rowIndex2,MatrixMeta_[key2].startRow);
        CHECK_LT(req.rowIndex2,MatrixMeta_[key2].endRow);
        res.reqmatrixmeta.push_back(req);
        rowdot(req,res);
        }
      break;

    case psfType::ColDot:
        {

        Key key2 = req.key2;
        CHECK_EQ(MatrixMeta_[key2].matrixId,req.matrixId2);


        CHECK_GE(req.colIndex,MatrixMeta_[key].startCol);
        CHECK_LT(req.colIndex,MatrixMeta_[key].endCol);
        CHECK_GE(req.colIndex2,MatrixMeta_[key2].startCol);
        CHECK_LT(req.colIndex2,MatrixMeta_[key2].endCol);
        res.reqmatrixmeta.push_back(req);
        coldot(req,res);
         }

     break;

     default:
        LOG(ERROR)<<"unsupported op";
//        break;     
   }

}

////////////////// pull function

   void pullall(const Key key, const ReqMatrixMeta& req, KVPairs<Val>& res){

            int size = MatrixValue_[key].size();
            for(int i = 0 ; i < size ;i++) 
                res.vals.push_back(MatrixValue_[key][i]);
            res.lens.push_back(size);
  
   }


   void rowdot(const ReqMatrixMeta& req, KVPairs<Val>& res){

       Key key = req.key;
       Key key2 = req.key2;
       int row1 = req.rowIndex; 
       int row2 = req.rowIndex2;

       int ele1 = MatrixMeta_[key].endCol-MatrixMeta_[key].startCol;
       int ele2 = MatrixMeta_[key2].endCol-MatrixMeta_[key2].startCol;
       CHECK_EQ(ele1,ele2);
       Val result=0;
       int startPoint1=(row1-MatrixMeta_[key].startRow)*ele1;
       int startPoint2 = (row2-MatrixMeta_[key2].startRow)*ele2;

       for(int i = 0 ; i< ele1;i++) result+=MatrixValue_[key][startPoint1++]*MatrixValue_[key2][startPoint2++];
      
       res.vals.push_back(result);
       res.lens.push_back(1);



    }
    
    void coldot(const ReqMatrixMeta& req, KVPairs<Val>& res){

       Key key = req.key;
       Key key2 = req.key2;

       int col1 = req.colIndex; 
       int col2 = req.colIndex2;

       int ele1 = MatrixMeta_[key].endRow- MatrixMeta_[key].startRow;
       int ele2 = MatrixMeta_[key2].endRow-MatrixMeta_[key2].startRow;

       CHECK_EQ(ele1,ele2);

       Val result=0;
       int startPoint1 = col1- MatrixMeta_[key].startCol;
       int startPoint2 = col2- MatrixMeta_[key2].startCol;

       int inc1 = MatrixMeta_[key].endCol- MatrixMeta_[key].startCol;

       int inc2 = MatrixMeta_[key2].endCol- MatrixMeta_[key2].startCol;


       for(int i =0; i < ele1;i++) {result+=MatrixValue_[key][startPoint1]*MatrixValue_[key2][startPoint2]; startPoint1+=inc1 ; startPoint2+=inc2;}
       res.vals.push_back(result);
       res.lens.push_back(1);

    }

    void getrow(const Key key, const ReqMatrixMeta& req, KVPairs<Val>& res){

            int targetRow = req.rowIndex;
            int elePerRow = MatrixMeta_[key].endCol- MatrixMeta_[key].startCol; 
            // ensure start point
            int startPoint = (targetRow-MatrixMeta_[key].startRow)*elePerRow;
            
            for(int i = startPoint; i< startPoint+elePerRow;i++) res.vals.push_back(MatrixValue_[key][i]);
            res.lens.push_back(elePerRow);
     }
 
    void rowsum(const Key key, const ReqMatrixMeta& req, KVPairs<Val>& res){

            int targetRow = req.rowIndex; 
            int elePerRow = MatrixMeta_[key].endCol- MatrixMeta_[key].startCol; 
            // ensure start point
            int startPoint = (targetRow-MatrixMeta_[key].startRow)*elePerRow;
            Val  result = 0 ;
            for(int i = startPoint; i< startPoint+elePerRow;i++) result+=MatrixValue_[key][i];
            res.vals.push_back(result);
            res.lens.push_back(1);


      }

    void getcol(const Key key, const ReqMatrixMeta& req, KVPairs<Val>& res){
     
            int targetCol = req.colIndex;
            int elePerRow = MatrixMeta_[key].endCol- MatrixMeta_[key].startCol; 
            // ensure start point
            int startPoint = (targetCol-MatrixMeta_[key].startCol);
            for(int i = startPoint; i< elePerRow* (MatrixMeta_[key].endRow- MatrixMeta_[key].startRow) ; i+=elePerRow) res.vals.push_back(MatrixValue_[key][i]);
            res.lens.push_back(MatrixMeta_[key].endRow-MatrixMeta_[key].startRow);

     }

    void colsum(const Key key, const ReqMatrixMeta& req, KVPairs<Val>& res){
            int targetCol = req.colIndex;
            int elePerRow = MatrixMeta_[key].endCol- MatrixMeta_[key].startCol; 
            // ensure start point
            int startPoint = (targetCol-MatrixMeta_[key].startCol);
            Val result = 0 ;
            for(int i = startPoint; i< elePerRow* (MatrixMeta_[key].endRow- MatrixMeta_[key].startRow) ; i+=elePerRow) result+=MatrixValue_[key][i]; 
            res.vals.push_back(result);
            res.lens.push_back(1);

     }

std::unordered_map<Key,std::vector<Val>> MatrixValue_;
std::unordered_map<Key,ServerMatrixMeta> MatrixMeta_;

};


/////////////////////////////////////////////////////////////////////////////////////


template <typename Val>
struct KVServerDefaultHandle {
  void operator()(
      const KVMeta& req_meta, const KVPairs<Val>& req_data, KVServer<Val>* server) {
    
    size_t n = req_data.keys.size();

    //CHECK_EQ(n,req_data.matrixmeta.size());
   // std::cout<< req_data.matrixmeta[0].matrixId<<std::endl;

    KVPairs<Val> res;
    if (!req_meta.pull) {

      CHECK_EQ(n, req_data.vals.size());

    } else {
      res.keys = req_data.keys;
    }
    for (size_t i = 0; i < n; ++i) {
      Key key = req_data.keys[i];
      if (req_meta.push) {
        store[key] += req_data.vals[i];
      }
      if (req_meta.pull) {

        res.vals.push_back(store[key]);

      }
    }

    server->Response(req_meta, res);
  }
  std::unordered_map<Key, Val> store;
};



///////////////////////////////////////////////////////////////////////////////

template <typename Val>
void KVServer<Val>::Process(const Message& msg) {

  if (msg.meta.simple_app) {
    SimpleApp::Process(msg); return;
  }

  KVMeta meta;
  meta.cmd       = msg.meta.head;
  meta.push      = msg.meta.push;
  meta.pull      = msg.meta.pull;
  meta.sender    = msg.meta.sender;
  meta.timestamp = msg.meta.timestamp;
  meta.customer_id = msg.meta.customer_id;

  KVPairs<Val> data;
  int n = msg.data.size();

  if (n) {

    //LOG(INFO)<<"n"<<n;
    CHECK_GE(n, 3);
    data.keys = msg.data[0];
    data.vals = msg.data[1];
    int t = 2;
    if (n > 3) { // n ==4 

     // CHECK_EQ(n, 3);
      data.lens = msg.data[t];
      CHECK_EQ(data.lens.size(), data.keys.size());
      t++;

    }

   // if(n=3){
   
    if(meta.push){
   // CHECK_EQ(n,4);
    data.matrixmeta=msg.data[t];
    CHECK_EQ(data.matrixmeta.size(), data.keys.size());
    }
   
    if(meta.pull){
   data.reqmatrixmeta = msg.data[t];
   CHECK_EQ(data.reqmatrixmeta.size(),data.keys.size());

   }

 // }  
  
 }

  CHECK(request_handle_);

  request_handle_(meta, data, this); 

}

template <typename Val>
void KVServer<Val>::Response(const KVMeta& req, const KVPairs<Val>& res) {
  Message msg;
  msg.meta.app_id = obj_->app_id();
  msg.meta.customer_id = req.customer_id;
  msg.meta.request     = false;
  msg.meta.push        = req.push;
  msg.meta.pull        = req.pull;
  msg.meta.head        = req.cmd;
  msg.meta.timestamp   = req.timestamp;
  msg.meta.recver      = req.sender;
  if (res.keys.size()) {
    msg.AddData(res.keys);
    msg.AddData(res.vals);
    if (res.lens.size()) {
      msg.AddData(res.lens);
    }
    if(res.reqmatrixmeta.size()){
        msg.AddData(res.reqmatrixmeta);
   }
  }
  Postoffice::Get()->van()->Send(msg);
}




template <typename Val>
void KVWorker<Val>::DefaultSlicer(
    const KVPairs<Val>& send, const std::vector<Range>& ranges,
    typename KVWorker<Val>::SlicedKVs* sliced) {
  
  sliced->resize(ranges.size());

  // find the positions in msg.key
  size_t n = ranges.size();
  std::vector<size_t> pos(n+1);

  const Key* begin = send.keys.begin();
  const Key* end = send.keys.end();

  for (size_t i = 0; i < n; ++i) {
    if (i == 0) {
      pos[0] = std::lower_bound(begin, end, ranges[0].begin()) - begin;
      begin += pos[0];
    } else {
      CHECK_EQ(ranges[i-1].end(), ranges[i].begin());
    }
    size_t len = std::lower_bound(begin, end, ranges[i].end()) - begin;
    begin += len;

    pos[i+1] = pos[i] + len;

    // don't send it to servers for empty kv
    sliced->at(i).first = (len != 0);
  }

  CHECK_EQ(pos[n], send.keys.size());

  if (send.keys.empty()) return;

  // the length of value
  size_t k = 0, val_begin = 0, val_end = 0;
  if (send.lens.empty()) {
    k = send.vals.size() / send.keys.size();
    CHECK_EQ(k * send.keys.size(), send.vals.size());
  } else {
    CHECK_EQ(send.keys.size(), send.lens.size());
   // CHECK_EQ(send.keys.size(), send.matrixmeta.size()); // new 
  }

  // slice
  for (size_t i = 0; i < n; ++i) {
    if (pos[i+1] == pos[i]) {
      sliced->at(i).first = false;
      continue;
    }
    sliced->at(i).first = true;
    auto& kv = sliced->at(i).second;
    kv.keys = send.keys.segment(pos[i], pos[i+1]);

    // new add 
    if(send.matrixmeta.size()) kv.matrixmeta = send.matrixmeta.segment(pos[i],pos[i+1]);
    if(send.reqmatrixmeta.size()) kv.reqmatrixmeta = send.reqmatrixmeta.segment(pos[i],pos[i+1]);

    if (send.lens.size()) {
      kv.lens = send.lens.segment(pos[i], pos[i+1]);

      for (int l : kv.lens) val_end += l;
      kv.vals = send.vals.segment(val_begin, val_end);
      val_begin = val_end;
    } else {

      kv.vals = send.vals.segment(pos[i]*k, pos[i+1]*k);

    }
  }
}


template <typename Val>
void KVWorker<Val>::Send(int timestamp, bool push, bool pull, int cmd, const KVPairs<Val>& kvs) {
  // slice the message
  SlicedKVs sliced;
  slicer_(kvs, Postoffice::Get()->GetServerKeyRanges(), &sliced);

  // need to add response first, since it will not always trigger the callback
  int skipped = 0;
  for (size_t i = 0; i < sliced.size(); ++i) {
    if (!sliced[i].first) ++skipped;
  }
  obj_->AddResponse(timestamp, skipped);
  if ((size_t)skipped == sliced.size()) {
    RunCallback(timestamp);
  }

  for (size_t i = 0; i < sliced.size(); ++i) {
    const auto& s = sliced[i];
    if (!s.first) continue;
    Message msg;
    msg.meta.app_id = obj_->app_id();
    msg.meta.customer_id = obj_->customer_id();
    msg.meta.request     = true;
    msg.meta.push        = push;
    msg.meta.pull        = pull;
    msg.meta.head        = cmd;
    msg.meta.timestamp   = timestamp;
    msg.meta.recver      = Postoffice::Get()->ServerRankToID(i);
    msg.meta.priority    = kvs.priority;
    const auto& kvs = s.second;

    if (kvs.keys.size()) {

      msg.AddData(kvs.keys);
      msg.AddData(kvs.vals);

      if (kvs.lens.size()) {
        msg.AddData(kvs.lens);
      }

     // new add
     if(kvs.matrixmeta.size()){
       
       msg.AddData(kvs.matrixmeta); // 
       
    }

     if(kvs.reqmatrixmeta.size()){
        msg.AddData(kvs.reqmatrixmeta);
    }

  }
    Postoffice::Get()->van()->Send(msg);
  }
}




template <typename Val>
void KVWorker<Val>::Process(const Message& msg) {
  if (msg.meta.simple_app) {
    SimpleApp::Process(msg); return;
  }
  // store the data for pulling
  int ts = msg.meta.timestamp;
  if (msg.meta.pull) {
    CHECK_GE(msg.data.size(), (size_t)2);
    KVPairs<Val> kvs;
    kvs.keys = msg.data[0];
    kvs.vals = msg.data[1];
    if (msg.data.size() > (size_t)2) {
      kvs.lens = msg.data[2];
    }
    if(msg.data.size()>(size_t)3) {
      kvs.reqmatrixmeta=msg.data[3];
    }
    mu_.lock();
    recv_kvs_[ts].push_back(kvs);
    mu_.unlock();
  }

  // finished, run callbacks
  if (obj_->NumResponse(ts) == Postoffice::Get()->num_servers() - 1)  {
    RunCallback(ts);
  }
}

template <typename Val>
void KVWorker<Val>::RunCallback(int timestamp) {
  mu_.lock();
  auto it = callbacks_.find(timestamp);
  if (it != callbacks_.end()) {
    mu_.unlock();

    CHECK(it->second);
    it->second();

    mu_.lock();
    callbacks_.erase(it);
  }
  mu_.unlock();
}


template <typename Val>
template <typename C, typename D>
int KVWorker<Val>::AddPullCB(
    const SArray<Key>& keys, C* vals, const SArray<ReqMatrixMeta>& reqmeta , D* lens, int cmd,
    const Callback& cb) {

  int ts = obj_->NewRequest(kServerGroup);


  AddCallback(ts, [this, ts, keys, vals, lens, cb]() mutable {
      mu_.lock();
      auto& kvs = recv_kvs_[ts];
      mu_.unlock();

      // do check
      size_t total_key = 0, total_val = 0;
      size_t total_req = 0 ; // new add
 
      for (const auto& s : kvs) {
        Range range = FindRange(keys, s.keys.front(), s.keys.back()+1);
        CHECK_EQ(range.size(), s.keys.size())
            << "unmatched keys size from one server";

        if (lens) CHECK_EQ(s.lens.size(), s.keys.size());

        total_key += s.keys.size();
        total_val += s.vals.size();
        total_req += s.reqmatrixmeta.size(); // new add 

      }

      CHECK_EQ(total_key, keys.size()) << "lost some servers?";
      LOG(INFO)<<"total kes: "<<total_key<<" total req: "<<total_req; 
      CHECK_EQ(total_key, total_req) << "lost some servers?"; // new add
      LOG(INFO)<<"startcol: "<<kvs[0].reqmatrixmeta[0].startCol<<"endcol: "<<kvs[0].reqmatrixmeta[0].endCol;
        
      // fill vals and lens
      std::sort(kvs.begin(), kvs.end(), [](
          const KVPairs<Val>& a, const KVPairs<Val>& b) {
                  return a.keys.front() < b.keys.front();
        });


      CHECK_NOTNULL(vals);

      if (vals->empty()) {
        vals->resize(total_val); // std::vector
      } else { 
        CHECK_EQ(vals->size(), total_val);
      }

      Val* p_vals = vals->data();
      int *p_lens = nullptr;

      if (lens) {
        if (lens->empty()) {
          lens->resize(keys.size());
        } else {
          CHECK_EQ(lens->size(), keys.size());
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        memcpy(p_vals, s.vals.data(), s.vals.size() * sizeof(Val));
        p_vals += s.vals.size();

        if (p_lens) {
          memcpy(p_lens, s.lens.data(), s.lens.size() * sizeof(int));
          p_lens += s.lens.size();
        }

      }

      //TODO: CHECK lens and data size
      mu_.lock();
      recv_kvs_.erase(ts);
      mu_.unlock();
      if (cb) cb();
    });

  return ts;

}





//////////////////////////////////////////////////////////////////////////

// behavior different according to different psfType

template <typename Val>
template <typename C, typename D>
int KVWorker<Val>::AddPullMLCB(
    const SArray<Key>& keys, C* vals, const SArray<ReqMatrixMeta>& reqmeta , D* lens, int cmd,
    const Callback& cb) {


  int ts = obj_->NewRequest(kServerGroup);

  AddCallback(ts, [this, ts, keys, vals, lens, cb]() mutable {

      mu_.lock();
      auto& kvs = recv_kvs_[ts];
      mu_.unlock();

      // do check
      size_t total_key = 0, total_val = 0;
      size_t total_req = 0 ; // new add


      for (const auto& s : kvs) {

        
        Range range = FindRange(keys, s.keys.front(), s.keys.back()+1);
        CHECK_EQ(range.size(), s.keys.size())
            << "unmatched keys size from one server";

        if (lens) CHECK_EQ(s.lens.size(), s.keys.size()); 

        total_key += s.keys.size();
        total_val += s.vals.size();
        total_req += s.reqmatrixmeta.size(); // new add 
       
      }
       
      CHECK_EQ(total_key, keys.size()) << "lost some servers?";
     
      // LOG(INFO)<<"total kes: "<<total_key<<" total req: "<<total_req;

      CHECK_EQ(total_key, total_req) << "lost some servers?"; // new add


      // fill vals and lens
      std::sort(kvs.begin(), kvs.end(), [](
          const KVPairs<Val>& a, const KVPairs<Val>& b) {
                  return a.keys.front() < b.keys.front();
        });

      CHECK_NOTNULL(vals);


/////////////////////////////////////////////////////////////////////////////
 

      // for one pull , reqmatrixmeta is same for all the key, so
      // check kvs[0].reqmatrixmeta[0].type to get psffunc.
      
      psfType type = kvs[0].reqmatrixmeta[0].type;
      
      switch(type){

      case psfType::PullAll:
        {
   
      if (vals->empty()) {

        vals->resize(total_val); // std::vector

      } else {

//        std::cout<<"add pullcb"<<vals->size()<<" "<<total_val<<std::endl;

        CHECK_EQ(vals->size(), total_val);

      }

      Val* p_vals = vals->data();
      int *p_lens = nullptr;


      if (lens) {
        if (lens->empty()) {
          lens->resize(1);
        } else {
          CHECK_EQ(lens->size(), 1);
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        memcpy(p_vals, s.vals.data(), s.vals.size() * sizeof(Val));
        p_vals += s.vals.size();

        CHECK_EQ(s.lens.size(),1);
        *p_lens = * p_lens + *s.lens.data();


     //   if (p_lens) {
     //     memcpy(p_lens, s.lens.data(), s.lens.size() * sizeof(int));
     //     p_lens += s.lens.size();
     //  }

      }

       }
       break;
      
      case psfType::GetRow:
       {

      if (vals->empty()) {
        vals->resize(total_val); // std::vector
      } else {
        CHECK_EQ(vals->size(), total_val);
      }

      Val* p_vals = vals->data();
      int *p_lens = nullptr;


      if (lens) {
        if (lens->empty()) {
          lens->resize(keys.size());
        } else {
          CHECK_EQ(lens->size(), keys.size());
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        memcpy(p_vals, s.vals.data(), s.vals.size() * sizeof(Val));
        p_vals += s.vals.size();

        if (p_lens) {
          memcpy(p_lens, s.lens.data(), s.lens.size() * sizeof(int));
          p_lens += s.lens.size();
        }

      }

       }
       break;

      case psfType::RowSum:
       {

      if (vals->empty()) {
        vals->resize(1,0); // the size of res is 1
      } else {

        CHECK_EQ(vals->size(), 1);
        if(*vals->data()!=0) *vals->data() = 0 ;
        

      }

      Val* p_vals = vals->data();
      int *p_lens = nullptr;


      if (lens) {
        if (lens->empty()) {
          lens->resize(1);
        } else {
          CHECK_EQ(lens->size(), 1);
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        CHECK_EQ(s.vals.size(),1);
        *p_vals = *p_vals + *s.vals.data();

        CHECK_EQ(*s.lens.data(),1);

      }

      *p_lens = 1;

      }

      break;
      case psfType::ColDot:
       {

      if (vals->empty()) {
        vals->resize(1,0); // the size of res is 1
      } else {
        CHECK_EQ(vals->size(), 1);
        if(*vals->data()!=0)*vals->data()=0;
      }

      Val* p_vals = vals->data();
      int *p_lens = nullptr;


      if (lens) {
        if (lens->empty()) {
          lens->resize(1);
        } else {
          CHECK_EQ(lens->size(), 1);
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        CHECK_EQ(s.vals.size(),1);
//        std::cout<<*s.vals.data()<<std::endl;
        *p_vals = *p_vals + *s.vals.data();
        CHECK_EQ(*s.lens.data(),1);

      }

      *p_lens = 1;

      }

       break;

     default:
        LOG(ERROR)<<"not supported psfType";

     }

     // for example , for get row psfunc, after we aggregate result from all the servers, 
     // we need to check the result val lens equals to the row length of that matrix, in order to 
     // aggregate result from all servers in order, when we pull from servers, 
     // we let the reqmatrixmeta to acquire the start col and end col from the matrixmeta info stored in the server. 
     // and sort the kvs in worker end by the start col.

////////////////////////////////////////////////////////////////////////////

/*
      if (vals->empty()) {
        vals->resize(total_val); // std::vector
      } else {
        CHECK_EQ(vals->size(), total_val);
      }



      Val* p_vals = vals->data();
      int *p_lens = nullptr;


      if (lens) {
        if (lens->empty()) {
          lens->resize(keys.size());
        } else {
          CHECK_EQ(lens->size(), keys.size());
        }
        p_lens = lens->data();
      }

      for (const auto& s : kvs) {

        memcpy(p_vals, s.vals.data(), s.vals.size() * sizeof(Val));
        p_vals += s.vals.size();

        if (p_lens) {
          memcpy(p_lens, s.lens.data(), s.lens.size() * sizeof(int));
          p_lens += s.lens.size();
        }

      }
*/

///////////////////////////////////////////////////////////////////////////////

      mu_.lock();
      recv_kvs_.erase(ts);
      mu_.unlock();
      if (cb) cb();
    });

  return ts;

}


/////////////////////////////////////////////////////////////////////////////////////////////////



}  // namespace ps
#endif

