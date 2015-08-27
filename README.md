
Original project http://code.google.com/p/machine-learning-algorithms-details-and-implementation <br>
Google is shutting down Google code, so I have to move it here MANUALLY (Yes, Google code's Automatical exporting doesn't work for my project). <br>

It's a project created by me on April 2012. Well, I wrote these models for fun! Please note that the original license was GPL and now it is LGPL. <br>
I am trying to make the following descriptions the same as [here](http://code.google.com/p/machine-learning-algorithms-details-and-implementation)

# machine-learning-algorithms-details-and-implementation
We will discuss principles of some popular machine learning algorithms in more detailed ways than most text books. In many text books or papers, the principle of a model is discussed but not detailed enough for implementation. Besides, some derivations are ignored. We want to eliminate all the confusion. In addition, we will implement some algorithms.

# The Purpose of the Project
First of all, I do not intend to create an efficient or easy-to-use machine learning toolkit. In fact, I just want to check if I understand all the details of a specific algorithm. Furthermore, during the implementation, I found that compared to the derivation of an algorithm, the implementation is rather simple (e.g. LDA, the main part of the algorithm is less than 100 lines of code). Therefore, if you think implementing a machine learning algorithm is pretty cool, I just want to tell you "focus on the principles!"

## 1. Support Vector Machines
The reason why I want to implement svm can be found [here](http://fantasyorg.blog.163.com/blog/static/109276109201232893256743/). [svm-1.0](https://github.com/XingxingZhang/machine-learning-algorithms-details-and-implementation/tree/master/svm-1.0) is an implementation of svm, and I employed the algorithm in [1] to solve the following optimization problem:
```
min 0.5 * alpha^T * Q * alpha - e^T * alpha
s.t. 
     0 <= alpha[i] <= C     i = 1,...,m
     y^T * alpha = 0
```
The algorithm is a combination of SMO[2] and SVM-light[3] and it is very simple, graceful:) and efficient.

The current version is 1.0. Multi-class classification is not supported, and I will add this in version 1.1.

### 1.1. What's in this package?
src: the source code
bin: the executable binary
doc: the reading list of svm and the developer munual
app: an gender detector which can detect the gender of an Chinese name
### 1.2. Enviroment
The code is written in C++ under Windows Cygwin, so it can also run on Linux and Mac OS X.

## 2. Maximum Entropy Models
[maxent-1.0](https://github.com/XingxingZhang/machine-learning-algorithms-details-and-implementation/tree/master/maxent-1.0) is an implementation of Maximum Entropy Models[4]. I employed GIS (Generative Iterative Scaling) and exponential prior[5]. Well, GIS is a kind of slow, and in version 2.0, I will add IIS, L-BFGS and other faster algorithms.

Well, precisely, maxent-1.0 is an implementation of logistic regression. Maximum Entropy Modeling is a methodology, and in fact Maximum Entropy Modeling is also used in CRF.

There are many tutorials about Maximum Entropy Models. I think these two ([6],[7]) are enough to understand the model. Personally, I strongly recommend [7] (although this paper mainly discuss the principles of CRF, the MEM part is extraordinarily clear).

## 3. Latent Dirichlet Allocation
[lda-1.0](https://github.com/XingxingZhang/machine-learning-algorithms-details-and-implementation/tree/master/lda-1.0) is an implementation of Latent Dirichlet Allocation[8]. The parameters are estimated via Gibbs Sampling. Admittedly, Gibbs Sampling is cool and easy to implement. However, it is soooooooo slow!!! In order to get an accurate estimation, you must sample as many times as possible (e.g. 2000) and it will take a lot of time especially for some large corpus.

If you want to understand LDA (I mean Gibbs-sampling-based LDA), you should know what is Gibbs sampling. Fortunately, [10] is a great tutorial of Gibbs sampling and prior math knowledge is not required. After finishing [10], you can start [9], and I think it will not take you long to master LDA, if you really understand the details in [10].

A Tiny Implementation Issue
Actually, my implementation is a little faster than [gibbslda++0.2](http://gibbslda.sourceforge.net/)(their latest version). Mine takes 25m39.606s on "trndocs.dat" (which is contained in GibbsLDA++0.2.tar.gz) with default model parameters, and theirs takes 27m31.397s. The experiment was conducted on a 2G laptop with a Mac operating system.

During the estimation, we will compute p(z_j=k|z_-j,w) k = 1, ..., K and sample z_j according to the probability distribution. They did it this way:
```
    // cumulate multinomial parameters
    for (int k = 1; k < K; k++) {
        p[k] += p[k - 1];
    }
    // scaled sample because of unnormalized p[]
    double u = ((double)random() / RAND_MAX) * p[K - 1];
    
    for (topic = 0; topic < K; topic++) {
        if (p[topic] > u) {
            break;
        }
    }
```
They were linear searching a sorted array, and I employed binary search.

## 4. Conditional Random Fields
Currently, I have figured out the principle of CRF and read the code of CRF++. However, I do not intend to implement it as it is really time consuming.

I must say that CRF is so similar to MaxEnt. Actually, we can say that CRF is a sequential version of MaxEnt. The only difference between them is that for CRF, the computation is exponential and therefore we need to employ dynamic programming (forward backward algorithm and Viterbi algorithm) to reduce the time complexity.

By employing the Maximum Entropy principle, we can get that the exponential form of potential functions in CRF is not just for computational convenience and it can be explained by Maximum Entropy principle. Besides, we can also get that maximizing the log likelihood is equivalent to minimizing the Lagrange function in maximum entropy assumption together with its constrains.

There are also many good tutorials for CRF, such as [11]. However, some tutorials are really long. As a result, I only read [7] and the chapter 3 of [12].

# Reference
[1] Chih-Chung Chang and Chih-Jen Lin. LIBSVM: Introduction and Benchmarks <br>
[2] John C. Platt. Sequential Minimal Optimization: A Fast Algorithm for Training Support Vector Machines <br>
[3] Thorsten Joachims. Making Large-Scale SVM Learning Practical <br>
[4] Adam L. Berger, Stephen A. Della Pietra, and Vincent J. Della Pietra. A maximum entropy approach to natural language processing. Computational Linguistics, 22(1):39–71, 1996. <br>
[5] Joshua Goodman. Exponential priors for maximum entropy models. Technical report, Microsoft Research, 2003. <br>
[6] Robert Malouf. Maximum Entropy Models <br>
[7] Roman Klinger and Katrin Tomanek. Classical Probabilistic Models and Conditional Random Fields <br>
[8] D. M. Blei, A. Y. Ng, and M. I. Jordan, Latent Dirichlet allocation, Journal of Machine Learning Research, vol. 3, pp. 993–1022, 2003. <br>
[9] G. Heinrich, Parameter estimation for text analysis, Arbylon publications. <br>
[10] Philip Resnik and Eric Hardisty. 2010. Gibbs sampling for the uninitiated. Technical Report UMIACS-TR-2010-04, University of Maryland. <br>
[11] Charles Sutton, Andrew McCallum?. An Introduction to Conditional Random Fields. Foundations and Trends in Machine Learning 4 (4). 2012. <br>
[12] Trevor A. Cohn, 2007. Scaling Conditional Random Fields for Natural Language Processing. <br>


