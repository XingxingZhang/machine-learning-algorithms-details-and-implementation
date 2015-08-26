#!/usr/bin/python
# -*- coding:GBK -*-

import sys

def convert(testfile,predictfile,resultfile):
    test = open(testfile,'r')
    predict = open(predictfile,'r')
    result = open(resultfile,'w')

    while True:
        line_t = test.readline()
        line_p = predict.readline()
        if not line_t:
            break
        line_t = line_t.strip()
        ans = line_t.split('\t')[1]
        line_p = line_p.strip()
        fx = float(line_p)
        if fx < 0:
            label = '女'
            if ans == '0':
                label += '\tY'
            else:
                label += '\tN'
        else:
            label = '男'
            if ans == '1':
                label += '\tY'
            else:
                label += '\tN'
        result.write(line_t+'\t'+label+'\n')

    test.close()
    predict.close()
    result.close()

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print '%s <svm-test-data> <svm-predict> <result-file>' % sys.argv[0]
        sys.exit(1)
    convert(sys.argv[1],sys.argv[2],sys.argv[3])
