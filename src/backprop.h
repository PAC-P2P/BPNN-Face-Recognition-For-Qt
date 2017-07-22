#ifndef BACKPROP_H
#define BACKPROP_H

#define BIGRND 0x7fffffff       // 最大的带符号整型数

/***
    神经网络的数据结构。
    该网络是一个全连接的前馈三层神经网络。
    每一层中的单元0是阈值单元，这意味着其他单元在[1,n]内取值

 ***/

typedef struct {
  int input_n;                  /* number of input units 输入层神经元的个数 */
  int hidden_n;                 /* number of hidden units 隐藏层神经元的个数 */
  int output_n;                 /* number of output units 输出层神经元的个数 */

  double *input_units;          /* the input units 输入层神经元 下标从1开始 */
  double *hidden_units;         /* the hidden units 隐藏层神经元 下标从1开始 */
  double *output_units;         /* the output units 输出层神经元 下标从1开始 */

  double *hidden_delta;         /* storage for hidden unit error 隐藏单元误差 */
  double *output_delta;         /* storage for output unit error 输出单元误差 */

  double *target;               /* storage for target vector 目标向量 下标从1开始 */

  double **input_weights;       /* weights from input to hidden layer 输入层神经元和隐藏层神经元的联接权值 */
  double **hidden_weights;      /* weights from hidden to output layer 隐藏层神经元和输出层神经元的联接权值 */

                                /*** The next two are for momentum 冲量 ***/
  double **input_prev_weights;  /* previous change on input to hidden wgt */
  double **hidden_prev_weights; /* previous change on hidden to output wgt */
} BPNN;


/*** User-level functions ***/

void bpnn_initialize(int);

BPNN *bpnn_create(int, int, int);
void bpnn_free(BPNN *);

void bpnn_train(BPNN *, double, double, double *, double *);
void bpnn_feedforward(BPNN *);

void bpnn_save(BPNN *, char *);
BPNN *bpnn_read(char *);

#endif // BACKPROP_H
