# svm-app
Machine learning application. Uses SVM method from LIBSVM library.
# Installation on Debian 10
# Install dependencies:
	apt-get install git qt5-default gnuplot
# Install Python 3 dependencies:
	apt-get install python3-pip
	pip3 install numpy scipy matplotlib
# Clone repo:
	git clone https://github.com/boidachenkop/svm-app.git
# Compile:
	cd /svm-app/svm-app/
	make
	g++ -Wall -Wconversion -O3 -fPIC svm-predict.c svm.o -o svm-predict -lm
	g++ -Wall -Wconversion -O3 -fPIC svm-train.c svm.o -o svm-train -lm
# Run:
	./svm-app


