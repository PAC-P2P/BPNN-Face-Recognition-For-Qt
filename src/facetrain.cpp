#include <QDir>
#include <QString>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "facetrain.h"
#include "dir.h"

// 网络名
#define NETNAME "/facenet.net"

// 训练集名
#define TRAINNAME "/imagesSet_train.list"

// 测试集名
#define TESTNAME1 "/imagesSet_testAll.list"
#define TESTNAME2 "/imagesSet_rec.list"

// 数据目录
#define DATA "data"

// 随机产生器的种子
#define SEED 102194

// 定义保存网络所需的最小训练次数
#define SAVEDELTA 100

// 学习速率
#define LEARNRATE 0.3

// 冲量
#define IMPULSE 0.3

//extern char *strcpy();
extern void exit();

// 评估表现
int evaluate_performance(BPNN *net, double *err)
{
  bool flag = true; // 样例匹配成功为true

  *err = 0.0;
  double delta;
  int j;

  // 计算输出层均方误差之和
  for (j = 1; j <= net->output_n; j++)
  {
    delta = net->target[j] - net->output_units[j];
    *err += (0.5 * delta * delta);
  }

  for (j = 1; j <= net->output_n; j++) {
    /*** If the target unit is on... ***/
    if (net->target[j] > 0.5) {
      if (net->output_units[j] > 0.5) {
        /*** If the output unit is on, then we correctly recognized me! ***/
      } else /*** otherwise, we didn't think it was me... ***/
      {
        flag = false;
      }
    } else /*** Else, the target unit is off... ***/
    {
      if (net->output_units[j] > 0.5) {
        /*** If the output unit is on, then we mistakenly thought it was me ***/
        flag = false;
      } else {
        /*** else, we correctly realized that it wasn't me ***/
      }
    }
  }

  if (flag)
    return 1;
  else
    return 0;
}

/*** Computes the performance of a net on the images in the imagelist. ***/
/*** Prints out the percentage correct on the image set, and the
     average error between the target and the output units for the set. ***/
int performance_on_imagelist(BPNN *net, IMAGELIST *il, int list_errors, std::map<std::string, int> *map_userId)
{
  double err, val;
  int i, n, j, correct;

  err = 0.0;
  correct = 0;
  n = il->n;  // n：图片集中图片张数
  if (n > 0) {
    // 遍历图片列表中每张图片
    for (i = 0; i < n; i++) {

      /*** Load the image into the input layer. **/
      load_input_with_image(il->list[i], net);

      /*** Run the net on this input. **/
      bpnn_feedforward(net);

      /*** Set up the target vector for this image. **/
      load_target(il->list[i], net, map_userId);

      /*** See if it got it right. ***/
      if (evaluate_performance(net, &val)) {
        //匹配成功，计数器加1
        correct++;
      }
      else if (list_errors)
      {
        printf("%s", NAME(il->list[i]));

        // for (j = 1; j <= net->output_n; j++)
        // {
        //   printf("%.3f ", net->output_units[j]);
        // }
        printf("\n");
      }
      err += val; // 列表中所有图片 输出层 均方误差之和
    }

    err = err / (double)n;  // 列表中所有图片 输出层 均方误差之和 的平均数

    if (!list_errors)
      /* bthom==================================
         this line prints part of the ouput line
         discussed in section 3.1.2 of homework
          */
      // 输出 匹配准确率 和 误差
      printf("%g%%  %g \n", ((double)correct / (double)n) * 100.0, err);
  } else {
    if (!list_errors)
      printf("0.0 0.0 ");
  }
  return 0;
}

