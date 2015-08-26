#!/bin/sh

svm_train_data=name-train.txt
svm_test_data=name-test.txt
train_raw_data=train-corpus.txt
test_raw_data=test-corpus.txt
svm_model=model
svm_predict=name-predict.txt
result=name-result.txt

echo 'generate training data'
./TrainDataGenerator.py -c $train_raw_data $test_raw_data $svm_train_data $svm_test_data
train_cmd="./svm-train -t 3 $svm_train_data $svm_model"
predict_cmd="./svm-predict $svm_test_data $svm_model $svm_predict"
echo $train_cmd
$train_cmd
echo $predict_cmd
$predict_cmd
echo 'to std format'
./tostd.py $test_raw_data $svm_predict $result
