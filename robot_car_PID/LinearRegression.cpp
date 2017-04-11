#include <LinearRegression.h>


Fifo::Fifo() {
  _first=0;
  _last=0;
  _size=0;
  }

void Fifo::push(double value) {
  _buff[_last]=value;
  _last=(_last+1)%queueLength;
  _size++;
 int i;
}

double Fifo::pop() {
  double value= _buff[_first];
  _first=(_first+1)%queueLength;
  _size--;
  return value;
}


int Fifo::getSize() {
  return _size;
}


LinearRegression::LinearRegression() {
  n=0;
  sum_X=0.0;
  sum_Y=0.0;
  sum_X2=0.0;
  sum_XY=0.0;
}

void LinearRegression::add(double X,double Y) {
  n+=1;
  sum_X+=double(X);
  sum_Y+=double(Y);
  sum_X2+=double(X)*double(X);
  sum_XY+=double(X)*double(Y);
}

void LinearRegression::sub(double X,double Y) {
  n-=1;
  sum_X-=double(X);
  sum_Y-=double(Y);
  sum_X2-=double(X)*double(X);
  sum_XY-=double(X)*double(Y);
}

double LinearRegression::getCoefficient() {
  if (n==1)
  	return sum_Y/sum_X;
  return (sum_XY - (sum_X*sum_Y)/n) / ( sum_X2 - sum_X*sum_X/n);
}  

double LinearRegression::getIntercept() {
  return predict(0.0);
}  

double LinearRegression::predict(double X) {
  double coef = getCoefficient();
  return double(coef*(X-sum_X/n) + sum_Y/n);
}  


RunningLinearRegression::RunningLinearRegression(int nbPoints) {
 list_X=Fifo();
 list_Y=Fifo();
 _nbPoints=nbPoints;
 _linearRegression = LinearRegression();

};

void RunningLinearRegression::add(double X,double Y) {
  if (list_X.getSize() == _nbPoints ) {
    _linearRegression.sub(list_X.pop(),list_Y.pop());
    }    
  list_X.push(X);
  list_Y.push(Y);
  _linearRegression.add(X,Y);
}

double RunningLinearRegression::getCoefficient() {
  return _linearRegression.getCoefficient();
}  

double RunningLinearRegression::getIntercept() {
  return _linearRegression.getIntercept();
}  

double RunningLinearRegression::predict(double X) {
  return _linearRegression.predict(X);
}  