// 评估图片集的匹配情况
std::string result_on_imagelist(BPNN *net, IMAGELIST *il, int list_errors, std::map<std::string, int> *map_userId)
{
  double err, val;
  int i, n, j, correct;
  std::string s_userId;

  err = 0.0;
  correct = 0;

  n = il->n; // 图片集元素个数

  if (n > 0) {
    for (i = 0; i < n; i++) {
      /*** Load the image into the input layer. **/
      // 装载图片到输入层
      load_input_with_image(il->list[i], net);

      /*** Run the net on this input. **/
      // 在此输入的基础上运行这个网络
      bpnn_feedforward(net);

      /*** Set up the target vector for this image. **/
      // 设置目标向量
      load_target(il->list[i], net, map_userId);

      // 输出图片的名称
      printf("Picture name: %s\n", NAME(il->list[i]));

      int map_userNum = map_userId->size(), i_flag_num = 0, i_flag_i = 0;
      std::map<std::string, int>::iterator map_iter;

      for(int i = 1; i  <= map_userNum; ++i)
      {
          printf("--output_units-->> %f\n", net->output_units[i]);
          if(net->output_units[i] > 0.5)
          {
              // 统计输出权值大于0.5的输出单元个数和索引
              i_flag_num ++;
              i_flag_i = i;
          }
      }
      if(1 == i_flag_num)
      {
          for(map_iter = map_userId->begin(); map_iter != map_userId->end(); ++map_iter)
          {
              if(i_flag_i == map_iter->second)
              {
                  printf("He is --> %s \n", (map_iter->first).c_str());
                  s_userId = map_iter->first;
              }
          }
      }
      else
      {
          printf("I do not know who he is...\n");
      }

      /*** See if it got it right. ***/
      if (evaluate_performance(net, &val)) {
        correct++;
        printf("Yes\n");
      } else {
        printf("No\n");
      }

      printf("\n");

      err += val;
    }

    err = err / (double)n;

    // 输出 匹配准确率 和 平均误差
    if (!list_errors)
      printf("Accuracy rate of: %g%%  Average error: %g \n\n",
             ((double)correct / (double)n) * 100.0, err);
  } else {
    if (!list_errors)
      printf("0.0 0.0 ");
  }
  return s_userId;
}

int backprop_face(IMAGELIST *trainlist, IMAGELIST *test1list, IMAGELIST *test2list,
                  int epochs, int savedelta, char *netname, int list_errors, std::map<std::string, int> *map_userId)
{
  IMAGE *iimg;
  BPNN *net;
  int train_n, epoch, i, imgsize;
  double out_err, hid_err, sumerr;
  int userNum = map_userId->size();

  train_n = trainlist->n;

  /*** Read network in if it exists, otherwise make one from scratch ***/
  if ((net = bpnn_read(netname)) == NULL) {
    if (train_n > 0) {
      printf("Creating new network '%s'\n", netname);
      iimg = trainlist->list[0];
      imgsize = ROWS(iimg) * COLS(iimg);
      /* bthom ===========================
        make a net with:
          imgsize inputs, peopleNum hiden units, and peopleNum output unit
    【图片规模】的输入层单元个数，【训练集总人数】个隐藏层单元，【训练集总人数】个输出层单元
          */
      net = bpnn_create(imgsize, userNum, userNum);
    } else {
      printf("Need some images to train on!\n");
      return -1;
    }
  }

  if (epochs > 0) {
    /*** 训练进行中（epochs次） ***/
    printf("Training underway (going to %d epochs)\n", epochs);
    /*** 每epochs次保存网络 ***/
    printf("Will save network every %d epochs\n", savedelta);
    fflush(stdout);
  }

  /*** 迭代前输出测试表现 ***/
  /*** Print out performance before any epochs have been completed. ***/
  printf("\nBefore iteration: \n");
  printf("Training set error: 0.0\n");
  printf("Evaluate the performance of the training set : ");
  performance_on_imagelist(net, trainlist, 0, map_userId);
  printf("Evaluate performance of the test set 1 :");
  performance_on_imagelist(net, test1list, 0, map_userId);
  printf("Evaluate performance of the test set 2 :");
  performance_on_imagelist(net, test2list, 0, map_userId);
  printf("\n");
  fflush(stdout);
  if (list_errors) {
    printf(
        "\nThe picture classification failed in the training set ! \n");
    performance_on_imagelist(net, trainlist, 1, map_userId);
    printf("\nThe picture classification failed in the test set 1 ! \n");
    performance_on_imagelist(net, test1list, 1, map_userId);
    printf("\nThe picture classification failed in the test set 2 ! \n");
    performance_on_imagelist(net, test2list, 1, map_userId);
  }

  /************** 开始训练！ ****************************/
  /************** Train it *****************************/
  for (epoch = 1; epoch <= epochs; epoch++) {

    // 输出迭代次数
    printf("Iteration number: %d \n", epoch);
    fflush(stdout);

    sumerr = 0.0;
    for (i = 0; i < train_n; i++) {

      /** Set up input units on net with image i **/
      // 用训练集中图片i来设置输入层单元
      load_input_with_image(trainlist->list[i], net);

      /** Set up target vector for image i **/
      // 为图片i设置目标向量
      load_target(trainlist->list[i], net, map_userId);

      /** Run backprop, learning rate 0.3, momentum 0.3 **/
      /** 运行反向传播算法，学习速率0.3，冲量0.3 **/
      bpnn_train(net, LEARNRATE, IMPULSE, &out_err, &hid_err);

      sumerr += (out_err + hid_err);  // 训练集中所有图片作为输入，网络的 输出层 和 隐藏层 的误差之和
    }
    printf("Training set error: %g \n", sumerr);

    // 评估测试集，测试集1，测试集2 的表现
    /*** Evaluate performance on train, test, test2, and print perf ***/
    printf("Evaluate the performance of the training set : ");
    performance_on_imagelist(net, trainlist, 0, map_userId);
    printf("Evaluate performance of the test set 1 :");
    performance_on_imagelist(net, test1list, 0, map_userId);
    printf("Evaluate performance of the test set 2 :");
    performance_on_imagelist(net, test2list, 0, map_userId);
    printf("\n");
    fflush(stdout);

    /*** Save network every 'savedelta' epochs ***/
    if (!(epoch % savedelta)) {
      bpnn_save(net, netname);
    }
  }
  printf("\n");
  fflush(stdout);
  /************** 迭代结束 ****************************/

  /************** 预测结果 ****************************/

  // 输出测试集中每张图片的匹配情况
  printf("Matching at the end of the iteration: \n\n");
  printf("Test set 1 : \n\n");
  result_on_imagelist(net, test1list, 0, map_userId);
  printf("Test set 2 : \n\n");
  result_on_imagelist(net, test2list, 0, map_userId);

  /** Save the trained network **/
  if (epochs > 0) {
    bpnn_save(net, netname);
  }
  return 0;
}


