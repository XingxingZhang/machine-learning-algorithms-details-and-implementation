#!/usr/bin/python
#-*- coding:GBK -*-

import random
import sys

FILE_ENCODE = "GBK"

fea_dict = dict()

def add2dict(fea):
    if not fea in fea_dict:
        key = len(fea_dict)
        fea_dict[fea] = key

def n_gram_fea(name,N,add):
    flist = []
    for i in xrange(len(name)-N+1):
        fea = name[i:i+N]
        if add:
            add2dict(fea)
        else:
            flist.append(fea)
    return flist

def to_svm_format(infile,outfile):
    op = open(infile,'r')
    for line in op:
        line = line.decode(FILE_ENCODE).strip()
        strs = line.split('\t')
        name = strs[0]
        name = name[1:]
        n_gram_fea(name,1,True)
        #n_gram_fea(name,2,True)
        #n_gram_fea(name,3,True)
    op.close()
    add2dict('SINGLE_CHAR')
    add2dict('2_SAME_CHAR')

    op = open(infile,'r')
    out = open(outfile,'w')
    for line in op:
        line = line.decode(FILE_ENCODE).strip()
        strs = line.split('\t')
        name = strs[0]
        name = name[1:]
        if strs[1] == '1':
            label = '1'
        else:
            label = '-1'
        flist = n_gram_fea(name,1,False)
        #flist.extend( n_gram_fea(name,2,False) )
        #flist.extend( n_gram_fea(name,3,False) )
        '''
        if len(name) == 1:
            flist.append('SINGLE_CHAR')
        elif len(name) == 2 and name[0] == name[1]:
            flist.append('2_SAME_CHAR')
        '''
        vec = [0] * len(fea_dict)
        for fea in flist:
            index = fea_dict[fea]
            vec[index] = 1
        out.write(label)
        for i in xrange(len(vec)):
            if vec[i] != 0:
                out.write( ' %d:%d'%(i,vec[i]) )
        out.write('\n')
    op.close()
    out.close()

def rand_div_corpus(infile,ratio,trainfile,testfile):
    op = open(infile,'r')
    train = open(trainfile,'w')
    test = open(testfile,'w')
    data = []
    for line in op:
        data.append(line)
    for i in xrange(len(data)-1):
        size = len(data)-i
        index = random.randint(0,size-1)
        tmp = data[index]
        data[index] = data[size-1]
        data[size-1] = tmp
    N = int(len(data) * ratio)
    for i in xrange(len(data)):
        if i < N:
            train.write(data[i])
        else:
            test.write(data[i])
    op.close()
    train.close()
    test.close()

def print_usage():
    print 'Usage:'
    print '[1] randomly divide <infile> into <outfile1> <outfile1> by ratio <ratio>'
    print '%s -d <infile> <ratio> <outfile1> <outfile2>' % sys.argv[0]
    print '[2] convert raw data to svm_format: raw data format: \'[name]\\t(0|1)\' for each line (0 indicate female, 1 indicate male)'
    print '%s -c <train-raw-data> <test-raw-data> <svm-train-data> <svm-test-data>' % sys.argv[0]


if __name__ == '__main__':
    if len(sys.argv) != 6:
        print_usage()
        sys.exit(1)

    if sys.argv[1] == '-d':
        ratio = float(sys.argv[3])
        rand_div_corpus(sys.argv[2],ratio,sys.argv[4],sys.argv[5])
    elif sys.argv[1] == '-c':
        to_svm_format(sys.argv[2],sys.argv[4])
        to_svm_format(sys.argv[3],sys.argv[5])
    else:
        print 'invalid option'
        print_usage()
        sys.exit(1)