void facetrain(int trainingTimes /*int argc, char *argv*/)
{
    char netname[256], trainname[256], testname1[256], testname2[256];
    IMAGELIST *trainlist, *test1list, *test2list;
    int epochs, seed, savedelta, list_errors, userNum;

    // 初始化
    userNum = 0;
    list_errors = 0;
    netname[0] = trainname[0] = testname1[0] = testname2[0] = '\0';

    // Set the data path
    QDir dir = programRootDir();
    dir.cd(DATA);
    QString data_path = dir.absolutePath();

    std::string str_data_path = data_path.toStdString();

    std::map<std::string, int> *map_userId = new std::map<std::string, int>;

    // 网络名
    strcpy(netname, (str_data_path + NETNAME).c_str());

    // 训练集名
    strcpy(trainname, (str_data_path + TRAINNAME).c_str());

    // 测试集名
    strcpy(testname1, (str_data_path + TESTNAME1).c_str());
    strcpy(testname2, (str_data_path + TESTNAME2).c_str());

    // 训练次数
    epochs = trainingTimes;

    // 随机产生器的种子
    seed = SEED;

    // 保存网络所需的最小训练次数
    savedelta =SAVEDELTA;

    /*** Create imagelists ***/
    trainlist = imgl_alloc();
    test1list = imgl_alloc();
    test2list = imgl_alloc();

    /*** If any train, test1, or test2 sets have been specified, then
         load them in. ***/
    if (trainname[0] != '\0')
        imgl_load_images_from_textfile(trainlist, trainname, map_userId);
    if (testname1[0] != '\0')
        imgl_load_images_from_textfile(test1list, testname1);
    if (testname2[0] != '\0')
        imgl_load_images_from_textfile(test2list, testname2);

    userNum = map_userId->size();
    if(userNum == 0)
    {
        printf("The training set has no user data!\n");
        exit(-1);
    }

    /*** Don't try to train if there's no training data ***/
    if (trainname[0] == '\0') {
        epochs = 0;
    }

    /*** 显示训练集，测试集1，测试集2中图片数量 ***/
    /*** Show number of images in train, test1, test2 ***/
    printf("%d images in training set\n", trainlist->n);
    printf("%d images in test1 set\n", test1list->n);
    printf("%d images in test2 set\n", test2list->n);


    /*** 初始化神经网络包 ***/
    /*** Initialize the neural net package ***/
    bpnn_initialize(seed);

    /*** If we've got at least one image to train on, go train the net ***/
    // 假如我们至少有1张图片来训练，那么就开始训练吧！
    backprop_face(trainlist, test1list, test2list, epochs, savedelta, netname, list_errors, map_userId);

//    exit(0);
}
